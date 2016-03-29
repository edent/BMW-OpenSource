QT      +=  webkit network
HEADERS =   previewer.h \
            mainwindow.h
SOURCES =   main.cpp \
            previewer.cpp \
            mainwindow.cpp
FORMS   =   previewer.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/previewer
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/previewer
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000CF6F
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
