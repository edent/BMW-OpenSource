TEMPLATE         = \
                 subdirs
SUBDIRS          += \
                 animatedtiles \
                 appchooser \
                 easing \
                 moveblocks \
                 states \
                 stickman \

# install
target.path = $$[QT_INSTALL_EXAMPLES]/animation
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS animation.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/animation
INSTALLS += target sources
