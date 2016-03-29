include(../s60pluginbase.pri)

TARGET  = qts60plugin_3_2

contains(S60_VERSION, 3.1) {
    SOURCES += ../src/qlocale_3_1.cpp \
        ../src/qdesktopservices_3_1.cpp \
        ../src/qcoreapplication_3_1.cpp
} else {
    SOURCES += ../src/qlocale_3_2.cpp \
        ../src/qdesktopservices_3_2.cpp \
        ../src/qcoreapplication_3_2.cpp
    LIBS += -ldirectorylocalizer -lefsrv
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

TARGET.UID3=0x2001E621
