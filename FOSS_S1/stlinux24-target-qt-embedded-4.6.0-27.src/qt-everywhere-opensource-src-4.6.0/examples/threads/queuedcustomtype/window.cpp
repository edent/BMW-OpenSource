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
#include "window.h"

//! [Window constructor start]
Window::Window()
{
    thread = new RenderThread();
//! [Window constructor start] //! [set up widgets and connections]

    label = new QLabel();
    label->setAlignment(Qt::AlignCenter);

    loadButton = new QPushButton(tr("&Load image..."));
    resetButton = new QPushButton(tr("&Stop"));
    resetButton->setEnabled(false);

    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(resetButton, SIGNAL(clicked()), thread, SLOT(stopProcess()));
    connect(thread, SIGNAL(finished()), this, SLOT(resetUi()));
//! [set up widgets and connections] //! [connecting signal with custom type]
    connect(thread, SIGNAL(sendBlock(Block)), this, SLOT(addBlock(Block)));
//! [connecting signal with custom type]

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addLayout(buttonLayout);

//! [Window constructor finish]
    setWindowTitle(tr("Queued Custom Type"));
}
//! [Window constructor finish]

void Window::loadImage()
{
    QStringList formats;
    foreach (QByteArray format, QImageReader::supportedImageFormats())
        if (format.toLower() == format)
            formats.append("*." + format);

    QString newPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
        path, tr("Image files (%1)").arg(formats.join(" ")));

    if (newPath.isEmpty())
        return;

    QImage image(newPath);
    if (!image.isNull()) {
        loadImage(image);
        path = newPath;
    }
}

void Window::loadImage(const QImage &image)
{
    QDesktopWidget desktop;
    QImage useImage;
    QRect space = desktop.availableGeometry();
    if (image.width() > 0.75*space.width() || image.height() > 0.75*space.height())
        useImage = image.scaled(0.75*space.width(), 0.75*space.height(),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
    else
        useImage = image;

    pixmap = QPixmap(useImage.width(), useImage.height());
    pixmap.fill(qRgb(255, 255, 255));
    label->setPixmap(pixmap);
    loadButton->setEnabled(false);
    resetButton->setEnabled(true);
    thread->processImage(useImage);
}

//! [Adding blocks to the display]
void Window::addBlock(const Block &block)
{
    QColor color = block.color();
    color.setAlpha(64);

    QPainter painter;
    painter.begin(&pixmap);
    painter.fillRect(block.rect(), color);
    painter.end();
    label->setPixmap(pixmap);
}
//! [Adding blocks to the display]

void Window::resetUi()
{
    loadButton->setEnabled(true);
    resetButton->setEnabled(false);
}
