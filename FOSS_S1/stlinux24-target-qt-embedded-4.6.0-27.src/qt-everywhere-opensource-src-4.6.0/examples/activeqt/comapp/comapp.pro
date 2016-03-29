TEMPLATE = app
CONFIG  += qaxserver

# Input
SOURCES += main.cpp

RC_FILE  = comapp.rc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/comapp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS comapp.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/comapp
INSTALLS += target sources
