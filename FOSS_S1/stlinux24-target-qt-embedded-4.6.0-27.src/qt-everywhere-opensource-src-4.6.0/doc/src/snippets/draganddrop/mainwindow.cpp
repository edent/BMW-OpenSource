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

#include "dragwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QFrame *centralWidget = new QFrame(this);

    QLabel *mimeTypeLabel = new QLabel(tr("MIME types:"), centralWidget);
    mimeTypeCombo = new QComboBox(centralWidget);

    QLabel *dataLabel = new QLabel(tr("Amount of data (bytes):"), centralWidget);
    dragWidget = new DragWidget(centralWidget);

    connect(dragWidget, SIGNAL(mimeTypes(const QStringList &)),
            this, SLOT(setMimeTypes(const QStringList &)));
    connect(dragWidget, SIGNAL(dragResult(const QString &)),
            this, SLOT(setDragResult(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(mimeTypeLabel);
    mainLayout->addWidget(mimeTypeCombo);
    mainLayout->addSpacing(32);
    mainLayout->addWidget(dataLabel);
    mainLayout->addWidget(dragWidget);

    statusBar();
    dragWidget->setData(QString("text/plain"), QByteArray("Hello world"));
    setCentralWidget(centralWidget);
    setWindowTitle(tr("Drag and Drop"));
}

void MainWindow::setDragResult(const QString &actionText)
{
    statusBar()->showMessage(actionText);
}

void MainWindow::setMimeTypes(const QStringList &types)
{
    mimeTypeCombo->clear();
    mimeTypeCombo->addItems(types);
}
