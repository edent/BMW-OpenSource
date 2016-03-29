# Include those manually as they do not contain any directory specification

SOURCES        += ../colorbutton.cpp ../main.cpp ../previewframe.cpp ../previewwidget.cpp ../mainwindow.cpp ../paletteeditoradvanced.cpp \
    ../mainwindowbase.cpp ../paletteeditoradvancedbase.cpp ../previewwidgetbase.cpp
HEADERS        += ../colorbutton.h ../previewframe.h ../previewwidget.h ../mainwindow.h ../paletteeditoradvanced.h \
    ../mainwindowbase.h ../paletteeditoradvancedbase.h ../previewwidgetbase.h

FORMS        = ../mainwindowbase.ui ../paletteeditoradvancedbase.ui ../previewwidgetbase.ui

TR_DIR = $$PWD/../../../translations
TRANSLATIONS = \
    $$TR_DIR/qtconfig_pl.ts \
    $$TR_DIR/qtconfig_ru.ts \
    $$TR_DIR/qtconfig_zh_CN.ts \
    $$TR_DIR/qtconfig_zh_TW.ts
