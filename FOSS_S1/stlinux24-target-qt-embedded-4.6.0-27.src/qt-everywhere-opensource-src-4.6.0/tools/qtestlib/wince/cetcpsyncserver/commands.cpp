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
#include "commands.h"
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#ifdef Q_OS_WINCE
#include <windows.h>
#endif

/////////////////////////////////////////////////////
//       Abstract Command Implementation           //
/////////////////////////////////////////////////////
AbstractCommand::AbstractCommand()
: m_socket(0)
{
}

AbstractCommand::~AbstractCommand()
{
}

void AbstractCommand::reportSuccess()
{
    m_socket->write(COMMAND_SUCCESS, strlen(COMMAND_SUCCESS));
    m_socket->waitForBytesWritten();
}

void AbstractCommand::reportError()
{
    m_socket->write(COMMAND_ERROR, strlen(COMMAND_ERROR));
    m_socket->waitForBytesWritten();
}

void AbstractCommand::dataReceived(QByteArray&)
{
    debugOutput(1, "AbstractCommand::dataReceived NOT SUPPOSED TO BE HERE");
}

void AbstractCommand::commandFinished()
{
    debugOutput(1, "AbstractCommand::commandFinished()NOT SUPPOSED TO BE HERE");
}

void AbstractCommand::setSocket(QTcpSocket* socket) 
{
    debugOutput(0, "AbstractCommand::setSocket()");
    Q_ASSERT(socket);
    m_socket = socket;
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(_readData()));
    reportSuccess();
}

QTcpSocket* AbstractCommand::socket() 
{ 
    return m_socket; 
}

void AbstractCommand::_readData()
{
    QByteArray arr = m_socket->readAll();
    dataReceived(arr);
}

void AbstractCommand::_disconnect()
{
}

/////////////////////////////////////////////////////
//       Create File Command Implementation        //
/////////////////////////////////////////////////////
CreateFileCommand::CreateFileCommand()
: m_dataCount(0)
{
    debugOutput(0, "CreateFileCommand::CreateFileCommand");
    m_options.fileSize= -1;
}

CreateFileCommand::~CreateFileCommand()
{
    debugOutput(0, "CreateFileCommand::~CreateFileCommand");
    if (m_file.isOpen()) {
        fprintf(stderr, "****************FILE IS STILL OPENED AND HAVENT FINISHED WRITING**********************\n");
        fprintf(stderr, "Current: %d Expected: %d\n", m_dataCount , m_options.fileSize);
        m_file.close();
    }
}

void CreateFileCommand::dataReceived(QByteArray &data)
{
    bool successful = true;
    // If we haven't received the options yet
    if (m_options.fileSize == -1) {
        CreateFileOptions* opt = (CreateFileOptions*) data.data();
        memcpy(&m_options , opt , sizeof(CreateFileOptions));

        if (QFileInfo(QString::fromLatin1(m_options.fileName)).exists()) {
            if (m_options.overwriteExisting) {
#ifdef Q_OS_WINCE
                SetFileAttributes(QFileInfo(m_options.fileName).absoluteFilePath().utf16(), FILE_ATTRIBUTE_NORMAL);
#endif
                QFile::remove(m_options.fileName);
            } else
                successful = false;
        }
        m_file.setFileName(QString::fromLatin1(m_options.fileName));
        if (!m_file.open(QIODevice::WriteOnly))
            successful = false;
        else
            debugOutput(3, QString::fromLatin1("Creating file: %1").arg(m_options.fileName));
    } else { // write buffer on disc
        if (!m_file.isOpen())
            return;
        m_file.write(data);
        m_dataCount += data.size();
        if (m_dataCount >= m_options.fileSize) {
            // We do not care about more data than announced
            m_file.close();
        }
    }
    
    if (successful)
        reportSuccess();
    else
        reportError();
}

