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
#include "qtcesterconnection.h"
#include <transfer_global.h>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

extern void debugOutput(const QString& text, int level);

#pragma warning(disable:4996)

#define END_ERROR(s, a) \
            if(a) qDebug() << a; \
            _freeSocket(s); \
            return false;

QtCesterConnection::QtCesterConnection()
        : AbstractRemoteConnection()
{
}

QtCesterConnection::~QtCesterConnection()
{
}

bool QtCesterConnection::connect(QVariantList&)
{
    // We connect with each command, so this is always true
    // The command itself will fail then
    connected = true;
    return true;
}

void QtCesterConnection::disconnect()
{
    connected = false;
}

bool QtCesterConnection::isConnected() const
{
    return connected;
}

bool QtCesterConnection::copyFileToDevice(const QString &localSource, const QString &deviceDest, bool failIfExists)
{
    debugOutput( qPrintable(QString::fromLatin1("Copy File: %1 -> %2").arg(localSource).arg(deviceDest)),0);
    QFile localFile(localSource);
    QFileInfo info(localSource);
    if (!localFile.exists() || !localFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open File!";
        return false;
    }
    
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_CREATE_FILE)) {
        END_ERROR(socket, "Could not initialized command");
    }
    
    CreateFileOptions option;
    strcpy(option.fileName, qPrintable(deviceDest));
#ifdef Q_OS_WIN
    // Copy FileTime for update verification
    FILETIME creationTime, accessTime, writeTime;
    HANDLE localHandle = CreateFile(localSource.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if (localHandle != INVALID_HANDLE_VALUE) {
        if (GetFileTime(localHandle, &creationTime, &accessTime, &writeTime)) {
            LocalFileTimeToFileTime(&writeTime, &writeTime);
            option.fileTime = writeTime;
        }
        CloseHandle(localHandle);
    }
    DWORD attributes = GetFileAttributes(localSource.utf16());
    if (attributes != -1 )
        option.fileAttributes = attributes;
#endif
    option.fileSize = info.size();
    option.overwriteExisting = !failIfExists;
    
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send options...");
    }

    if (!_checkResult(socket)) {
        END_ERROR(socket, "Server did not accept configuration");
    }
    
    int bytesWritten = 0;
    const int bufferSize = 1024;
    QByteArray data;
    while (bytesWritten < option.fileSize) {
        data = localFile.read(bufferSize);
        bytesWritten += data.size();
#ifdef Q_OS_WIN
        wprintf( L"%s -> %s (%d / %d) %d %%\r", localSource.utf16() , deviceDest.utf16(),
                 bytesWritten , option.fileSize, (100*bytesWritten)/option.fileSize );
#endif
        if (!_sendData(socket, data.constData(), data.size())) {
            END_ERROR(socket, "Error during file transfer");
        }
        if (!_checkResult(socket)) {
            END_ERROR(socket, "Got some strange result");
        }
    }
#ifdef Q_OS_WIN
    wprintf( L"\n"); // We should jump to next line...
#endif
    if (bytesWritten != option.fileSize) {
        END_ERROR(socket, "Did not send sufficient data");
    }
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::copyDirectoryToDevice(const QString &localSource, const QString &deviceDest, bool recursive)
{
    QTcpSocket* socket = NULL;
    QFileInfo info(localSource);
    if (!info.exists() || !info.isDir()) {
        END_ERROR(socket, "Input directory invalid");
    }
    
    createDirectory(deviceDest, true);
    QDir dir(localSource);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo item, list) {
        QString targetName = deviceDest + QLatin1String("\\") + item.fileName();
        if (item.isDir()) {
            if (recursive) {
                if (!copyDirectoryToDevice(item.absoluteFilePath() , targetName, recursive))
                    return false;
            }
        } else {
            if (!copyFileToDevice(item.absoluteFilePath(), targetName))
                return false;
        }
    }
    return true;
}

bool QtCesterConnection::copyFileFromDevice(const QString &deviceSource, const QString &localDest, bool failIfExists)
{
    QFile targetFile(localDest);
    QTcpSocket* socket = 0;
    if (targetFile.exists() && failIfExists) {
        END_ERROR(socket, "Local file not supposed to be overwritten");
    }
    
    if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        END_ERROR(socket, "Could not open local file for writing");
    }
    
    if (!_initCommand(socket, COMMAND_READ_FILE)) {
        END_ERROR(socket, "Could not establish connection");
    }
    
    ReadFileOptions option;
    strcpy(option.fileName, qPrintable(deviceSource));
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send options");
    }
    
    QByteArray data;
    if (!_receiveData(socket, data)) {
        END_ERROR(socket, "Did not receive any data");
    }
    
    ReadFileReply* reply = (ReadFileReply*) data.data();
    if (!reply->fileValid) {
        END_ERROR(socket, "Requested file invalid");
    }
    
    int fileSize = reply->fileSize;
    int currentSize = 0;
    // ### TODO: make a little bit more error-prone
    do {
        _sendData(socket, COMMAND_SUCCESS, strlen(COMMAND_SUCCESS));
        _receiveData(socket, data);
        currentSize += data.size();
        targetFile.write(data);
    } while(currentSize < fileSize);
    
    _freeSocket(socket);
    targetFile.close();
    return true;
}
 
