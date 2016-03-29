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

#include "dialog.h"

//! [Dialog constructor part1]
Dialog::Dialog()
{
    desktopGeometry = QApplication::desktop()->availableGeometry(0);

    setWindowTitle(tr("SIP Dialog Example"));
    QScrollArea *scrollArea = new QScrollArea(this);
    QGroupBox *groupBox = new QGroupBox(scrollArea);
    groupBox->setTitle(tr("SIP Dialog Example"));
    QGridLayout *gridLayout = new QGridLayout(groupBox);
    groupBox->setLayout(gridLayout);
//! [Dialog constructor part1]

//! [Dialog constructor part2]
    QLineEdit* lineEdit = new QLineEdit(groupBox);
    lineEdit->setText(tr("Open and close the SIP"));
    lineEdit->setMinimumWidth(220);

    QLabel* label = new QLabel(groupBox);
    label->setText(tr("This dialog resizes if the SIP is opened"));
    label->setMinimumWidth(220);

    QPushButton* button = new QPushButton(groupBox);
    button->setText(tr("Close Dialog"));
    button->setMinimumWidth(220);
//! [Dialog constructor part2]

//! [Dialog constructor part3]
    if (desktopGeometry.height() < 400)
        gridLayout->setVerticalSpacing(80);
    else
        gridLayout->setVerticalSpacing(150);

    gridLayout->addWidget(label);
    gridLayout->addWidget(lineEdit);
    gridLayout->addWidget(button);
//! [Dialog constructor part3]

//! [Dialog constructor part4]
    scrollArea->setWidget(groupBox);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(scrollArea);
    setLayout(layout);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//! [Dialog constructor part4]

//! [Dialog constructor part5]
    connect(button, SIGNAL(clicked()),
        qApp, SLOT(closeAllWindows()));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), 
        this, SLOT(desktopResized(int)));
}
//! [Dialog constructor part5]

//! [desktopResized() function]
void Dialog::desktopResized(int screen)
{
    if (screen != 0)
        return;
    reactToSIP();
}
//! [desktopResized() function]

//! [reactToSIP() function]
void Dialog::reactToSIP()
{
    QRect availableGeometry = QApplication::desktop()->availableGeometry(0);

    if (desktopGeometry != availableGeometry) {
        if (windowState() | Qt::WindowMaximized)
            setWindowState(windowState() & ~Qt::WindowMaximized);

        setGeometry(availableGeometry);
    }

    desktopGeometry = availableGeometry;
}
//! [reactToSIP() function]