void CreateFileCommand::commandFinished()
{
    debugOutput(0, "CreateFileCommand::commandFinished");
#ifdef Q_OS_WIN
    // We need to set the file attributes for intelligent time comparisons
    QString tmpFile = QString::fromLatin1(m_options.fileName);
    HANDLE handle = CreateFile(tmpFile.utf16(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle != INVALID_HANDLE_VALUE) {
        SetFileTime(handle, &(m_options.fileTime), NULL, NULL);
        CloseHandle(handle);
    }
    SetFileAttributes(tmpFile.utf16(), m_options.fileAttributes);
#endif
}

/////////////////////////////////////////////////////
//    Create Directory Command Implementation      //
/////////////////////////////////////////////////////
CreateDirectoryCommand::CreateDirectoryCommand()
        : AbstractCommand()
{
    debugOutput(0, "CreateDirectoryCommand::CreateDirectoryCommand");
}

CreateDirectoryCommand::~CreateDirectoryCommand()
{
    debugOutput(0, "CreateDirectoryCommand::~CreateDirectoryCommand()");
}

void CreateDirectoryCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "CreateDirectoryCommand::dataReceived()");
    CreateDirectoryOptions* options = (CreateDirectoryOptions*) data.data();
    debugOutput(3, QString::fromLatin1("Creating directory: %1").arg(options->dirName));
    bool success = true;
    QDir dir;
    if (options->recursively)
        success = dir.mkpath(options->dirName);
    else
        success = dir.mkdir(options->dirName);
    
    if (success)
        reportSuccess();
    else
        reportError();
}

void CreateDirectoryCommand::commandFinished()
{
    debugOutput(0, "CreateDirectoryCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//        Copy File Command Implementation         //
/////////////////////////////////////////////////////
CopyFileCommand::CopyFileCommand()
        : AbstractCommand()
{
    debugOutput(0, "CopyFileCommand::CopyFileCommand()");
}

CopyFileCommand::~CopyFileCommand()
{
    debugOutput(0, "CopyFileCommand::~CopyFileCommand()");
}

void CopyFileCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "CopyFileCommand::dataReceived()");
    CopyFileOptions* options = (CopyFileOptions*) data.data();
    debugOutput(3, QString::fromLatin1("Copy File: %1 ->  %2").arg(options->from).arg(options->to));
    bool success = true;
    if (QFileInfo(options->to).exists()) {
        if (options->overwriteExisting)
            QFile::remove(options->to);
        else
            success = false;
    }
    if (success)
        if (!QFile::copy(options->from , options->to))
            success = false;
    
    if (success)
        reportSuccess();
    else
        reportError();
}

void CopyFileCommand::commandFinished()
{
    debugOutput(0, "CopyFileCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//      Copy Directory Command Implementation      //
/////////////////////////////////////////////////////
CopyDirectoryCommand::CopyDirectoryCommand()
        : AbstractCommand()
{
    debugOutput(0, "CopyDirectoryCommand::CopyDirectoryCommand()");
}

CopyDirectoryCommand::~CopyDirectoryCommand()
{
    debugOutput(0, "CopyDirectoryCommand::~CopyDirectoryCommand()");
}

void CopyDirectoryCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "CopyDirectoryCommand::dataReceived()");
    CopyDirectoryOptions* options = (CopyDirectoryOptions*) data.data();
    debugOutput(3, QString::fromLatin1("Copy Directory: %1 %2").arg(options->from).arg(options->to));
    if (copyDir(QLatin1String(options->from) , QLatin1String(options->to) , options->recursive))
        reportSuccess();
    else
        reportError();
}

void CopyDirectoryCommand::commandFinished()
{
    debugOutput(0, "CopyDirectoryCommand::commandFinished()");
}

bool CopyDirectoryCommand::copyDir(const QString &from, const QString &to, bool recursive)
{
    QDir().mkpath(to);
    QDir sourceDir(from);
    QDir destDir(to);
    QStringList entries = sourceDir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach (QString item , entries) {
        QString itemFrom = sourceDir.absoluteFilePath(item);
        QString itemTo = destDir.absoluteFilePath(item);
        if (QFileInfo(item).isDir()) {
            if (recursive && !copyDir(itemFrom, itemTo, recursive))
                return false;
        } else {
            if (!QFile::copy(itemFrom, itemTo))
                return false;
        }
    }
    return true;
}

