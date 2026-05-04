QT += core gui widgets network

CONFIG += c++17

TARGET = ejercicio7
TEMPLATE = app

SOURCES += \
    appconfig.cpp \
    appcontroller.cpp \
    clima.cpp \
    logger.cpp \
    login.cpp \
    main.cpp \
    ventana.cpp \
    wallpapermanager.cpp

HEADERS += \
    appconfig.h \
    appcontroller.h \
    clima.h \
    logger.h \
    login.h \
    pantalla.h \
    ventana.h \
    wallpapermanager.h

FORMS += \
    login.ui

RESOURCES += \
    resources.qrc

