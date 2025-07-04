#ifndef TESTSEARCHENGINE_H
#define TESTSEARCHENGINE_H

#include <QtTest/QtTest>
#include "converterjson.h"
#include "invertedindex.h"
#include "searchserver.h"

class TestSearchEngine : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testLoadConfig();
    void testLoadRequests();

    void testIndexing();
    void testLookup();

    void testSearchSingleQuery();
    void testSearchMultipleQueries();
};

#endif // TESTSEARCHENGINE_H
