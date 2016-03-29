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

#include "qdirectfbwindowsurface.h"
#include "qdirectfbscreen.h"
#include "qdirectfbpaintengine.h"

#include <private/qwidget_p.h>
#include <qwidget.h>
#include <qwindowsystem_qws.h>
#include <qpaintdevice.h>
#include <qvarlengtharray.h>

#ifndef QT_NO_QWS_DIRECTFB

QT_BEGIN_NAMESPACE

QDirectFBWindowSurface::QDirectFBWindowSurface(DFBSurfaceFlipFlags flip, QDirectFBScreen *scr)
    : QDirectFBPaintDevice(scr)
    , sibling(0)
#ifndef QT_NO_DIRECTFB_WM
    , dfbWindow(0)
#endif
    , flipFlags(flip)
    , boundingRectFlip(scr->directFBFlags() & QDirectFBScreen::BoundingRectFlip)
{
#ifdef QT_NO_DIRECTFB_WM
    mode = Offscreen;
#endif
    setSurfaceFlags(Opaque | Buffered);
#ifdef QT_DIRECTFB_TIMING
    frames = 0;
    timer.start();
#endif
}

QDirectFBWindowSurface::QDirectFBWindowSurface(DFBSurfaceFlipFlags flip, QDirectFBScreen *scr, QWidget *widget)
    : QWSWindowSurface(widget), QDirectFBPaintDevice(scr)
    , sibling(0)
#ifndef QT_NO_DIRECTFB_WM
    , dfbWindow(0)
#endif
    , flipFlags(flip)
    , boundingRectFlip(scr->directFBFlags() & QDirectFBScreen::BoundingRectFlip)
{
    SurfaceFlags flags = 0;
    if (!widget || widget->window()->windowOpacity() == 0xff)
        flags |= Opaque;
#ifdef QT_NO_DIRECTFB_WM
    if (widget && widget->testAttribute(Qt::WA_PaintOnScreen)) {
        flags = RegionReserved;
        mode = Primary;
    } else {
        mode = Offscreen;
        flags = Buffered;
    }
#else
    noSystemBackground = widget && widget->testAttribute(Qt::WA_NoSystemBackground);
    if (noSystemBackground)
        flags &= ~Opaque;
#endif
    setSurfaceFlags(flags);
#ifdef QT_DIRECTFB_TIMING
    frames = 0;
    timer.start();
#endif
}

QDirectFBWindowSurface::~QDirectFBWindowSurface()
{
    releaseSurface();
    // these are not tracked by QDirectFBScreen so we don't want QDirectFBPaintDevice to release it
}

bool QDirectFBWindowSurface::isValid() const
{
    return true;
}

#ifdef QT_DIRECTFB_WM
void QDirectFBWindowSurface::raise()
{
    if (IDirectFBWindow *window = directFBWindow()) {
        window->RaiseToTop(window);
    }
}

IDirectFBWindow *QDirectFBWindowSurface::directFBWindow() const
{
    return (dfbWindow ? dfbWindow : (sibling ? sibling->dfbWindow : 0));
}


void QDirectFBWindowSurface::createWindow(const QRect &rect)
{
    IDirectFBDisplayLayer *layer = screen->dfbDisplayLayer();
    if (!layer)
        qFatal("QDirectFBWindowSurface: Unable to get primary display layer!");

    DFBWindowDescription description;
    memset(&description, 0, sizeof(DFBWindowDescription));

    description.caps = DWCAPS_NODECORATION;
    description.flags = DWDESC_CAPS|DWDESC_SURFACE_CAPS|DWDESC_PIXELFORMAT|DWDESC_HEIGHT|DWDESC_WIDTH|DWDESC_POSX|DWDESC_POSY;
#if (Q_DIRECTFB_VERSION >= 0x010200)
    description.flags |= DWDESC_OPTIONS;
#endif

    if (noSystemBackground) {
        description.caps |= DWCAPS_ALPHACHANNEL;
#if (Q_DIRECTFB_VERSION >= 0x010200)
        description.options |= DWOP_ALPHACHANNEL;
#endif
    }

    description.posx = rect.x();
    description.posy = rect.y();
    description.width = rect.width();
    description.height = rect.height();
    description.surface_caps = DSCAPS_NONE;
    if (screen->directFBFlags() & QDirectFBScreen::VideoOnly)
        description.surface_caps |= DSCAPS_VIDEOONLY;
    const QImage::Format format = (noSystemBackground ? screen->alphaPixmapFormat() : screen->pixelFormat());
    description.pixelformat = QDirectFBScreen::getSurfacePixelFormat(format);
    if (QDirectFBScreen::isPremultiplied(format))
        description.surface_caps = DSCAPS_PREMULTIPLIED;

    DFBResult result = layer->CreateWindow(layer, &description, &dfbWindow);

    if (result != DFB_OK)
        DirectFBErrorFatal("QDirectFBWindowSurface::createWindow", result);

    if (window()) {
        DFBWindowID winid;
        result = dfbWindow->GetID(dfbWindow, &winid);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBWindowSurface::createWindow. Can't get ID", result);
        } else {
            window()->setProperty("_q_DirectFBWindowID", winid);
        }
    }

    Q_ASSERT(!dfbSurface);
    dfbWindow->GetSurface(dfbWindow, &dfbSurface);
    updateFormat();
}