/////////////////////////////////////////////////////
//        Delete File Command Implementation       //
/////////////////////////////////////////////////////
DeleteFileCommand::DeleteFileCommand()
        : AbstractCommand()
{
    debugOutput(0, "DeleteFileCommand::DeleteFileCommand()");
}

DeleteFileCommand::~DeleteFileCommand()
{
    debugOutput(0, "DeleteFileCommand::~DeleteFileCommand()");
}

void DeleteFileCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "DeleteFileCommand::dataReceived()");
    DeleteFileOptions* options = (DeleteFileOptions*) data.data();
    debugOutput(3, QString::fromLatin1("Delete File: %1").arg(options->fileName));
    bool success = true;
    QFile file(options->fileName);
    if (file.exists()) {
#ifdef Q_OS_WINCE
        SetFileAttributes(QFileInfo(options->fileName).absoluteFilePath().utf16(), FILE_ATTRIBUTE_NORMAL);
#endif
        success = file.remove();
    } else
        success = false;

    if (success)
        reportSuccess();
    else
        reportError();
}

void DeleteFileCommand::commandFinished()
{
    debugOutput(0, "DeleteFileCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//     Delete Directory Command Implementation     //
/////////////////////////////////////////////////////
DeleteDirectoryCommand::DeleteDirectoryCommand()
        : AbstractCommand()
{
    debugOutput(0, "DeleteDirectoryCommand::DeleteDirectoryCommand()");
}

DeleteDirectoryCommand::~DeleteDirectoryCommand()
{
    debugOutput(0, "DeleteDirectoryCommand::~DeleteDirectoryCommand()");
}

void DeleteDirectoryCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "DeleteDirectoryCommand::dataReceived()");
    DeleteDirectoryOptions* options = (DeleteDirectoryOptions*) data.data();
    debugOutput(3, QString::fromLatin1("Delete directory: %1").arg(options->dirName));
    if (deleteDirectory(QLatin1String(options->dirName), options->recursive, options->failIfContentExists))
        reportSuccess();
    else
        reportError();
}

void DeleteDirectoryCommand::commandFinished()
{
    debugOutput(0, "DeleteDirectoryCommand::commandFinished()");
}

bool DeleteDirectoryCommand::deleteDirectory(const QString &dirName, bool recursive, bool failIfContentExists)
{
    QDir dir(dirName);
    if (!dir.exists())
        return false;

    QStringList itemList = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    if (itemList.size() > 0 && failIfContentExists)
        return false;

    foreach (QString item, itemList) {
        QString itemName = dir.absoluteFilePath(item);
        if (QFileInfo(itemName).isDir()) {
            if (recursive && !deleteDirectory(itemName, recursive, failIfContentExists))
                return false;
        } else {
            if (!dir.remove(item))
                return false;
        }
    }
    QString lastName = dir.dirName();
    dir.cdUp();
    dir.rmpath(lastName);
    return true;
}

/////////////////////////////////////////////////////
//         Execute Command Implementation          //
/////////////////////////////////////////////////////
ExecuteCommand::ExecuteCommand()
        : AbstractCommand()
        , m_argumentCount(0)
        , m_timeout(-1)
{
    debugOutput(0, "ExecuteCommand::ExecuteCommand()");
}

ExecuteCommand::~ExecuteCommand()
{
    debugOutput(0, "ExecuteCommand::~ExecuteCommand()");
}

void ExecuteCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "ExecuteCommand::dataReceived()");
    
    if (m_argumentCount == 0) {
        ExecuteOptions* options = (ExecuteOptions*) data.data();
        if (!QFileInfo(options->appName).exists()) {
            debugOutput(1, "Error execute: application does not exist");
            reportError();
            return;
        }
    
        m_program = QLatin1String(options->appName);
        m_argumentCount = options->argumentsCount;
        m_waitFinished = options->waitForFinished;
        m_timeout = options->timeout;
        if (m_argumentCount == 0)
            m_argumentCount = -1; // to trigger startup on next receive
        reportSuccess();
    } else if (m_arguments.size() < m_argumentCount) {
        m_arguments += data;
        reportSuccess();
    } else { // do the execution
        if (data == COMMAND_SUCCESS)
            _doExecute();
    }
}

