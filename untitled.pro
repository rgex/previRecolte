#-------------------------------------------------
#
# Project created by QtCreator 2014-07-15T11:37:14
#
#-------------------------------------------------

QT       += core gui sql
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
    meteodatabaseinterface.cpp \
    editavgmeteoform.cpp \
    edityearmeteoform.cpp \
    editmeteodataform.cpp \
    helpform.cpp

HEADERS  += mainwindow.h \
    varietesananas.h \
    varietedatabaseinterface.h \
    importcsv.h \
    sitesdatabaseinterface.h \
    htmlchartmaker.h \
    site.h \
    meteo.h \
    meteodatabaseinterface.h \
    editavgmeteoform.h \
    edityearmeteoform.h \
    editmeteodataform.h \
    helpform.h

FORMS    += mainwindow.ui \
    editavgmeteoform.ui \
    edityearmeteoform.ui \
    editmeteodataform.ui \
    helpform.ui

unix: CONFIG += link_pkgconfig

unix: LIBS += -lsqlite3

RESOURCES += \
    projectRessources.qrc

