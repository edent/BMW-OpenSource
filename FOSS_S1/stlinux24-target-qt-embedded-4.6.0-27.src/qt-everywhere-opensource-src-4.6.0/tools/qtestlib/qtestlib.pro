TEMPLATE = subdirs
!wince*: SUBDIRS += updater chart
wince*: contains(QT_CONFIG, cetest): SUBDIRS += wince
CONFIG += ordered
