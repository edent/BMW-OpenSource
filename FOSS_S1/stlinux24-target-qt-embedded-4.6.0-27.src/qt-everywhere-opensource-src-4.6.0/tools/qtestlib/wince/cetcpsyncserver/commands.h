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
#ifndef COMMANDS_INCL
#define COMMANDS_INCL

#include "transfer_global.h"

#include <QtNetwork/QTcpSocket>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <windows.h>

// debug output
#define DEBUG_LEVEL 2
inline void debugOutput(int level, const char* text)
{
    if (level >= DEBUG_LEVEL)
        qDebug() << text;
}

inline void debugOutput(int level, const QString &text)
{
    if (level >= DEBUG_LEVEL)
        qDebug() << text;
}
// Basic abtract command class
class AbstractCommand : public QObject
{
    Q_OBJECT
public:
    AbstractCommand();
    virtual ~AbstractCommand();

    void setSocket(QTcpSocket*);
    QTcpSocket* socket();

    void reportSuccess();
    void reportError();

public slots:
    virtual void dataReceived(QByteArray&);
    virtual void commandFinished();

private slots:
    void _readData();
    void _disconnect();

private:
    QTcpSocket* m_socket;
};

// File Creation class
class CreateFileCommand : public AbstractCommand
{
    Q_OBJECT
public:
    CreateFileCommand();
    ~CreateFileCommand();

public slots:
    void dataReceived(QByteArray&);
    void commandFinished();

private:
    CreateFileOptions m_options;
    QFile m_file;
    int m_dataCount;
};

inline AbstractCommand* instCreateFile() { return new CreateFileCommand(); }

// Directory Creation class
class CreateDirectoryCommand : public AbstractCommand
{
    Q_OBJECT
public:
    CreateDirectoryCommand();
    ~CreateDirectoryCommand();
    
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
};
inline AbstractCommand* instCreateDirectory() { return new CreateDirectoryCommand(); }

// File copy class
class CopyFileCommand : public AbstractCommand
{
    Q_OBJECT
public:
    CopyFileCommand();
    ~CopyFileCommand();
    
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
};
inline AbstractCommand* instCopyFile() { return new CopyFileCommand(); }

// Copy directory class
class CopyDirectoryCommand : public AbstractCommand
{
    Q_OBJECT
public:
    CopyDirectoryCommand();
    ~CopyDirectoryCommand();

public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
private:
    bool copyDir(const QString &from, const QString &to, bool recursive);
};
inline AbstractCommand* instCopyDirectory() { return new CopyDirectoryCommand(); }

// Delete File class
class DeleteFileCommand : public AbstractCommand
{
    Q_OBJECT
public:
    DeleteFileCommand();
    ~DeleteFileCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
};
inline AbstractCommand* instDeleteFile() { return new DeleteFileCommand(); }

// Delete Directory class
class DeleteDirectoryCommand : public AbstractCommand
{
    Q_OBJECT
public:
    DeleteDirectoryCommand();
    ~DeleteDirectoryCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
private:
    bool deleteDirectory(const QString &dirName, bool recursive, bool failIfContentExists);
};
inline AbstractCommand* instDeleteDirectory() { return new DeleteDirectoryCommand(); }

// Execute application class
class ExecuteCommand : public AbstractCommand
{
    Q_OBJECT
public:
    ExecuteCommand();
    ~ExecuteCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
private:
    void _doExecute();
    QString m_program;
    QStringList m_arguments;
    int m_argumentCount;
    bool m_waitFinished;
    int m_timeout;
};
inline AbstractCommand* instExecution() { return new ExecuteCommand(); }

// Read File class
class ReadFileCommand : public AbstractCommand
{
    Q_OBJECT
public:
    ReadFileCommand();
    ~ReadFileCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
private:
    QString m_fileName;
    QFile m_file;
    qint64 m_currentPos;
    qint64 m_fileSize;
};
inline AbstractCommand* instReadFile() { return new ReadFileCommand(); }

// Read Directory class
class ReadDirectoryCommand : public AbstractCommand
{
    Q_OBJECT
public:
    ReadDirectoryCommand();
    ~ReadDirectoryCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
private:
    QString m_dirName;
    QDir m_dir;
    QDirIterator* m_iterator;
};
inline AbstractCommand* instReadDirectory() { return new ReadDirectoryCommand(); }

// Read File Time class
class FileTimeCommand : public AbstractCommand
{
    Q_OBJECT
public:
    FileTimeCommand();
    ~FileTimeCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
};
inline AbstractCommand* instFileTime() { return new FileTimeCommand(); }

// Time stamp class
class TimeStampCommand : public AbstractCommand
{
    Q_OBJECT
public:
    TimeStampCommand();
    ~TimeStampCommand();
public slots:
    void dataReceived(QByteArray&);
    void commandFinished();
};
inline AbstractCommand* instTimeStamp() { return new TimeStampCommand(); }

// Access part
typedef AbstractCommand* (*instantiator)();

struct CommandInfo
{
    CommandInfo(const QString &name, instantiator func) : commandName(name) , commandFunc(func) { }
    QString commandName;
    instantiator commandFunc;
};

inline QList<CommandInfo> availableCommands()
{
    QList<CommandInfo> list;
    list.append(CommandInfo(QLatin1String(COMMAND_CREATE_FILE), instCreateFile));
    list.append(CommandInfo(QLatin1String(COMMAND_CREATE_DIRECTORY), instCreateDirectory));
    list.append(CommandInfo(QLatin1String(COMMAND_COPY_FILE), instCopyFile));
    list.append(CommandInfo(QLatin1String(COMMAND_COPY_DIRECTORY), instCopyDirectory));
    list.append(CommandInfo(QLatin1String(COMMAND_DELETE_FILE), instDeleteFile));
    list.append(CommandInfo(QLatin1String(COMMAND_DELETE_DIRECTORY), instDeleteDirectory));
    list.append(CommandInfo(QLatin1String(COMMAND_EXECUTE), instExecution));
    list.append(CommandInfo(QLatin1String(COMMAND_READ_FILE), instReadFile));
    list.append(CommandInfo(QLatin1String(COMMAND_READ_DIRECTORY), instReadDirectory));
    list.append(CommandInfo(QLatin1String(COMMAND_FILE_TIME), instFileTime));
    list.append(CommandInfo(QLatin1String(COMMAND_TIME_STAMP), instTimeStamp));
    return list;
}

#endif
