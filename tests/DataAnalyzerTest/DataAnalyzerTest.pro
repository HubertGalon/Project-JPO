QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    ../../dataanalyzer.cpp \
    ../../measurement.cpp \
    DataAnalyzerTest.cpp

HEADERS += \
    ../../dataanalyzer.h \
    ../../measurement.h

INCLUDEPATH += ..
