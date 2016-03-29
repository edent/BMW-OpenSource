# This block serves the minimalistic resource file for S60 3.1 platforms.
# Note there is no way to ifdef S60 version in mmp file, that is why the resource
# file is always compiled for WINSCW
minimalAppResource31 = \
    "SOURCEPATH s60framework" \
    "START RESOURCE s60main.rss" \
    "HEADER" \
    "TARGETPATH resource\apps" \
    "END"
MMP_RULES += minimalAppResource31

SOURCES += s60framework/qs60mainapplication.cpp \
           s60framework/qs60mainappui.cpp \
           s60framework/qs60maindocument.cpp

HEADERS += s60framework/qs60mainapplication_p.h \
           s60framework/qs60mainapplication.h \
           s60framework/qs60mainappui.h \
           s60framework/qs60maindocument.h
