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

#include "scanitem.h"
#include "colors.h"

#define ITEM_WIDTH 16
#define ITEM_HEIGHT 16

ScanItem::ScanItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    useSharedImage(QString(__FILE__));
}

ScanItem::~ScanItem()
{
}

QImage *ScanItem::createImage(const QMatrix &matrix) const
{
    QRect scaledRect = matrix.mapRect(QRect(0, 0, ITEM_WIDTH, ITEM_HEIGHT));
    QImage *image = new QImage(scaledRect.width(), scaledRect.height(), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.setRenderHint(QPainter::Antialiasing);

    if (Colors::useEightBitPalette){
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.setBrush(QColor(206, 246, 117));
        painter.drawEllipse(1, 1, scaledRect.width()-2, scaledRect.height()-2);
    }
    else {
        painter.setPen(QPen(QColor(0, 0, 0, 15), 1));
//        painter.setBrush(QColor(206, 246, 117, 150));
        painter.setBrush(QColor(0, 0, 0, 15));
        painter.drawEllipse(1, 1, scaledRect.width()-2, scaledRect.height()-2);
    }
    return image;
}


