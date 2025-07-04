#include "converterjson.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTextStream>
#include <QDir>

static QByteArray readFile(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << QString::fromStdString(path);
        return QByteArray();
    }
    return file.readAll();
}

bool ConverterJson::loadConfig(const std::string& filepath, ConfigData& config, const QString& basePath)
{
    m_basePath = basePath;

    QFile file(QString::fromStdString(filepath));
    if (!file.exists()) {
        qCritical() << "config file is missing";
        return false;
    }
    QByteArray data = readFile(filepath);
    if (data.isEmpty()) {
        qCritical() << "config file is empty";
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) return false;

    if (!doc.isObject()) return false;

    QJsonObject rootObj = doc.object();

    if (!rootObj.contains("config") || !rootObj["config"].isObject()) {
        qCritical() << "config file is empty";
        return false;
    }

    QJsonObject configObj = rootObj["config"].toObject();
    config.name = configObj.value("name").toString().toStdString();
    config.version = configObj.value("version").toString().toStdString();
    config.maxResponses = configObj.value("max_responses").toInt(5);

    std::string app_version = "1.0";
    if (config.version != app_version) {
        qCritical() << "config.json has incorrect file version";
        return false;
    }

    if (!rootObj.contains("files") || !rootObj["files"].isArray()) return false;

    QJsonArray filesArray = rootObj["files"].toArray();
    config.files.clear();
    for (const QJsonValue& val : filesArray) {
        if (val.isString()) {
            config.files.push_back(val.toString().toStdString());
        }
    }

    m_config = config;
    m_textDocuments.clear();

    for (const auto& relPath : config.files) {
        QString absPath = QDir(m_basePath).filePath(QString::fromStdString(relPath));
        QFile docFile(absPath);
        if (!docFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open text file:" << absPath;
            m_textDocuments.push_back("");
            continue;
        }
        QTextStream in(&docFile);
        QString content = in.readAll();
        m_textDocuments.push_back(content.toStdString());
        docFile.close();
    }

    return true;
}

bool ConverterJson::loadRequests(const std::string& filepath, std::vector<std::string>& requests)
{
    QByteArray data = readFile(filepath);
    if (data.isEmpty()) return false;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) return false;

    if (!doc.isObject()) return false;

    QJsonObject rootObj = doc.object();

    if (!rootObj.contains("requests") || !rootObj["requests"].isArray()) return false;

    QJsonArray requestsArray = rootObj["requests"].toArray();
    requests.clear();
    for (const QJsonValue& val : requestsArray) {
        if (val.isString()) {
            requests.push_back(val.toString().toStdString());
        }
    }
    m_requests = requests;
    return true;
}

std::vector<std::string> ConverterJson::GetTextDocuments()
{
    return m_textDocuments;
}

int ConverterJson::GetResponsesLimit()
{
    return m_config.maxResponses;
}

std::vector<std::string> ConverterJson::GetRequests()
{
    return m_requests;
}

void ConverterJson::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers)
{
    QJsonObject answersObj;

    for (size_t i = 0; i < answers.size(); ++i) {
        QString key = QString("request%1").arg(i + 1, 3, 10, QChar('0'));
        QJsonObject resultObj;

        if (answers[i].empty()) {
            resultObj["result"] = "false";
        } else {
            resultObj["result"] = "true";
            QJsonArray relevanceArray;
            for (const auto& pair : answers[i]) {
                QJsonObject docObj;
                docObj["docid"] = pair.first;
                docObj["rank"] = QJsonValue::fromVariant(pair.second);
                relevanceArray.append(docObj);
            }
            resultObj["relevance"] = relevanceArray;
        }

        answersObj[key] = resultObj;
    }

    QJsonObject rootObj;
    rootObj["answers"] = answersObj;

    QJsonDocument doc(rootObj);

    QString answersPath = QDir(m_basePath).filePath("answers.json");
    QFile file(answersPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open answers file for writing";
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}
