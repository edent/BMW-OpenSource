/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "initprojectdeploy_symbian.h"
#include <QDirIterator>
#include <project.h>
#include <qxmlstream.h>
#include <qsettings.h>
#include <qdebug.h>

#define PLUGIN_STUB_DIR "qmakepluginstubs"
#define SYSBIN_DIR "\\sys\\bin"

#define SUFFIX_DLL "dll"
#define SUFFIX_EXE "exe"
#define SUFFIX_QTPLUGIN "qtplugin"

static void fixEpocRootStr(QString& path)
{
    path.replace("\\", "/");

    if (path.size() > 1 && path[1] == QChar(':')) {
        path = path.mid(2);
    }

    if (!path.size() || path[path.size()-1] != QChar('/')) {
        path += QChar('/');
    }
}

#define SYMBIAN_SDKS_KEY "HKEY_LOCAL_MACHINE\\Software\\Symbian\\EPOC SDKs"

static QString epocRootStr;

QString epocRoot()
{
    if (!epocRootStr.isEmpty()) {
        return epocRootStr;
    }

    // First, check the env variable
    epocRootStr = qgetenv("EPOCROOT");

    if (epocRootStr.isEmpty()) {
        // No EPOCROOT set, check the default device
        // First check EPOCDEVICE env variable
        QString defaultDevice = qgetenv("EPOCDEVICE");

        // Check the windows registry via QSettings for devices.xml path
        QSettings settings(SYMBIAN_SDKS_KEY, QSettings::NativeFormat);
        QString devicesXmlPath = settings.value("CommonPath").toString();

        if (!devicesXmlPath.isEmpty()) {
            // Parse xml for correct device
            devicesXmlPath += "/devices.xml";
            QFile devicesFile(devicesXmlPath);
            if (devicesFile.open(QIODevice::ReadOnly)) {
                QXmlStreamReader xml(&devicesFile);
                while (!xml.atEnd()) {
                    xml.readNext();
                    if (xml.isStartElement() && xml.name() == "devices") {
                        if (xml.attributes().value("version") == "1.0") {
                            // Look for correct device
                            while (!(xml.isEndElement() && xml.name() == "devices") && !xml.atEnd()) {
                                xml.readNext();
                                if (xml.isStartElement() && xml.name() == "device") {
                                    if ((defaultDevice.isEmpty() && xml.attributes().value("default") == "yes") ||
                                        (!defaultDevice.isEmpty() && (xml.attributes().value("id").toString() + QString(":") + xml.attributes().value("name").toString()) == defaultDevice)) {
                                        // Found the correct device
                                        while (!(xml.isEndElement() && xml.name() == "device") && !xml.atEnd()) {
                                            xml.readNext();
                                            if (xml.isStartElement() && xml.name() == "epocroot") {
                                                epocRootStr = xml.readElementText();
                                                fixEpocRootStr(epocRootStr);
                                                return epocRootStr;
                                            }
                                        }
                                        xml.raiseError("No epocroot element found");
                                    }
                                }
                            }
                        } else {
                            xml.raiseError("Invalid 'devices' element version");
                        }
                    }
                }
                if (xml.hasError()) {
                    fprintf(stderr, "ERROR: \"%s\" when parsing devices.xml\n", qPrintable(xml.errorString()));
                }
            } else {
                fprintf(stderr, "Could not open devices.xml (%s)\n", qPrintable(devicesXmlPath));
            }
        } else {
            fprintf(stderr, "Could not retrieve " SYMBIAN_SDKS_KEY " setting\n");
        }

        fprintf(stderr, "Failed to determine epoc root.\n");
        if (!defaultDevice.isEmpty())
            fprintf(stderr, "The device indicated by EPOCDEVICE environment variable (%s) could not be found.\n", qPrintable(defaultDevice));
        fprintf(stderr, "Either set EPOCROOT or EPOCDEVICE environment variable to a valid value, or provide a default Symbian device.\n");

        // No valid device found; set epocroot to "/"
        epocRootStr = QLatin1String("/");
    }

    fixEpocRootStr(epocRootStr);
    return epocRootStr;
}


static bool isPlugin(const QFileInfo& info, const QString& devicePath)
{
    // Libraries are plugins if deployment path is something else than
    // SYSBIN_DIR with or without drive letter
    if (0 == info.suffix().compare(QLatin1String(SUFFIX_DLL), Qt::CaseInsensitive) &&
            (devicePath.size() < 8 ||
             (0 != devicePath.compare(QLatin1String(SYSBIN_DIR), Qt::CaseInsensitive) &&
              0 != devicePath.mid(1).compare(QLatin1String(":" SYSBIN_DIR), Qt::CaseInsensitive)))) {
        return true;
    } else {
        return false;
    }
}

