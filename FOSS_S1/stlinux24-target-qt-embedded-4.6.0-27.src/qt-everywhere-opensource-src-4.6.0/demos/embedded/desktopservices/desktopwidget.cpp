/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

// EXTERNAL INCLUDES
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDesktopServices>

// INTERNAL INCLUDES
#include "linktab.h"
#include "contenttab.h"

// CLASS HEADER
#include "desktopwidget.h"

// CONSTRUCTORS & DESTRUCTORS
DesktopWidget::DesktopWidget(QWidget *parent) : QWidget(parent)

{
    QTabWidget *tabWidget = new QTabWidget(this);

    // Images
    ContentTab* imageTab = new ContentTab(tabWidget);
    imageTab->init(QDesktopServices::PicturesLocation,
                   "*.png;*.jpg;*.jpeg;*.bmp;*.gif",
                   ":/resources/photo.png");
    tabWidget->addTab(imageTab, tr("Images"));

    // Music
    ContentTab* musicTab = new ContentTab(tabWidget);
    musicTab->init(QDesktopServices::MusicLocation,
                   "*.wav;*.mp3;*.mp4",
                   ":/resources/music.png");
    tabWidget->addTab(musicTab, tr("Music"));

    // Links
    LinkTab* othersTab = new LinkTab(tabWidget);;
    // Given icon file will be overriden by LinkTab
    othersTab->init(QDesktopServices::PicturesLocation, "", "");
    tabWidget->addTab(othersTab, tr("Links"));

    // Layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabWidget);
    setLayout(layout);
}

DesktopWidget::~DesktopWidget()
{
}

// End of file
