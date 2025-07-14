#include "converterjson.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    const std::string config_filename = "config.json";
    if (!std::filesystem::exists(config_filename)) {
        throw std::runtime_error("config file is missing");
    }

    std::ifstream config_file(config_filename);
    if (!config_file.is_open()) {
        throw std::runtime_error("cannot open config file");
    }

    if (config_file.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("config file is empty");
    }

    json config_json;
    config_file >> config_json;

    if (config_json.find("config") == config_json.end()) {
        throw std::runtime_error("config file is empty");
    }

    auto& config = config_json["config"];

    if (!config.contains("version") || config["version"] != "1.0") {
        throw std::runtime_error("config.json has incorrect file version");
    }

    if (!config_json.contains("files") || !config_json["files"].is_array()) {
        throw std::runtime_error("config.json missing files array");
    }

    std::vector<std::string> docs;
    for (const auto& filepath : config_json["files"]) {
        std::string path = filepath.get<std::string>();
        if (!std::filesystem::exists(path)) {
            std::cerr << "Warning: file not found: " << path << std::endl;
            docs.push_back("");
            continue;
        }
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Warning: cannot open file: " << path << std::endl;
            docs.push_back("");
            continue;
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        docs.push_back(ss.str());
    }

    std::cout << "Starting " << config["name"].get<std::string>() << std::endl;

    return docs;
}

int ConverterJSON::GetResponsesLimit() {
    const std::string config_filename = "config.json";
    std::ifstream config_file(config_filename);
    if (!config_file.is_open()) {
        throw std::runtime_error("cannot open config file");
    }

    if (config_file.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("config file is empty");
    }

    json config_json;
    config_file >> config_json;

    if (config_json.find("config") == config_json.end()) {
        throw std::runtime_error("config file is empty");
    }

    auto& config = config_json["config"];

    if (config.contains("max_responses") && config["max_responses"].is_number_integer()) {
        return config["max_responses"].get<int>();
    }

    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    const std::string requests_filename = "requests.json";
    if (!std::filesystem::exists(requests_filename)) {
        throw std::runtime_error("requests.json file is missing");
    }

    std::ifstream requests_file(requests_filename);
    if (!requests_file.is_open()) {
        throw std::runtime_error("cannot open requests.json");
    }

    if (requests_file.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("requests file is empty");
    }

    json requests_json;
    requests_file >> requests_json;

    if (!requests_json.contains("requests") || !requests_json["requests"].is_array()) {
        throw std::runtime_error("requests.json missing requests array");
    }

    std::vector<std::string> requests;
    for (const auto& req : requests_json["requests"]) {
        requests.push_back(req.get<std::string>());
    }

    return requests;
}

void ConverterJSON::PutAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    const std::string answers_filename = "answers.json";

    std::ofstream answers_file(answers_filename, std::ios::trunc);
    if (!answers_file.is_open()) {
        throw std::runtime_error("cannot open answers.json for writing");
    }

    json answers_json;
    json answers_obj;

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string request_id = "request" + std::string(i < 9 ? "00" : i < 99 ? "0" : "") + std::to_string(i + 1);

        if (answers[i].empty()) {
            answers_obj[request_id]["result"] = "false";
        } else {
            answers_obj[request_id]["result"] = "true";
            if (answers[i].size() == 1) {
                answers_obj[request_id]["docid"] = answers[i][0].first;
                answers_obj[request_id]["rank"] = answers[i][0].second;
            } else {
                json relevance = json::array();
                for (const auto& [docid, rank] : answers[i]) {
                    relevance.push_back({ {"docid", docid}, {"rank", rank} });
                }
                answers_obj[request_id]["relevance"] = relevance;
            }
        }
    }

    answers_json["answers"] = answers_obj;

    answers_file << answers_json.dump(4);
}