static bool isBinary(const QFileInfo& info)
{
    if (0 == info.suffix().compare(QLatin1String(SUFFIX_DLL), Qt::CaseInsensitive) ||
            0 == info.suffix().compare(QLatin1String(SUFFIX_EXE), Qt::CaseInsensitive)) {
        return true;
    } else {
        return false;
    }
}

static void createPluginStub(const QFileInfo& info,
                             const QString& devicePath,
                             DeploymentList &deploymentList,
                             QStringList& generatedDirs,
                             QStringList& generatedFiles)
{
    QDir().mkpath(QLatin1String(PLUGIN_STUB_DIR "\\"));
    if (!generatedDirs.contains(PLUGIN_STUB_DIR))
        generatedDirs << PLUGIN_STUB_DIR;
    // Plugin stubs must have different name from the actual plugins, because
    // the toolchain for creating ROM images cannot handle non-binary .dll files properly.
    QFile stubFile(QLatin1String(PLUGIN_STUB_DIR "\\") + info.completeBaseName() + "." SUFFIX_QTPLUGIN);
    if (stubFile.open(QIODevice::WriteOnly)) {
        if (!generatedFiles.contains(stubFile.fileName()))
            generatedFiles << stubFile.fileName();
        QTextStream t(&stubFile);
        // Add note to stub so that people will not wonder what it is.
        // Creation date is added to make new stub to deploy always to
        // force plugin cache miss when loading plugins.
        t << "This file is a Qt plugin stub file. The real Qt plugin is located in " SYSBIN_DIR ". Created:" << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    } else {
        fprintf(stderr, "cannot deploy \"%s\" because of plugin stub file creation failed\n", info.fileName().toLatin1().constData());
    }
    QFileInfo stubInfo(stubFile);
    deploymentList.append(CopyItem(Option::fixPathToLocalOS(stubInfo.absoluteFilePath()),
                                   Option::fixPathToLocalOS(devicePath + "\\" + stubInfo.fileName())));
}

QString generate_uid(const QString& target)
{
    static QMap<QString, QString> targetToUid;

    QString tmp = targetToUid[target];

    if (!tmp.isEmpty()) {
        return tmp;
    }

    unsigned long hash = 5381;
    int c;

    for (int i = 0; i < target.size(); ++i) {
        c = target.at(i).toAscii();
        hash ^= c + ((c - i) << i % 20) + ((c + i) << (i + 5) % 20) + ((c - 2 * i) << (i + 10) % 20) + ((c + 2 * i) << (i + 15) % 20);
    }

    tmp.setNum(hash, 16);
    for (int i = tmp.size(); i < 8; ++i)
        tmp.prepend("0");

    targetToUid[target] = tmp;

    return tmp;
}

// UIDs starting with 0xE are test UIDs in symbian
QString generate_test_uid(const QString& target)
{
    QString tmp = generate_uid(target);
    tmp.replace(0, 1, "E");
    tmp.prepend("0x");

    return tmp;
}


