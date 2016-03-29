TARGET     = QtScript
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += JSC=QTJSC jscyyparse=qtjscyyparse jscyylex=qtjscyylex jscyyerror=qtjscyyerror
DEFINES   += QT_BUILD_SCRIPT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
DEFINES   += QLALR_NO_QSCRIPTGRAMMAR_DEBUG_INFO
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000       ### FIXME

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)

CONFIG += building-libs

# FIXME: shared the statically built JavaScriptCore

# Fetch the base WebKit directory from the WEBKITDIR environment variable;
# fall back to src/3rdparty otherwise
WEBKITDIR = $$(WEBKITDIR)
isEmpty(WEBKITDIR) {
    WEBKITDIR = $$PWD/../3rdparty/javascriptcore
    GENERATED_SOURCES_DIR = generated
} else {
    message(using external WebKit from $$WEBKITDIR)
    CONFIG -= QTDIR_build
}
include($$WEBKITDIR/WebKit.pri)

# Disable a few warnings on Windows.
# These are in addition to the ones disabled in WebKit.pri
win32-msvc*: QMAKE_CXXFLAGS += -wd4396 -wd4099

# Windows CE-specific stuff copied from WebCore.pro
# ### Should rather be in JavaScriptCore.pri?
wince* {
    INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/os-wince
    INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/os-win32
    LIBS += -lmmtimer
}

include($$WEBKITDIR/JavaScriptCore/JavaScriptCore.pri)

INCLUDEPATH += $$WEBKITDIR/JavaScriptCore
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/parser
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/bytecompiler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/debugger
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/runtime
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/wtf
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/unicode
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/interpreter
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/jit
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/profiler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/wrec
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/API
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/bytecode
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/assembler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/generated

# This line copied from WebCore.pro
DEFINES += WTF_USE_JAVASCRIPTCORE_BINDINGS=1 WTF_CHANGES=1

DEFINES += NDEBUG

solaris-g++:isEqual(QT_ARCH,sparc) {
    CONFIG -= separate_debug_info
    CONFIG += no_debug_info
}

# Avoid JSC C API functions being exported.
DEFINES += JS_EXPORT="" JS_EXPORTDATA=""

INCLUDEPATH += $$PWD

include(script.pri)

symbian:TARGET.UID3=0x2001B2E1
