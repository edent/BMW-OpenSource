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

#include "scriptbreakpointmanager.h"

ScriptBreakpointManager::ScriptBreakpointManager()
{
}

ScriptBreakpointManager::~ScriptBreakpointManager()
{
}

bool ScriptBreakpointManager::hasBreakpoints() const
{
    return !breakpoints.isEmpty();
}

int ScriptBreakpointManager::setBreakpoint(const QString &fileName, int lineNumber)
{
    breakpoints.append(ScriptBreakpointInfo(fileName, lineNumber));
    return breakpoints.size() - 1;
}

int ScriptBreakpointManager::setBreakpoint(const QString &functionName, const QString &fileName)
{
    breakpoints.append(ScriptBreakpointInfo(functionName, fileName));
    return breakpoints.size() - 1;
}

int ScriptBreakpointManager::setBreakpoint(const QScriptValue &function)
{
    breakpoints.append(ScriptBreakpointInfo(function));
    return breakpoints.size() - 1;
}

void ScriptBreakpointManager::removeBreakpoint(int id)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id] = ScriptBreakpointInfo();
}

int ScriptBreakpointManager::findBreakpoint(const QString &fileName, int lineNumber) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::File)
            continue;
        if (brk.fileName == fileName && brk.lineNumber == lineNumber)
            return i;
    }
    return -1;
}

int ScriptBreakpointManager::findBreakpoint(const QString &functionName, const QString &fileName) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::FunctionName)
            continue;
        if (brk.functionName == functionName && brk.fileName == fileName)
            return i;
    }
    return -1;
}

int ScriptBreakpointManager::findBreakpoint(const QScriptValue &function) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::Function)
            continue;
        if (brk.function.strictlyEquals(function))
            return i;
    }
    return -1;
}

bool ScriptBreakpointManager::isBreakpointEnabled(int id) const
{
    return breakpoints.value(id).enabled;
}

void ScriptBreakpointManager::setBreakpointEnabled(int id, bool enabled)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].enabled = enabled;
}

QString ScriptBreakpointManager::breakpointCondition(int id) const
{
    return breakpoints.value(id).condition;
}

void ScriptBreakpointManager::setBreakpointCondition(int id, const QString &expression)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].condition = expression;
}

int ScriptBreakpointManager::breakpointIgnoreCount(int id) const
{
    return breakpoints.value(id).ignoreCount;
}

void ScriptBreakpointManager::setBreakpointIgnoreCount(int id, int ignoreCount)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].ignoreCount = ignoreCount;
}

bool ScriptBreakpointManager::isBreakpointSingleShot(int id) const
{
    return breakpoints.value(id).singleShot;
}

void ScriptBreakpointManager::setBreakpointSingleShot(int id, bool singleShot)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].singleShot = singleShot;
}
