QT          += xmlpatterns
FORMS       += schema.ui
HEADERS     = mainwindow.h ../shared/xmlsyntaxhighlighter.h
RESOURCES   = schema.qrc
SOURCES     = main.cpp mainwindow.cpp ../shared/xmlsyntaxhighlighter.cpp
INCLUDEPATH += ../shared/

target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/schema
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.xq *.html files
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/schema
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7C6
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
