HEADERS       = audioinput.h
SOURCES       = audioinput.cpp \
                main.cpp

QT           += multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audioinput
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS audioinput.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audioinput
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7BF
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
