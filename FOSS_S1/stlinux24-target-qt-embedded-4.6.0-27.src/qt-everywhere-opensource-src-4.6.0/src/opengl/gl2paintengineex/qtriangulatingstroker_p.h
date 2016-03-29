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

#ifndef QTRIANGULATINGSTROKER_P_H
#define QTRIANGULATINGSTROKER_P_H

#include <private/qdatabuffer_p.h>
#include <private/qvectorpath_p.h>
#include <private/qbezier_p.h>
#include <private/qnumeric_p.h>
#include <private/qmath_p.h>

QT_BEGIN_NAMESPACE

class QTriangulatingStroker
{
public:
    void process(const QVectorPath &path, const QPen &pen);

    inline int vertexCount() const { return m_vertices.size(); }
    inline const float *vertices() const { return m_vertices.data(); }

    inline void setInvScale(qreal invScale) { m_inv_scale = invScale; }

private:
    inline void emitLineSegment(float x, float y, float nx, float ny);
    inline void moveTo(const qreal *pts);
    inline void lineTo(const qreal *pts);
    void cubicTo(const qreal *pts);
    inline void join(const qreal *pts);
    inline void normalVector(float x1, float y1, float x2, float y2, float *nx, float *ny);
    inline void endCap(const qreal *pts);
    inline void arc(float x, float y);
    void endCapOrJoinClosed(const qreal *start, const qreal *cur, bool implicitClose, bool endsAtStart);


    QDataBuffer<float> m_vertices;

    float m_cx, m_cy;           // current points
    float m_nvx, m_nvy;         // normal vector...
    float m_width;
    qreal m_miter_limit;

    int m_roundness;            // Number of line segments in a round join
    qreal m_sin_theta;          // sin(m_roundness / 360);
    qreal m_cos_theta;          // cos(m_roundness / 360);
    qreal m_inv_scale;
    float m_curvyness_mul;
    float m_curvyness_add;

    Qt::PenJoinStyle m_join_style;
    Qt::PenCapStyle m_cap_style;
};

class QDashedStrokeProcessor
{
public:
    QDashedStrokeProcessor();

    void process(const QVectorPath &path, const QPen &pen);

    inline void addElement(QPainterPath::ElementType type, qreal x, qreal y) {
        m_points.add(x);
        m_points.add(y);
        m_types.add(type);
    }

    inline int elementCount() const { return m_types.size(); }
    inline qreal *points() const { return m_points.data(); }
    inline QPainterPath::ElementType *elementTypes() const { return m_types.data(); }

    inline void setInvScale(qreal invScale) { m_inv_scale = invScale; }

private:
    QDataBuffer<qreal> m_points;
    QDataBuffer<QPainterPath::ElementType> m_types;
    QDashStroker m_dash_stroker;
    qreal m_inv_scale;
};





inline void QTriangulatingStroker::normalVector(float x1, float y1, float x2, float y2,
                                                float *nx, float *ny)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float pw;

    if (dx == 0)
        pw = m_width / qAbs(dy);
    else if (dy == 0)
        pw = m_width / qAbs(dx);
    else
        pw = m_width / sqrt(dx*dx + dy*dy);

    *nx = -dy * pw;
    *ny = dx * pw;
}



inline void QTriangulatingStroker::emitLineSegment(float x, float y, float vx, float vy)
{
    m_vertices.add(x + vx);
    m_vertices.add(y + vy);
    m_vertices.add(x - vx);
    m_vertices.add(y - vy);
}



// We draw a full circle for any round join or round cap which is a
// bit of overkill...
inline void QTriangulatingStroker::arc(float x, float y)
{
    float dx = m_width;
    float dy = 0;
    for (int i=0; i<=m_roundness; ++i) {
        float tmpx = dx * m_cos_theta - dy * m_sin_theta;
        float tmpy = dx * m_sin_theta + dy * m_cos_theta;
        dx = tmpx;
        dy = tmpy;
        emitLineSegment(x, y, dx, dy);
    }
}



