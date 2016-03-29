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

int main()
{
    int x, y;
    {
        // BIT ACCESS
        QImage image;
        // sets bit at (x, y) to 1
        if (image.format() == QImage::Format_MonoLSB)
            image.scanLine(y)[x >> 3] |= 1 << (x & 7);
        else
            image.scanLine(y)[x >> 3] |= 1 << (7 - (x & 7));
    }

    {
        // 8-BIT ACCESS
        QImage image;
        // set entry 19 in the color table to yellow
        image.setColor(19, qRgb(255, 255, 0));

        // set 8 bit pixel at (x,y) to value yellow (in color table)
        image.scanLine(y)[x] = 19;
    }

    {
        // 32-BIT
        QImage image;
        // sets 32 bit pixel at (x,y) to yellow.
        uint *ptr = reinterpret_cast<uint *>(image.scanLine(y)) + x;
        *ptr = qRgb(255, 255, 0);
    }

    {
        // SAVE
//! [0]
        QImage image;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG"); // writes image into ba in PNG format
//! [0]
    }

    {
        // PIX SAVE
//! [1]
        QPixmap pixmap;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
//! [1]
    }

    {
        // MASK
//! [2]
        QPixmap alpha("image-with-alpha.png");
        QPixmap alphacopy = alpha;
        alphacopy.setMask(alphacopy.mask());
//! [2]
    }
}
