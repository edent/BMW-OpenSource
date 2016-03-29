TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    uitools \
    lib \
    components \
    designer

CONFIG(shared,shared|static):SUBDIRS += plugins

wince*: SUBDIRS -= designer plugins
symbian: SUBDIRS = uitools
contains(DEFINES, QT_NO_CURSOR): SUBDIRS -= lib components