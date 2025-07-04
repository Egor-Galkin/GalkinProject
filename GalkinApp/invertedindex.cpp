#include "invertedindex.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <QtConcurrent/QtConcurrent>

static std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::vector<std::string> InvertedIndex::SplitIntoWords(const std::string& text) const {
    std::vector<std::string> words;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        words.push_back(ToLower(word));
    }
    return words;
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    docs = input_docs;
    freq_dictionary.clear();

    std::vector<std::map<std::string, size_t>> word_counts(docs.size());

    QtConcurrent::blockingMap(docs, [&](const std::string& doc) {
        std::map<std::string, size_t> local_count;
        auto words = SplitIntoWords(doc);
        for (const auto& w : words) {
            if (!w.empty()) ++local_count[w];
        }
        return local_count;
    });

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        std::map<std::string, size_t> local_count;
        auto words = SplitIntoWords(docs[doc_id]);
        for (const auto& w : words) {
            if (!w.empty()) ++local_count[w];
        }
        for (const auto& [word, count] : local_count) {
            freq_dictionary[word].push_back({doc_id, count});
        }
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::string key = ToLower(word);
    if (freq_dictionary.count(key)) {
        return freq_dictionary[key];
    }
    return {};
}
