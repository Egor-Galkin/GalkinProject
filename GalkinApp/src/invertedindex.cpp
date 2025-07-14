#include "invertedindex.h"
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs);
    freq_dictionary.clear();

    std::mutex mutex;
    std::vector<std::thread> threads;

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        threads.emplace_back([this, &mutex, doc_id]() {
            std::istringstream iss(docs[doc_id]);
            std::unordered_map<std::string, size_t> word_count;

            std::string word;
            while (iss >> word) {
                ++word_count[word];
            }

            std::lock_guard<std::mutex> lock(mutex);
            for (const auto& [word, count] : word_count) {
                freq_dictionary[word].push_back({ doc_id, count });
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    auto it = freq_dictionary.find(word);
    if (it == freq_dictionary.end()) {
        return {};
    }
    return it->second;
}
