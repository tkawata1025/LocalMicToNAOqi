#-------------------------------------------------
#
# Project created by QtCreator 2015-03-24T19:05:31
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LocalMicToNAO
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    audioproxy.cpp

HEADERS  += mainwindow.h NAOqi/nao_mic_interface/nao_mic_interface.h \
    audioproxy.h

FORMS    += mainwindow.ui


LIBS += -L"$$PWD/NAOqi/build/lib" -lNaoMicInterface
