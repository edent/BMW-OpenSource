TEMPLATE = lib
TARGET	 = hierarchyax

CONFIG	+= qt warn_off qaxserver dll
contains(CONFIG, static):DEFINES += QT_NODLL

SOURCES	 = objects.cpp main.cpp
HEADERS	 = objects.h
RC_FILE	 = $$QT_SOURCE_TREE/src/activeqt/control/qaxserver.rc
DEF_FILE = $$QT_SOURCE_TREE/src/activeqt/control/qaxserver.def

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/hierarchy
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS hierarchy.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/hierarchy
INSTALLS += target sources
