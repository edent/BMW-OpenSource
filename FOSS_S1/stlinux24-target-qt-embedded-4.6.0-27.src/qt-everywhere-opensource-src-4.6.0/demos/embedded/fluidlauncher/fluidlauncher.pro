TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += \
           demoapplication.h \
           fluidlauncher.h \
           pictureflow.h \
           slideshow.h

SOURCES += \
           demoapplication.cpp \
           fluidlauncher.cpp \
           main.cpp \
           pictureflow.cpp \
           slideshow.cpp

embedded{
    target.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.html config.xml screenshots slides
    sources.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    INSTALLS += target sources
}

wince*{
    QT += svg

    BUILD_DIR = release
    if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
        BUILD_DIR = debug
    }

    executables.sources = \
        $$QT_BUILD_TREE/demos/embedded/embeddedsvgviewer/$${BUILD_DIR}/embeddedsvgviewer.exe \
        $$QT_BUILD_TREE/demos/embedded/styledemo/$${BUILD_DIR}/styledemo.exe \
        $$QT_BUILD_TREE/demos/deform/$${BUILD_DIR}/deform.exe \
        $$QT_BUILD_TREE/demos/pathstroke/$${BUILD_DIR}/pathstroke.exe \
        $$QT_BUILD_TREE/examples/graphicsview/elasticnodes/$${BUILD_DIR}/elasticnodes.exe \
        $$QT_BUILD_TREE/examples/widgets/wiggly/$${BUILD_DIR}/wiggly.exe \
        $$QT_BUILD_TREE/examples/painting/concentriccircles/$${BUILD_DIR}/concentriccircles.exe \
        $$QT_BUILD_TREE/examples/draganddrop/$${BUILD_DIR}/fridgemagnets.exe

    executables.path = .

    files.sources = $$PWD/screenshots $$PWD/slides $$PWD/../embeddedsvgviewer/shapes.svg
    files.path = .

    config.sources = $$PWD/config_wince/config.xml
    config.path = .

    DEPLOYMENT += config files executables

    DEPLOYMENT_PLUGIN += qgif qjpeg qmng qsvg
}

symbian {
    load(data_caging_paths)

    TARGET.UID3 = 0xA000A641
    ICON = $$QT_SOURCE_TREE/src/s60installs/qt.svg

    executables.sources = \
        styledemo.exe \
        deform.exe \
        pathstroke.exe \
        wiggly.exe \
        qftp.exe \
        saxbookmarks.exe \
        desktopservices.exe \
        fridgemagnets.exe \
        softkeys.exe \
        raycasting.exe \
        flickable.exe \
        digiflip.exe \
        lightmaps.exe \
        flightinfo.exe

    executables.path = /sys/bin

    reg_resource.sources = \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/styledemo_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/deform_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/pathstroke_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/wiggly_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/qftp_reg.rsc\
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/saxbookmarks_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/desktopservices_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/fridgemagnets_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/softkeys_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/raycasting_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/flickable_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/digiflip_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/lightmaps_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/flightinfo_reg.rsc

    contains(QT_CONFIG, phonon) {
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/qmediaplayer_reg.rsc
    }


    reg_resource.path = $$REG_RESOURCE_IMPORT_DIR

    resource.sources = \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/styledemo.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/deform.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/pathstroke.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/wiggly.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qftp.rsc\
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/saxbookmarks.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/desktopservices.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/fridgemagnets.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/softkeys.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/raycasting.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flickable.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/digiflip.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/lightmaps.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flightinfo.rsc


    resource.path = $$APP_RESOURCE_DIR

    mifs.sources = \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/fluidlauncher.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/styledemo.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/deform.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/pathstroke.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/wiggly.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qftp.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/saxbookmarks.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/desktopservices.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/fridgemagnets.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/softkeys.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/raycasting.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flickable.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/digiflip.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/lightmaps.mif \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flightinfo.mif
    mifs.path = $$APP_RESOURCE_DIR

    contains(QT_CONFIG, svg) {
        executables.sources += \
            embeddedsvgviewer.exe \
            weatherinfo.exe

        reg_resource.sources += \
            $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/embeddedsvgviewer_reg.rsc \
            $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/weatherinfo_reg.rsc

        resource.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/embeddedsvgviewer.rsc \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/weatherinfo.rsc

        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/embeddedsvgviewer.mif \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/weatherinfo.mif

    }
    contains(QT_CONFIG, webkit) {
        executables.sources += anomaly.exe
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/anomaly_reg.rsc
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/anomaly.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/anomaly.mif
    }

    contains(QT_CONFIG, phonon) {
        executables.sources += qmediaplayer.exe
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qmediaplayer.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qmediaplayer.mif
    }

    contains(QT_CONFIG, script) {
        executables.sources += context2d.exe
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/context2d_reg.rsc
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/context2d.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/context2d.mif
    }

    files.sources = $$PWD/screenshots $$PWD/slides
    files.path = .

    config.sources = $$PWD/config_s60/config.xml
    config.path = .

    viewerimages.sources = $$PWD/../embeddedsvgviewer/shapes.svg
    viewerimages.path = /data/images/qt/demos/embeddedsvgviewer

    # demos/mediaplayer make also use of these files.
    desktopservices_music.sources = \
        $$PWD/../desktopservices/data/*.mp3 \
        $$PWD/../desktopservices/data/*.wav
    desktopservices_music.path = /data/sounds

    desktopservices_images.sources = $$PWD/../desktopservices/data/*.png
    desktopservices_images.path = /data/images

    saxbookmarks.sources = $$PWD/../../../examples/xml/saxbookmarks/frank.xbel
    saxbookmarks.sources += $$PWD/../../../examples/xml/saxbookmarks/jennifer.xbel
    saxbookmarks.path = /data/qt/saxbookmarks

    DEPLOYMENT += config files executables viewerimages saxbookmarks reg_resource resource \
        mifs desktopservices_music desktopservices_images

    TARGET.EPOCHEAPSIZE = 100000 20000000
}
