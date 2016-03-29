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

#include "headingitem.h"
#include "colors.h"

HeadingItem::HeadingItem(const QString &text, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->text = text;
    this->noSubPixeling = true;
}

QImage *HeadingItem::createImage(const QMatrix &matrix) const
{
    float sx = qMin(matrix.m11(), matrix.m22());
    float sy = matrix.m22() < sx ? sx : matrix.m22();
    QFontMetrics fm(Colors::headingFont());

    float w = fm.width(this->text) + 1;
    float h = fm.height();
    float xShadow = 3.0f;
    float yShadow = 3.0f;

    QImage *image = new QImage(int((w + xShadow) * sx), int((h + yShadow) * sy), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.setFont(Colors::headingFont());
    painter.scale(sx, sy);

    //draw shadow
    QLinearGradient brush_shadow(xShadow, yShadow, w, yShadow);
    brush_shadow.setSpread(QLinearGradient::PadSpread);
    if (Colors::useEightBitPalette)
        brush_shadow.setColorAt(0.0f, QColor(0, 0, 0));
    else
        brush_shadow.setColorAt(0.0f, QColor(0, 0, 0, 100));
    QPen pen_shadow;
    pen_shadow.setBrush(brush_shadow);
    painter.setPen(pen_shadow);
    painter.drawText(int(xShadow), int(yShadow), int(w), int(h), Qt::AlignLeft, this->text);

    // draw text
    QLinearGradient brush_text(0, 0, w, w);
    brush_text.setSpread(QLinearGradient::PadSpread);
    brush_text.setColorAt(0.0f, QColor(255, 255, 255));
    brush_text.setColorAt(0.2f, QColor(255, 255, 255));
    brush_text.setColorAt(0.5f, QColor(190, 190, 190));
    QPen pen_text;
    pen_text.setBrush(brush_text);
    painter.setPen(pen_text);
    painter.drawText(0, 0, int(w), int(h), Qt::AlignLeft, this->text);
    return image;
}


void HeadingItem::animationStarted(int)
{
    this->noSubPixeling = false;
}


void HeadingItem::animationStopped(int)
{
    this->noSubPixeling = true;
}
