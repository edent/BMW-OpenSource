TEMPLATE      = subdirs
SUBDIRS       = simpletextviewer

# install
target.path = $$[QT_INSTALL_EXAMPLES]/assistant
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS assistant.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/assistant
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
