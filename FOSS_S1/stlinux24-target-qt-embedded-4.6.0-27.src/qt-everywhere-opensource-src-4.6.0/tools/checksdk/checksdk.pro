TEMPLATE = app
DESTDIR = ../../bin
TARGET = checksdk
DEPENDPATH += .
INCLUDEPATH += .
QT = 
CONFIG += console

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

DEFINES        += QT_BOOTSTRAPPED QT_NO_CODECS QT_LITE_UNICODE QT_NO_LIBRARY \
                  QT_NO_STL QT_NO_COMPRESS QT_NO_DATASTREAM  \
                  QT_NO_TEXTCODEC QT_NO_UNICODETABLES QT_NO_THREAD \
                  QT_NO_SYSTEMLOCALE QT_NO_GEOM_VARIANT \
                  QT_NODLL QT_NO_QOBJECT 
                  
INCLUDEPATH = \
              $$QT_BUILD_TREE/src/corelib/arch \
              $$QT_BUILD_TREE/include \
              $$QT_BUILD_TREE/include/QtCore

DEPENDPATH += $$INCLUDEPATH $$QT_BUILD_TREE/src/corelib/base $$QT_BUILD_TREE/src/corelib/tools $$QT_BUILD_TREE/src/corelib/io

# Input
SOURCES += \
           main.cpp \
           cesdkhandler.cpp

HEADERS += \
           cesdkhandler.h

include(../../src/tools/bootstrap/bootstrap.pri)

