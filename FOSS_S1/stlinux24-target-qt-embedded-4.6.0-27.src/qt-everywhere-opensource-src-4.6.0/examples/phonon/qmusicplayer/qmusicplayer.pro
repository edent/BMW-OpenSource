QT        += phonon

HEADERS   += mainwindow.h
SOURCES   += main.cpp \
             mainwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
INSTALLS += target sources

wince*{
DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
}

symbian:TARGET.UID3 = 0xA000CF6A
