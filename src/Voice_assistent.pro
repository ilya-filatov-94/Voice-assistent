#-------------------------------------------------
#
# Project created by QtCreator 2023-02-24T14:58:45
#
#-------------------------------------------------

QT       += core gui multimedia network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Voice_assistent
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/Qt-Secret/Qt-Secret.pri)

CONFIG += c++17

SOURCES += \
        audiorecorder.cpp \
        commandsexec.cpp \
        datamapper.cpp \
        geocoderyandexapi.cpp \
        main.cpp \
        mainwindow.cpp \
        sliderbutton.cpp \
        vkasrspeechrecognition.cpp \
        yandexspeechrecognition.cpp

HEADERS += \
        audioprobedevice.h \
        audiorecorder.h \
        commandsexec.h \
        datamapper.h \
        geocoderyandexapi.h \
        mainwindow.h \
        sliderbutton.h \
        vkasrspeechrecognition.h \
        yandexspeechrecognition.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

RC_ICONS = "microphone_new.ico"
