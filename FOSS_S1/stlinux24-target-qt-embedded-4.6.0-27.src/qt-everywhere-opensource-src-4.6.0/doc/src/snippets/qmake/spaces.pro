#! [quoting library paths with spaces]
win32:LIBS += $$quote(C:/mylibs/extra libs/extra.lib)
unix:LIBS += $$quote(-L/home/user/extra libs) -lextra
#! [quoting library paths with spaces]

#! [quoting include paths with spaces]
win32:INCLUDEPATH += $$quote(C:/mylibs/extra headers)
unix:INCLUDEPATH += $$quote(/home/user/extra headers)
#! [quoting include paths with spaces]
