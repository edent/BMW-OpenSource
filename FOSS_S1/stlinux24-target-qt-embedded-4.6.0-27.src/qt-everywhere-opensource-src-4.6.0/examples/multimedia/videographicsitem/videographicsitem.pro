QT += multimedia

contains(QT_CONFIG, opengl): QT += opengl

HEADERS   += videoplayer.h \
             videoitem.h

SOURCES   += main.cpp \
             videoplayer.cpp \
             videoitem.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/videographicsitem
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/videographicsitem
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7C2
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
