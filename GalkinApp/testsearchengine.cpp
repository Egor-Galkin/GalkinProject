#include "testsearchengine.h"
#include <QDir>
#include <QDebug>

void TestSearchEngine::initTestCase()
{
    qDebug() << "Current working directory:" << QDir::currentPath();
}

void TestSearchEngine::cleanupTestCase()
{
}

void TestSearchEngine::testLoadConfig()
{
    ConverterJson converter;
    ConfigData config;

    QString projectRoot = QDir::currentPath() + "/../../../";
    QString configPath = QDir(projectRoot).filePath("config.json");
    qDebug() << "Config path:" << configPath;

    bool ok = converter.loadConfig(configPath.toStdString(), config, projectRoot);
    QVERIFY(ok);
    QCOMPARE(QString::fromStdString(config.name), QString("GalkinSearchEngine"));
    QCOMPARE(config.maxResponses, 5);
    QVERIFY(!config.files.empty());
}

void TestSearchEngine::testLoadRequests()
{
    ConverterJson converter;
    std::vector<std::string> requests;

    QString projectRoot = QDir::currentPath() + "/../../../";
    QString requestsPath = QDir(projectRoot).filePath("requests.json");
    qDebug() << "Requests path:" << requestsPath;

    bool ok = converter.loadRequests(requestsPath.toStdString(), requests);
    QVERIFY(ok);
    QVERIFY(!requests.empty());
    QCOMPARE(QString::fromStdString(requests[0]), QString("indexing algorithms"));
}

void TestSearchEngine::testIndexing()
{
    InvertedIndex index;
    std::vector<std::string> docs = {
        "apple banana apple",
        "banana orange",
        "orange apple"
    };
    index.UpdateDocumentBase(docs);

    QCOMPARE(index.GetWordCount("apple").size(), 2);
    QCOMPARE(index.GetWordCount("banana").size(), 2);
    QCOMPARE(index.GetWordCount("orange").size(), 2);

    auto entries = index.GetWordCount("apple");
    bool foundDoc0 = false;
    for (const auto& e : entries) {
        if (e.doc_id == 0) {
            QCOMPARE(e.count, size_t(2));
            foundDoc0 = true;
        }
    }
    QVERIFY(foundDoc0);
}

void TestSearchEngine::testLookup()
{
    InvertedIndex index;
    std::vector<std::string> docs = { "test document" };
    index.UpdateDocumentBase(docs);

    auto entries = index.GetWordCount("missingword");
    QVERIFY(entries.empty());
}

void TestSearchEngine::testSearchSingleQuery()
{
    std::vector<std::string> docs = {
        "apple banana",
        "banana orange",
        "orange apple"
    };
    InvertedIndex index;
    index.UpdateDocumentBase(docs);

    SearchServer server(index, 5);

    std::vector<std::vector<RelativeIndex>> results = server.search({ "apple banana" });
    QVERIFY(!results.empty());
    QVERIFY(!results[0].empty());

    for (const auto& res : results[0]) {
        QVERIFY(res.doc_id == 0);
    }
}

void TestSearchEngine::testSearchMultipleQueries()
{
    std::vector<std::string> docs = {
        "apple banana",
        "banana orange",
        "orange apple"
    };
    InvertedIndex index;
    index.UpdateDocumentBase(docs);

    SearchServer server(index, 5);

    std::vector<std::string> queries = { "apple banana", "banana orange" };
    std::vector<std::vector<RelativeIndex>> results = server.search(queries);

    QCOMPARE(results.size(), size_t(2));
    QVERIFY(!results[0].empty());
    QVERIFY(!results[1].empty());
}
