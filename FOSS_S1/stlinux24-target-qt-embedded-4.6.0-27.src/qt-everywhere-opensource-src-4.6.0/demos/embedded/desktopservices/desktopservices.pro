TEMPLATE = app
TARGET =
INCLUDEPATH += .

HEADERS += desktopwidget.h contenttab.h linktab.h
SOURCES += desktopwidget.cpp contenttab.cpp linktab.cpp main.cpp

RESOURCES += desktopservices.qrc

music.sources = data/*.mp3 data/*.wav
image.sources = data/*.png

target.path = $$[QT_INSTALL_DEMOS]/embedded/desktopservices
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded/desktopservices

symbian {
    TARGET.UID3 = 0xA000C611
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    ICON = ./resources/heart.svg
    music.path = /data/sounds/
    image.path = /data/images/
    DEPLOYMENT += music image
}

wince*{
    music.path = "\My Documents\My Music"
    image.path = "\My Documents\My Pictures"
    DEPLOYMENT += music image
}

INSTALLS += target sources
