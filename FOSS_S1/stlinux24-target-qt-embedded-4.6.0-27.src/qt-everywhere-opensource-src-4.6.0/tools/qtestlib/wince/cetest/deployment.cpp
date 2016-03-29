/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "deployment.h"
#include "remoteconnection.h"
#include <option.h>
#include <qdir.h>
#include <qfile.h>
#include <qstring.h>

extern void debugOutput(const QString& text, int level);

bool DeploymentHandler::deviceCopy(const DeploymentList &deploymentList)
{
    for (int i=0; i<deploymentList.size(); ++i) {
        CopyItem item = deploymentList.at(i);
        m_connection->createDirectory(item.to.left(item.to.lastIndexOf(QLatin1Char('\\'))));
        if (!m_connection->copyFileToDevice(item.from , item.to)) {
            debugOutput(QString::fromLatin1("Error while copy: %1 -> %2").arg(item.from).arg(item.to),0);
            return false;
        }
    }
    return true;
}

bool DeploymentHandler::deviceDeploy(const DeploymentList &deploymentList)
{
    DeploymentList copyList;
    for (int i=0; i<deploymentList.size(); ++i) {
#if defined(Q_OS_WIN)
        HANDLE localHandle = CreateFile(deploymentList.at(i).from.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
        if (localHandle == INVALID_HANDLE_VALUE) {
            copyList.append(deploymentList.at(i));
            continue;
        }
        FILETIME localCreationTime;
        if (!GetFileTime(localHandle, NULL, NULL, &localCreationTime) || !m_connection->timeStampForLocalFileTime(&localCreationTime)) {
            copyList.append(deploymentList.at(i));
            CloseHandle(localHandle);
            continue;
        }
        CloseHandle(localHandle);

        FILETIME deviceCreationTime;
        if (!m_connection->fileCreationTime(deploymentList.at(i).to , &deviceCreationTime)) {
            copyList.append(deploymentList.at(i));
            continue;
        }
        
        int res = CompareFileTime(&localCreationTime, &deviceCreationTime);
        if (res != 0)
            copyList.append(deploymentList.at(i));
        else
            debugOutput(QString::fromLatin1("Skipping File %1, already latest version").arg(deploymentList.at(i).from),0);
#else
        copyList.append(deploymentList.at(i));
#endif
    }
    return deviceCopy(copyList);
}

void DeploymentHandler::cleanup(const DeploymentList &deploymentList)
{
    for (int i=0; i<deploymentList.size(); ++i) {
        m_connection->deleteFile(deploymentList.at(i).to);
#ifdef Q_OS_WIN
        QString path = deploymentList.at(i).to;
        int pos;
        while ( (pos = path.lastIndexOf(QLatin1Char('\\'))) > 0) {
            path = path.left(pos);
            if (!m_connection->deleteDirectory(path, false, true))
                break;
        }
#endif
    }
}

void DeploymentHandler::initQtDeploy(QMakeProject *project, DeploymentList &deploymentList, const QString &testPath)
{
    QString targetPath = project->values("deploy.path").join(" ");
    if (targetPath.isEmpty())
        targetPath = testPath;
    if (targetPath.endsWith("/") || targetPath.endsWith("\\"))
        targetPath = targetPath.mid(0,targetPath.size()-1);

    // Only deploy Qt libs for shared build
    if (!project->values("QMAKE_QT_DLL").isEmpty() && !project->values("QMAKE_LIBDIR").isEmpty()) {
        QStringList libs = project->values("LIBS");
        QStringList qtLibs;
        QStringList libPaths;
        foreach (QString item, libs) {

            if (item.startsWith("-L")) {
                // -L -> a directory containing DLLs
                libPaths << item.mid(2);
                continue;
            }

            QStringList libCandidates;

            if (item.startsWith("-l")) {
                // -l -> a library located within one of the standard library paths
                QString lib = item.mid(2);

                // Check if it's a Qt library first, then check in all paths given with -L.
                // Note Qt libraries get a `4' appended to them, others don't.
                libCandidates << project->values("QMAKE_LIBDIR").at(0) + QDir::separator() + lib + QLatin1String("4.dll");
                foreach (QString const& libPath, libPaths) {
                    libCandidates << libPath + QDir::separator() + lib + QLatin1String(".dll");
                }
            } else {
                libCandidates << item.replace(".lib",".dll");
            }

            foreach (QString const& file, libCandidates) {
                QFileInfo info(file);
                if (info.exists()) {
                    qtLibs += info.dir().absoluteFilePath(info.fileName());
                    break;
                }
            }
        }
        for (QStringList::ConstIterator it = qtLibs.constBegin(); it != qtLibs.constEnd(); ++it) {
                QString dllName = *it;
                QFileInfo info(dllName);
                if (!info.exists())
                    continue;
                deploymentList.append(CopyItem(Option::fixPathToLocalOS(info.absoluteFilePath()) ,
                    Option::fixPathToLocalOS(targetPath + "/" + info.fileName())));
        }
    }

#ifndef QT_CETEST_NO_ACTIVESYNC
    // QtRemote deployment. We always deploy to \Windows
    if (!project->values("QMAKE_LIBDIR").isEmpty()) {
        QString remoteLibName = QLatin1String("QtRemote.dll");
        QString remoteLib = Option::fixPathToLocalOS(project->values("QMAKE_LIBDIR").at(0) + QDir::separator() + remoteLibName);
        if (QFile::exists(remoteLib))
            deploymentList.append(CopyItem(remoteLib, QString::fromLatin1("\\Windows\\") + remoteLibName));
        else
            debugOutput(QString::fromLatin1("Could not find QtRemote. Might not be able to launch target executable"),0);
    }
#endif

    // C-runtime deployment
    QString runtime = project->values("QT_CE_C_RUNTIME").join(QLatin1String(" "));
    debugOutput(QString::fromLatin1("Runtime:%1").arg(runtime), 2);
    if (!runtime.isEmpty() && (runtime != QLatin1String("no"))) {
        QString runtimeVersion = QLatin1String("msvcr");
        const QString mkspec = project->values("QMAKESPEC").first();
        if (mkspec.endsWith("2008"))
            runtimeVersion.append("90");
        else
            runtimeVersion.append("80");
        if (project->isActiveConfig("debug"))
            runtimeVersion.append("d");
        runtimeVersion.append(".dll");

        if (runtime == "yes") {
            // Auto-find C-runtime
            QString vcInstallDir = qgetenv("VCINSTALLDIR");
            if (!vcInstallDir.isEmpty()) {
                vcInstallDir += "\\ce\\dll\\";
                vcInstallDir += project->values("CE_ARCH").join(QLatin1String(" "));
                if (!QFileInfo(vcInstallDir + QDir::separator() + runtimeVersion).exists())
                    runtime.clear();
                else
                    runtime = vcInstallDir;
            }
        }

        if (!runtime.isEmpty()) {
            deploymentList.append(CopyItem(Option::fixPathToLocalOS(runtime + "/" + runtimeVersion ) ,
                Option::fixPathToLocalOS(targetPath + "/" + runtimeVersion)));
        }
    }
}

void DeploymentHandler::initProjectDeploy(QMakeProject* project, DeploymentList &deploymentList, const QString &testPath)
{
    QString targetPath = project->values("deploy.path").join(" ");
    if (targetPath.isEmpty())
        targetPath = testPath;
    if (targetPath.endsWith("/") || targetPath.endsWith("\\"))
        targetPath = targetPath.mid(0,targetPath.size()-1);

    QStringList& list = project->values("DEPLOYMENT");
    if (list.isEmpty())
        return;

    for (int it = 0; it < list.size(); ++it) {
        QString argSource = list.at(it) + QString(".sources");
        QString argPath = list.at(it) + QString(".path");
        if ((project->values(argSource).isEmpty() || project->values(argPath).isEmpty()) && list.at(it) != "deploy") {
            debugOutput(QString::fromLatin1("cannot deploy \"%1\" because of missing data.").arg(list.at(it)), 0);
            continue;
        }

        QString addPath = project->values(argPath).join(QLatin1String(" "));
        if (addPath == QLatin1String("."))
            addPath.clear();
        if (!addPath.startsWith("/") && !addPath.startsWith(QLatin1String("\\")))
            addPath = targetPath + "/" + addPath;

        QStringList addSources = project->values(argSource);
        addSources.replaceInStrings(QLatin1String("/"), QLatin1String("\\"));
        for(int index=0; index < addSources.size(); ++index) {
            QString dirstr = qmake_getpwd();
            QString filestr = Option::fixPathToLocalOS(addSources.at(index), false, false);
            int slsh = filestr.lastIndexOf(Option::dir_sep);
            if(slsh != -1) {
                dirstr = filestr.left(slsh+1);
                filestr = filestr.right(filestr.length() - slsh - 1);
            }
            if(dirstr.right(Option::dir_sep.length()) != Option::dir_sep)
                dirstr += Option::dir_sep;
            QFileInfo info(dirstr + filestr);
            
            static int addQMakeDeployCounter = 0;
            QStringList entryList = info.absoluteDir().entryList(QStringList() << info.fileName());
            if (entryList.size() > 1) {
                foreach(QString s, entryList) {
                    // We do not include directories when using wildcards
                    QFileInfo wildInfo(info.absolutePath() + "/" + s);
                    if (wildInfo.isDir()) {
                        continue;
                    }
                    QString appendedQmakeDeploy = QString::fromLatin1("_q_make_additional_deploy_%1").arg(addQMakeDeployCounter++);
                    project->parse(appendedQmakeDeploy + QLatin1String(".sources = \"") + wildInfo.absoluteFilePath());
                    project->parse(appendedQmakeDeploy + QLatin1String(".path    = \"") + addPath);
                    list.append(appendedQmakeDeploy);
                }
                continue;
            }
                                                                            
            if (info.isDir()) {
                QDir additionalDir(dirstr + filestr);
                QStringList additionalEntries = additionalDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::NoSymLinks);
                foreach(QString item, additionalEntries) {
                QString appendedDeploy = QString::fromLatin1("_q_make_additional_deploy_%1").arg(addQMakeDeployCounter++);
                    project->parse(appendedDeploy + QLatin1String(".sources = \"") + Option::fixPathToLocalOS(additionalDir.absoluteFilePath(item)) + QLatin1String("\""));
                    QString appendTargetPath = project->values(argPath).join(QLatin1String(" "));
                    if (appendTargetPath == QLatin1String("."))
                        appendTargetPath = filestr;
                    else
                        appendTargetPath.append(QLatin1String("\\") + filestr);
                    project->parse(appendedDeploy + QLatin1String(".path = ") + appendTargetPath);
                    list.append(appendedDeploy);
                }
            } else if (entryList.size() == 1)
                deploymentList.append(CopyItem(Option::fixPathToLocalOS(info.absolutePath() + "/" + entryList.at(0)) ,
                    Option::fixPathToLocalOS(addPath + "/" + entryList.at(0))));
        }
    }
}
