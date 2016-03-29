HEADERS += rsslisting.h
SOURCES += main.cpp rsslisting.cpp
QT += network xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml/rsslisting
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS rsslisting.pro 
sources.path = $$[QT_INSTALL_EXAMPLES]/xml/rsslisting
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

