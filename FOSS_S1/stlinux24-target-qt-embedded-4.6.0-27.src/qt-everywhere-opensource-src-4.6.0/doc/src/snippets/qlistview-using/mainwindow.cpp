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
#include "model.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *itemsMenu = new QMenu(tr("&Items"));

    insertAction = itemsMenu->addAction(tr("&Insert Item"));
    removeAction = itemsMenu->addAction(tr("&Remove Item"));
    QAction *ascendingAction = itemsMenu->addAction(tr("Sort in &Ascending Order"));
    QAction *descendingAction = itemsMenu->addAction(tr("Sort in &Descending Order"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(itemsMenu);

    QStringList strings;
    strings << tr("Oak") << tr("Fir") << tr("Pine") << tr("Birch")
            << tr("Hazel") << tr("Redwood") << tr("Sycamore") << tr("Chestnut");
    model = new StringListModel(strings, this);
/*  For convenient quoting:
    QListView *listView = new QListView(this);
*/
    listView = new QListView(this);
    listView->setModel(model);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(ascendingAction, SIGNAL(triggered()), this, SLOT(sortAscending()));
    connect(descendingAction, SIGNAL(triggered()), this, SLOT(sortDescending()));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(insertItem()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));
    connect(listView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateMenus(const QModelIndex &)));

    updateMenus(listView->selectionModel()->currentIndex());

    setCentralWidget(listView);
    setWindowTitle(tr("List View"));
}

void MainWindow::sortAscending()
{
    model->sort(0, Qt::AscendingOrder);
}

void MainWindow::sortDescending()
{
    model->sort(0, Qt::DescendingOrder);
}

void MainWindow::insertItem()
{
    QModelIndex currentIndex = listView->currentIndex();
    if (!currentIndex.isValid())
        return;

    QString itemText = QInputDialog::getText(this, tr("Insert Item"),
        tr("Input text for the new item:"));

    if (itemText.isNull())
        return;

    if (model->insertRow(currentIndex.row(), QModelIndex())) {
        QModelIndex newIndex = model->index(currentIndex.row(), 0, QModelIndex());
        model->setData(newIndex, itemText, Qt::EditRole);

        QString toolTipText = tr("Tooltip:") + itemText;
        QString statusTipText = tr("Status tip:") + itemText;
        QString whatsThisText = tr("What's This?:") + itemText;
        model->setData(newIndex, toolTipText, Qt::ToolTipRole);
        model->setData(newIndex, toolTipText, Qt::StatusTipRole);
        model->setData(newIndex, whatsThisText, Qt::WhatsThisRole);
    }
}

void MainWindow::removeItem()
{
    QModelIndex currentIndex = listView->currentIndex();
    if (!currentIndex.isValid())
        return;

    model->removeRow(currentIndex.row(), QModelIndex());
}

void MainWindow::updateMenus(const QModelIndex &current)
{
    insertAction->setEnabled(current.isValid());
    removeAction->setEnabled(current.isValid());
}
