TEMPLATE  = subdirs
SUBDIRS   = styledemo raycasting flickable digiflip

contains(QT_CONFIG, svg) {
    SUBDIRS += embeddedsvgviewer \
               desktopservices
    !vxworks:!qnx:SUBDIRS += fluidlauncher
}

SUBDIRS += lightmaps
SUBDIRS += flightinfo
contains(QT_CONFIG, svg) {
    SUBDIRS += weatherinfo
}

contains(QT_CONFIG, webkit) {
    SUBDIRS += anomaly
}

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
