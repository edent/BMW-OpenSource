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
QDir("/home/user/Documents")
QDir("C:/Documents and Settings")
//! [0]


//! [1]
QDir("images/landscape.png")
//! [1]


//! [2]
QDir("Documents/Letters/Applications").dirName() // "Applications"
QDir().dirName()                                 // "."
//! [2]


//! [3]
QDir directory("Documents/Letters");
QString path = directory.filePath("contents.txt");
QString absolutePath = directory.absoluteFilePath("contents.txt");
//! [3]


//! [4]
QDir dir("example");
if (!dir.exists())
    qWarning("Cannot find the example directory");
//! [4]


//! [5]
QDir dir = QDir::root();                 // "/"
if (!dir.cd("tmp")) {                    // "/tmp"
    qWarning("Cannot find the \"/tmp\" directory");
} else {
    QFile file(dir.filePath("ex1.txt")); // "/tmp/ex1.txt"
    if (!file.open(QIODevice::ReadWrite))
        qWarning("Cannot create the file %s", file.name());
}
//! [5]


//! [6]
QString bin = "/local/bin";         // where /local/bin is a symlink to /usr/bin
QDir binDir(bin);
QString canonicalBin = binDir.canonicalPath();
// canonicalBin now equals "/usr/bin"

QString ls = "/local/bin/ls";       // where ls is the executable "ls"
QDir lsDir(ls);
QString canonicalLs = lsDir.canonicalPath();
// canonicalLS now equals "/usr/bin/ls".
//! [6]


//! [7]
QDir dir("/home/bob");
QString s;

s = dir.relativeFilePath("images/file.jpg");     // s is "images/file.jpg"
s = dir.relativeFilePath("/home/mary/file.txt"); // s is "../mary/file.txt"
//! [7]


//! [8]
QDir::setSearchPaths("icons", QStringList(QDir::homePath() + "/images"));
QDir::setSearchPaths("docs", QStringList(":/embeddedDocuments"));
...
QPixmap pixmap("icons:undo.png"); // will look for undo.png in QDir::homePath() + "/images"
QFile file("docs:design.odf"); // will look in the :/embeddedDocuments resource path
//! [8]


//! [9]
QDir dir("/tmp/root_link");
dir = dir.canonicalPath();
if (dir.isRoot())
    qWarning("It is a root link");
//! [9]


//! [10]
// The current directory is "/usr/local"
QDir d1("/usr/local/bin");
QDir d2("bin");
if (d1 == d2)
    qDebug("They're the same");
//! [10]


//! [11]
// The current directory is "/usr/local"
QDir d1("/usr/local/bin");
d1.setFilter(QDir::Executable);
QDir d2("bin");
if (d1 != d2)
    qDebug("They differ");
//! [11]


//! [12]
C:/Documents and Settings/Username
//! [12]


//! [13]
Q_INIT_RESOURCE(myapp);
//! [13]


//! [14]
inline void initMyResource() { Q_INIT_RESOURCE(myapp); }

namespace MyNamespace
{
    ...

    void myFunction()
    {
        initMyResource();
    }
}
//! [14]


//! [15]
Q_CLEANUP_RESOURCE(myapp);
//! [15]

}
