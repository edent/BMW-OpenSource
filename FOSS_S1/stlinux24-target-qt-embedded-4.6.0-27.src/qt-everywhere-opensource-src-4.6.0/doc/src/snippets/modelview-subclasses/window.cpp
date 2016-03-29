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

#include <QAction>
#include <QDataStream>
#include <QMenu>
#include <QMenuBar>
#include <QFile>
#include <QFileDialog>
#include <QListView>

#include "window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Model/View example");

    setupModelView();

    QAction *openAction = new QAction(tr("&Open"), this);
    QAction *quitAction = new QAction(tr("E&xit"), this);
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAction);
    fileMenu->addAction(quitAction);
    menuBar()->addMenu(fileMenu);

    connect(openAction, SIGNAL(triggered()), this, SLOT(selectOpenFile()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setCentralWidget(view);
}

void MainWindow::setupModelView()
{
    model = new LinearModel(this);
    view = new LinearView(this);
    view->setModel(model);
}

void MainWindow::selectOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select a file to open"), "", tr("Sound files (*.wav)"));
    
    if (!fileName.isEmpty())
        openFile(fileName);
}

void MainWindow::openFile(const QString &fileName)
{
    QFile file(fileName);
    int length = file.size();

    if (file.open(QFile::ReadOnly)) {
        model->removeRows(0, model->rowCount());

        int rows = (length - 0x2c)/2;
        model->insertRows(0, rows);

        // Perform some dodgy tricks to extract the data from the file.
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::LittleEndian);

        Q_INT16 left;
        Q_INT16 right;

        for (int row = 0; row < rows; ++row) {
            QModelIndex index = model->index(row);

            stream >> left >> right;
            model->setData(index, int(left / 256));
        }
    }
}
