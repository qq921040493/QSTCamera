#-------------------------------------------------
#
# Project created by QtCreator 2020-05-08T12:37:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSTcamera
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    v4l2.cpp

HEADERS  += mainwindow.h \
    v4l2.h

FORMS    += mainwindow.ui
