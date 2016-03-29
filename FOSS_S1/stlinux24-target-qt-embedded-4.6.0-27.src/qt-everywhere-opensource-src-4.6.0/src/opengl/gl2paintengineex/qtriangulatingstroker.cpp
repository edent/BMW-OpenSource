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

#include "qtriangulatingstroker_p.h"
#include <qmath.h>

QT_BEGIN_NAMESPACE

#define CURVE_FLATNESS Q_PI / 8




void QTriangulatingStroker::endCapOrJoinClosed(const qreal *start, const qreal *cur,
                                               bool implicitClose, bool endsAtStart)
{
    if (endsAtStart) {
        join(start + 2);
    } else if (implicitClose) {
        join(start);
        lineTo(start);
        join(start+2);
    } else {
        endCap(cur);
    }
    int count = m_vertices.size();
    m_vertices.add(m_vertices.at(count-2));
    m_vertices.add(m_vertices.at(count-1));
}


void QTriangulatingStroker::process(const QVectorPath &path, const QPen &pen)
{
    const qreal *pts = path.points();
    const QPainterPath::ElementType *types = path.elements();
    int count = path.elementCount();
    if (count < 2)
        return;

    float realWidth = qpen_widthf(pen);
    if (realWidth == 0)
        realWidth = 1;

    m_width = realWidth / 2;

    bool cosmetic = pen.isCosmetic();
    if (cosmetic) {
        m_width = m_width * m_inv_scale;
    }

    m_join_style = qpen_joinStyle(pen);
    m_cap_style = qpen_capStyle(pen);
    m_vertices.reset();
    m_miter_limit = pen.miterLimit() * qpen_widthf(pen);

    // The curvyness is based on the notion that I originally wanted
    // roughly one line segment pr 4 pixels. This may seem little, but
    // because we sample at constantly incrementing B(t) E [0<t<1], we
    // will get longer segments where the curvature is small and smaller
    // segments when the curvature is high.
    //
    // To get a rough idea of the length of each curve, I pretend that
    // the curve is a 90 degree arc, whose radius is
    // qMax(curveBounds.width, curveBounds.height). Based on this
    // logic we can estimate the length of the outline edges based on
    // the radius + a pen width and adjusting for scale factors
    // depending on if the pen is cosmetic or not.
    //
    // The curvyness value of PI/14 was based on,
    // arcLength=2*PI*r/4=PI/2 and splitting length into somewhere
    // between 3 and 8 where 5 seemed to be give pretty good results
    // hence: Q_PI/14. Lower divisors will give more detail at the
    // direct cost of performance.

    // simplfy pens that are thin in device size (2px wide or less)
    if (realWidth < 2.5 && (cosmetic || m_inv_scale == 1)) {
        if (m_cap_style == Qt::RoundCap)
            m_cap_style = Qt::SquareCap;
        if (m_join_style == Qt::RoundJoin)
            m_join_style = Qt::MiterJoin;
        m_curvyness_add = 0.5;
        m_curvyness_mul = CURVE_FLATNESS / m_inv_scale;
        m_roundness = 1;
    } else if (cosmetic) {
        m_curvyness_add = realWidth / 2;
        m_curvyness_mul = CURVE_FLATNESS;
        m_roundness = qMax<int>(4, realWidth * CURVE_FLATNESS);
    } else {
        m_curvyness_add = m_width;
        m_curvyness_mul = CURVE_FLATNESS / m_inv_scale;
        m_roundness = qMax<int>(4, realWidth * m_curvyness_mul);
    }

    // Over this level of segmentation, there doesn't seem to be any
    // benefit, even for huge penWidth
    if (m_roundness > 24)
        m_roundness = 24;

    m_sin_theta = qFastSin(Q_PI / m_roundness);
    m_cos_theta = qFastCos(Q_PI / m_roundness);

    const qreal *endPts = pts + (count<<1);
    const qreal *startPts;

    Qt::PenCapStyle cap = m_cap_style;

    if (!types) {
        startPts = pts;

        bool endsAtStart = startPts[0] == *(endPts-2) && startPts[1] == *(endPts-1);

        if (endsAtStart || path.hasImplicitClose())
            m_cap_style = Qt::FlatCap;
        moveTo(pts);
        m_cap_style = cap;
        pts += 2;
        lineTo(pts);
        pts += 2;
        while (pts < endPts) {
            join(pts);
            lineTo(pts);
            pts += 2;
        }

        endCapOrJoinClosed(startPts, pts-2, path.hasImplicitClose(), endsAtStart);

    } else {
        bool endsAtStart;
        while (pts < endPts) {
            switch (*types) {
            case QPainterPath::MoveToElement: {
                if (pts != path.points())
                    endCapOrJoinClosed(startPts, pts-2, path.hasImplicitClose(), endsAtStart);

                startPts = pts;
                int end = (endPts - pts) / 2;
                int i = 2; // Start looking to ahead since we never have two moveto's in a row
                while (i<end && types[i] != QPainterPath::MoveToElement) {
                    ++i;
                }
                endsAtStart = startPts[0] == pts[i*2 - 2] && startPts[1] == pts[i*2 - 1];
                if (endsAtStart || path.hasImplicitClose())
                    m_cap_style = Qt::FlatCap;

                moveTo(pts);
                m_cap_style = cap;
                pts+=2;
                ++types;
                break; }
            case QPainterPath::LineToElement:
                if (*(types - 1) != QPainterPath::MoveToElement)
                    join(pts);
                lineTo(pts);
                pts+=2;
                ++types;
                break;
            case QPainterPath::CurveToElement:
                if (*(types - 1) != QPainterPath::MoveToElement)
                    join(pts);
                cubicTo(pts);
                pts+=6;
                types+=3;
                break;
            default:
                Q_ASSERT(false);
                break;
            }
        }

        endCapOrJoinClosed(startPts, pts-2, path.hasImplicitClose(), endsAtStart);
    }
}

