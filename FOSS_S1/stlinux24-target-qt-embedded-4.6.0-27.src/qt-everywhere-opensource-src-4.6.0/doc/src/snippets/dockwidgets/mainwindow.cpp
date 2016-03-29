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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Dock Widgets");

    setupDockWindow();
    setupContents();
    setupMenus();

    textBrowser = new QTextBrowser(this);

    connect(headingList, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(updateText(QListWidgetItem *)));

    updateText(headingList->item(0));
    headingList->setCurrentRow(0);
    setCentralWidget(textBrowser);
}

void MainWindow::setupContents()
{
    QFile titlesFile(":/Resources/titles.txt");
    titlesFile.open(QFile::ReadOnly);
    int chapter = 0;

    do {
        QString line = titlesFile.readLine().trimmed();
        QStringList parts = line.split("\t", QString::SkipEmptyParts);
        if (parts.size() != 2)
            break;

        QString chapterTitle = parts[0];
        QString fileName = parts[1];

        QFile chapterFile(fileName);

        chapterFile.open(QFile::ReadOnly);
        QListWidgetItem *item = new QListWidgetItem(chapterTitle, headingList);
        item->setData(Qt::DisplayRole, chapterTitle);
        item->setData(Qt::UserRole, chapterFile.readAll());
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        chapterFile.close();

        chapter++;
    } while (titlesFile.isOpen());

    titlesFile.close();
}

void MainWindow::setupDockWindow()
{
//! [0]
    contentsWindow = new QDockWidget(tr("Table of Contents"), this);
    contentsWindow->setAllowedAreas(Qt::LeftDockWidgetArea
                                  | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, contentsWindow);

    headingList = new QListWidget(contentsWindow);
    contentsWindow->setWidget(headingList);
//! [0]
}

void MainWindow::setupMenus()
{
    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);
}

void MainWindow::updateText(QListWidgetItem *item)
{
    QString text = item->data(Qt::UserRole).toString();
    textBrowser->setHtml(text);
}
