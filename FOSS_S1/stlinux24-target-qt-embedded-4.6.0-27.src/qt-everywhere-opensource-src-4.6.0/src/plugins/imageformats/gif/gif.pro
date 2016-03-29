TARGET  = qgif
include(../../qpluginbase.pri)

HEADERS += qgifhandler.h
SOURCES += main.cpp \
           qgifhandler.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

symbian:TARGET.UID3=0x2001E61A
