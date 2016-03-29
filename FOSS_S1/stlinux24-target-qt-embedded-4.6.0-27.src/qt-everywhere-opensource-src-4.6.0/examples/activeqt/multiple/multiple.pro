TEMPLATE = lib
TARGET	 = multipleax

CONFIG	+= qt warn_off qaxserver dll
contains(CONFIG, static):DEFINES += QT_NODLL

SOURCES	 = main.cpp
HEADERS	 = ax1.h ax2.h
RC_FILE  = multipleax.rc
DEF_FILE = $$QT_SOURCE_TREE/src/activeqt/control/qaxserver.def

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/multiple
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS multiple.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/multiple
INSTALLS += target sources
