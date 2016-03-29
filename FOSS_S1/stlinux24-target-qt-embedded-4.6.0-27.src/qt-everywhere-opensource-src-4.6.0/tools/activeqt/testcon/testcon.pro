TEMPLATE = app

CONFIG	+= qaxserver qaxserver_no_postlink qaxcontainer
# QT += qt3support

# ui_qaxselect.h
INCLUDEPATH += $$QT_SOURCE_TREE/tools/activeqt/container/debug \
    $$QT_SOURCE_TREE/tools/activeqt/container/release \
    $$QT_BUILD_TREE/src/activeqt/container \

SOURCES	 = main.cpp docuwindow.cpp mainwindow.cpp invokemethod.cpp changeproperties.cpp ambientproperties.cpp controlinfo.cpp
HEADERS	 = docuwindow.h mainwindow.h invokemethod.h changeproperties.h ambientproperties.h controlinfo.h
FORMS	 = mainwindow.ui invokemethod.ui changeproperties.ui ambientproperties.ui controlinfo.ui
RC_FILE	 = testcon.rc

win32-borland {
    QMAKE_POST_LINK = -midl $$QT_SOURCE_TREE/tools/activeqt/testcon/testcon.idl
} else {
    !win32-g++:QMAKE_POST_LINK = midl $$QT_SOURCE_TREE/tools/activeqt/testcon/testcon.idl && move testcon.tlb $(TARGETDIR)

}
