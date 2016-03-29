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

#include "window.h"
#include "environment.h"
#include "context2d.h"
#include "qcontext2dcanvas.h"
#include <QHBoxLayout>
#include <QListWidget>
#include <QDir>
#include <QMessageBox>

#ifndef QT_NO_SCRIPTTOOLS
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScriptEngineDebugger>
#endif

static QString scriptsDir()
{
    if (QFile::exists("./scripts"))
        return "./scripts";
    return ":/scripts";
}

//! [0]
Window::Window(QWidget *parent)
    : QWidget(parent)
#ifndef QT_NO_SCRIPTTOOLS
      , m_debugger(0), m_debugWindow(0)
#endif
{
    m_env = new Environment(this);
    QObject::connect(m_env, SIGNAL(scriptError(QScriptValue)),
                     this, SLOT(reportScriptError(QScriptValue)));

    Context2D *context = new Context2D(this);
    context->setSize(150, 150);
    m_canvas = new QContext2DCanvas(context, m_env, this);
    m_canvas->setFixedSize(context->size());
    m_canvas->setObjectName("tutorial");
    m_env->addCanvas(m_canvas);
//! [0]

#ifndef QT_NO_SCRIPTTOOLS
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(m_canvas);
    m_debugButton = new QPushButton(tr("Run in Debugger"));
    connect(m_debugButton, SIGNAL(clicked()), this, SLOT(runInDebugger()));
    vbox->addWidget(m_debugButton);
#endif

    QHBoxLayout *hbox = new QHBoxLayout(this);
    m_view = new QListWidget(this);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hbox->addWidget(m_view);
#ifndef QT_NO_SCRIPTTOOLS
    hbox->addLayout(vbox);
#else
    hbox->addWidget(m_canvas);
#endif

//! [1]
    QDir dir(scriptsDir());
    QFileInfoList entries = dir.entryInfoList(QStringList() << "*.js");
    for (int i = 0; i < entries.size(); ++i)
        m_view->addItem(entries.at(i).fileName());
    connect(m_view, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(selectScript(QListWidgetItem*)));
//! [1]

    setWindowTitle(tr("Context 2D"));
}

//! [2]
void Window::selectScript(QListWidgetItem *item)
{
    QString fileName = item->text();
    runScript(fileName, /*debug=*/false);
}
//! [2]

void Window::reportScriptError(const QScriptValue &error)
{
    QMessageBox::warning(this, tr("Context 2D"), tr("Line %0: %1")
                         .arg(error.property("lineNumber").toInt32())
                         .arg(error.toString()));
}

#ifndef QT_NO_SCRIPTTOOLS
//! [3]
void Window::runInDebugger()
{
    QListWidgetItem *item = m_view->currentItem();
    if (item) {
        QString fileName = item->text();
        runScript(fileName, /*debug=*/true);
    }
}
//! [3]
#endif

//! [4]
void Window::runScript(const QString &fileName, bool debug)
{
    QFile file(scriptsDir() + "/" + fileName);
    file.open(QIODevice::ReadOnly);
    QString contents = file.readAll();
    file.close();
    m_env->reset();

#ifndef QT_NO_SCRIPTTOOLS
    if (debug) {
        if (!m_debugger) {
            m_debugger = new QScriptEngineDebugger(this);
            m_debugWindow = m_debugger->standardWindow();
            m_debugWindow->setWindowModality(Qt::ApplicationModal);
            m_debugWindow->resize(1280, 704);
        }
        m_debugger->attachTo(m_env->engine());
        m_debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    } else {
        if (m_debugger)
            m_debugger->detach();
    }
#else
    Q_UNUSED(debug);
#endif

    QScriptValue ret = m_env->evaluate(contents, fileName);

#ifndef QT_NO_SCRIPTTOOLS
    if (m_debugWindow)
        m_debugWindow->hide();
#endif

    if (ret.isError())
        reportScriptError(ret);
}
//! [4]
