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

#ifndef SCRIPTDEBUGGER_H
#define SCRIPTDEBUGGER_H

#include <QtCore/qlist.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE
class QScriptEngine;
class QScriptValue;
class QTextStream;
QT_END_NAMESPACE

class ScriptDebuggerPrivate;
class ScriptDebugger
{
public:
    ScriptDebugger(QScriptEngine *engine);
    virtual ~ScriptDebugger();

    void breakAtNextStatement();

    void setBreakpoint(const QString &fileName, int lineNumber);
    void setBreakpoint(const QString &functionName, const QString &fileName = QString());
    void setBreakpoint(const QScriptValue &function);

    QTextStream *inputStream() const;
    void setInputStream(QTextStream *inputStream);

    QTextStream *outputStream() const;
    void setOutputStream(QTextStream *outputStream);

    QTextStream *errorStream() const;
    void setErrorStream(QTextStream *errorStream);

protected:
    ScriptDebugger(QScriptEngine *engine, ScriptDebuggerPrivate &dd);
    ScriptDebuggerPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(ScriptDebugger)
    Q_DISABLE_COPY(ScriptDebugger)
};

#endif // SCRIPTDEBUGGER_H
