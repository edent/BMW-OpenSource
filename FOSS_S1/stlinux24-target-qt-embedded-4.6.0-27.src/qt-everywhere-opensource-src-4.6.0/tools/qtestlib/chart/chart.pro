HEADERS += $$PWD/database.h  $$PWD/reportgenerator.h 
SOURCES += $$PWD/database.cpp  $$PWD/reportgenerator.cpp 
SOURCES += main.cpp
RESOURCES = $$PWD/chart.qrc

QT += sql xml
CONFIG += console
CONFIG -= app_bundle


TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
TARGET = chart


