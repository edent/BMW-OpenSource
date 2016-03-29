/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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


void wrapInFunction()
{

//! [0]
QProcess builder;
builder.setProcessChannelMode(QProcess::MergedChannels);
builder.start("make", QStringList() << "-j2");

if (!builder.waitForFinished())
    qDebug() << "Make failed:" << builder.errorString();
else
    qDebug() << "Make output:" << builder.readAll();
//! [0]


//! [1]
QProcess more;
more.start("more");
more.write("Text to display");
more.closeWriteChannel();
// QProcess will emit readyRead() once "more" starts printing
//! [1]


//! [2]
command1 | command2
//! [2]


//! [3]
QProcess process1;
QProcess process2;

process1.setStandardOutputProcess(&process2);

process1.start("command1");
process2.start("command2");
//! [3]


//! [4]
class SandboxProcess : public QProcess
{
    ...
 protected:
     void setupChildProcess();
    ...
};

void SandboxProcess::setupChildProcess()
{
    // Drop all privileges in the child process, and enter
    // a chroot jail.
#if defined Q_OS_UNIX
    ::setgroups(0, 0);
    ::chroot("/etc/safe");
    ::chdir("/");
    ::setgid(safeGid);
    ::setuid(safeUid);
    ::umask(0);
#endif
}

//! [4]


//! [5]
QProcess process;
process.start("del /s *.txt");
// same as process.start("del", QStringList() << "/s" << "*.txt");
...
//! [5]


//! [6]
QProcess process;
process.start("dir \"My Documents\"");
//! [6]


//! [7]
QProcess process;
process.start("dir \"\"\"My Documents\"\"\"");
//! [7]


//! [8]
QStringList environment = QProcess::systemEnvironment();
// environment = {"PATH=/usr/bin:/usr/local/bin",
//                "USER=greg", "HOME=/home/greg"}
//! [8]

}
