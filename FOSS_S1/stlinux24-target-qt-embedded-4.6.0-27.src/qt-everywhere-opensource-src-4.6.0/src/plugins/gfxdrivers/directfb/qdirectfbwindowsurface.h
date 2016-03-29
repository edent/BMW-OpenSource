/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QDIRECFBWINDOWSURFACE_H
#define QDIRECFBWINDOWSURFACE_H

#include "qdirectfbpaintengine.h"
#include "qdirectfbpaintdevice.h"
#include "qdirectfbscreen.h"

#ifndef QT_NO_QWS_DIRECTFB

#include <private/qpaintengine_raster_p.h>
#include <private/qwindowsurface_qws_p.h>

#ifdef QT_DIRECTFB_TIMING
#include <qdatetime.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QDirectFBWindowSurface : public QWSWindowSurface, public QDirectFBPaintDevice
{
public:
    QDirectFBWindowSurface(DFBSurfaceFlipFlags flipFlags, QDirectFBScreen *scr);
    QDirectFBWindowSurface(DFBSurfaceFlipFlags flipFlags, QDirectFBScreen *scr, QWidget *widget);
    ~QDirectFBWindowSurface();

#ifdef QT_DIRECTFB_WM
    void raise();
#endif
    bool isValid() const;

    void setGeometry(const QRect &rect);

    QString key() const { return QLatin1String("directfb"); }
    QByteArray permanentState() const;
    void setPermanentState(const QByteArray &state);

    bool scroll(const QRegion &area, int dx, int dy);

    bool move(const QPoint &offset);

    QImage image() const { return QImage(); }
    QPaintDevice *paintDevice() { return this; }

    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);

    void beginPaint(const QRegion &);
    void endPaint(const QRegion &);

    IDirectFBSurface *surfaceForWidget(const QWidget *widget, QRect *rect) const;
    IDirectFBSurface *directFBSurface() const;
#ifdef QT_DIRECTFB_WM
    IDirectFBWindow *directFBWindow() const;
#endif
private:
    void setOpaque(bool opaque);
    void updateFormat();
    void releaseSurface();
    QDirectFBWindowSurface *sibling;

#ifdef QT_DIRECTFB_WM
    void createWindow(const QRect &rect);
    IDirectFBWindow *dfbWindow;
#else
    enum Mode {
        Primary,
        Offscreen
    } mode;
#endif

    DFBSurfaceFlipFlags flipFlags;
    bool noSystemBackground;
    bool boundingRectFlip;
#ifdef QT_DIRECTFB_TIMING
    int frames;
    QTime timer;
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_QWS_DIRECTFB

#endif // QDIRECFBWINDOWSURFACE_H
