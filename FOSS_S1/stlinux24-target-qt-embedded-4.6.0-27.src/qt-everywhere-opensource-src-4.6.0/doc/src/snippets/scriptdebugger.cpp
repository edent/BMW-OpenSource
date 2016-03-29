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

#include <QtGui>
#include <QtScript>
#include <QtScriptTools>

// Perhaps shpw entire example for getting debugger up with script
int main(int argv, char **args)
{
    QApplication app(argv, args);

     QString fileName("helloscript.qs");
     QFile scriptFile(fileName);
     scriptFile.open(QIODevice::ReadOnly);
     QTextStream stream(&scriptFile);
     QString contents = stream.readAll();
     scriptFile.close();

    QScriptEngine *engine = new QScriptEngine();

    QScriptEngineDebugger *debugger = new QScriptEngineDebugger();
    debugger->attachTo(engine);

    // Set up configuration with only stack and code
    QWidget *widget = new QWidget;
//![0]
    QWidget *codeWindow = debugger->widget(QScriptEngineDebugger::CodeWidget);
    QWidget *stackWidget = debugger->widget(QScriptEngineDebugger::StackWidget);

    QLayout *layout = new QHBoxLayout;
    layout->addWidget(codeWindow);
    layout->addWidget(stackWidget);
//![0]

//![1]
    QAction *continueAction = debugger->action(QScriptEngineDebugger::ContinueAction);
    QAction *stepOverAction = debugger->action(QScriptEngineDebugger::StepOverAction);
    QAction *stepIntoAction = debugger->action(QScriptEngineDebugger::StepIntoAction);

    QToolBar *toolBar = new QToolBar;
    toolBar->addAction(continueAction);
//![1]
    toolBar->addAction(stepOverAction);
    toolBar->addAction(stepIntoAction);

    layout->addWidget(toolBar);
    continueAction->setIcon(QIcon("copy.png"));

    debugger->setAutoShowStandardWindow(false);

    widget->setLayout(layout);
    widget->show();

     QPushButton button;
     QScriptValue scriptButton = engine->newQObject(&button);
     engine->globalObject().setProperty("button", scriptButton);

//![2]
     debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
     engine->evaluate(contents, fileName);
//![2]

    return app.exec();
}

