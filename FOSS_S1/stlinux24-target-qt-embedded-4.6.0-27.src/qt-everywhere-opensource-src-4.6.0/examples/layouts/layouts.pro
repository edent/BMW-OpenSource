TEMPLATE      = subdirs
SUBDIRS       = basiclayouts \
                borderlayout \
                dynamiclayouts \
                flowlayout

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/layouts
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
