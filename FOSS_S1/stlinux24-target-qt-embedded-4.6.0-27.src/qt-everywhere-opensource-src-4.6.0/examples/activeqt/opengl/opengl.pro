TEMPLATE = app
TARGET	 = openglax

CONFIG	+= qt warn_off qaxserver

QT += opengl

HEADERS	 = glbox.h \
	   globjwin.h
SOURCES	 = glbox.cpp \
	   globjwin.cpp \
	   main.cpp
RC_FILE	 = $$QT_SOURCE_TREE/src/activeqt/control/qaxserver.rc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/opengl
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS opengl.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/opengl
INSTALLS += target sources
