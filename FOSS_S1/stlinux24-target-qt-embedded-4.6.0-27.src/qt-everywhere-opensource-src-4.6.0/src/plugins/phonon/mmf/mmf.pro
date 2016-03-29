# MMF Phonon backend

QT += phonon
TARGET = phonon_mmf
PHONON_MMF_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/mmf

# Uncomment the following line in order to use the CDrmPlayerUtility client
# API for audio playback, rather than CMdaAudioPlayerUtility.
#CONFIG += phonon_mmf_audio_drm

phonon_mmf_audio_drm {
    LIBS += -lDrmAudioPlayUtility
    DEFINES += QT_PHONON_MMF_AUDIO_DRM
} else {
    LIBS += -lmediaclientaudio
}

# This is necessary because both epoc32/include and Phonon contain videoplayer.h.
# By making /epoc32/include the first SYSTEMINCLUDE, we ensure that
# '#include <videoplayer.h>' picks up the Symbian header, as intended.
PREPEND_INCLUDEPATH = /epoc32/include

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

HEADERS +=                                           \
           $$PHONON_MMF_DIR/abstractaudioeffect.h    \
           $$PHONON_MMF_DIR/abstractmediaplayer.h    \
           $$PHONON_MMF_DIR/abstractplayer.h         \
           $$PHONON_MMF_DIR/ancestormovemonitor.h    \
           $$PHONON_MMF_DIR/audioequalizer.h         \
           $$PHONON_MMF_DIR/audiooutput.h            \
           $$PHONON_MMF_DIR/audioplayer.h            \
           $$PHONON_MMF_DIR/backend.h                \
           $$PHONON_MMF_DIR/bassboost.h              \
           $$PHONON_MMF_DIR/defs.h                   \
           $$PHONON_MMF_DIR/dummyplayer.h            \
           $$PHONON_MMF_DIR/effectfactory.h          \
           $$PHONON_MMF_DIR/mediaobject.h            \
           $$PHONON_MMF_DIR/mmf_medianode.h          \
           $$PHONON_MMF_DIR/mmf_videoplayer.h        \
           $$PHONON_MMF_DIR/objectdump.h             \
           $$PHONON_MMF_DIR/objectdump_symbian.h     \
           $$PHONON_MMF_DIR/objecttree.h             \
           $$PHONON_MMF_DIR/utils.h                  \
           $$PHONON_MMF_DIR/videooutput.h            \
           $$PHONON_MMF_DIR/videooutputobserver.h    \
           $$PHONON_MMF_DIR/videowidget.h            \
           $$PHONON_MMF_DIR/volumeobserver.h

SOURCES +=                                           \
           $$PHONON_MMF_DIR/abstractaudioeffect.cpp  \
           $$PHONON_MMF_DIR/abstractmediaplayer.cpp  \
           $$PHONON_MMF_DIR/abstractplayer.cpp       \
           $$PHONON_MMF_DIR/ancestormovemonitor.cpp  \
           $$PHONON_MMF_DIR/audioequalizer.cpp       \
           $$PHONON_MMF_DIR/audiooutput.cpp          \
           $$PHONON_MMF_DIR/audioplayer.cpp          \
           $$PHONON_MMF_DIR/backend.cpp              \
           $$PHONON_MMF_DIR/bassboost.cpp            \
           $$PHONON_MMF_DIR/dummyplayer.cpp          \
           $$PHONON_MMF_DIR/effectfactory.cpp        \
           $$PHONON_MMF_DIR/mediaobject.cpp          \
           $$PHONON_MMF_DIR/mmf_medianode.cpp        \
           $$PHONON_MMF_DIR/mmf_videoplayer.cpp      \
           $$PHONON_MMF_DIR/objectdump.cpp           \
           $$PHONON_MMF_DIR/objectdump_symbian.cpp   \
           $$PHONON_MMF_DIR/objecttree.cpp           \
           $$PHONON_MMF_DIR/utils.cpp                \
           $$PHONON_MMF_DIR/videooutput.cpp          \
           $$PHONON_MMF_DIR/videowidget.cpp

LIBS += -lcone
LIBS += -lws32

# This is only needed for debug builds, but is always linked against.
LIBS += -lhal

TARGET.CAPABILITY = all -tcb

LIBS += -lmediaclientvideo        # For CVideoPlayerUtility
LIBS += -lcone                    # For CCoeEnv
LIBS += -lws32                    # For RWindow
LIBS += -lefsrv                   # For file server
LIBS += -lapgrfx -lapmime         # For recognizer
LIBS += -lmmfcontrollerframework  # For CMMFMetaDataEntry

# These are for effects.
LIBS += -lAudioEqualizerEffect -lBassBoostEffect -lDistanceAttenuationEffect -lDopplerBase -lEffectBase -lEnvironmentalReverbEffect -lListenerDopplerEffect -lListenerLocationEffect -lListenerOrientationEffect -lLocationBase -lLoudnessEffect -lOrientationBase -lSourceDopplerEffect -lSourceLocationEffect -lSourceOrientationEffect -lStereoWideningEffect

# This is needed for having the .qtplugin file properly created on Symbian.
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)

TARGET.UID3=0x2001E629

