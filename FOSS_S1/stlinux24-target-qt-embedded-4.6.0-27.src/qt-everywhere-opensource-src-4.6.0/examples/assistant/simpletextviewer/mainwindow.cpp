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

#include <QtGui>

#include "mainwindow.h"
#include "findfiledialog.h"

//! [0]
MainWindow::MainWindow()
{
    textViewer = new QTextEdit;
    textViewer->setReadOnly(true);
    QFile file("documentation/intro.html");
    if (file.open(QIODevice::ReadOnly))
        textViewer->setHtml(file.readAll());

    setCentralWidget(textViewer);

    createActions();
    createMenus();

    initializeAssistant();

    setWindowTitle(tr("Simple Text Viewer"));
    resize(750, 400);
}
//! [0]

//! [1]
void MainWindow::closeEvent(QCloseEvent *)
{
    if (assistantClient)
        assistantClient->closeAssistant();
}
//! [1]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Text Viewer"),
                         tr("This example demonstrates how to use\n" \
                            "Qt Assistant as help system for your\n" \
                            "own application."));
}

//! [2]
void MainWindow::assistant()
{
    assistantClient->showPage(QLibraryInfo::location(QLibraryInfo::ExamplesPath) +
                                              QDir::separator() +
                                              "assistant/simpletextviewer/documentation/index.html");
}
//! [2]

//! [3]
void MainWindow::open()
{
    FindFileDialog dialog(textViewer, assistantClient);
    dialog.exec();
}
//! [3]

void MainWindow::createActions()
{
    assistantAct = new QAction(tr("Help Contents"), this);
    assistantAct->setShortcuts(QKeySequence::HelpContents);
    connect(assistantAct, SIGNAL(triggered()), this, SLOT(assistant()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(tr("Ctrl+C"));
    connect(clearAct, SIGNAL(triggered()), textViewer, SLOT(clear()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(clearAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(assistantAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);


    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(helpMenu);
}

//! [4]
void MainWindow::initializeAssistant()
{
    assistantClient = new QAssistantClient(QLibraryInfo::location(QLibraryInfo::BinariesPath), this);

    QStringList arguments;
    arguments << "-profile" << QString("documentation") + QDir::separator() + QString("simpletextviewer.adp");
    assistantClient->setArguments(arguments);
}
//! [4]
