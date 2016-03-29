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

#include "activesyncconnection.h"
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo>
#include <rapi.h>

extern void debugOutput(const QString& text, int level);

ActiveSyncConnection::ActiveSyncConnection()
        : AbstractRemoteConnection()
        , connected(false)
{
}

ActiveSyncConnection::~ActiveSyncConnection()
{
    if (isConnected())
        disconnect();
}

bool ActiveSyncConnection::connect(QVariantList&)
{
    if (connected)
        return true;
    connected = false;
    RAPIINIT init;
    init.cbSize = sizeof(init);
    if (CeRapiInitEx(&init) != S_OK)
        return connected;

    DWORD res;
    res = WaitForMultipleObjects(1,&(init.heRapiInit),true, 5000);
    if ((res == -1) || (res == WAIT_TIMEOUT) || (init.hrRapiInit != S_OK))
        return connected;

    connected = true;
    return connected;
}

void ActiveSyncConnection::disconnect()
{
    connected = false;
    CeRapiUninit();
}

bool ActiveSyncConnection::isConnected() const
{
    return connected;
}

bool ActiveSyncConnection::copyFileToDevice(const QString &localSource, const QString &deviceDest, bool failIfExists)
{
    if (failIfExists) {
        CE_FIND_DATA search;
        HANDLE searchHandle = CeFindFirstFile(deviceDest.utf16(), &search);
        if (searchHandle != INVALID_HANDLE_VALUE) {
            CeFindClose(searchHandle);
            return false;
        }
    }

    QFile file(localSource);
    if (!file.exists())
        return false;
    if (!file.open(QIODevice::ReadOnly)) {
        debugOutput(QString::fromLatin1("  Could not open source file"),2);
        if (file.size() == 0) {
            // Create an empy file
            deleteFile(deviceDest);
            HANDLE deviceHandle = CeCreateFile(deviceDest.utf16(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            if (deviceHandle != INVALID_HANDLE_VALUE) {
                CeCloseHandle(deviceHandle);
                return true;
            }
        } else {
            qWarning("Could not open %s: %s", qPrintable(localSource), qPrintable(file.errorString()));
        }
        return false;
    }

    deleteFile(deviceDest);
    HANDLE deviceHandle = CeCreateFile(deviceDest.utf16(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
        qWarning("Could not create %s: %s", qPrintable(deviceDest), strwinerror(CeGetLastError()).constData());
        return false;
    }

    DWORD written = 0;
    int currentPos = 0;
    int size = file.size();
    DWORD toWrite = 0;
    const int bufferSize = 65000;
    QByteArray data;
    data.reserve(bufferSize);
    while (currentPos < size) {
        data = file.read(bufferSize);
        if (data.size() <= 0) {
            wprintf( L"Error while reading file!\n");
            return false;
        }
        if (size - currentPos > bufferSize )
            toWrite = bufferSize;
        else
            toWrite = size - currentPos;
        if (toWrite == 0)
            break;
        if (!CeWriteFile(deviceHandle, data.data() , toWrite, &written, NULL)) {
            qWarning("Could not write to %s: %s", qPrintable(deviceDest), strwinerror(CeGetLastError()).constData());
            return false;
        }
        currentPos += written;
        data.clear();
        wprintf( L"%s -> %s (%d / %d) %d %%\r", localSource.utf16() , deviceDest.utf16(), currentPos , size, (100*currentPos)/size );
    }
    wprintf(L"\n");

    // Copy FileTime for update verification
    FILETIME creationTime, accessTime, writeTime;
    HANDLE localHandle = CreateFile(localSource.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if (localHandle != INVALID_HANDLE_VALUE) {
        if (GetFileTime(localHandle, &creationTime, &accessTime, &writeTime)) {
            LocalFileTimeToFileTime(&writeTime, &writeTime);
            if (!CeSetFileTime(deviceHandle, &writeTime, NULL, NULL)) {
                debugOutput(QString::fromLatin1("  Could not write time values"), 0);
            }
        }
        CloseHandle(localHandle);
    }
    CeCloseHandle(deviceHandle);

    DWORD attributes = GetFileAttributes(localSource.utf16());
    if (attributes != -1 )
        CeSetFileAttributes(deviceDest.utf16(), attributes);
    return true;
}

bool ActiveSyncConnection::copyDirectoryToDevice(const QString &localSource, const QString &deviceDest, bool recursive)
{
    QDir dir(localSource);
    if (!dir.exists())
        return false;

    deleteDirectory(deviceDest, recursive);
    CeCreateDirectory(deviceDest.utf16(), NULL);
    foreach(QString entry, dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QString source = localSource + "\\" + entry;
        QString target = deviceDest + "\\" + entry;
        QFileInfo info(source);
        if (info.isDir()) {
            if (recursive) {
                if (!copyDirectoryToDevice(source, target, recursive))
                    return false;
            }
        } else {
            if (!copyFileToDevice(source, target))
                return false;
        }
    }
    return true;
}

bool ActiveSyncConnection::copyFileFromDevice(const QString &deviceSource, const QString &localDest, bool failIfExists)
{
    QFile target(localDest);
    if (failIfExists && target.exists()) {
        debugOutput(QString::fromLatin1("  Not allowed to overwrite file"), 2);
        return false;
    }

    if (target.exists())
        target.remove();

    HANDLE deviceHandle = CeCreateFile(deviceSource.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
        debugOutput(QString::fromLatin1("  Could not open file on device"), 2);
        return false;
    }

    DWORD fileSize = CeGetFileSize( deviceHandle, NULL );
    if (fileSize == -1) {
        debugOutput(QString::fromLatin1("  Could not stat filesize of remote file"), 2);
        CeCloseHandle(deviceHandle);
        return false;
    }

    if (!target.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        debugOutput(QString::fromLatin1("  Could not open local file for writing"), 2);
        CeCloseHandle(deviceHandle);
        return false;
    }

    int bufferSize = 65000;
    char *buffer = (char*) malloc(bufferSize);
    DWORD bufferRead = 0;
    DWORD bufferWritten = 0;
    bool readUntilEnd = false;
    while(CeReadFile(deviceHandle, buffer, bufferSize, &bufferRead, NULL)) {
        if (bufferRead == 0) {
            readUntilEnd = true;
            break;
        }
        target.write(buffer, bufferRead);
        bufferWritten += bufferRead;
        wprintf(L"%s -> %s (%d / %d) %d %%\r", deviceSource.utf16(), localDest.utf16(), bufferWritten, fileSize, (100*bufferWritten)/fileSize);
    }
    wprintf(L"\n");

    if (!readUntilEnd) {
        debugOutput(QString::fromLatin1("  an error occured during copy"), 2);
        return false;
    }

    CeCloseHandle(deviceHandle);
    return true;
}

bool ActiveSyncConnection::copyDirectoryFromDevice(const QString &deviceSource, const QString &localDest, bool recursive)
{
    if (!QDir(localDest).exists() && !QDir(localDest).mkpath(QDir(localDest).absolutePath())) {
        debugOutput(QString::fromLatin1("  Could not create local path"), 2);
    }

    QString searchArg = deviceSource + "\\*";
    CE_FIND_DATA data;
    HANDLE searchHandle = CeFindFirstFile(searchArg.utf16(), &data);
    if (searchHandle == INVALID_HANDLE_VALUE) {
        // We return true because we might be in a recursive call
        // where nothing is to copy and the copy process
        // might still be correct
        return true;
    }

    do {
        QString srcFile = deviceSource + "\\" + QString::fromWCharArray(data.cFileName);
        QString destFile = localDest + "\\" + QString::fromWCharArray(data.cFileName);
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (recursive && !copyDirectoryFromDevice(srcFile, destFile, recursive)) {
                wprintf(L"Copy of subdirectory(%s) failed\n", srcFile.utf16());
                return false;
            }
        } else {
            copyFileFromDevice(srcFile, destFile, false);
        }
    } while(CeFindNextFile(searchHandle, &data));
    CeFindClose(searchHandle);
    return true;
}

bool ActiveSyncConnection::copyFile(const QString &srcFile, const QString &destFile, bool failIfExists)
{
    return CeCopyFile(QDir::toNativeSeparators(srcFile).utf16(),
                      QDir::toNativeSeparators(destFile).utf16(), failIfExists);
}

bool ActiveSyncConnection::copyDirectory(const QString &srcDirectory, const QString &destDirectory,
                                      bool recursive)
{
    CeCreateDirectory(destDirectory.utf16(), NULL);
    QString searchArg = srcDirectory + "\\*";
    CE_FIND_DATA data;
    HANDLE searchHandle = CeFindFirstFile(searchArg.utf16(), &data);
    if (searchHandle == INVALID_HANDLE_VALUE) {
        // We return true because we might be in a recursive call
        // where nothing is to copy and the copy process
        // might still be correct
        return true;
    }

    do {
        QString srcFile = srcDirectory + "\\" + QString::fromWCharArray(data.cFileName);
        QString destFile = destDirectory + "\\" + QString::fromWCharArray(data.cFileName);
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (recursive && !copyDirectory(srcFile, destFile, recursive)) {
                wprintf(L"Copy of subdirectory(%s) failed\n", srcFile.utf16());
                return false;
            }
        } else {
            debugOutput(QString::fromLatin1("Copy %1 -> %2\n").arg(srcFile).arg(destFile), 0);
            CeCopyFile(srcFile.utf16(), destFile.utf16(), false);
        }
    } while(CeFindNextFile(searchHandle, &data));
    CeFindClose(searchHandle);
    return true;
}

bool ActiveSyncConnection::deleteFile(const QString &fileName)
{
    CeSetFileAttributes(fileName.utf16(), FILE_ATTRIBUTE_NORMAL);
    return CeDeleteFile(fileName.utf16());
}

bool ActiveSyncConnection::deleteDirectory(const QString &directory, bool recursive, bool failIfContentExists)
{
    HANDLE hFind;
    CE_FIND_DATA FindFileData;
    QString FileName = directory + "\\*";
    hFind = CeFindFirstFile(FileName.utf16(), &FindFileData);
    if( hFind == INVALID_HANDLE_VALUE )
        return CeRemoveDirectory(directory.utf16());

    if (failIfContentExists)
        return false;

    do {
        QString FileName = directory + "\\" + QString::fromWCharArray(FindFileData.cFileName);
        if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (recursive)
                if (!deleteDirectory(FileName, recursive, failIfContentExists))
                    return false;
        } else {
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                CeSetFileAttributes(FileName.utf16(), FILE_ATTRIBUTE_NORMAL);
            if( !CeDeleteFile(FileName.utf16()) )
                break;
        }
    } while(CeFindNextFile(hFind,&FindFileData));
    CeFindClose(hFind);

    return CeRemoveDirectory(directory.utf16());
}

bool ActiveSyncConnection::execute(QString program, QString arguments, int timeout, int *returnValue)
{
    if (!isConnected()) {
        qWarning("Cannot execute, connect to device first!");
        return false;
    }

    PROCESS_INFORMATION* pid = new PROCESS_INFORMATION;
    bool result = false;
    if (timeout != 0) {
        // If we want to wait, we have to use CeRapiInvoke, as CeCreateProcess has no way to wait
        // until the process ends. The lib must have been build and also deployed already.
        if (!isConnected() && !connect())
            return false;

        QString dllLocation = "\\Windows\\QtRemote.dll";
        QString functionName = "qRemoteLaunch";

        DWORD outputSize;
        BYTE* output;
        IRAPIStream *stream;
        int returned = 0;
        DWORD error = 0;
        HRESULT res = CeRapiInvoke(dllLocation.utf16(), functionName.utf16(), 0, 0, &outputSize, &output, &stream, 0);
        if (S_OK != res) {
            DWORD ce_error = CeGetLastError();
            if (S_OK != ce_error) {
                qWarning("Error invoking %s on %s: %s", qPrintable(functionName),
                    qPrintable(dllLocation), strwinerror(ce_error).constData());
            } else {
                qWarning("Error: %s on %s unexpectedly returned %d", qPrintable(functionName),
                    qPrintable(dllLocation), res);
            }
        } else {
            DWORD written;
            int strSize = program.length();
            if (S_OK != stream->Write(&strSize, sizeof(strSize), &written)) {
                qWarning("   Could not write appSize to process");
                return false;
            }
            if (S_OK != stream->Write(program.utf16(), program.length()*sizeof(wchar_t), &written)) {
                qWarning("   Could not write appName to process");
                return false;
            }
            strSize = arguments.length();
            if (S_OK != stream->Write(&strSize, sizeof(strSize), &written)) {
                qWarning("   Could not write argumentSize to process");
                return false;
            }
            if (S_OK != stream->Write(arguments.utf16(), arguments.length()*sizeof(wchar_t), &written)) {
                qWarning("   Could not write arguments to process");
                return false;
            }
            if (S_OK != stream->Write(&timeout, sizeof(timeout), &written)) {
                qWarning("   Could not write waiting option to process");
                return false;
            }

            if (S_OK != stream->Read(&returned, sizeof(returned), &written)) {
                qWarning("   Could not access return value of process");
            }
            if (S_OK != stream->Read(&error, sizeof(error), &written)) {
                qWarning("   Could not access error code");
            }

            if (error) {
                qWarning("Error on target: %s", strwinerror(error).constData());
                result = false;
            }
            else {
                result = true;
            }
        }

        if (returnValue)
            *returnValue = returned;
    } else {
        // We do not need to invoke another lib etc, if we are not interested in results anyway...
        result = CeCreateProcess(program.utf16(), arguments.utf16(), 0, 0, false, 0, 0, 0, 0, pid);
    }
    return result;
}

bool ActiveSyncConnection::createDirectory(const QString &path, bool deleteBefore)
{
    if (deleteBefore)
        deleteDirectory(path);
    QStringList separated = path.split(QLatin1Char('\\'));
    QString current = QLatin1String("\\");
    bool result;
    for (int i=1; i < separated.size(); ++i) {
        current += separated.at(i);
        result = CeCreateDirectory(current.utf16(), NULL);
        current += QLatin1String("\\");
    }
    return result;
}

bool ActiveSyncConnection::timeStampForLocalFileTime(FILETIME* fTime) const
{
    QString tmpFile = QString::fromLatin1("\\qt_tmp_ftime_convert");
    HANDLE remoteHandle = CeCreateFile(tmpFile.utf16(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (remoteHandle == INVALID_HANDLE_VALUE)
        return false;

    LocalFileTimeToFileTime(fTime, fTime);

    if (!CeSetFileTime(remoteHandle, fTime, NULL, NULL)) {
        CeCloseHandle(remoteHandle);
        return false;
    }

    CeCloseHandle(remoteHandle);
    remoteHandle = CeCreateFile(tmpFile.utf16(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (remoteHandle == INVALID_HANDLE_VALUE)
        return false;
    if (!CeGetFileTime(remoteHandle, fTime, NULL, NULL)) {
        CeCloseHandle(remoteHandle);
        return false;
    }

    CeCloseHandle(remoteHandle);
    CeDeleteFile(tmpFile.utf16());
    return true;
}

bool ActiveSyncConnection::fileCreationTime(const QString &fileName, FILETIME* deviceCreationTime) const
{
    HANDLE deviceHandle = CeCreateFile(fileName.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if (deviceHandle == INVALID_HANDLE_VALUE)
        return false;

    bool result = true;
    if (!CeGetFileTime(deviceHandle, deviceCreationTime, NULL, NULL))
        result = false;

    CeCloseHandle(deviceHandle);
    return result;
}
