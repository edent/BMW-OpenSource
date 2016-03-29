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

#include "scriptdebugger.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "*** you must specify a script file to evaluate (try example.qs)\n");
        return(-1);
    }

    QString fileName = QString::fromLatin1(argv[1]);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        fprintf(stderr, "*** failed to open `%s' for reading\n", argv[1]);
        return(-1);
    }

    QScriptEngine engine;
    QString code = QTextStream(&file).readAll();
    file.close();

    fprintf(stdout, "\n*** Welcome to qsdbg. Debugger commands start with a . (period)\n");
    fprintf(stdout, "*** Any other input will be evaluated by the script interpreter.\n");
    fprintf(stdout, "*** Type .help for help.\n\n");

    ScriptDebugger *dbg = new ScriptDebugger(&engine);
    dbg->breakAtNextStatement();

    engine.evaluate(code, fileName);

    return 0;
}
