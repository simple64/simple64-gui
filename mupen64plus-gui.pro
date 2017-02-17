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
    osal/osal_dynamiclib_unix.c \
    osal/osal_files_unix.c \
    libco/libco.c \
    vidext.cpp \
    oglwindow.cpp \
    interface/common.c \
    interface/core_interface.c \
    interface/plugin.c \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    vidext.h \
    interface/common.h \
    oglwindow.h \
    interface/core_interface.h \
    interface/plugin.h \
    settingsdialog.h

FORMS    += mainwindow.ui

QMAKE_INCDIR += api osal interface

QMAKE_LFLAGS += -ldl
