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
#ifndef TRANSFER_GLOBAL_H
#define TRANSFER_GLOBAL_H

#include <QtCore/qglobal.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define SERVER_PORT 12145

#define MAX_NAME_LENGTH 512
#define MAX_ARGUMENTS 10

// Defines for commands sent/received
#define COMMAND_CREATE_FILE "CREATEFILE"
#define COMMAND_CREATE_DIRECTORY "CREATEDIR"
#define COMMAND_COPY_FILE "COPYFILE"
#define COMMAND_COPY_DIRECTORY "COPYDIR"
#define COMMAND_DELETE_FILE "DELETEFILE"
#define COMMAND_DELETE_DIRECTORY "DELETEDIR"
#define COMMAND_EXECUTE "EXECUTE"
#define COMMAND_QUIT_SERVER "QUIT"
#define COMMAND_FILE_TIME "FILETIME"
#define COMMAND_TIME_STAMP "TIMESTAMP"

// Report back commands
#define COMMAND_SUCCESS "SUCCESS"
#define COMMAND_ERROR "ERROR"

// Defines for commands that send data back to requester
#define COMMAND_READ_FILE "READFILE"
#define COMMAND_READ_DIRECTORY "READDIR"

#include <QtCore/qglobal.h>
// Option-Structures for commands

struct CreateFileOptions
{
    char fileName[MAX_NAME_LENGTH];
#ifdef Q_OS_WIN
    FILETIME fileTime;
    DWORD fileAttributes;
#endif
    int fileSize;
    bool overwriteExisting;
};

struct CreateDirectoryOptions
{
    char dirName[MAX_NAME_LENGTH];
    bool recursively; // in case of \foo\bar create \foo if it does not exist
};

struct CopyFileOptions
{
    char from[MAX_NAME_LENGTH];
    char to[MAX_NAME_LENGTH];
    bool overwriteExisting;
};

struct CopyDirectoryOptions
{
    char from[MAX_NAME_LENGTH];
    char to[MAX_NAME_LENGTH];
    bool recursive;
};

struct DeleteFileOptions
{
    char fileName[MAX_NAME_LENGTH];
};

struct DeleteDirectoryOptions
{
    char dirName[MAX_NAME_LENGTH];
    bool recursive;
    bool failIfContentExists;
};

struct ExecuteOptions
{
    char appName[MAX_NAME_LENGTH];
    int argumentsCount;
    bool waitForFinished;
    int timeout;
};

struct ReadFileOptions
{
    char fileName[MAX_NAME_LENGTH];
};

struct ReadFileReply
{
    qint64 fileSize;
    bool fileValid;
};

struct ReadDirectoryOptions
{
    char dirName[MAX_NAME_LENGTH];
};

struct ReadDirectoryItem
{
    char name[MAX_NAME_LENGTH];
    qint64 size;
    bool isDirectory;
    bool hasMore;
};

#define FileTimeOptions ReadFileOptions

struct ReadDirectoryReply
{
    bool entryValid;
    int itemCount; // might change during iteration
};
#endif
