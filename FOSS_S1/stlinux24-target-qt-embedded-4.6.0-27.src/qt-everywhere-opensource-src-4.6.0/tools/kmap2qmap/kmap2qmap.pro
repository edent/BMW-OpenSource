
TEMPLATE = app
DESTDIR = ../../bin
QT = core
CONFIG += console
CONFIG -= app_bundle

DEPENDPATH += $$QT_SOURCE_TREE/src/gui/embedded
INCLUDEPATH += $$QT_SOURCE_TREE/src/gui/embedded

# Input
SOURCES += main.cpp