bool QtCesterConnection::copyDirectoryFromDevice(const QString& /*deviceSource*/
                                                 , const QString& /*localDest*/
                                                 , bool /*recursive*/)
{
    qDebug() << "To be implemented!! Should not be needed for autotest system";
    exit(-1);
    return false;
}

bool QtCesterConnection::copyFile(const QString &srcFile, const QString &destFile, bool failIfExists)
{
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_COPY_FILE)) {
        END_ERROR(socket, "Could not establish connection for copy");
    }
    
    CopyFileOptions option;
    strcpy(option.from, qPrintable(srcFile));
    strcpy(option.to, qPrintable(destFile));
    option.overwriteExisting = !failIfExists;
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send copy options");
    }
    
    if (!_checkResult(socket)) {
        END_ERROR(socket, "Copy failed");
    }
    
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::copyDirectory(const QString &srcDirectory, const QString &destDirectory,
                                      bool recursive)
{
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_COPY_DIRECTORY)) {
        END_ERROR(socket, "Could not establish connection for dir copy");
    }
    
    CopyDirectoryOptions option;
    strcpy(option.from, qPrintable(srcDirectory));
    strcpy(option.to, qPrintable(destDirectory));
    option.recursive = recursive;
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send dir copy options");
    }
    
    if (!_checkResult(socket)) {
        END_ERROR(socket, "Dir Copy failed");
    }
    
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::deleteFile(const QString &fileName)
{
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_DELETE_FILE)) {
        END_ERROR(socket, "Could not establish connection for file deletion");
    }
    
    DeleteFileOptions option;
    strcpy(option.fileName, qPrintable(fileName));
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send file options");
    }
    
    if (!_checkResult(socket)) {
        //END_ERROR(socket, "File Deletion failed");
        // This is actually not an error so ignore it.
    }
    
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::deleteDirectory(const QString &directory, bool recursive, bool failIfContentExists)
{
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_DELETE_DIRECTORY)) {
        END_ERROR(socket, "Could not establish connection for dir deletion");
    }
    
    DeleteDirectoryOptions option;
    strcpy(option.dirName, qPrintable(directory));
    option.recursive = recursive;
    option.failIfContentExists = failIfContentExists;
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send dir options");
    }
    
    if (!_checkResult(socket)) {
        // we do not write an error as this will fail a lot on recursive.
        END_ERROR(socket, 0);
    }
    
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::execute(QString program,
                                 QString arguments,
                                 int timeout,
                                 int *returnValue)
{
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_EXECUTE)) {
        END_ERROR(socket, "Could not establish connection for dir deletion");
    }

    ExecuteOptions options;
    strcpy(options.appName, qPrintable(program));
    QStringList argList = arguments.split(QLatin1Char(' '));
    options.argumentsCount = qMin(argList.size(), MAX_ARGUMENTS);
    options.waitForFinished = true;
    options.timeout = timeout;
    if (!_sendData(socket, (char*) &options, sizeof(options))) {
        END_ERROR(socket, "Could not send dir options");
    }
    if (!_checkResult(socket)) {
        END_ERROR(socket, "Did not receive an answer");
    }

    for (int i=0; i < options.argumentsCount; ++i) {
        char someData[MAX_NAME_LENGTH];
        strcpy(someData, qPrintable(argList[i]));
        if (!_sendData(socket, someData, MAX_NAME_LENGTH)) {
            END_ERROR(socket, "Could not send argument");
        }
        if (!_checkResult(socket)) {
            END_ERROR(socket, "Failure in argument send");
        }
    }

    // trigger the startup
    if (!_sendData(socket, COMMAND_SUCCESS, strlen(COMMAND_SUCCESS))) {
        END_ERROR(socket, "Could not trigger startup");
    }
    
    const int waitTime = 60 * 60 * 1000;
    if (!socket->waitForReadyRead(waitTime)) {
        END_ERROR(socket, "Process timed out");
    }
    
    QByteArray result = socket->readAll();
    if (result != COMMAND_SUCCESS) {
        if (returnValue)
            *returnValue = -1; // just some at least
        END_ERROR(socket, "Application did not start or returned error");
    }
    
    if (returnValue)
        *returnValue = 0;
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::createDirectory(const QString &path, bool deleteBefore)
{
    if (deleteBefore)
        deleteDirectory(path, true, true);
    
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_CREATE_DIRECTORY)) {
        END_ERROR(socket, "Could not establish connection for dir creation");
    }
    
    CreateDirectoryOptions option;
    strcpy(option.dirName, qPrintable(path));
    option.recursively = true;
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send dir options");
    }
    
    if (!_checkResult(socket)) {
        END_ERROR(socket, "Dir creation failed");
    }
    
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::timeStampForLocalFileTime(FILETIME* fTime) const
{
    if (!fTime)
        return false;
    
    FILETIME copyTime = *fTime;
    LocalFileTimeToFileTime(&copyTime, &copyTime);
    
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_TIME_STAMP)) {
        END_ERROR(socket, "Could not establish time stamp connection");
    }
    
    if (!_sendData(socket, (char*) &copyTime, sizeof(copyTime))) {
        END_ERROR(socket, "Could not send stamp time");
    }
    
    QByteArray data;
    if (!_receiveData(socket, data)) {
        END_ERROR(socket, "Did not receive time stamp or connection interrupted");
    }
    
    copyTime = *((FILETIME*)data.data());
    if (copyTime.dwLowDateTime == -1 && copyTime.dwHighDateTime == -1) {
        END_ERROR(socket, "remote Time stamp failed!");
    }
    
    *fTime = copyTime;
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::fileCreationTime(const QString &fileName, FILETIME* deviceCreationTime) const
{
    if (!deviceCreationTime)
        return false;
    
    QTcpSocket* socket = 0;
    if (!_initCommand(socket, COMMAND_FILE_TIME)) {
        END_ERROR(socket, "Could not establish connection for file time access");
    }
    
    FileTimeOptions option;
    strcpy(option.fileName, qPrintable(fileName));
    if (!_sendData(socket, (char*) &option, sizeof(option))) {
        END_ERROR(socket, "Could not send file time name");
    }
    
    QByteArray data;
    if (!_receiveData(socket, data)) {
        END_ERROR(socket, "File Time request failed");
    }
    
    FILETIME* resultTime = (FILETIME*) data.data();
    if (resultTime->dwLowDateTime == -1 && resultTime->dwHighDateTime == -1) {
        END_ERROR(socket, 0);
        debugOutput("Could not access file time", 0);
    }
    
    *deviceCreationTime = *resultTime;
    _freeSocket(socket);
    return true;
}

