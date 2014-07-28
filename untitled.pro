#-------------------------------------------------
#
# Project created by QtCreator 2014-07-15T11:37:14
#
#-------------------------------------------------

QT       += core gui
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    varietesananas.cpp \
    varietedatabaseinterface.cpp \
    importcsv.cpp \
    sitesdatabaseinterface.cpp \
    htmlchartmaker.cpp \
    site.cpp \
    meteo.cpp \
    meteodatabaseinterface.cpp

HEADERS  += mainwindow.h \
    varietesananas.h \
    varietedatabaseinterface.h \
    importcsv.h \
    sitesdatabaseinterface.h \
    htmlchartmaker.h \
    site.h \
    meteo.h \
    meteodatabaseinterface.h

FORMS    += mainwindow.ui

unix: CONFIG += link_pkgconfig


unix: LIBS += -lleveldb


unix: LIBS += -lboost_serialization
