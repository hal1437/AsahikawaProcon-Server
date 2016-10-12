#-------------------------------------------------
#
# Project created by QtCreator 2015-03-20T02:08:40
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
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
    ComClient.cpp \
    GameBoard.cpp \
    ManualClientDialog.cpp \
    ManualClient.cpp \
    MapEditerDialog.cpp \
    SettingDialog.cpp \
    ClientSettingForm.cpp \
    StableLog.cpp \
    DesignDialog.cpp

HEADERS  += mainwindow.h \
    startupdialog.h \
    GameSystem.h \
    TcpClient.h \
    BaseClient.h \
    ComClient.h \
    GameBoard.h \
    ManualClientDialog.h \
    ManualClient.h \
    MapEditerDialog.h \
    SettingDialog.h \
    ClientSettingForm.h \
    Definition.h \
    StableLog.h \
    DesignDialog.h

FORMS    += mainwindow.ui \
    startupdialog.ui \
    GameBoard.ui \
    ManualClientDialog.ui \
    MapEditerDialog.ui \
    SettingDialog.ui \
    ClientSettingForm.ui \
    DesignDialog.ui

RESOURCES += \
    Resource.qrc

win32:{
    DEFINES += OS_WIN32
    RC_FILE += icons.rc
}
mac:{
    DEFINES += OS_MACOSX
    RESOURCES += Music.qrc
}
