
FORMS       += forms/mainwindow.ui
QT +=       xmlpatterns webkit
SOURCES     += qobjectxmlmodel.cpp main.cpp mainwindow.cpp ../shared/xmlsyntaxhighlighter.cpp
HEADERS     += qobjectxmlmodel.h  mainwindow.h ../shared/xmlsyntaxhighlighter.h
RESOURCES   = queries.qrc
INCLUDEPATH += ../shared/

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/qobjectxmlmodel
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.xq *.html
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/qobjectxmlmodel
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7C8
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
