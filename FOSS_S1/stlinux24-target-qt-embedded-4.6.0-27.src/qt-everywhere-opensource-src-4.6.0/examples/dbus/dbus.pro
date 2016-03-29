TEMPLATE = subdirs
SUBDIRS = listnames \
	  pingpong \
	  complexpingpong \
	  dbus-chat \
	  remotecontrolledcar

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dbus
sources.files = *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dbus
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
