#include <QCoreApplication>
#include "../testsearchengine.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestSearchEngine test;
    return QTest::qExec(&test, argc, argv);
}
