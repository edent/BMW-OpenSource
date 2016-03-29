HEADERS       = ftpwindow.h
SOURCES       = ftpwindow.cpp \
                main.cpp
RESOURCES    += ftp.qrc
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/qftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/network/qftp
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000A648
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    HEADERS +=  sym_iap_util.h
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY="NetworkServices ReadUserData WriteUserData"
    LIBS+=-lesock -lcommdb -linsock # For IAP selection
}
