#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <string>
#include <vector>
#include <QString>

struct ConfigData {
    std::string name;
    std::string version;
    int maxResponses = 5;
    std::vector<std::string> files;
};

class ConverterJson {
public:
    ConverterJson() = default;

    bool loadConfig(const std::string& filepath, ConfigData& config, const QString& basePath);
    bool loadRequests(const std::string& filepath, std::vector<std::string>& requests);

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();

    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    ConfigData m_config;
    std::vector<std::string> m_textDocuments;
    std::vector<std::string> m_requests;
    QString m_basePath;
};

#endif // CONVERTERJSON_H