void initProjectDeploySymbian(QMakeProject* project,
                              DeploymentList &deploymentList,
                              const QString &testPath,
                              bool deployBinaries,
                              const QString &platform,
                              const QString &build,
                              QStringList& generatedDirs,
                              QStringList& generatedFiles)
{
    QString targetPath = project->values("deploy.path").join(" ");
    if (targetPath.isEmpty())
        targetPath = testPath;
    if (targetPath.endsWith("/") || targetPath.endsWith("\\"))
        targetPath = targetPath.mid(0, targetPath.size() - 1);

    bool targetPathHasDriveLetter = false;
    if (targetPath.size() > 1) {
        targetPathHasDriveLetter = targetPath.at(1) == QLatin1Char(':');
    }
    QString deploymentDrive = targetPathHasDriveLetter ? targetPath.left(2) : QLatin1String("c:");

    foreach(QString item, project->values("DEPLOYMENT")) {
        QString devicePath = project->first(item + ".path");
        if (!deployBinaries
                && !devicePath.isEmpty()
                && (0 == devicePath.compare(project->values("APP_RESOURCE_DIR").join(""), Qt::CaseInsensitive)
                    || 0 == devicePath.compare(project->values("REG_RESOURCE_IMPORT_DIR").join(""), Qt::CaseInsensitive))) {
            // Do not deploy resources in emulator builds, as that seems to cause conflicts
            // If there is ever a real need to deploy pre-built resources for emulator,
            // BLD_INF_RULES.prj_exports can be used as a workaround.
            continue;
        }

        bool devicePathHasDriveLetter = false;
        if (devicePath.size() > 1) {
            devicePathHasDriveLetter = devicePath.at(1) == QLatin1Char(':');
        }

        if (devicePath.isEmpty() || devicePath == QLatin1String(".")) {
            devicePath = targetPath;
        }
        // check if item.path is relative (! either / or \)
        else if (!(devicePath.at(0) == QLatin1Char('/')
                   || devicePath.at(0) == QLatin1Char('\\')
                   || devicePathHasDriveLetter)) {
            // create output path
            devicePath = Option::fixPathToLocalOS(QDir::cleanPath(targetPath + QLatin1Char('\\') + devicePath));
        } else {
            if (0 == platform.compare(QLatin1String("winscw"), Qt::CaseInsensitive)) {
                if (devicePathHasDriveLetter) {
                    devicePath = epocRoot() + "epoc32\\winscw\\" + devicePath.remove(1, 1);
                } else {
                    devicePath = epocRoot() + "epoc32\\winscw\\c" + devicePath;
                }
            } else {
                // Drive letter needed if targetpath contains one and it is not already in
                if (targetPathHasDriveLetter && !devicePathHasDriveLetter) {
                    devicePath = deploymentDrive + devicePath;
                }
            }
        }

        devicePath.replace(QLatin1String("/"), QLatin1String("\\"));

        if (!deployBinaries &&
                0 == devicePath.right(8).compare(QLatin1String(SYSBIN_DIR), Qt::CaseInsensitive)) {
            // Skip deploying to SYSBIN_DIR for anything but binary deployments
            // Note: Deploying pre-built binaries also follow this rule, so emulator builds
            // will not get those deployed. Since there is no way to differentiate currently
            // between pre-built binaries for emulator and HW anyway, this is not a major issue.
            continue;
        }

        foreach(QString source, project->values(item + ".sources")) {
            source = Option::fixPathToLocalOS(source);
            QString nameFilter;
            QFileInfo info(source);
            QString searchPath;
            bool dirSearch = false;

            if (info.isDir()) {
                nameFilter = QLatin1String("*");
                searchPath = info.absoluteFilePath();
                dirSearch = true;
            } else {
                if (info.exists() || source.indexOf('*') != -1) {
                    nameFilter = source.split('\\').last();
                    searchPath = info.absolutePath();
                } else {
                    // Entry was not found. That is ok if it is a binary, since those do not necessarily yet exist.
                    // Dlls need to be processed even when not deploying binaries for the stubs
                    if (isBinary(info)) {
                        if (deployBinaries) {
                            // Executables and libraries are deployed to \sys\bin
                            QFileInfo releasePath(epocRoot() + "epoc32\\release\\" + platform + "\\" + build + "\\");
                            if(devicePathHasDriveLetter) {
                                deploymentList.append(CopyItem(Option::fixPathToLocalOS(releasePath.absolutePath() + "\\" + info.fileName(), false, true),
                                                               Option::fixPathToLocalOS(devicePath.left(2) + QLatin1String(SYSBIN_DIR "\\") + info.fileName())));
                            } else {
                                deploymentList.append(CopyItem(Option::fixPathToLocalOS(releasePath.absolutePath() + "\\" + info.fileName(), false, true),
                                                               Option::fixPathToLocalOS(deploymentDrive + QLatin1String(SYSBIN_DIR "\\") + info.fileName())));
                            }
                        }
                        if (isPlugin(info, devicePath)) {
                            createPluginStub(info, devicePath, deploymentList, generatedDirs, generatedFiles);
                            continue;
                        }
                    } else {
                        // Generate deployment even if file doesn't exist, as this may be the case
                        // when generating .pkg files.
                        deploymentList.append(CopyItem(Option::fixPathToLocalOS(info.absoluteFilePath()),
                                                       Option::fixPathToLocalOS(devicePath + "\\" + info.fileName())));
                        continue;
                    }
                }
            }

            int pathSize = info.absolutePath().size();
            QDirIterator iterator(searchPath, QStringList() << nameFilter
                                  , QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks
                                  , dirSearch ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

            while (iterator.hasNext()) {
                iterator.next();
                QFileInfo iteratorInfo(iterator.filePath());
                QString absoluteItemPath = Option::fixPathToLocalOS(iteratorInfo.absolutePath());
                int diffSize = absoluteItemPath.size() - pathSize;

                if (!iteratorInfo.isDir()) {
                    if (isPlugin(iterator.fileInfo(), devicePath)) {
                        // This deploys pre-built plugins. Other pre-built binaries will deploy normally,
                        // as they have SYSBIN_DIR target path.
                        if (deployBinaries) {
                            deploymentList.append(CopyItem(Option::fixPathToLocalOS(absoluteItemPath + "\\" + iterator.fileName()),
                                                           Option::fixPathToLocalOS(deploymentDrive + QLatin1String(SYSBIN_DIR "\\") + iterator.fileName())));
                        }
                        createPluginStub(info, devicePath + "\\" + absoluteItemPath.right(diffSize), deploymentList, generatedDirs, generatedFiles);
                        continue;
                    } else {
                        deploymentList.append(CopyItem(Option::fixPathToLocalOS(absoluteItemPath + "\\" + iterator.fileName()),
                                                       Option::fixPathToLocalOS(devicePath + "\\" + absoluteItemPath.right(diffSize) + "\\" + iterator.fileName())));
                    }
                }
            }
        }
    }
}
