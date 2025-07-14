#include "converterjson.h"
#include "invertedindex.h"
#include "searchserver.h"
#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << "Current path: " << current_path << std::endl;

    std::filesystem::path project_root = current_path.parent_path();

    try {
        std::filesystem::current_path(project_root);
        std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to change directory: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "------------------------------\n";

    try {
        ConverterJSON converter;
        auto docs = converter.GetTextDocuments();
        int max_responses = converter.GetResponsesLimit();
        auto requests = converter.GetRequests();

        std::cout << "Loaded " << docs.size() << " documents\n";
        std::cout << "Loaded " << requests.size() << " requests\n";

        InvertedIndex index;
        std::cout << "Building inverted index...\n";
        index.UpdateDocumentBase(docs);
        std::cout << "Indexing completed\n";

        SearchServer server(index, max_responses);
        std::cout << "Running search...\n";
        auto answers = server.search(requests);
        std::cout << "Search completed\n";

        std::vector<std::vector<std::pair<int, float>>> converted_answers;
        converted_answers.reserve(answers.size());

        for (const auto& vec_rel : answers) {
            std::vector<std::pair<int, float>> vec_pair;
            vec_pair.reserve(vec_rel.size());
            for (const auto& rel : vec_rel) {
                vec_pair.emplace_back(static_cast<int>(rel.doc_id), rel.rank);
            }
            converted_answers.push_back(std::move(vec_pair));
        }

        converter.PutAnswers(converted_answers);

        std::cout << "Results saved successfully.\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}