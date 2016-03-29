HEADERS         = svgtextobject.h \
                  window.h
SOURCES         = main.cpp \
                  svgtextobject.cpp \
                  window.cpp

QT += svg

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext/textobject
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext/textobject
INSTALLS += target sources

filesToDeploy.sources = files/*.svg
filesToDeploy.path = files
DEPLOYMENT += filesToDeploy

