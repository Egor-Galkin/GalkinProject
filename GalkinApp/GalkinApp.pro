QT = core testlib concurrent

CONFIG += c++17 console testcase

SOURCES += \
        converterjson.cpp \
        invertedindex.cpp \
        main.cpp \
        searchserver.cpp \
        testsearchengine.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    converterjson.h \
    invertedindex.h \
    searchserver.h \
    testsearchengine.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    config.json \
    requests.json \
    resources/file001.txt \
    resources/file002.txt \
    resources/file003.txt \
    resources/file004.txt
