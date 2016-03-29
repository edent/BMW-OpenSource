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

#include "CeTcpSyncConnection.h"
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo>

static const QString ceTcpSyncProgram = "cetcpsync";
extern void debugOutput(const QString& text, int level);

CeTcpSyncConnection::CeTcpSyncConnection()
        : AbstractRemoteConnection()
        , connected(false)
{
}

CeTcpSyncConnection::~CeTcpSyncConnection()
{
    if (isConnected())
        disconnect();
}

bool CeTcpSyncConnection::connect(QVariantList&)
{
    // We connect with each command, so this is always true
    // The command itself will fail then
    const QString cmd = ceTcpSyncProgram + " noop";
    if (system(qPrintable(cmd)) != 0)
        return false;
    connected = true;
    return true;
}

void CeTcpSyncConnection::disconnect()
{
    connected = false;
}

bool CeTcpSyncConnection::isConnected() const
{
    return connected;
}

inline QString boolToString(bool b)
{
    return b ? "true" : "false";
}

static bool fileTimeFromString(FILETIME& ft, const QString& str)
{
    int idx = str.indexOf("*");
    if (idx <= 0)
        return false;
    bool ok;
    ft.dwLowDateTime = str.left(idx).toULong(&ok);
    if (!ok)
        return false;
    ft.dwHighDateTime = str.mid(idx+1).toULong(&ok);
    return ok;
}

static QString fileTimeToString(FILETIME& ft)
{
    return QString::number(ft.dwLowDateTime) + "*" + QString::number(ft.dwHighDateTime);
}

bool CeTcpSyncConnection::copyFileToDevice(const QString &localSource, const QString &deviceDest, bool failIfExists)
{
    QString cmd = ceTcpSyncProgram + " copyFileToDevice \"" + localSource + "\" \"" + deviceDest + "\" " + boolToString(failIfExists);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::copyDirectoryToDevice(const QString &localSource, const QString &deviceDest, bool recursive)
{
    QString cmd = ceTcpSyncProgram + " copyDirectoryToDevice \"" + localSource + "\" \"" + deviceDest + "\" " + boolToString(recursive);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::copyFileFromDevice(const QString &deviceSource, const QString &localDest, bool failIfExists)
{
    QString cmd = ceTcpSyncProgram + " copyFileFromDevice \"" + deviceSource + "\" \"" + localDest + "\" " + boolToString(failIfExists);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::copyDirectoryFromDevice(const QString &deviceSource, const QString &localDest, bool recursive)
{
    QString cmd = ceTcpSyncProgram + " copyDirectoryFromDevice \"" + deviceSource + "\" \"" + localDest + "\" " + boolToString(recursive);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::copyFile(const QString &srcFile, const QString &destFile, bool failIfExists)
{
    QString cmd = ceTcpSyncProgram + " copyFile \"" + srcFile + "\" \"" + destFile + "\" " + boolToString(failIfExists);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::copyDirectory(const QString &srcDirectory, const QString &destDirectory,
                                        bool recursive)
{
    QString cmd = ceTcpSyncProgram + " copyDirectory \"" + srcDirectory + "\" \"" + destDirectory + "\" " + boolToString(recursive);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::deleteFile(const QString &fileName)
{
    QString cmd = ceTcpSyncProgram + " deleteFile \"" + fileName + "\"";
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::deleteDirectory(const QString &directory, bool recursive, bool failIfContentExists)
{
    QString cmd = ceTcpSyncProgram + " deleteDirectory \"" + directory + "\" " + boolToString(recursive) + " " + boolToString(failIfContentExists);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::execute(QString program, QString arguments, int timeout, int *returnValue)
{
    QString cmd = ceTcpSyncProgram + " execute \"" + program + "\" \"" + arguments + "\" " + QString::number(timeout);
    int exitCode = system(qPrintable(cmd));
    if (returnValue)
        *returnValue = exitCode;
    return true;
}

bool CeTcpSyncConnection::createDirectory(const QString &path, bool deleteBefore)
{
    QString cmd = ceTcpSyncProgram + " createDirectory \"" + path + "\" " + boolToString(deleteBefore);
    return system(qPrintable(cmd)) == 0;
}

bool CeTcpSyncConnection::timeStampForLocalFileTime(FILETIME* fTime) const
{
    QString cmd = ceTcpSyncProgram + " timeStampForLocalFileTime " + fileTimeToString(*fTime) + " >qt_cetcpsyncdata.txt";
    if (system(qPrintable(cmd)) != 0)
        return false;

    QFile file("qt_cetcpsyncdata.txt");
    if (!file.open(QIODevice::ReadOnly))
        return false;

    bool result = fileTimeFromString(*fTime, file.readLine());
    file.close();
    file.remove();
    return result;
}

bool CeTcpSyncConnection::fileCreationTime(const QString &fileName, FILETIME* deviceCreationTime) const
{
    QString cmd = ceTcpSyncProgram + " fileCreationTime \"" + fileName + "\" >qt_cetcpsyncdata.txt";
    if (system(qPrintable(cmd)) != 0)
        return false;

    QFile file("qt_cetcpsyncdata.txt");
    if (!file.open(QIODevice::ReadOnly))
        return false;

    bool result = fileTimeFromString(*deviceCreationTime, file.readLine());
    file.close();
    file.remove();
    return result;
}
