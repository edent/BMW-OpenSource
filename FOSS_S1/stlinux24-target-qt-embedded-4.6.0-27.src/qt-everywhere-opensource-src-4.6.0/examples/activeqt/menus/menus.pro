TEMPLATE = app
TARGET	 = menusax

CONFIG	+= qt warn_off qaxserver

SOURCES	 = main.cpp menus.cpp
HEADERS	 = menus.h
RC_FILE	 = $$QT_SOURCE_TREE/src/activeqt/control/qaxserver.rc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/menus
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS menus.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/menus
INSTALLS += target sources
