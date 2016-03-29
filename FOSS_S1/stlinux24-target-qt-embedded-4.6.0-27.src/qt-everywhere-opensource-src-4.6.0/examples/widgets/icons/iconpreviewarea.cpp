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

#include "iconpreviewarea.h"

//! [0]
IconPreviewArea::IconPreviewArea(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    stateLabels[0] = createHeaderLabel(tr("Off"));
    stateLabels[1] = createHeaderLabel(tr("On"));
    Q_ASSERT(NumStates == 2);

    modeLabels[0] = createHeaderLabel(tr("Normal"));
    modeLabels[1] = createHeaderLabel(tr("Active"));
    modeLabels[2] = createHeaderLabel(tr("Disabled"));
    modeLabels[3] = createHeaderLabel(tr("Selected"));
    Q_ASSERT(NumModes == 4);

    for (int j = 0; j < NumStates; ++j)
        mainLayout->addWidget(stateLabels[j], j + 1, 0);

    for (int i = 0; i < NumModes; ++i) {
        mainLayout->addWidget(modeLabels[i], 0, i + 1);

        for (int j = 0; j < NumStates; ++j) {
            pixmapLabels[i][j] = createPixmapLabel();
            mainLayout->addWidget(pixmapLabels[i][j], j + 1, i + 1);
        }
    }
}
//! [0]

//! [1]
void IconPreviewArea::setIcon(const QIcon &icon)
{
    this->icon = icon;
    updatePixmapLabels();
}
//! [1]

//! [2]
void IconPreviewArea::setSize(const QSize &size)
{
    if (size != this->size) {
        this->size = size;
        updatePixmapLabels();
    }
}
//! [2]

//! [3]
QLabel *IconPreviewArea::createHeaderLabel(const QString &text)
{
    QLabel *label = new QLabel(tr("<b>%1</b>").arg(text));
    label->setAlignment(Qt::AlignCenter);
    return label;
}
//! [3]

//! [4]
QLabel *IconPreviewArea::createPixmapLabel()
{
    QLabel *label = new QLabel;
    label->setEnabled(false);
    label->setAlignment(Qt::AlignCenter);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setBackgroundRole(QPalette::Base);
    label->setAutoFillBackground(true);
    label->setMinimumSize(132, 132);
    return label;
}
//! [4]

//! [5]
void IconPreviewArea::updatePixmapLabels()
{
    for (int i = 0; i < NumModes; ++i) {
        QIcon::Mode mode;
        if (i == 0) {
            mode = QIcon::Normal;
        } else if (i == 1) {
            mode = QIcon::Active;
        } else if (i == 2) {
            mode = QIcon::Disabled;
        } else {
            mode = QIcon::Selected;
        }

        for (int j = 0; j < NumStates; ++j) {
            QIcon::State state = (j == 0) ? QIcon::Off : QIcon::On;
            QPixmap pixmap = icon.pixmap(size, mode, state);
            pixmapLabels[i][j]->setPixmap(pixmap);
            pixmapLabels[i][j]->setEnabled(!pixmap.isNull());
        }
    }
}
//! [5]
