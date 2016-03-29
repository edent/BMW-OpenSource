QT      +=  webkit network
HEADERS =   googlechat.h
SOURCES =   main.cpp \
            googlechat.cpp
FORMS = form.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlechat
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlechat
INSTALLS += target sources

symbian:TARGET.UID3 = 0xA000CF6E