static DFBResult setWindowGeometry(IDirectFBWindow *dfbWindow, const QRect &old, const QRect &rect)
{
    DFBResult result = DFB_OK;
    const bool isMove = old.isEmpty() || rect.topLeft() != old.topLeft();
    const bool isResize = rect.size() != old.size();

#if (Q_DIRECTFB_VERSION >= 0x010000)
    if (isResize && isMove) {
        result = dfbWindow->SetBounds(dfbWindow, rect.x(), rect.y(),
                                      rect.width(), rect.height());
    } else if (isResize) {
        result = dfbWindow->Resize(dfbWindow,
                                   rect.width(), rect.height());
    } else if (isMove) {
        result = dfbWindow->MoveTo(dfbWindow, rect.x(), rect.y());
    }
#else
    if (isResize) {
        result = dfbWindow->Resize(dfbWindow,
                                   rect.width(), rect.height());
    }
    if (isMove) {
        result = dfbWindow->MoveTo(dfbWindow, rect.x(), rect.y());
    }
#endif
    return result;
}
#endif // QT_NO_DIRECTFB_WM

void QDirectFBWindowSurface::setGeometry(const QRect &rect)
{
    const QRect oldRect = geometry();
    if (oldRect == rect)
        return;

    IDirectFBSurface *oldSurface = dfbSurface;
    const bool sizeChanged = oldRect.size() != rect.size();
    if (sizeChanged) {
        delete engine;
        engine = 0;
        releaseSurface();
        Q_ASSERT(!dfbSurface);
    }

    if (rect.isNull()) {
#ifndef QT_NO_DIRECTFB_WM
        if (dfbWindow) {
            if (window())
                window()->setProperty("_q_DirectFBWindowID", QVariant());

            dfbWindow->Release(dfbWindow);
            dfbWindow = 0;
        }
#endif
        Q_ASSERT(!dfbSurface);
#ifdef QT_DIRECTFB_SUBSURFACE
        Q_ASSERT(!subSurface);
#endif
    } else {
#ifdef QT_DIRECTFB_WM
        if (!dfbWindow) {
            createWindow(rect);
        } else {
            setWindowGeometry(dfbWindow, oldRect, rect);
            Q_ASSERT(!sizeChanged || !dfbSurface);
            if (sizeChanged)
                dfbWindow->GetSurface(dfbWindow, &dfbSurface);
        }
#else
        IDirectFBSurface *primarySurface = screen->primarySurface();
        DFBResult result = DFB_OK;
        if (mode == Primary) {
            Q_ASSERT(primarySurface);
            if (rect == screen->region().boundingRect()) {
                dfbSurface = primarySurface;
            } else {
                const DFBRectangle r = { rect.x(), rect.y(),
                                         rect.width(), rect.height() };
                result = primarySurface->GetSubSurface(primarySurface, &r, &dfbSurface);
            }
        } else { // mode == Offscreen
            if (!dfbSurface) {
                dfbSurface = screen->createDFBSurface(rect.size(), screen->pixelFormat(), QDirectFBScreen::DontTrackSurface);
            }
        }
        if (result != DFB_OK)
            DirectFBErrorFatal("QDirectFBWindowSurface::setGeometry()", result);
#endif
    }
    if (oldSurface != dfbSurface)
        updateFormat();

    if (oldRect.size() != rect.size()) {
        QWSWindowSurface::setGeometry(rect);
    } else {
        QWindowSurface::setGeometry(rect);
    }
}

QByteArray QDirectFBWindowSurface::permanentState() const
{
    QByteArray state(sizeof(this), 0);
    *reinterpret_cast<const QDirectFBWindowSurface**>(state.data()) = this;
    return state;
}

void QDirectFBWindowSurface::setPermanentState(const QByteArray &state)
{
    if (state.size() == sizeof(this)) {
        sibling = *reinterpret_cast<QDirectFBWindowSurface *const*>(state.constData());
        Q_ASSERT(sibling);
        sibling->setSurfaceFlags(surfaceFlags());
    }
}

static inline void scrollSurface(IDirectFBSurface *surface, const QRect &r, int dx, int dy)
{
    const DFBRectangle rect = { r.x(), r.y(), r.width(), r.height() };
    surface->Blit(surface, surface, &rect, r.x() + dx, r.y() + dy);
    const DFBRegion region = { rect.x + dx, rect.y + dy, r.right() + dx, r.bottom() + dy };
    surface->Flip(surface, &region, DSFLIP_BLIT);
}

