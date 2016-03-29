TEMPLATE = app
TARGET = cetcpsyncsvr
DEPENDPATH += .
QT -= gui
QT += network

CONFIG += console

HEADERS +=  \
            connectionmanager.h \
            commands.h \
            transfer_global.h

SOURCES +=  \
            connectionmanager.cpp \
            commands.cpp \
            main.cpp
