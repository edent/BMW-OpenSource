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

#include <QApplication>
#include <QBuffer>
#include <QPalette>

static void main_snippet()
{
//! [0]
    QBuffer buffer;
    char ch;

    buffer.open(QBuffer::ReadWrite);
    buffer.write("Qt rocks!");
    buffer.seek(0);
    buffer.getChar(&ch);  // ch == 'Q'
    buffer.getChar(&ch);  // ch == 't'
    buffer.getChar(&ch);  // ch == ' '
    buffer.getChar(&ch);  // ch == 'r'
//! [0]
}

static void write_datastream_snippets()
{
//! [1]
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << QApplication::palette();
//! [1]
}

static void read_datastream_snippets()
{
    QByteArray byteArray;

//! [2]
    QPalette palette;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    in >> palette;
//! [2]
}

static void bytearray_ptr_ctor_snippet()
{
//! [3]
    QByteArray byteArray("abc");
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    buffer.seek(3);
    buffer.write("def", 3);
    buffer.close();
    // byteArray == "abcdef"
//! [3]
}

static void setBuffer_snippet()
{
//! [4]
    QByteArray byteArray("abc");
    QBuffer buffer;
    buffer.setBuffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    buffer.seek(3);
    buffer.write("def", 3);
    buffer.close();
    // byteArray == "abcdef"
//! [4]
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    main_snippet();
    bytearray_ptr_ctor_snippet();
    write_datastream_snippets();
    read_datastream_snippets();
    setBuffer_snippet();
    return 0;
}
