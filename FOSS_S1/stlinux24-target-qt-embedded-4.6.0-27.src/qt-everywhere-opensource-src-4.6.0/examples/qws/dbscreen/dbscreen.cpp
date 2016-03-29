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

#include "dbscreen.h"
#include <QApplication>

//! [0]
bool DBScreen::initDevice()
{
    if (!QLinuxFbScreen::initDevice())
        return false;

    QScreenCursor::initSoftwareCursor();
    image = new QImage(deviceWidth(), deviceHeight(), pixelFormat());
    painter = new QPainter(image);

    return true;
}
//! [0]

//! [1]
void DBScreen::shutdownDevice()
{
    QLinuxFbScreen::shutdownDevice();
    delete painter;
    delete image;
}
//! [1]

//! [2]
void DBScreen::solidFill(const QColor &color, const QRegion &region)
{
    QVector<QRect> rects = region.rects();
    for (int i = 0; i  < rects.size(); i++)
        painter->fillRect(rects.at(i), color);
}
//! [2]

//! [3]
void DBScreen::blit(const QImage &image, const QPoint &topLeft, const QRegion &region)
{
    QVector<QRect> rects = region.rects();
    for (int i = 0; i < rects.size(); i++) {
        QRect destRect = rects.at(i);
        QRect srcRect(destRect.x()-topLeft.x(), destRect.y()-topLeft.y(), destRect.width(), destRect.height());
        painter->drawImage(destRect.topLeft(), image, srcRect);
    }
}
//! [3]

//! [4]
void DBScreen::exposeRegion(QRegion region, int changing)
{
    QLinuxFbScreen::exposeRegion(region, changing);
    QLinuxFbScreen::blit(*image, QPoint(0, 0), region);
}
//! [4]

