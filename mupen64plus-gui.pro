#-------------------------------------------------
#
# Project created by QtCreator 2017-02-16T19:17:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mupen64plus-gui
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    vidext.cpp \
    settingsdialog.cpp \
    interface/common.cpp \
    interface/core_interface.cpp \
    interface/plugin.cpp \
    plugindialog.cpp \
    oglwindow.cpp \
    workerthread.cpp \
    settingclasses.cpp \
    cheatdialog.cpp \
    interface/cheat.cpp \
    controllerdialog.cpp \
    keyselect.cpp \
    interface/sdl_key_converter.c

win32 {
SOURCES += osal/osal_dynamiclib_win32.c \
    osal/osal_files_win32.c

DEFINES -= UNICODE

LIBS += -Wl,-Bdynamic -lSDL2
}

!win32 {
SOURCES += osal/osal_dynamiclib_unix.c \
    osal/osal_files_unix.c

LIBS += -ldl -lSDL2
}

HEADERS  += mainwindow.h \
    vidext.h \
    interface/common.h \
    interface/core_interface.h \
    interface/plugin.h \
    settingsdialog.h \
    osal/osal_dynamiclib.h \
    workerthread.h \
    plugindialog.h \
    oglwindow.h \
    settingclasses.h \
    cheatdialog.h \
    interface/cheat.h \
    controllerdialog.h \
    keyselect.h \
    interface/sdl_key_converter.h

FORMS    += mainwindow.ui

QMAKE_INCDIR += api osal interface

CONFIG += c++11

QMAKE_PROJECT_DEPTH = 0
