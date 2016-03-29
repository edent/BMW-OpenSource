TEMPLATE = app
DEPENDPATH += . ../shared/
INCLUDEPATH += . ../shared/
TARGET=tst_deployment_mac
CONFIG += qtestlib

# Input
SOURCES += tst_deployment_mac.cpp ../shared/shared.cpp
HEADERS += ../shared/shared.h

