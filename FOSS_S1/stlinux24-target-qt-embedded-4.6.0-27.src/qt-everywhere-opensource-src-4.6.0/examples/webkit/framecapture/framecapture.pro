QT      += webkit

HEADERS = framecapture.h
SOURCES = main.cpp \
          framecapture.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/framecapture
sources.files = $$SOURCES $$HEADERS
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/framecapture
INSTALLS += target sources
