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
#include "displaywidget.h"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    QPainterPath car;
    QPainterPath house;

    QFile file(":resources/shapes.dat");
    file.open(QFile::ReadOnly);
    QDataStream stream(&file);
    stream >> car >> house >> tree >> moon;
    file.close();

    shapeMap[Car] = car;
    shapeMap[House] = house;

    background = Sky;
    shapeColor = Qt::darkYellow;
    shape = House;
}

//! [paint event]
void DisplayWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(painter);
    painter.end();
}
//! [paint event]

//! [paint function]
void DisplayWidget::paint(QPainter &painter)
{
//![paint picture]
    painter.setClipRect(QRect(0, 0, 200, 200));
    painter.setPen(Qt::NoPen);

    switch (background) {
    case Sky:
    default:
        painter.fillRect(QRect(0, 0, 200, 200), Qt::darkBlue);
        painter.translate(145, 10);
        painter.setBrush(Qt::white);
        painter.drawPath(moon);
        painter.translate(-145, -10);
        break;
    case Trees:
    {
        painter.fillRect(QRect(0, 0, 200, 200), Qt::darkGreen);
        painter.setBrush(Qt::green);
        painter.setPen(Qt::black);
        for (int y = -55, row = 0; y < 200; y += 50, ++row) {
            int xs;
            if (row == 2 || row == 3)
                xs = 150;
            else
                xs = 50;
            for (int x = 0; x < 200; x += xs) {
                painter.save();
                painter.translate(x, y);
                painter.drawPath(tree);
                painter.restore();
            }
        }
        break;
    }
    case Road:
        painter.fillRect(QRect(0, 0, 200, 200), Qt::gray);
        painter.setPen(QPen(Qt::white, 4, Qt::DashLine));
        painter.drawLine(QLine(0, 35, 200, 35));
        painter.drawLine(QLine(0, 165, 200, 165));
        break;
    }

    painter.setBrush(shapeColor);
    painter.setPen(Qt::black);
    painter.translate(100, 100);
    painter.drawPath(shapeMap[shape]);
//![paint picture]
}
//! [paint function]

QColor DisplayWidget::color() const
{
    return shapeColor;
}

void DisplayWidget::setBackground(Background background)
{
    this->background = background;
    update();
}

void DisplayWidget::setColor(const QColor &color)
{
    this->shapeColor = color;
    update();
}

void DisplayWidget::setShape(Shape shape)
{
    this->shape = shape;
    update();
}
