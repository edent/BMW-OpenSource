TARGET   = cetcpsync
DESTDIR = ../../../../bin
CONFIG   += console
CONFIG   -= app_bundle
QT       += network
QT       -= gui
TEMPLATE = app

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

INCLUDEPATH += ../cetcpsyncserver

SOURCES += main.cpp \
           remoteconnection.cpp \
           qtcesterconnection.cpp

HEADERS += \
           remoteconnection.h \
           qtcesterconnection.h
