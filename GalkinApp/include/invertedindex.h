#pragma once

#include <string>
#include <vector>
#include <map>

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator==(const Entry& other) const {
        return doc_id == other.doc_id && count == other.count;
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase(std::vector<std::string> input_docs);
    std::vector<Entry> GetWordCount(const std::string& word) const;

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
};
