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

#include <QApplication>
#include <QPainter>
#include <QTime>
#include "block.h"
#include "window.h"

QImage createImage(int width, int height)
{
    QImage image(width, height, QImage::Format_RGB16);
    QPainter painter;
    QPen pen;
    pen.setStyle(Qt::NoPen);
    QBrush brush(Qt::blue);

    painter.begin(&image);
    painter.fillRect(image.rect(), brush);
    brush.setColor(Qt::white);
    painter.setPen(pen);
    painter.setBrush(brush);

    static const QPointF points1[3] = {
        QPointF(4, 4),
        QPointF(7, 4),
        QPointF(5.5, 1)
    };

    static const QPointF points2[3] = {
        QPointF(1, 4),
        QPointF(7, 4),
        QPointF(10, 10)
    };

    static const QPointF points3[3] = {
        QPointF(4, 4),
        QPointF(10, 4),
        QPointF(1, 10)
    };

    painter.setWindow(0, 0, 10, 10);

    int x = 0;
    int y = 0;
    int starWidth = image.width()/3;
    int starHeight = image.height()/3; 

    QRect rect(x, y, starWidth, starHeight);

    for (int i = 0; i < 9; ++i) {

        painter.setViewport(rect);
        painter.drawPolygon(points1, 3);
        painter.drawPolygon(points2, 3);
        painter.drawPolygon(points3, 3);

        if (i % 3 == 2) {
            y = y + starHeight;
            rect.moveTop(y);

            x = 0;
            rect.moveLeft(x);

        } else {
            x = x + starWidth;
            rect.moveLeft(x);
        }
    }

    painter.end();
    return image;
}

//! [main function] //! [main start]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//! [main start] //! [register meta-type for queued communications]
    qRegisterMetaType<Block>();
//! [register meta-type for queued communications]
    qsrand(QTime::currentTime().elapsed());

    Window window;
    window.show();

    window.loadImage(createImage(256, 256));
//! [main finish]
    return app.exec();
}
//! [main finish] //! [main function]
