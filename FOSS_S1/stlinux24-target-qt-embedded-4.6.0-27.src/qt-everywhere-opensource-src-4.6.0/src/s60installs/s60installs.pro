# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

symbian: {
    load(data_caging_paths)

    SUBDIRS=
    # WARNING: Changing TARGET name will break Symbian SISX upgrade functionality
    # DO NOT TOUCH TARGET VARIABLE IF YOU ARE NOT SURE WHAT YOU ARE DOING
    TARGET = "Qt"
    TARGET.UID3 = 0x2001E61C
    VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}

    qtresources.sources = $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/s60main.rsc
    qtresources.path = c:$$APP_RESOURCE_DIR

    qtlibraries.sources = \
        QtCore.dll \
        QtXml.dll \
        QtGui.dll \
        QtNetwork.dll \
        QtTest.dll \
        QtSql.dll

    qts60plugindeployment = \
        "IF package(0x1028315F)" \
        "   \"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/qts60plugin_5_0.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/qts60plugin_3_2.dll\" - \"c:\\sys\\bin\\qts60plugin_3_2.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/qts60plugin_3_1.dll\" - \"c:\\sys\\bin\\qts60plugin_3_1.dll\"" \
        "ELSE" \
        "   \"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/qts60plugin_5_0.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0.dll\"" \
        "ENDIF"
    qtlibraries.pkg_postrules += qts60plugindeployment

    sqlitedeployment = \
        "; Deploy sqlite onto phone that does not have it (this should be replaced with embedded sis file when available)" \
        "IF NOT package(0x2002533b) " \
        "\"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/sqlite3.dll\" - \"c:\\sys\\bin\\sqlite3.dll\"" \
        "ENDIF"
    qtlibraries.pkg_postrules += sqlitedeployment

    qtlibraries.path = c:/sys/bin

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "


    qtlibraries.pkg_prerules = vendorinfo
    qtlibraries.pkg_prerules += "; Dependencies of Qt libraries"
    qtlibraries.pkg_prerules += "(0x20013851), 1, 5, 1, {\"PIPS Installer\"}"
    contains(QT_CONFIG, openssl) | contains(QT_CONFIG, openssl-linked) {
        qtlibraries.pkg_prerules += "(0x200110CB), 1, 5, 1, {\"Open C LIBSSL Common\"}"
    }
    contains(CONFIG, stl) {
        qtlibraries.pkg_prerules += "(0x2000F866), 1, 0, 0, {\"Standard C++ Library Common\"}"
    }

    !contains(QT_CONFIG, no-jpeg): imageformats_plugins.sources += qjpeg.dll
    !contains(QT_CONFIG, no-gif):  imageformats_plugins.sources += qgif.dll
    !contains(QT_CONFIG, no-mng):  imageformats_plugins.sources += qmng.dll
    !contains(QT_CONFIG, no-tiff): imageformats_plugins.sources += qtiff.dll
    !contains(QT_CONFIG, no-ico):  imageformats_plugins.sources += qico.dll
    imageformats_plugins.path = c:$$QT_PLUGINS_BASE_DIR/imageformats

    codecs_plugins.sources = qcncodecs.dll qjpcodecs.dll qtwcodecs.dll qkrcodecs.dll
    codecs_plugins.path = c:$$QT_PLUGINS_BASE_DIR/codecs

    contains(QT_CONFIG, phonon-backend) {
        phonon_backend_plugins.sources += phonon_mmf.dll

        phonon_backend_plugins.path = c:$$QT_PLUGINS_BASE_DIR/phonon_backend
        DEPLOYMENT += phonon_backend_plugins
    }

    DEPLOYMENT += qtresources qtlibraries imageformats_plugins codecs_plugins graphicssystems_plugins

    contains(QT_CONFIG, svg): {
       qtlibraries.sources += QtSvg.dll
       imageformats_plugins.sources += qsvg.dll
       iconengines_plugins.sources = qsvgicon.dll
       iconengines_plugins.path = c:$$QT_PLUGINS_BASE_DIR/iconengines
       DEPLOYMENT += iconengines_plugins
    }

    contains(QT_CONFIG, phonon): {
       qtlibraries.sources += Phonon.dll
    }

    contains(QT_CONFIG, script): {
        qtlibraries.sources += QtScript.dll
    }

    contains(QT_CONFIG, xmlpatterns): {
       qtlibraries.sources += QtXmlPatterns.dll
    }

    contains(QT_CONFIG, declarative): {
        qtlibraries.sources += QtDeclarative.dll
    }

    contains(QT_CONFIG, webkit): {
        qtlibraries.sources += QtWebKit.dll
    }

    graphicssystems_plugins.path = c:$$QT_PLUGINS_BASE_DIR/graphicssystems
    contains(QT_CONFIG, openvg) {
        qtlibraries.sources += QtOpenVG.dll
        graphicssystems_plugins.sources += qvggraphicssystem.dll
    }

    BLD_INF_RULES.prj_exports += "qt.iby $$CORE_MW_LAYER_IBY_EXPORT_PATH(qt.iby)"
    BLD_INF_RULES.prj_exports += "qtdemoapps.iby $$CORE_APP_LAYER_IBY_EXPORT_PATH(qtdemoapps.iby)"
}