bool QtCesterConnection::_createSocket(QTcpSocket*& result) const
{
    QTcpSocket* sock = new QTcpSocket();
    QByteArray ipAddress = qgetenv("DEVICE_IP");
    if (ipAddress.isEmpty()) {
        qWarning("Error: You need to have DEVICE_IP set");
        exit(0);
    }
    sock->connectToHost(QHostAddress(QString(ipAddress)), 12145);
    
    if (!sock->waitForConnected()) {
        qDebug() << "connection timeout...";
        result = NULL;
        return false;
    }
    result = sock;
    return true;
}

void QtCesterConnection::_freeSocket(QTcpSocket*& sock) const
{
    if (!sock)
        return;
    if (sock->state() == QAbstractSocket::ConnectedState) {
        sock->disconnectFromHost();
        // seems like no need to wait
        //sock->waitForDisconnected();
    }
    delete sock;
    sock = NULL;
#ifdef Q_OS_WIN
    Sleep(100);
#endif
}

bool QtCesterConnection::_initCommand(QTcpSocket*& sock, const char* command) const
{
    QTcpSocket* socket = NULL;
    if (!_createSocket(socket)) {
        END_ERROR(socket, "Could not connect to server");
    }
    
    if (!_sendData(socket, command, strlen(command)) || 
        !_checkResult(socket)) {
        END_ERROR(socket, "Cound not send command");
    }
    sock = socket;
    return true;
}

bool QtCesterConnection::_sendData(QTcpSocket*& sock, const char* data, int dataSize) const
{
    int amount = sock->write(data, dataSize);
    if (amount != dataSize) {
        fprintf(stderr, "*******COULD NOT SEND ENOUGH DATA*************\n");
    }
    return sock->waitForBytesWritten();
}

bool QtCesterConnection::_receiveData(QTcpSocket*& sock, QByteArray& data) const
{
    if (!sock->waitForReadyRead()) {
        qDebug() << "did not receive any data";
        return false;
    }
    data = sock->readAll();
    return true;
}

bool QtCesterConnection::_checkResult(QTcpSocket*& sock) const
{
    QByteArray response;
    if (!_receiveData(sock, response) || response != COMMAND_SUCCESS)
        return false;
    return true;
}

