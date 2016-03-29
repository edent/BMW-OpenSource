/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <QTime>
#include <QDebug>

#include <private/qt_x11_p.h>
#include <private/qimagepixmapcleanuphooks_p.h>

#include "qwindowsurface_x11gl_p.h"
#include "qpixmapdata_x11gl_p.h"

QT_BEGIN_NAMESPACE

QX11GLWindowSurface::QX11GLWindowSurface(QWidget* window)
    : QWindowSurface(window), m_GC(0), m_window(window)
{
}

QX11GLWindowSurface::~QX11GLWindowSurface()
{
    if (m_GC)
        XFree(m_GC);
}

QPaintDevice *QX11GLWindowSurface::paintDevice()
{
    return &m_backBuffer;
}

extern void *qt_getClipRects(const QRegion &r, int &num); // in qpaintengine_x11.cpp

void QX11GLWindowSurface::flush(QWidget *widget, const QRegion &widgetRegion, const QPoint &offset)
{
//    qDebug("QX11GLWindowSurface::flush()");
    QTime startTime = QTime::currentTime();
    if (m_backBuffer.isNull()) {
        qDebug("QHarmattanWindowSurface::flush() - backBuffer is null, not flushing anything");
        return;
    }

    QPoint widgetOffset = qt_qwidget_data(widget)->wrect.topLeft();
    QRegion windowRegion(widgetRegion);
    QRect boundingRect = widgetRegion.boundingRect();
    if (!widgetOffset.isNull())
        windowRegion.translate(-widgetOffset);
    QRect windowBoundingRect = windowRegion.boundingRect();

    int rectCount;
    XRectangle *rects = (XRectangle *)qt_getClipRects(windowRegion, rectCount);
    if (rectCount <= 0)
        return;
//         qDebug() << "XSetClipRectangles";
//         for  (int i = 0; i < num; ++i)
//             qDebug() << ' ' << i << rects[i].x << rects[i].x << rects[i].y << rects[i].width << rects[i].height;

    if (m_GC == 0) {
        m_GC = XCreateGC(X11->display, m_window->handle(), 0, 0);
        XSetGraphicsExposures(X11->display, m_GC, False);
    }

    XSetClipRectangles(X11->display, m_GC, 0, 0, rects, rectCount, YXBanded);
    XCopyArea(X11->display, m_backBuffer.handle(), m_window->handle(), m_GC,
              boundingRect.x() + offset.x(), boundingRect.y() + offset.y(),
              boundingRect.width(), boundingRect.height(),
              windowBoundingRect.x(), windowBoundingRect.y());
}

void QX11GLWindowSurface::setGeometry(const QRect &rect)
{
    if (rect.width() > m_backBuffer.size().width() || rect.height() > m_backBuffer.size().height()) {
        QSize newSize = rect.size();
//        QSize newSize(1024,512);
        qDebug() << "QX11GLWindowSurface::setGeometry() - creating a pixmap of size" << newSize;
        QX11GLPixmapData *pd = new QX11GLPixmapData;
        pd->resize(newSize.width(), newSize.height());
        m_backBuffer = QPixmap(pd);
    }

//    if (gc)
//        XFreeGC(X11->display, gc);
//    gc = XCreateGC(X11->display, d_ptr->device.handle(), 0, 0);
//    XSetGraphicsExposures(X11->display, gc, False);
    QWindowSurface::setGeometry(rect);
}

bool QX11GLWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    Q_UNUSED(area);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    return false;
}

/*
void QX11GLWindowSurface::beginPaint(const QRegion &region)
{
}

void QX11GLWindowSurface::endPaint(const QRegion &region)
{
}

QImage *QX11GLWindowSurface::buffer(const QWidget *widget)
{
}
*/

QT_END_NAMESPACE
