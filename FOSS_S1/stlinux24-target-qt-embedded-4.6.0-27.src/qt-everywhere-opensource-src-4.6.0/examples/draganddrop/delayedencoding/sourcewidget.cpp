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
#include <QtSvg>
#include "mimedata.h"
#include "sourcewidget.h"

SourceWidget::SourceWidget(QWidget *parent)
    : QWidget(parent)
{
    QFile imageFile(":/images/example.svg");
    imageFile.open(QIODevice::ReadOnly);
    imageData = imageFile.readAll();
    imageFile.close();

    QScrollArea *imageArea = new QScrollArea;
    imageLabel = new QSvgWidget;
    imageLabel->renderer()->load(imageData);
    imageArea->setWidget(imageLabel);
    //imageLabel->setMinimumSize(imageLabel->renderer()->viewBox().size());

    QLabel *instructTopLabel = new QLabel(tr("This is an SVG drawing:"));
    QLabel *instructBottomLabel = new QLabel(
        tr("Drag the icon to copy the drawing as a PNG file:"));
    QPushButton *dragIcon = new QPushButton(tr("Export"));
    dragIcon->setIcon(QIcon(":/images/drag.png"));

    connect(dragIcon, SIGNAL(pressed()), this, SLOT(startDrag()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(instructTopLabel, 0, 0, 1, 2);
    layout->addWidget(imageArea, 1, 0, 2, 2);
    layout->addWidget(instructBottomLabel, 3, 0);
    layout->addWidget(dragIcon, 3, 1);
    setLayout(layout);
    setWindowTitle(tr("Delayed Encoding"));
}

//![1]
void SourceWidget::createData(const QString &mimeType)
{
    if (mimeType != "image/png")
        return;

    QImage image(imageLabel->size(), QImage::Format_RGB32);
    QPainter painter;
    painter.begin(&image);
    imageLabel->renderer()->render(&painter);
    painter.end();

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    buffer.close();

    mimeData->setData("image/png", data);
}
//![1]

//![0]
void SourceWidget::startDrag()
{
    mimeData = new MimeData;

    connect(mimeData, SIGNAL(dataRequested(const QString &)),
            this, SLOT(createData(const QString &)), Qt::DirectConnection);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(":/images/drag.png"));

    drag->exec(Qt::CopyAction);
}
//![0]

