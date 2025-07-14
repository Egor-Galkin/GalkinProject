#include "searchserver.h"
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cctype>

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;

    for (const auto& query : queries_input) {
        std::istringstream iss(query);
        std::vector<std::string> words;
        std::string word;

        while (iss >> word) {
            for (auto& ch : word) {
                ch = static_cast<char>(std::tolower(ch));
            }
            words.push_back(word);
        }

        std::unordered_map<size_t, size_t> docid_to_count;

        for (const auto& w : words) {
            auto entries = _index.GetWordCount(w);

            for (const auto& e : entries) {
                docid_to_count[e.doc_id] += e.count;
            }
        }

        if (docid_to_count.empty()) {
            results.emplace_back();
            continue;
        }

        size_t max_count = 0;
        for (const auto& [doc_id, count] : docid_to_count) {
            if (count > max_count) {
                max_count = count;
            }
        }

        std::vector<RelativeIndex> rel_results;
        for (const auto& [doc_id, count] : docid_to_count) {
            float rank = static_cast<float>(count) / max_count;
            rel_results.push_back({doc_id, rank});
        }

        std::sort(rel_results.begin(), rel_results.end(),
                  [](const RelativeIndex& a, const RelativeIndex& b) {
                      if (a.rank == b.rank) {
                          return a.doc_id < b.doc_id;
                      }
                      return a.rank > b.rank;
                  });

        if (rel_results.size() > _max_responses) {
            rel_results.resize(_max_responses);
        }

        results.push_back(std::move(rel_results));
    }

    return results;
}
