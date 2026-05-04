QT += core gui widgets network

CONFIG += c++17

TARGET = ejercicio4
TEMPLATE = app

SOURCES += \
    apiclient.cpp \
    carddialog.cpp \
    cardwidget.cpp \
    columndialog.cpp \
    columnwidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    apiclient.h \
    carddialog.h \
    cardwidget.h \
    columndialog.h \
    columnwidget.h \
    kanbanmodels.h \
    mainwindow.h
