QT += core gui widgets

CONFIG += c++17

TARGET = ejercicio-8
TEMPLATE = app

SOURCES += \
    appcontroller.cpp \
    codeeditor.cpp \
    config.cpp \
    editorprincipal.cpp \
    logger.cpp \
    login.cpp \
    main.cpp \
    modobloqueado.cpp \
    validadorsintaxis.cpp

HEADERS += \
    appcontroller.h \
    codeeditor.h \
    config.h \
    editorprincipal.h \
    logger.h \
    login.h \
    modobloqueado.h \
    pantalla.h \
    validadorsintaxis.h

RESOURCES += resources.qrc

DISTFILES += config.ini
