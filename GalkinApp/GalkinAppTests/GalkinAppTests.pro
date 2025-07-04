QT = core testlib concurrent

CONFIG += c++17 console testcase

SOURCES += \
    testmain.cpp \
    ../testsearchengine.cpp \
    ../converterjson.cpp \
    ../invertedindex.cpp \
    ../searchserver.cpp

HEADERS += \
    ../testsearchengine.h \
    ../converterjson.h \
    ../invertedindex.h \
    ../searchserver.h

RESOURCES += ../resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
