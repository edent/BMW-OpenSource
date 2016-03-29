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
#include "cesdkhandler.h"
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>

void usage()
{
    printf("SDK Scanner - Convenience Tool to setup your environment\n");
    printf("              for crosscompilation to Windows CE\n");
    printf("Options:\n");
    printf("-help                This output\n");
    printf("-list                List all available SDKs\n");
    printf("-sdk <name>          Select specified SDK.\n");
    printf("                     Note: SDK names with spaces need to be\n");
    printf("                     specified in parenthesis\n");
    printf("                     default: Windows Mobile 5.0 Pocket PC SDK (ARMV4I)\n");
    printf("-script <file>       Create a script file which can be launched\n");
    printf("                     to setup your environment for specified SDK\n");
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        usage();
        return 0;
    }
    QString sdkName;
    bool operationList = false;
    QString scriptFile;

    QStringList arguments;
    for (int i=0; i < argc; ++i)
        arguments.append(QLatin1String(argv[i]));
    for (int i=1; i < arguments.size(); ++i) {
        if (arguments[i].toLower() == QLatin1String("-help")) {
            usage();
            return 0;
        } else if (arguments[i].toLower() == QLatin1String("-list")) {
            operationList = true;
        } else if (arguments[i].toLower() == QLatin1String("-sdk")) {
            if (i+1 >= arguments.size()) {
                qWarning("No SDK specified.");
                return -1;
            }
            sdkName = arguments[++i];
        } else if (arguments[i].toLower() == QLatin1String("-script")) {
            if (i+1 >= arguments.size()) {
                qWarning("No scriptfile specified.");
                return -1;
            }
            scriptFile = arguments[++i];
        } else {
            qWarning("Unknown option:%s", qPrintable(arguments[i]));
            usage();
            return -1;
        }
    }

    CeSdkHandler handler;
    if (!handler.parse()) {
        qWarning("Could not find any installed SDK, aborting!");
        return -1;
    }

    QList<CeSdkInfo> list = handler.listAll();

    if (operationList) {
        printf("Available SDKs:\n");
        for (QList<CeSdkInfo>::iterator it = list.begin(); it != list.end(); ++it) {
            printf("SDK Name: ");
            printf(qPrintable(it->name()));
            printf("\n");
        }
        return 0;
    }

    // Check for SDK Name, otherwise use Windows Mobile as default
    if (sdkName.isEmpty()) {
        qWarning("No SDK specified: Defaulting to Windows Mobile 5.0 Pocket PC SDK");
        sdkName = QString::fromLatin1("Windows Mobile 5.0 Pocket PC SDK (ARMV4I)");
    }

    // finally find the given SDK and prompt out the environment to be set
    for (QList<CeSdkInfo>::iterator it = list.begin(); it != list.end(); ++it ) {
        if (sdkName == it->name()) {
            if (!it->isValid()) {
                qWarning("Selected SDK is not valid!");
                return -1;
            } else if (!it->isSupported()) {
                qWarning("Selected SDK is not officially supported and might not work");
            }
            QString binPath, includePath, libPath;
            binPath = QString::fromLatin1("PATH=") + it->binPath();
            includePath = QString::fromLatin1("INCLUDE=") + it->includePath();
            libPath = QString::fromLatin1("LIB=") + it->libPath();
            if (scriptFile.isEmpty()) {
                printf("Please set up your environment with the following paths:\n");
                printf(qPrintable(binPath));
                printf("\n");
                printf(qPrintable(includePath));
                printf("\n");
                printf(qPrintable(libPath));
                printf("\n");
                return 0;
            } else {
                QFile file(scriptFile);
                if (!file.open(QIODevice::WriteOnly)) {
                    qWarning("Could not open target script file");
                    return -1;
                }
                QString content;
                content += QLatin1String("@echo off\n");
                content += QLatin1String("echo Environment Selection:") + sdkName + QLatin1String("\n");
                content += QLatin1String("set ") + binPath + QLatin1String("\n");
                content += QLatin1String("set ") + includePath + QLatin1String("\n");
                content += QLatin1String("set ") + libPath + QLatin1String("\n");
                file.write(content.toLatin1());
                return 0;
            }
        }
    }
    qWarning("Could not find specified SDK: %s" , qPrintable(sdkName));
    return -1;
}
