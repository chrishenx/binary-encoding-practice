#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T09:48:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = binary-encoding
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    binaryencoder.cpp

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    binaryencoder.h

FORMS    += mainwindow.ui
