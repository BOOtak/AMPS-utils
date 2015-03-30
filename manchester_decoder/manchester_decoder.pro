#-------------------------------------------------
#
# Project created by QtCreator 2015-03-25T21:43:55
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

TARGET = manchester_decoder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wavfile.cpp \
    utils.cpp \
    decoder.cpp

HEADERS  += mainwindow.h \
    wavfile.h \
    utils.h \
    decoder.h

FORMS    += mainwindow.ui
