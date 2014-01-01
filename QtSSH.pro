#-------------------------------------------------
#
# Project created by QtCreator 2013-10-06T22:31:13
#
#-------------------------------------------------

QT       += core gui sql network

#INCLUDEPATH += "/usr/local/include"
LIBS += -lssh2

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSSH
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    xzconfig.cpp \
    xzdatebase.cpp \
    xzhostinfo.cpp \
    ssh2.cpp \
    xzseqdock.cpp \
    xzprocessexecute.cpp \
    xzssh.cpp

HEADERS  += mainwindow.h \
    xzconfig.h \
    xzdatebase.h \
    xzhostinfo.h \
    ssh2.h \
    xzseqdock.h \
    xzprocessexecute.h \
    xzssh.h

FORMS    += mainwindow.ui

RESOURCES +=

OTHER_FILES += \
    design.txt