inline void QTriangulatingStroker::endCap(const qreal *pts)
{
    switch (m_cap_style) {
    case Qt::FlatCap:
        break;
    case Qt::SquareCap: {
        float dx = m_cx - *(pts - 2);
        float dy = m_cy - *(pts - 1);

        float len = m_width / sqrt(dx * dx + dy * dy);
        dx = dx * len;
        dy = dy * len;

        emitLineSegment(m_cx + dx, m_cy + dy, m_nvx, m_nvy);
        break; }
    case Qt::RoundCap:
        arc(m_cx, m_cy);
        break;
    default: break; // to shut gcc up...
    }
}


void QTriangulatingStroker::moveTo(const qreal *pts)
{
    m_cx = pts[0];
    m_cy = pts[1];

    float x2 = pts[2];
    float y2 = pts[3];
    normalVector(m_cx, m_cy, x2, y2, &m_nvx, &m_nvy);


    // To acheive jumps we insert zero-area tringles. This is done by
    // adding two identical points in both the end of previous strip
    // and beginning of next strip
    bool invisibleJump = m_vertices.size();

    switch (m_cap_style) {
    case Qt::FlatCap:
        if (invisibleJump) {
            m_vertices.add(m_cx + m_nvx);
            m_vertices.add(m_cy + m_nvy);
        }
        break;
    case Qt::SquareCap: {
        float dx = x2 - m_cx;
        float dy = y2 - m_cy;
        float len = m_width / sqrt(dx * dx + dy * dy);
        dx = dx * len;
        dy = dy * len;
        float sx = m_cx - dx;
        float sy = m_cy - dy;
        if (invisibleJump) {
            m_vertices.add(sx + m_nvx);
            m_vertices.add(sy + m_nvy);
        }
        emitLineSegment(sx, sy, m_nvx, m_nvy);
        break; }
    case Qt::RoundCap:
        if (invisibleJump) {
            m_vertices.add(m_cx + m_nvx);
            m_vertices.add(m_cy + m_nvy);
        }

        // This emitLineSegment is not needed for the arc, but we need
        // to start where we put the invisibleJump vertex, otherwise
        // we'll have visible triangles between subpaths.
        emitLineSegment(m_cx, m_cy, m_nvx, m_nvy);
        arc(m_cx, m_cy);
        break;
    default: break; // ssssh gcc...
    }
    emitLineSegment(m_cx, m_cy, m_nvx, m_nvy);
}



void QTriangulatingStroker::lineTo(const qreal *pts)
{
    emitLineSegment(pts[0], pts[1], m_nvx, m_nvy);
    m_cx = pts[0];
    m_cy = pts[1];
}



void QTriangulatingStroker::join(const qreal *pts)
{
    // Creates a join to the next segment (m_cx, m_cy) -> (pts[0], pts[1])
    normalVector(m_cx, m_cy, pts[0], pts[1], &m_nvx, &m_nvy);

    switch (m_join_style) {
    case Qt::BevelJoin:
        break;
    case Qt::MiterJoin: {
        int p1 = m_vertices.size() - 6;
        int p2 = m_vertices.size() - 2;
        QLineF line(m_vertices.at(p1), m_vertices.at(p1+1),
                    m_vertices.at(p2), m_vertices.at(p2+1));
        QLineF nextLine(m_cx - m_nvx, m_cy - m_nvy,
                        pts[0] - m_nvx, pts[1] - m_nvy);

        QPointF isect;
        if (line.intersect(nextLine, &isect) != QLineF::NoIntersection
            && QLineF(line.p2(), isect).length() <= m_miter_limit) {
            // The intersection point mirrored over the m_cx, m_cy point
            m_vertices.add(m_cx - (isect.x() - m_cx));
            m_vertices.add(m_cy - (isect.y() - m_cy));

            // The intersection point
            m_vertices.add(isect.x());
            m_vertices.add(isect.y());
        }
        // else
        // Do a plain bevel join if the miter limit is exceeded or if
        // the lines are parallel. This is not what the raster
        // engine's stroker does, but it is both faster and similar to
        // what some other graphics API's do.

        break; }
    case Qt::RoundJoin:
        arc(m_cx, m_cy);
        break;

    default: break; // gcc warn--
    }

    emitLineSegment(m_cx, m_cy, m_nvx, m_nvy);
}

QT_END_NAMESPACE

#endif
