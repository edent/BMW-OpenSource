TEMPLATE = app
QT += script
# Input
HEADERS += qcontext2dcanvas.h \
	   context2d.h \
	   domimage.h \
	   environment.h \
	   window.h
SOURCES += qcontext2dcanvas.cpp \
	   context2d.cpp \
	   domimage.cpp \
	   environment.cpp \
	   window.cpp \
	   main.cpp
RESOURCES += context2d.qrc

contains(QT_CONFIG, scripttools): QT += scripttools

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/context2d
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS context2d.pro scripts
sources.path = $$[QT_INSTALL_EXAMPLES]/script/context2d
INSTALLS += target sources

symbian:{
    TARGET.UID3 = 0xA000C608
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCHEAPSIZE = 0x200000 0xA00000
    contextScripts.path = .
    contextScripts.sources = scripts
    DEPLOYMENT += contextScripts
}