void QTriangulatingStroker::cubicTo(const qreal *pts)
{
    const QPointF *p = (const QPointF *) pts;
    QBezier bezier = QBezier::fromPoints(*(p - 1), p[0], p[1], p[2]);

    QRectF bounds = bezier.bounds();
    float rad = qMax(bounds.width(), bounds.height());
    int threshold = qMin<float>(64, (rad + m_curvyness_add) * m_curvyness_mul);
    if (threshold < 4)
        threshold = 4;
    qreal threshold_minus_1 = threshold - 1;
    float vx, vy;

    float cx = m_cx, cy = m_cy;
    float x, y;

    for (int i=1; i<threshold; ++i) {
        qreal t = qreal(i) / threshold_minus_1;
        QPointF p = bezier.pointAt(t);
        x = p.x();
        y = p.y();

        normalVector(cx, cy, x, y, &vx, &vy);

        emitLineSegment(x, y, vx, vy);

        cx = x;
        cy = y;
    }

    m_cx = cx;
    m_cy = cy;

    m_nvx = vx;
    m_nvy = vy;
}

static void qdashprocessor_moveTo(qreal x, qreal y, void *data)
{
    ((QDashedStrokeProcessor *) data)->addElement(QPainterPath::MoveToElement, x, y);
}

static void qdashprocessor_lineTo(qreal x, qreal y, void *data)
{
    ((QDashedStrokeProcessor *) data)->addElement(QPainterPath::LineToElement, x, y);
}

static void qdashprocessor_cubicTo(qreal, qreal, qreal, qreal, qreal, qreal, void *)
{
    Q_ASSERT(0); // The dasher should not produce curves...
}

QDashedStrokeProcessor::QDashedStrokeProcessor()
    : m_dash_stroker(0), m_inv_scale(1)
{
    m_dash_stroker.setMoveToHook(qdashprocessor_moveTo);
    m_dash_stroker.setLineToHook(qdashprocessor_lineTo);
    m_dash_stroker.setCubicToHook(qdashprocessor_cubicTo);
}

void QDashedStrokeProcessor::process(const QVectorPath &path, const QPen &pen)
{

    const qreal *pts = path.points();
    const QPainterPath::ElementType *types = path.elements();
    int count = path.elementCount();

    m_points.reset();
    m_types.reset();

    qreal width = qpen_widthf(pen);
    if (width == 0)
        width = 1;

    m_dash_stroker.setDashPattern(pen.dashPattern());
    m_dash_stroker.setStrokeWidth(pen.isCosmetic() ? width * m_inv_scale : width);
    m_dash_stroker.setMiterLimit(pen.miterLimit());
    qreal curvyness = sqrt(width) * m_inv_scale / 8;

    if (count < 2)
        return;

    const qreal *endPts = pts + (count<<1);

    m_dash_stroker.begin(this);

    if (!types) {
        m_dash_stroker.moveTo(pts[0], pts[1]);
        pts += 2;
        while (pts < endPts) {
            m_dash_stroker.lineTo(pts[0], pts[1]);
            pts += 2;
        }
    } else {
        while (pts < endPts) {
            switch (*types) {
            case QPainterPath::MoveToElement:
                m_dash_stroker.moveTo(pts[0], pts[1]);
                pts += 2;
                ++types;
                break;
            case QPainterPath::LineToElement:
                m_dash_stroker.lineTo(pts[0], pts[1]);
                pts += 2;
                ++types;
                break;
            case QPainterPath::CurveToElement: {
                QBezier b = QBezier::fromPoints(*(((const QPointF *) pts) - 1),
                                                *(((const QPointF *) pts)),
                                                *(((const QPointF *) pts) + 1),
                                                *(((const QPointF *) pts) + 2));
                QRectF bounds = b.bounds();
                int threshold = qMin<float>(64, qMax(bounds.width(), bounds.height()) * curvyness);
                if (threshold < 4)
                    threshold = 4;
                qreal threshold_minus_1 = threshold - 1;
                for (int i=0; i<threshold; ++i) {
                    QPointF pt = b.pointAt(i / threshold_minus_1);
                    m_dash_stroker.lineTo(pt.x(), pt.y());
                }
                pts += 6;
                types += 3;
                break; }
            default: break;
            }
        }
    }

    m_dash_stroker.end();
}

QT_END_NAMESPACE

