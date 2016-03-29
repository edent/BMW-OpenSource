TARGET = qtracegraphicssystem
include(../../qpluginbase.pri)

QT += network

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_trace.cpp

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
INCLUDEPATH += ../../../3rdparty/harfbuzz/src