void ExecuteCommand::_doExecute()
{
    debugOutput(0, "ExecuteCommand::_doExecute()");
    debugOutput(3, QString::fromLatin1("Execute: %1 %2").arg(m_program).arg(m_arguments.join(" ")));
    if (m_waitFinished) {
        QProcess process;
        process.start(m_program, m_arguments);
        if (process.waitForFinished(m_timeout) == false || process.exitCode() < 0)
            reportError();
        else
            reportSuccess();
    } else {
        if (QProcess::startDetached(m_program, m_arguments))
            reportSuccess();
        else
            reportError();
    }
}
void ExecuteCommand::commandFinished()
{
    debugOutput(0,"ExecuteCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//           Read File Implementation              //
/////////////////////////////////////////////////////
ReadFileCommand::ReadFileCommand()
        : AbstractCommand()
        , m_currentPos(0)
{
    debugOutput(0, "ReadFileCommand::ReadFileCommand()");
    m_fileName.clear();
}

ReadFileCommand::~ReadFileCommand()
{
    debugOutput(0, "ReadFileCommand::~ReadFileCommand()");
    if (m_file.isOpen())
        m_file.close();
}

void ReadFileCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "ReadFileCommand::dataReceived()");
    if (m_fileName.isEmpty()) {
        ReadFileOptions* option = (ReadFileOptions*) data.data();
        m_fileName = QLatin1String(option->fileName);
        QFileInfo info(m_fileName);
        m_file.setFileName(m_fileName);
        ReadFileReply reply;
        if (!info.exists() || !info.isFile() || !m_file.open(QIODevice::ReadOnly))
            reply.fileValid = false;
        else
            reply.fileValid = true;
        reply.fileSize = info.size();
        m_fileSize = reply.fileSize;
        socket()->write((char*) &reply, sizeof(reply));
        debugOutput(3, QString::fromLatin1("Reading file: %1").arg(m_fileName));
    } else {
        QTcpSocket* sock = socket(); // design failure???
        if (data != COMMAND_SUCCESS || m_currentPos >= m_fileSize) {
            sock->disconnectFromHost();
            return;
        }
        const int bufferSize = 1024;
        QByteArray buffer = m_file.read(bufferSize);
        m_currentPos += buffer.size();
        sock->write(buffer);
        sock->waitForBytesWritten();
    }
}

void ReadFileCommand::commandFinished()
{
    debugOutput(0, "ReadFileCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//        Read Directory Implementation            //
/////////////////////////////////////////////////////
ReadDirectoryCommand::ReadDirectoryCommand()
        : AbstractCommand()
        , m_iterator(0)
{
    debugOutput(0, "ReadDirectoryCommand::ReadDirectoryCommand");
    m_dirName.clear();
}

ReadDirectoryCommand::~ReadDirectoryCommand()
{
    debugOutput(0, "ReadDirectoryCommand::~ReadDirectoryCommand()");
    delete m_iterator;
}

void ReadDirectoryCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "ReadDirectoryCommand::dataReceived()");
    QTcpSocket* sock = socket();
    if (m_dirName.isEmpty()) {
        ReadDirectoryOptions* option = (ReadDirectoryOptions*) data.data();
        QFileInfo info(QLatin1String(option->dirName));
        debugOutput(3, QString::fromLatin1("Reading Directory entries: %1").arg(option->dirName));
        ReadDirectoryReply reply;
        if (!info.exists() || !info.isDir()) {
            reply.itemCount = -1;
            reply.entryValid = false;
        } else {
            m_dirName = QLatin1String(option->dirName);
            m_dir.setPath(m_dirName);
            m_iterator = new QDirIterator(m_dir);
            reply.itemCount = m_dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot).size();
            reply.entryValid = true;
        }
        sock->write((char*) &reply, sizeof(reply));
        sock->waitForBytesWritten();
    } else {
        if (data != COMMAND_SUCCESS) {
            qDebug() << "Something went wrong in the meantime";
            return;
        }
        ReadDirectoryItem reply;
        if (m_iterator->hasNext()) {
            m_iterator->next();
            QFileInfo info = m_iterator->fileInfo();
            strcpy(reply.name, qPrintable(info.absoluteFilePath()));
            reply.isDirectory = info.isDir();
            if (!reply.isDirectory)
                reply.size = info.size();
        }
        reply.hasMore = m_iterator->hasNext();
        sock->write((char*) &reply, sizeof(reply));
        sock->waitForBytesWritten();
    }
}

