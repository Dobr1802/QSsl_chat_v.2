#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T14:05:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++11

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sslserver.cpp \
    userscertificates.cpp

HEADERS  += mainwindow.h \
    sslserver.h \
    userscertificates.h

FORMS    += mainwindow.ui
