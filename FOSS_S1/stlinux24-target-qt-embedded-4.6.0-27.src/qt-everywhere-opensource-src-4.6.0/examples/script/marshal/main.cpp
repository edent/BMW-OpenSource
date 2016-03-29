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

#include <QtCore>
#include <QtScript>

template <class Container>
QScriptValue toScriptValue(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    for (it = begin; it != end; ++it)
        a.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void fromScriptValue(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

typedef QVector<int> IntVector;
typedef QVector<QString> StringVector;

Q_DECLARE_METATYPE(IntVector)
Q_DECLARE_METATYPE(StringVector)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QScriptEngine eng;
    // register our custom types
    qScriptRegisterMetaType<IntVector>(&eng, toScriptValue, fromScriptValue);
    qScriptRegisterMetaType<StringVector>(&eng, toScriptValue, fromScriptValue);

    QScriptValue val = eng.evaluate("[1, 4, 7, 11, 50, 3, 19, 60]");

    fprintf(stdout, "Script array: %s\n", qPrintable(val.toString()));

    IntVector iv = qscriptvalue_cast<IntVector>(val);

    fprintf(stdout, "qscriptvalue_cast to QVector<int>: ");
    for (int i = 0; i < iv.size(); ++i)
        fprintf(stdout, "%s%d", (i > 0) ? "," : "", iv.at(i));
    fprintf(stdout, "\n");

    val = eng.evaluate("[9, 'foo', 46.5, 'bar', 'Qt', 555, 'hello']");

    fprintf(stdout, "Script array: %s\n", qPrintable(val.toString()));

    StringVector sv = qscriptvalue_cast<StringVector>(val);

    fprintf(stdout, "qscriptvalue_cast to QVector<QString>: ");
    for (int i = 0; i < sv.size(); ++i)
        fprintf(stdout, "%s%s", (i > 0) ? "," : "", qPrintable(sv.at(i)));
    fprintf(stdout, "\n");

    return 0;
}
