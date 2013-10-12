#-------------------------------------------------
#
# Project created by QtCreator 2013-10-06T22:31:13
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSSH
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    xzconfig.cpp \
    xzdatebase.cpp \
    xzhostinfo.cpp

HEADERS  += mainwindow.h \
    xzconfig.h \
    xzdatebase.h \
    xzhostinfo.h

FORMS    += mainwindow.ui
