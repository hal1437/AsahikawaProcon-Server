#-------------------------------------------------
#
# Project created by QtCreator 2015-03-20T02:08:40
#
#-------------------------------------------------

QT       += core gui
QT       += network
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsahikawaProcon-Server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    startupdialog.cpp \
    GameSystem.cpp \
    TcpClient.cpp \
    BaseClient.cpp \
    ComClient.cpp

HEADERS  += mainwindow.h \
    startupdialog.h \
    GameSystem.h \
    TcpClient.h \
    BaseClient.h \
    ComClient.h

FORMS    += mainwindow.ui \
    startupdialog.ui
