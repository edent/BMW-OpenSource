# Additional Qt project file for qtmain lib on Symbian
TEMPLATE = lib
TARGET	 = qtmain
DESTDIR	 = $$QMAKE_LIBDIR_QT
QT       =

CONFIG	+= staticlib warn_on
CONFIG	-= qt shared

symbian {
    # Note: UID only needed for ensuring that no filename generation conflicts occur
    TARGET.UID3 = 0x2001E61F
    CONFIG      +=  png zlib
    CONFIG	-=  jpeg
    INCLUDEPATH	+=  tmp $$QMAKE_INCDIR_QT/QtCore $$MW_LAYER_SYSTEMINCLUDE
    SOURCES	 =  qts60main.cpp \
                    qts60main_mcrt0.cpp

    # s60main needs to be built in ARM mode for GCCE to work.
    MMP_RULES+="ALWAYS_BUILD_AS_ARM"

    # staticlib should not have any lib depencies in s60
    # This seems not to work, some hard coded libs are still added as dependency
    LIBS =
} else {
    error("$$_FILE_ is intended only for Symbian!")
}

include(../qbase.pri)
