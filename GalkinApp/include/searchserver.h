#pragma once

#include "invertedindex.h"
#include <vector>
#include <string>

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id && rank == other.rank;
    }
};

class SearchServer {
public:
    SearchServer(const InvertedIndex& index, int max_responses = 5)
        : _index(index), _max_responses(max_responses) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    const InvertedIndex& _index;
    int _max_responses;
};
