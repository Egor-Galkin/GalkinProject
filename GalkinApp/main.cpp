#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include "converterjson.h"
#include "invertedindex.h"
#include "searchserver.h"
#include <vector>
#include <string>
#include <QtConcurrent/QtConcurrent>

std::vector<std::vector<std::pair<int, float>>> ConvertAnswers(const std::vector<std::vector<RelativeIndex>>& input) {
    std::vector<std::vector<std::pair<int, float>>> result;
    for (const auto& vec : input) {
        std::vector<std::pair<int, float>> inner;
        for (const auto& rel : vec) {
            inner.emplace_back(static_cast<int>(rel.doc_id), rel.rank);
        }
        result.push_back(std::move(inner));
    }
    return result;
}

std::string readFileContent(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open text file:" << filePath;
        return "";
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content.toStdString();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConverterJson converter;
    ConfigData config;

    QString currentDir = QDir::currentPath();
    QDir dir(currentDir);
    QString projectRoot = dir.absoluteFilePath("../../");
    QFileInfo fi(projectRoot);
    QString canonicalProjectRoot = fi.canonicalFilePath();

    if (canonicalProjectRoot.isEmpty()) {
        qWarning() << "Failed to resolve canonical path for project root:" << projectRoot;
        canonicalProjectRoot = projectRoot;
    }

    QString configPath = QDir(canonicalProjectRoot).filePath("config.json");
    if (!converter.loadConfig(configPath.toStdString(), config, canonicalProjectRoot)) {
        qCritical() << "Failed to load config.json at" << configPath;
        return -1;
    }

    QString requestsPath = QDir(canonicalProjectRoot).filePath("requests.json");
    std::vector<std::string> requests_std;
    if (!converter.loadRequests(requestsPath.toStdString(), requests_std)) {
        qCritical() << "Failed to load requests.json at" << requestsPath;
        return -1;
    }

    qDebug() << "Search engine:" << QString::fromStdString(config.name);
    qDebug() << "Version:" << QString::fromStdString(config.version);

    InvertedIndex index;
    index.UpdateDocumentBase(converter.GetTextDocuments());

    SearchServer server(index, config.maxResponses);

    auto answers_std = server.search(requests_std);

    auto answers_for_save = ConvertAnswers(answers_std);

    converter.putAnswers(answers_for_save);

    const auto& texts_std = converter.GetTextDocuments();
    for (size_t i = 0; i < texts_std.size(); ++i) {
        qDebug() << "Doc" << static_cast<int>(i + 1) << ":" << QString::fromStdString(texts_std[i]).left(100) << "...";
    }

    QTimer::singleShot(0, &a, &QCoreApplication::quit);
    return a.exec();
}