void ReadDirectoryCommand::commandFinished()
{
    debugOutput(0, "ReadDirectoryCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//           File Time Implementation              //
/////////////////////////////////////////////////////
FileTimeCommand::FileTimeCommand()
        : AbstractCommand()
{
    debugOutput(0, "FileTimeCommand::FileTimeCommand()");
}

FileTimeCommand::~FileTimeCommand()
{
    debugOutput(0, "FileTimeCommand::~FileTimeCommand()");
}

void FileTimeCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "FileTimeCommand::dataReceived()");
    FileTimeOptions* option = (FileTimeOptions*) data.data();

    FILETIME resultTime;
    resultTime.dwLowDateTime = -1;
    resultTime.dwHighDateTime = -1;
    
#ifdef Q_OS_WIN
    QString fileName = QLatin1String(option->fileName);
    HANDLE deviceHandle = CreateFile(fileName.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    debugOutput(3, QString::fromLatin1("Asking FileTime: %1").arg(fileName));
    if (deviceHandle != INVALID_HANDLE_VALUE) {
        FILETIME deviceCreationTime;
        if (GetFileTime(deviceHandle, &deviceCreationTime, NULL, NULL)) {
            resultTime = deviceCreationTime;
        }
        CloseHandle(deviceHandle);
    }
#endif
    QTcpSocket* sock = socket();
    sock->write((char*) &resultTime, sizeof(resultTime));
    sock->waitForBytesWritten();
}

void FileTimeCommand::commandFinished()
{
    debugOutput(0, "FileTimeCommand::commandFinished()");
}

/////////////////////////////////////////////////////
//           Time Stamp Implementation             //
/////////////////////////////////////////////////////
TimeStampCommand::TimeStampCommand()
        : AbstractCommand()
{
    debugOutput(0, "TimeStampCommand::TimeStampCommand()");
}

TimeStampCommand::~TimeStampCommand()
{
    debugOutput(0, "TimeStampCommand::~TimeStampCommand()");
}

void TimeStampCommand::dataReceived(QByteArray &data)
{
    debugOutput(0, "TimeStampCommand::dataReceived()");
    FILETIME resultTime;
    resultTime.dwLowDateTime = -1;
    resultTime.dwHighDateTime = -1;
    
#ifdef Q_OS_WIN
    FILETIME stampTime = *((FILETIME*)data.data());
    
    QString tmpFile = QString::fromLatin1("\\qt_tmp_ftime_convert");
    HANDLE remoteHandle = CreateFile(tmpFile.utf16(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (remoteHandle != INVALID_HANDLE_VALUE) {
        if (!SetFileTime(remoteHandle, &stampTime, NULL, NULL)) {
            CloseHandle(remoteHandle);
        } else {
            CloseHandle(remoteHandle);
            remoteHandle = CreateFile(tmpFile.utf16(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
            if (remoteHandle != INVALID_HANDLE_VALUE) {
                if (GetFileTime(remoteHandle, &stampTime, NULL, NULL))
                    resultTime = stampTime;
                CloseHandle(remoteHandle);
                DeleteFile(tmpFile.utf16());
            }
        }
    }
    debugOutput(3, QString::fromLatin1("Asking TimeStamp"));
#endif
    QTcpSocket* sock = socket();
    sock->write((char*) &resultTime, sizeof(resultTime));
    sock->waitForBytesWritten();
}

void TimeStampCommand::commandFinished()
{
    debugOutput(0, "TimeStampCommand::commandFinished()");
}
