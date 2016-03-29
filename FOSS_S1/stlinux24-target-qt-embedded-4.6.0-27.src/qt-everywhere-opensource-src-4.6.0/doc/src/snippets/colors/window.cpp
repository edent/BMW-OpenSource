/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QFont font;
    font.setPixelSize(12);
    setFont(font);
}

void Window::closeEvent(QCloseEvent *event)
{
    QPixmap pixmap(size());
    render(&pixmap);
    pixmap.save("qt-colors.png");

    event->accept();
}

void Window::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);

    int h = 216 / 5;
    QRect r = QRect(0, 0, 160, h);
    painter.fillRect(r, Qt::white);
    painter.setPen(Qt::black);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("white"));
    r = QRect(0, h, 160, h);
    painter.fillRect(r, Qt::red);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("red"));
    r = QRect(0, h*2, 160, h);
    painter.fillRect(r, Qt::green);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("green"));
    r = QRect(0, h*3, 160, h);
    painter.fillRect(r, Qt::blue);
    painter.setPen(Qt::white);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("blue"));

    r = QRect(160, 0, 160, h);
    painter.fillRect(r, Qt::black);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("black"));
    r = QRect(160, h, 160, h);
    painter.fillRect(r, Qt::darkRed);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkRed"));
    r = QRect(160, h*2, 160, h);
    painter.fillRect(r, Qt::darkGreen);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkGreen"));
    r = QRect(160, h*3, 160, h);
    painter.fillRect(r, Qt::darkBlue);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkBlue"));

    r = QRect(320, 0, 160, h);
    painter.fillRect(r, Qt::cyan);
    painter.setPen(Qt::black);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("cyan"));
    r = QRect(320, h, 160, h);
    painter.fillRect(r, Qt::magenta);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("magenta"));
    r = QRect(320, h*2, 160, h);
    painter.fillRect(r, Qt::yellow);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("yellow"));
    r = QRect(320, h*3, 160, h);
    painter.fillRect(r, Qt::gray);
    painter.setPen(Qt::white);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("gray"));

    r = QRect(480, 0, 160, h);
    painter.fillRect(r, Qt::darkCyan);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkCyan"));
    r = QRect(480, h, 160, h);
    painter.fillRect(r, Qt::darkMagenta);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkMagenta"));
    r = QRect(480, h*2, 160, h);
    painter.fillRect(r, Qt::darkYellow);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkYellow"));
    r = QRect(480, h*3, 160, h);
    painter.fillRect(r, Qt::darkGray);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("darkGray"));

    r = QRect(0, h*4, 640, h);
    painter.fillRect(r, Qt::lightGray);
    painter.setPen(Qt::black);
    painter.drawText(r, Qt::AlignCenter, QLatin1String("lightGray"));

    painter.end();
}

