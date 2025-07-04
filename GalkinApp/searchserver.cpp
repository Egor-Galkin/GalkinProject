#include "searchserver.h"
#include <algorithm>
#include <map>
#include <set>
#include <sstream>

static std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

static std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        words.push_back(ToLower(word));
    }
    return words;
}

SearchServer::SearchServer(InvertedIndex& idx, int maxResponses)
    : _index(idx), _maxResponses(maxResponses)
{}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> all_results;

    for (const auto& query : queries_input) {
        std::vector<std::string> words = SplitIntoWords(query);

        std::set<std::string> unique_words(words.begin(), words.end());

        std::vector<std::string> sorted_words(unique_words.begin(), unique_words.end());
        std::sort(sorted_words.begin(), sorted_words.end(), [this](const std::string& a, const std::string& b) {
            return _index.GetWordCount(a).size() < _index.GetWordCount(b).size();
        });

        if (sorted_words.empty()) {
            all_results.emplace_back();
            continue;
        }

        std::vector<Entry> first_word_entries = _index.GetWordCount(sorted_words[0]);

        if (first_word_entries.empty()) {
            all_results.emplace_back();
            continue;
        }

        std::map<size_t, size_t> doc_to_count;
        for (const auto& entry : first_word_entries) {
            doc_to_count[entry.doc_id] = entry.count;
        }

        for (size_t i = 1; i < sorted_words.size(); ++i) {
            std::vector<Entry> entries = _index.GetWordCount(sorted_words[i]);
            std::map<size_t, size_t> current_docs;
            for (const auto& e : entries) {
                current_docs[e.doc_id] = e.count;
            }

            for (auto it = doc_to_count.begin(); it != doc_to_count.end();) {
                if (current_docs.find(it->first) == current_docs.end()) {
                    it = doc_to_count.erase(it);
                } else {
                    it->second += current_docs[it->first];
                    ++it;
                }
            }

            if (doc_to_count.empty()) break;
        }

        if (doc_to_count.empty()) {
            all_results.emplace_back();
            continue;
        }

        size_t max_relevance = 0;
        for (const auto& [doc_id, count] : doc_to_count) {
            if (count > max_relevance) max_relevance = count;
        }

        std::vector<RelativeIndex> results;
        for (const auto& [doc_id, count] : doc_to_count) {
            float rank = max_relevance > 0 ? static_cast<float>(count) / static_cast<float>(max_relevance) : 0.0f;
            results.push_back({doc_id, rank});
        }

        std::sort(results.begin(), results.end(), [](const RelativeIndex& a, const RelativeIndex& b) {
            return a.rank > b.rank;
        });

        if (results.size() > static_cast<size_t>(_maxResponses)) {
            results.resize(_maxResponses);
        }

        all_results.push_back(results);
    }

    return all_results;
}
