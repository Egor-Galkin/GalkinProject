#pragma once

#include <string>
#include <vector>

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void PutAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);
};
