/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qglobal.h> // for Q_WS_WIN define (non-PCH)

#include <QtGui/qpaintdevice.h>
#include <private/qwidget_p.h>
#include "qwindowsurface_s60_p.h"
#include "qpixmap_s60_p.h"
#include "qt_s60_p.h"
#include "private/qdrawhelper_p.h"

QT_BEGIN_NAMESPACE

struct QS60WindowSurfacePrivate
{
    QPixmap device;
    QList<QImage*> bufferImages;
};

QS60WindowSurface::QS60WindowSurface(QWidget* widget)
    : QWindowSurface(widget), d_ptr(new QS60WindowSurfacePrivate)
{

    TDisplayMode mode = S60->screenDevice()->DisplayMode();
    bool isOpaque = qt_widget_private(widget)->isOpaque;
    if (mode == EColor16MA && isOpaque)
        mode = EColor16MU; // Faster since 16MU -> 16MA is typically accelerated
    else if (mode == EColor16MU && !isOpaque)
        mode = EColor16MA; // Try for transparency anyway

    // We create empty CFbsBitmap here -> it will be resized in setGeometry
	CFbsBitmap *bitmap = q_check_ptr(new CFbsBitmap);	// CBase derived object needs check on new
    qt_symbian_throwIfError( bitmap->Create( TSize(0, 0), mode ) );
	
    QS60PixmapData *data = new QS60PixmapData(QPixmapData::PixmapType);
    data->fromSymbianBitmap(bitmap);
    d_ptr->device = QPixmap(data);
        
    setStaticContentsSupport(true);
}
QS60WindowSurface::~QS60WindowSurface()
{
    delete d_ptr;
}

void QS60WindowSurface::beginPaint(const QRegion &rgn)
{
    if (!qt_widget_private(window())->isOpaque) {
        QS60PixmapData *pixmapData = static_cast<QS60PixmapData *>(d_ptr->device.data_ptr().data());
        pixmapData->beginDataAccess();
        QImage &image = pixmapData->image;
        QRgb *data = reinterpret_cast<QRgb *>(image.bits());
        const int row_stride = image.bytesPerLine() / 4;

        const QVector<QRect> rects = rgn.rects();
        for (QVector<QRect>::const_iterator it = rects.begin(); it != rects.end(); ++it) {
            const int x_start = it->x();
            const int width = it->width();

            const int y_start = it->y();
            const int height = it->height();

            QRgb *row = data + row_stride * y_start;
            for (int y = 0; y < height; ++y) {
                qt_memfill(row + x_start, 0U, width);
                row += row_stride;
            }
        }
        pixmapData->endDataAccess();
    }
}

void QS60WindowSurface::endPaint(const QRegion &)
{
    qDeleteAll(d_ptr->bufferImages);
    d_ptr->bufferImages.clear();
}

QImage* QS60WindowSurface::buffer(const QWidget *widget)
{
    if (widget->window() != window())
        return 0;

    QPaintDevice *pdev = paintDevice();
    if (!pdev)
        return 0;

    const QPoint off = offset(widget);
    QImage *img = &(static_cast<QS60PixmapData *>(d_ptr->device.data_ptr().data())->image);
    
    QRect rect(off, widget->size());
    rect &= QRect(QPoint(), img->size());

    if (rect.isEmpty())
        return 0;

    img = new QImage(img->scanLine(rect.y()) + rect.x() * img->depth() / 8,
                     rect.width(), rect.height(),
                     img->bytesPerLine(), img->format());
    d_ptr->bufferImages.append(img);

    return img;
}

void QS60WindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &)
{
    const QVector<QRect> subRects = region.rects();
    for (int i = 0; i < subRects.count(); ++i) {
        TRect tr = qt_QRect2TRect(subRects[i]);
        widget->winId()->DrawNow(tr);
    }
}

bool QS60WindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    QRect rect = area.boundingRect();

    if (dx == 0 && dy == 0)
        return false;

    if (d_ptr->device.isNull())
        return false;

    QS60PixmapData *data = static_cast<QS60PixmapData*>(d_ptr->device.data_ptr().data());
    data->scroll(dx, dy, rect);

    return true;
}

QPaintDevice* QS60WindowSurface::paintDevice()
{
    return &d_ptr->device;
}

void QS60WindowSurface::setGeometry(const QRect& rect)
{
    if (rect == geometry())
        return;

    QS60PixmapData *data = static_cast<QS60PixmapData*>(d_ptr->device.data_ptr().data());
    data->resize(rect.width(), rect.height());

    QWindowSurface::setGeometry(rect);
}

CFbsBitmap* QS60WindowSurface::symbianBitmap() const
{
    QS60PixmapData *data = static_cast<QS60PixmapData*>(d_ptr->device.data_ptr().data());
    return data->cfbsBitmap;
}

QT_END_NAMESPACE

