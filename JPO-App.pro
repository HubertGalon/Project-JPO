QT       += core gui network charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    airqualityindex.cpp \
    apiclient.cpp \
    chartwidget.cpp \
    dataanalyzer.cpp \
    databasehandler.cpp \
    datamanager.cpp \
    geocoder.cpp \
    main.cpp \
    mainwindow.cpp \
    measurement.cpp \
    networkchecker.cpp \
    sensor.cpp \
    sensorlistwidget.cpp \
    station.cpp \
    stationlistwidget.cpp \

HEADERS += \
    airqualityindex.h \
    apiclient.h \
    chartwidget.h \
    dataanalyzer.h \
    databasehandler.h \
    datamanager.h \
    geocoder.h \
    mainwindow.h \
    measurement.h \
    networkchecker.h \
    sensor.h \
    sensorlistwidget.h \
    station.h \
    stationlistwidget.h

FORMS += \
    chartwidget.ui \
    mainwindow.ui \
    sensorlistwidget.ui \
    stationlistwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
