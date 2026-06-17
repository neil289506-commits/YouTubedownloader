QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YTDownloader
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    downloadworker.cpp

HEADERS += \
    mainwindow.h \
    downloadworker.h

RESOURCES += resources.qrc

RC_ICONS = app.ico
