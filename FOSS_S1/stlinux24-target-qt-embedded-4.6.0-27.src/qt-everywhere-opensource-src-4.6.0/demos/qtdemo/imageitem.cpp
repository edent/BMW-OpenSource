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

#include "imageitem.h"
#include "colors.h"

ImageItem::ImageItem(const QImage &image, int maxWidth, int maxHeight, QGraphicsScene *scene,
        QGraphicsItem *parent, bool adjustSize, float scale) : DemoItem(scene, parent)
{
    this->image = image;
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
    this->adjustSize = adjustSize;
    this->scale = scale;
}

QImage *ImageItem::createImage(const QMatrix &matrix) const
{
    QImage *original = new QImage(image);
    if (original->isNull()){
        return original; // nothing we can do about it...
    }

    QPoint size = matrix.map(QPoint(this->maxWidth, this->maxHeight));
    float w = size.x(); // x, y is the used as width, height
    float h = size.y();

    // Optimization: if image is smaller than maximum allowed size, just return the loaded image
    if (original->size().height() <= h && original->size().width() <= w && !this->adjustSize && this->scale == 1)
        return original;

    // Calculate what the size of the final image will be:
    w = qMin(w, float(original->size().width()) * this->scale);
    h = qMin(h, float(original->size().height()) * this->scale);

    float adjustx = 1.0f;
    float adjusty = 1.0f;
    if (this->adjustSize){
        adjustx = qMin(matrix.m11(), matrix.m22());
        adjusty = matrix.m22() < adjustx ? adjustx : matrix.m22();
        w *= adjustx;
        h *= adjusty;
    }

    // Create a new image with correct size, and draw original on it
    QImage *image = new QImage(int(w+2), int(h+2), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    if (this->adjustSize)
        painter.scale(adjustx, adjusty);
    if (this->scale != 1)
       painter.scale(this->scale, this->scale);
    painter.drawImage(0, 0, *original);

    if (!this->adjustSize){
       // Blur out edges
       int blur = 30;
       if (h < original->height()){
           QLinearGradient brush1(0, h - blur, 0, h);
           brush1.setSpread(QGradient::PadSpread);
           brush1.setColorAt(0.0, QColor(0, 0, 0, 0));
           brush1.setColorAt(1.0, Colors::sceneBg1);
           painter.fillRect(0, int(h) - blur, original->width(), int(h), brush1);
       }
       if (w < original->width()){
           QLinearGradient brush2(w - blur, 0, w, 0);
           brush2.setSpread(QGradient::PadSpread);
           brush2.setColorAt(0.0, QColor(0, 0, 0, 0));
           brush2.setColorAt(1.0, Colors::sceneBg1);
           painter.fillRect(int(w) - blur, 0, int(w), original->height(), brush2);
       }
    }
    delete original;
    return image;
}
