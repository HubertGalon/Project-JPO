QT += testlib
QT += network

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    ../../databasehandler.cpp \
    ../../station.cpp \
    ../../sensor.cpp \
    ../../measurement.cpp \
    DatabaseHandlerTest.cpp

HEADERS += \
    ../../databasehandler.h \
    ../../station.h \
    ../../sensor.h \
    ../../measurement.h

INCLUDEPATH += ..
