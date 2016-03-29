TEMPLATE      = subdirs
CONFIG       += ordered
SUBDIRS       = qmusicplayer \
                capabilities

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS phonon.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
