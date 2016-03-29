TEMPLATE =  subdirs
SUBDIRS +=  formextractor \
            previewer \
            fancybrowser

contains(QT_CONFIG, openssl):SUBDIRS += googlechat

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS webkit.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
