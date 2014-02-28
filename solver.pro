#-------------------------------------------------
#
# Project created by QtCreator 2014-01-10T18:00:28
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = solver
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    connectionwindow.cpp \
    model.cpp \
    matrix.cpp \
    taskwindow.cpp \
    dialogwithgraphicsview.cpp

HEADERS  += mainwindow.h \
    controller.h \
    connectionwindow.h \
    model.h \
    matrix.h \
    taskwindow.h \
    dialogwithgraphicsview.h

FORMS    += mainwindow.ui \
    connectionwindow.ui \
    taskwindow.ui \
    dialogwithgraphicsview.ui
