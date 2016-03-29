/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "bytearrayprototype.h"
#include <QtScript/QScriptEngine>

Q_DECLARE_METATYPE(QByteArray*)

ByteArrayPrototype::ByteArrayPrototype(QObject *parent)
    : QObject(parent)
{
}

ByteArrayPrototype::~ByteArrayPrototype()
{
}

//! [0]
QByteArray *ByteArrayPrototype::thisByteArray() const
{
    return qscriptvalue_cast<QByteArray*>(thisObject().data());
}
//! [0]

void ByteArrayPrototype::chop(int n)
{
    thisByteArray()->chop(n);
}

bool ByteArrayPrototype::equals(const QByteArray &other)
{
    return *thisByteArray() == other;
}

QByteArray ByteArrayPrototype::left(int len) const
{
    return thisByteArray()->left(len);
}

//! [1]
QByteArray ByteArrayPrototype::mid(int pos, int len) const
{
    return thisByteArray()->mid(pos, len);
}

QScriptValue ByteArrayPrototype::remove(int pos, int len)
{
    thisByteArray()->remove(pos, len);
    return thisObject();
}
//! [1]

QByteArray ByteArrayPrototype::right(int len) const
{
    return thisByteArray()->right(len);
}

QByteArray ByteArrayPrototype::simplified() const
{
    return thisByteArray()->simplified();
}

QByteArray ByteArrayPrototype::toBase64() const
{
    return thisByteArray()->toBase64();
}

QByteArray ByteArrayPrototype::toLower() const
{
    return thisByteArray()->toLower();
}

QByteArray ByteArrayPrototype::toUpper() const
{
    return thisByteArray()->toUpper();
}

QByteArray ByteArrayPrototype::trimmed() const
{
    return thisByteArray()->trimmed();
}

void ByteArrayPrototype::truncate(int pos)
{
    thisByteArray()->truncate(pos);
}

QString ByteArrayPrototype::toLatin1String() const
{
    return QString::fromLatin1(*thisByteArray());
}

//! [2]
QScriptValue ByteArrayPrototype::valueOf() const
{
    return thisObject().data();
}
//! [2]
