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

#include <QtScript>
#include "bytearrayclass.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QScriptEngine eng;

    ByteArrayClass *baClass = new ByteArrayClass(&eng);
    eng.globalObject().setProperty("ByteArray", baClass->constructor());

    qDebug() << "ba = new ByteArray(4):" << eng.evaluate("ba = new ByteArray(4)").toString();
    qDebug() << "ba instanceof ByteArray:" << eng.evaluate("ba instanceof ByteArray").toBool();
    qDebug() << "ba.length:" << eng.evaluate("ba.length").toNumber();
    qDebug() << "ba[1] = 123; ba[1]:" << eng.evaluate("ba[1] = 123; ba[1]").toNumber();
    qDebug() << "ba[7] = 224; ba.length:" << eng.evaluate("ba[7] = 224; ba.length").toNumber();
    qDebug() << "for-in loop:" << eng.evaluate("result = '';\n"
                                               "for (var p in ba) {\n"
                                               "  if (result.length > 0)\n"
                                               "    result += ', ';\n"
                                               "  result += '(' + p + ',' + ba[p] + ')';\n"
                                               "} result").toString();
    qDebug() << "ba.toBase64():" << eng.evaluate("b64 = ba.toBase64()").toString();
    qDebug() << "ba.toBase64().toLatin1String():" << eng.evaluate("b64.toLatin1String()").toString();
    qDebug() << "ba.valueOf():" << eng.evaluate("ba.valueOf()").toString();
    qDebug() << "ba.chop(2); ba.length:" << eng.evaluate("ba.chop(2); ba.length").toNumber();
    qDebug() << "ba2 = new ByteArray(ba):" << eng.evaluate("ba2 = new ByteArray(ba)").toString();
    qDebug() << "ba2.equals(ba):" << eng.evaluate("ba2.equals(ba)").toBool();
    qDebug() << "ba2.equals(new ByteArray()):" << eng.evaluate("ba2.equals(new ByteArray())").toBool();

    return 0;
}
