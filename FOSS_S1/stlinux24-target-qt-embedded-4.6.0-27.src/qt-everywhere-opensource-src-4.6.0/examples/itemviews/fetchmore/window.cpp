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
#include "filelistmodel.h"
#include "window.h"

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    FileListModel *model = new FileListModel(this);
    model->setDirPath(QLibraryInfo::location(QLibraryInfo::PrefixPath));

    QLabel *label = new QLabel(tr("&Directory:"));
    QLineEdit *lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);

    QListView *view = new QListView;
    view->setModel(model);

    logViewer = new QTextBrowser;
    logViewer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            model, SLOT(setDirPath(const QString &)));
    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            logViewer, SLOT(clear()));
    connect(model, SIGNAL(numberPopulated(int)),
            this, SLOT(updateLog(int)));
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(lineEdit, 0, 1);
    layout->addWidget(view, 1, 0, 1, 2);
    layout->addWidget(logViewer, 2, 0, 1, 2);

    setLayout(layout);
    setWindowTitle(tr("Fetch More Example"));
}

void Window::updateLog(int number)
{
    logViewer->append(tr("%1 items added.").arg(number));
}