bool QDirectFBWindowSurface::scroll(const QRegion &region, int dx, int dy)
{
    if (!dfbSurface || !(flipFlags & DSFLIP_BLIT) || region.isEmpty())
        return false;
    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    if (region.rectCount() == 1) {
        scrollSurface(dfbSurface, region.boundingRect(), dx, dy);
    } else {
        const QVector<QRect> rects = region.rects();
        const int n = rects.size();
        for (int i=0; i<n; ++i) {
            scrollSurface(dfbSurface, rects.at(i), dx, dy);
        }
    }
    return true;
}

bool QDirectFBWindowSurface::move(const QPoint &moveBy)
{
    setGeometry(geometry().translated(moveBy));
    return true;
}

void QDirectFBWindowSurface::setOpaque(bool opaque)
{
    SurfaceFlags flags = surfaceFlags();
    if (opaque != (flags & Opaque)) {
        if (opaque) {
            flags |= Opaque;
        } else {
            flags &= ~Opaque;
        }
        setSurfaceFlags(flags);
    }
}


void QDirectFBWindowSurface::flush(QWidget *widget, const QRegion &region,
                                   const QPoint &offset)
{
    QWidget *win = window();
    if (!win)
        return;

    QWExtra *extra = qt_widget_private(widget)->extraData();
    if (extra && extra->proxyWidget)
        return;

    const quint8 windowOpacity = quint8(win->windowOpacity() * 0xff);
    const QRect windowGeometry = geometry();
#ifdef QT_DIRECTFB_WM
    const bool wasNoSystemBackground = noSystemBackground;
    noSystemBackground = win->testAttribute(Qt::WA_NoSystemBackground);
    quint8 currentOpacity;
    Q_ASSERT(dfbWindow);
    dfbWindow->GetOpacity(dfbWindow, &currentOpacity);
    if (currentOpacity != windowOpacity) {
        dfbWindow->SetOpacity(dfbWindow, windowOpacity);
    }

    setOpaque(noSystemBackground || windowOpacity != 0xff);
    if (wasNoSystemBackground != noSystemBackground) {
        releaseSurface();
        dfbWindow->Release(dfbWindow);
        dfbWindow = 0;
        createWindow(windowGeometry);
        win->update();
        return;
    }
    screen->flipSurface(dfbSurface, flipFlags, region, offset);
    if (noSystemBackground) {
        dfbSurface->Clear(dfbSurface, 0, 0, 0, 0);
    }
#else
    setOpaque(windowOpacity != 0xff);
    if (mode == Offscreen) {
        screen->exposeRegion(region.translated(offset + geometry().topLeft()), 0);
    } else {
        screen->flipSurface(dfbSurface, flipFlags, region, offset);
    }
#endif

#ifdef QT_DIRECTFB_TIMING
    enum { Secs = 3 };
    ++frames;
    if (timer.elapsed() >= Secs * 1000) {
        qDebug("%d fps", int(double(frames) / double(Secs)));
        frames = 0;
        timer.restart();
    }
#endif
}

void QDirectFBWindowSurface::beginPaint(const QRegion &)
{
    if (!engine) {
        engine = new QDirectFBPaintEngine(this);
    }
}

void QDirectFBWindowSurface::endPaint(const QRegion &)
{
#ifdef QT_NO_DIRECTFB_SUBSURFACE
    unlockSurface();
#endif
}

IDirectFBSurface *QDirectFBWindowSurface::directFBSurface() const
{
    if (!dfbSurface && sibling && sibling->dfbSurface)
        return sibling->dfbSurface;
    return dfbSurface;
}


IDirectFBSurface *QDirectFBWindowSurface::surfaceForWidget(const QWidget *widget, QRect *rect) const
{
    Q_ASSERT(widget);
    if (!dfbSurface) {
        if (sibling && (!sibling->sibling || sibling->dfbSurface))
            return sibling->surfaceForWidget(widget, rect);
        return 0;
    }
    QWidget *win = window();
    Q_ASSERT(win);
    if (rect) {
        if (win == widget) {
            *rect = widget->rect();
        } else {
            *rect = QRect(widget->mapTo(win, QPoint(0, 0)), widget->size());
        }
    }

    Q_ASSERT(win == widget || win->isAncestorOf(widget));
    return dfbSurface;
}

void QDirectFBWindowSurface::updateFormat()
{
    imageFormat = dfbSurface ? QDirectFBScreen::getImageFormat(dfbSurface) : QImage::Format_Invalid;
}

void QDirectFBWindowSurface::releaseSurface()
{
    if (dfbSurface) {
#ifdef QT_DIRECTFB_SUBSURFACE
        releaseSubSurface();
#else
        unlockSurface();
#endif
#ifdef QT_NO_DIRECTFB_WM
        Q_ASSERT(screen->primarySurface());
        if (dfbSurface != screen->primarySurface())
#endif

            dfbSurface->Release(dfbSurface);
        dfbSurface = 0;
    }
}


QT_END_NAMESPACE

#endif // QT_NO_QWS_DIRECTFB


