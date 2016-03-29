TEMPLATE      = subdirs
SUBDIRS      += comapp \
                hierarchy \
                menus \
                multiple \
                simple \
                webbrowser \
                wrapper

contains(QT_CONFIG, opengl):SUBDIRS += opengl

# For now only the contain examples with mingw, for the others you need
# an IDL compiler
win32-g++|wince*:SUBDIRS = webbrowser

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS activeqt.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt
INSTALLS += target sources
