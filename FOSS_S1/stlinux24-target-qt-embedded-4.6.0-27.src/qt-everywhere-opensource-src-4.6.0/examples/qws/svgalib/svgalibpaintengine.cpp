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

#include "svgalibpaintengine.h"

#include <QColor>
#include <vga.h>
#include <vgagl.h>

SvgalibPaintEngine::SvgalibPaintEngine()
{
}

SvgalibPaintEngine::~SvgalibPaintEngine()
{
}

//! [0]
bool SvgalibPaintEngine::begin(QPaintDevice *dev)
{
    device = dev;
    pen = Qt::NoPen;
    simplePen = true;
    brush = Qt::NoBrush;
    simpleBrush = true;
    matrix = QMatrix();
    simpleMatrix = true;
    setClip(QRect(0, 0, device->width(), device->height()));
    opaque = true;
    aliased = true;
    sourceOver = true;

    return QRasterPaintEngine::begin(dev);
}
//! [0]

//! [1]
bool SvgalibPaintEngine::end()
{
    gl_setclippingwindow(0, 0, device->width() - 1, device->height() - 1);
    return QRasterPaintEngine::end();
}
//! [1]

//! [2]
void SvgalibPaintEngine::updateState(const QPaintEngineState &state)
{
    QPaintEngine::DirtyFlags flags = state.state();

    if (flags & DirtyTransform) {
        matrix = state.matrix();
        simpleMatrix = (matrix.m12() == 0 && matrix.m21() == 0);
    }

    if (flags & DirtyPen) {
        pen = state.pen();
        simplePen = (pen.width() == 0 || pen.widthF() <= 1)
                    && (pen.style() == Qt::NoPen || pen.style() == Qt::SolidLine)
                    && (pen.color().alpha() == 255);
    }

    if (flags & DirtyBrush) {
        brush = state.brush();
        simpleBrush = (brush.style() == Qt::SolidPattern
                       || brush.style() == Qt::NoBrush)
                      && (brush.color().alpha() == 255);
    }

    if (flags & DirtyClipRegion)
        setClip(state.clipRegion());

    if (flags & DirtyClipEnabled) {
        clipEnabled = state.isClipEnabled();
        updateClip();
    }

    if (flags & DirtyClipPath) {
        setClip(QRegion());
        simpleClip = false;
    }

    if (flags & DirtyCompositionMode) {
        const QPainter::CompositionMode m = state.compositionMode();
        sourceOver = (m == QPainter::CompositionMode_SourceOver);
    }

    if (flags & DirtyOpacity)
        opaque = (state.opacity() == 256);

    if (flags & DirtyHints)
        aliased = !(state.renderHints() & QPainter::Antialiasing);

    QRasterPaintEngine::updateState(state);
}
//! [2]

//! [3]
void SvgalibPaintEngine::setClip(const QRegion &region)
{
    if (region.isEmpty())
        clip = QRect(0, 0, device->width(), device->height());
    else
        clip = matrix.map(region) & QRect(0, 0, device->width(), device->height());
    clipEnabled = true;
    updateClip();
}
//! [3]

//! [4]
void SvgalibPaintEngine::updateClip()
{
    QRegion clipRegion = QRect(0, 0, device->width(), device->height());

    if (!systemClip().isEmpty())
        clipRegion &= systemClip();
    if (clipEnabled)
        clipRegion &= clip;

    simpleClip = (clipRegion.rects().size() <= 1);

    const QRect r = clipRegion.boundingRect();
    gl_setclippingwindow(r.left(), r.top(),
                         r.x() + r.width(),
                         r.y() + r.height());
}
//! [4]

//! [5]
void SvgalibPaintEngine::drawRects(const QRect *rects, int rectCount)
{
    const bool canAccelerate = simplePen && simpleBrush && simpleMatrix
                               && simpleClip && opaque && aliased
                               && sourceOver;
    if (!canAccelerate) {
        QRasterPaintEngine::drawRects(rects, rectCount);
        return;
    }

    for (int i = 0; i < rectCount; ++i) {
        const QRect r = matrix.mapRect(rects[i]);
        if (brush != Qt::NoBrush) {
            gl_fillbox(r.left(), r.top(), r.width(), r.height(),
                       brush.color().rgba());
        }
        if (pen != Qt::NoPen) {
            const int c = pen.color().rgba();
            gl_hline(r.left(), r.top(), r.right(), c);
            gl_hline(r.left(), r.bottom(), r.right(), c);
            gl_line(r.left(), r.top(), r.left(), r.bottom(), c);
            gl_line(r.right(), r.top(), r.right(), r.bottom(), c);
        }
    }
}
//! [5]
