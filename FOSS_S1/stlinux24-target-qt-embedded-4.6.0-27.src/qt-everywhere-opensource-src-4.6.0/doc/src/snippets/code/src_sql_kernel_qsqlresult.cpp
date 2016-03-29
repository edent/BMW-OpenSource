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

//! [0]
QSqlQuery q;
q.prepare("insert into test (i1, i2, s) values (?, ?, ?)");

QVariantList col1;
QVariantList col2;
QVariantList col3;

col1 << 1 << 3;
col2 << 2 << 4;
col3 << "hello" << "world";

q.bindValue(0, col1);
q.bindValue(1, col2);
q.bindValue(2, col3);

if (!q.execBatch())
    qDebug() << q.lastError();
//! [0]


//! [1]
QSqlQuery query = ...
QVariant v = query.result()->handle();
if (v.isValid() && qstrcmp(v.typeName(), "sqlite3_stmt*")) {
    // v.data() returns a pointer to the handle
    sqlite3_stmt *handle = *static_cast<sqlite3_stmt **>(v.data());
    if (handle != 0) { // check that it is not NULL
        ...
    }
}
//! [1]


//! [2]
if (v.typeName() == "PGresult*") {
    PGresult *handle = *static_cast<PGresult **>(v.data());
    if (handle != 0) ...
}

if (v.typeName() == "MYSQL_STMT*") {
    MYSQL_STMT *handle = *static_cast<MYSQL_STMT **>(v.data());
    if (handle != 0) ...
}
//! [2]
