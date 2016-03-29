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

#include "context2d.h"

#include <QVariant>

#include <math.h>
static const double Q_PI   = 3.14159265358979323846;   // pi

#define DEGREES(t) ((t) * 180.0 / Q_PI)

#define qClamp(val, min, max) qMin(qMax(val, min), max)
static QList<qreal> parseNumbersList(QString::const_iterator &itr)
{
    QList<qreal> points;
    QString temp;
    while ((*itr).isSpace())
        ++itr;
    while ((*itr).isNumber() ||
           (*itr) == '-' || (*itr) == '+' || (*itr) == '.') {
        temp = QString();

        if ((*itr) == '-')
            temp += *itr++;
        else if ((*itr) == '+')
            temp += *itr++;
        while ((*itr).isDigit())
            temp += *itr++;
        if ((*itr) == '.')
            temp += *itr++;
        while ((*itr).isDigit())
            temp += *itr++;
        while ((*itr).isSpace())
            ++itr;
        if ((*itr) == ',')
            ++itr;
        points.append(temp.toDouble());
        //eat spaces
        while ((*itr).isSpace())
            ++itr;
    }

    return points;
}

QColor colorFromString(const QString &name)
{
    QString::const_iterator itr = name.constBegin();
    QList<qreal> compo;
    if (name.startsWith("rgba(")) {
        ++itr; ++itr; ++itr; ++itr; ++itr;
        compo = parseNumbersList(itr);
        if (compo.size() != 4) {
            return QColor();
        }
        //alpha seems to be always between 0-1
        compo[3] *= 255;
        return QColor((int)compo[0], (int)compo[1],
                      (int)compo[2], (int)compo[3]);
    } else if (name.startsWith("rgb(")) {
        ++itr; ++itr; ++itr; ++itr;
        compo = parseNumbersList(itr);
        if (compo.size() != 3) {
            return QColor();
        }
        return QColor((int)qClamp(compo[0], qreal(0), qreal(255)),
                      (int)qClamp(compo[1], qreal(0), qreal(255)),
                      (int)qClamp(compo[2], qreal(0), qreal(255)));
    } else {
        //QRgb color;
        //CSSParser::parseColor(name, color);
        return QColor(name);
    }
}


static QPainter::CompositionMode compositeOperatorFromString(const QString &compositeOperator)
{
    if ( compositeOperator == "source-over" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "source-out" ) {
        return QPainter::CompositionMode_SourceOut;
    } else if ( compositeOperator == "source-in" ) {
        return QPainter::CompositionMode_SourceIn;
    } else if ( compositeOperator == "source-atop" ) {
        return QPainter::CompositionMode_SourceAtop;
    } else if ( compositeOperator == "destination-atop" ) {
        return QPainter::CompositionMode_DestinationAtop;
    } else if ( compositeOperator == "destination-in" ) {
        return QPainter::CompositionMode_DestinationIn;
    } else if ( compositeOperator == "destination-out" ) {
        return QPainter::CompositionMode_DestinationOut;
    } else if ( compositeOperator == "destination-over" ) {
        return QPainter::CompositionMode_DestinationOver;
    } else if ( compositeOperator == "darker" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "lighter" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "copy" ) {
        return QPainter::CompositionMode_Source;
    } else if ( compositeOperator == "xor" ) {
        return QPainter::CompositionMode_Xor;
    }

    return QPainter::CompositionMode_SourceOver;
}

static QString compositeOperatorToString(QPainter::CompositionMode op)
{
    switch (op) {
    case QPainter::CompositionMode_SourceOver:
        return "source-over";
    case QPainter::CompositionMode_DestinationOver:
        return "destination-over";
    case QPainter::CompositionMode_Clear:
        return "clear";
    case QPainter::CompositionMode_Source:
        return "source";
    case QPainter::CompositionMode_Destination:
        return "destination";
    case QPainter::CompositionMode_SourceIn:
        return "source-in";
    case QPainter::CompositionMode_DestinationIn:
        return "destination-in";
    case QPainter::CompositionMode_SourceOut:
        return "source-out";
    case QPainter::CompositionMode_DestinationOut:
        return "destination-out";
    case QPainter::CompositionMode_SourceAtop:
        return "source-atop";
    case QPainter::CompositionMode_DestinationAtop:
        return "destination-atop";
    case QPainter::CompositionMode_Xor:
        return "xor";
    case QPainter::CompositionMode_Plus:
        return "plus";
    case QPainter::CompositionMode_Multiply:
        return "multiply";
    case QPainter::CompositionMode_Screen:
        return "screen";
    case QPainter::CompositionMode_Overlay:
        return "overlay";
    case QPainter::CompositionMode_Darken:
        return "darken";
    case QPainter::CompositionMode_Lighten:
        return "lighten";
    case QPainter::CompositionMode_ColorDodge:
        return "color-dodge";
    case QPainter::CompositionMode_ColorBurn:
        return "color-burn";
    case QPainter::CompositionMode_HardLight:
        return "hard-light";
    case QPainter::CompositionMode_SoftLight:
        return "soft-light";
    case QPainter::CompositionMode_Difference:
        return "difference";
    case QPainter::CompositionMode_Exclusion:
        return "exclusion";
    default:
        break;
    }
    return QString();
}

void Context2D::save()
{
    m_stateStack.push(m_state);
}


void Context2D::restore()
{
    if (!m_stateStack.isEmpty()) {
        m_state = m_stateStack.pop();
        m_state.flags = AllIsFullOfDirt;
    }
}


void Context2D::scale(qreal x, qreal y)
{
    m_state.matrix.scale(x, y);
    m_state.flags |= DirtyTransformationMatrix;
}


void Context2D::rotate(qreal angle)
{
    m_state.matrix.rotate(DEGREES(angle));
    m_state.flags |= DirtyTransformationMatrix;
}


void Context2D::translate(qreal x, qreal y)
{
    m_state.matrix.translate(x, y);
    m_state.flags |= DirtyTransformationMatrix;
}


void Context2D::transform(qreal m11, qreal m12, qreal m21, qreal m22,
                          qreal dx, qreal dy)
{
    QMatrix mat(m11, m12,
                m21, m22,
                dx, dy);
    m_state.matrix *= mat;
    m_state.flags |= DirtyTransformationMatrix;
}


void Context2D::setTransform(qreal m11, qreal m12, qreal m21, qreal m22,
                             qreal dx, qreal dy)
{
    QMatrix mat(m11, m12,
                m21, m22,
                dx, dy);
    m_state.matrix = mat;
    m_state.flags |= DirtyTransformationMatrix;
}


QString Context2D::globalCompositeOperation() const
{
    return compositeOperatorToString(m_state.globalCompositeOperation);
}

void Context2D::setGlobalCompositeOperation(const QString &op)
{
    QPainter::CompositionMode mode =
        compositeOperatorFromString(op);
    m_state.globalCompositeOperation = mode;
    m_state.flags |= DirtyGlobalCompositeOperation;
}

QVariant Context2D::strokeStyle() const
{
    return m_state.strokeStyle;
}

void Context2D::setStrokeStyle(const QVariant &style)
{
    if (qVariantCanConvert<CanvasGradient>(style)) {
        CanvasGradient cg = qvariant_cast<CanvasGradient>(style);
        m_state.strokeStyle = cg.value;
    } else {
        QColor color = colorFromString(style.toString());
        m_state.strokeStyle = color;
    }
    m_state.flags |= DirtyStrokeStyle;
}

QVariant Context2D::fillStyle() const
{
    return m_state.fillStyle;
}

//! [3]
void Context2D::setFillStyle(const QVariant &style)
{
    if (qVariantCanConvert<CanvasGradient>(style)) {
        CanvasGradient cg = qvariant_cast<CanvasGradient>(style);
        m_state.fillStyle = cg.value;
    } else {
        QColor color = colorFromString(style.toString());
        m_state.fillStyle = color;
    }
    m_state.flags |= DirtyFillStyle;
}
//! [3]

qreal Context2D::globalAlpha() const
{
    return m_state.globalAlpha;
}

void Context2D::setGlobalAlpha(qreal alpha)
{
    m_state.globalAlpha = alpha;
    m_state.flags |= DirtyGlobalAlpha;
}


CanvasGradient Context2D::createLinearGradient(qreal x0, qreal y0,
                                               qreal x1, qreal y1)
{
    QLinearGradient g(x0, y0, x1, y1);
    return CanvasGradient(g);
}


CanvasGradient Context2D::createRadialGradient(qreal x0, qreal y0,
                                               qreal r0, qreal x1,
                                               qreal y1, qreal r1)
{
    QRadialGradient g(QPointF(x1, y1), r0+r1, QPointF(x0, y0));
    return CanvasGradient(g);
}

qreal Context2D::lineWidth() const
{
    return m_state.lineWidth;
}

void Context2D::setLineWidth(qreal w)
{
    m_state.lineWidth = w;
    m_state.flags |= DirtyLineWidth;
}

//! [0]
QString Context2D::lineCap() const
{
    switch (m_state.lineCap) {
    case Qt::FlatCap:
        return "butt";
    case Qt::SquareCap:
        return "square";
    case Qt::RoundCap:
        return "round";
    default: ;
    }
    return QString();
}

void Context2D::setLineCap(const QString &capString)
{
    Qt::PenCapStyle style;
    if (capString == "round")
        style = Qt::RoundCap;
    else if (capString == "square")
        style = Qt::SquareCap;
    else //if (capString == "butt")
        style = Qt::FlatCap;
    m_state.lineCap = style;
    m_state.flags |= DirtyLineCap;
}
//! [0]

QString Context2D::lineJoin() const
{
    switch (m_state.lineJoin) {
    case Qt::RoundJoin:
        return "round";
    case Qt::BevelJoin:
        return "bevel";
    case Qt::MiterJoin:
        return "miter";
    default: ;
    }
    return QString();
}

void Context2D::setLineJoin(const QString &joinString)
{
    Qt::PenJoinStyle style;
    if (joinString == "round")
        style = Qt::RoundJoin;
    else if (joinString == "bevel")
        style = Qt::BevelJoin;
    else //if (joinString == "miter")
        style = Qt::MiterJoin;
    m_state.lineJoin = style;
    m_state.flags |= DirtyLineJoin;
}

qreal Context2D::miterLimit() const
{
    return m_state.miterLimit;
}

void Context2D::setMiterLimit(qreal m)
{
    m_state.miterLimit = m;
    m_state.flags |= DirtyMiterLimit;
}

void Context2D::setShadowOffsetX(qreal x)
{
    m_state.shadowOffsetX = x;
    m_state.flags |= DirtyShadowOffsetX;
}

void Context2D::setShadowOffsetY(qreal y)
{
    m_state.shadowOffsetY = y;
    m_state.flags |= DirtyShadowOffsetY;
}

void Context2D::setShadowBlur(qreal b)
{
    m_state.shadowBlur = b;
    m_state.flags |= DirtyShadowBlur;
}

void Context2D::setShadowColor(const QString &str)
{
    m_state.shadowColor = colorFromString(str);
    m_state.flags |= DirtyShadowColor;
}

qreal Context2D::shadowOffsetX() const
{
    return m_state.shadowOffsetX;
}

qreal Context2D::shadowOffsetY() const
{
    return m_state.shadowOffsetY;
}


qreal Context2D::shadowBlur() const
{
    return m_state.shadowBlur;
}


QString Context2D::shadowColor() const
{
    return m_state.shadowColor.name();
}


void Context2D::clearRect(qreal x, qreal y, qreal w, qreal h)
{
    beginPainting();
    m_painter.save();
    m_painter.setMatrix(m_state.matrix, false);
    m_painter.setCompositionMode(QPainter::CompositionMode_Source);
    m_painter.fillRect(QRectF(x, y, w, h), QColor(0, 0, 0, 0));
    m_painter.restore();
    scheduleChange();
}


//! [1]
void Context2D::fillRect(qreal x, qreal y, qreal w, qreal h)
{
    beginPainting();
    m_painter.save();
    m_painter.setMatrix(m_state.matrix, false);
    m_painter.fillRect(QRectF(x, y, w, h), m_painter.brush());
    m_painter.restore();
    scheduleChange();
}
//! [1]


void Context2D::strokeRect(qreal x, qreal y, qreal w, qreal h)
{
    QPainterPath path;
    path.addRect(x, y, w, h);
    beginPainting();
    m_painter.save();
    m_painter.setMatrix(m_state.matrix, false);
    m_painter.strokePath(path, m_painter.pen());
    m_painter.restore();
    scheduleChange();
}


void Context2D::beginPath()
{
    m_path = QPainterPath();
}


void Context2D::closePath()
{
    m_path.closeSubpath();
}


void Context2D::moveTo(qreal x, qreal y)
{
    QPointF pt = m_state.matrix.map(QPointF(x, y));
    m_path.moveTo(pt);
}


void Context2D::lineTo(qreal x, qreal y)
{
    QPointF pt = m_state.matrix.map(QPointF(x, y));
    m_path.lineTo(pt);
}


void Context2D::quadraticCurveTo(qreal cpx, qreal cpy, qreal x, qreal y)
{
    QPointF cp = m_state.matrix.map(QPointF(cpx, cpy));
    QPointF xy = m_state.matrix.map(QPointF(x, y));
    m_path.quadTo(cp, xy);
}


void Context2D::bezierCurveTo(qreal cp1x, qreal cp1y,
                              qreal cp2x, qreal cp2y, qreal x, qreal y)
{
    QPointF cp1 = m_state.matrix.map(QPointF(cp1x, cp1y));
    QPointF cp2 = m_state.matrix.map(QPointF(cp2x, cp2y));
    QPointF end = m_state.matrix.map(QPointF(x, y));
    m_path.cubicTo(cp1, cp2, end);
}


void Context2D::arcTo(qreal x1, qreal y1, qreal x2, qreal y2, qreal radius)
{
    //FIXME: this is surely busted
    QPointF st  = m_state.matrix.map(QPointF(x1, y1));
    QPointF end = m_state.matrix.map(QPointF(x2, y2));
    m_path.arcTo(st.x(), st.y(),
                 end.x()-st.x(), end.y()-st.y(),
                 radius, 90);
}


void Context2D::rect(qreal x, qreal y, qreal w, qreal h)
{
    QPainterPath path; path.addRect(x, y, w, h);
    path = m_state.matrix.map(path);
    m_path.addPath(path);
}

void Context2D::arc(qreal xc, qreal yc, qreal radius,
                    qreal sar, qreal ear,
                    bool anticlockwise)
{
    //### HACK
    // In Qt we don't switch the coordinate system for degrees
    // and still use the 0,0 as bottom left for degrees so we need
    // to switch
    sar = -sar;
    ear = -ear;
    anticlockwise = !anticlockwise;
    //end hack

    float sa = DEGREES(sar);
    float ea = DEGREES(ear);

    double span = 0;

    double xs     = xc - radius;
    double ys     = yc - radius;
    double width  = radius*2;
    double height = radius*2;

    if (!anticlockwise && (ea < sa)) {
        span += 360;
    } else if (anticlockwise && (sa < ea)) {
        span -= 360;
    }

    //### this is also due to switched coordinate system
    // we would end up with a 0 span instead of 360
    if (!(qFuzzyCompare(span + (ea - sa) + 1, 1) &&
          qFuzzyCompare(qAbs(span), 360))) {
        span   += ea - sa;
    }

    QPainterPath path;
    path.moveTo(QPointF(xc + radius  * cos(sar),
                                yc - radius  * sin(sar)));

    path.arcTo(xs, ys, width, height, sa, span);
    path = m_state.matrix.map(path);
    m_path.addPath(path);
}


void Context2D::fill()
{
    beginPainting();
    m_painter.fillPath(m_path, m_painter.brush());
    scheduleChange();
}


void Context2D::stroke()
{
    beginPainting();
    m_painter.save();
    m_painter.setMatrix(m_state.matrix, false);
    QPainterPath tmp = m_state.matrix.inverted().map(m_path);
    m_painter.strokePath(tmp, m_painter.pen());
    m_painter.restore();
    scheduleChange();
}


void Context2D::clip()
{
    m_state.clipPath = m_path;
    m_state.flags |= DirtyClippingRegion;
}


bool Context2D::isPointInPath(qreal x, qreal y) const
{
    return m_path.contains(QPointF(x, y));
}


ImageData Context2D::getImageData(qreal sx, qreal sy, qreal sw, qreal sh)
{
    Q_UNUSED(sx);
    Q_UNUSED(sy);
    Q_UNUSED(sw);
    Q_UNUSED(sh);
    return ImageData();
}


void Context2D::putImageData(ImageData image, qreal dx, qreal dy)
{
    Q_UNUSED(image);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
}

Context2D::Context2D(QObject *parent)
    : QObject(parent), m_changeTimerId(-1)
{
    reset();
}

const QImage &Context2D::endPainting()
{
    if (m_painter.isActive())
        m_painter.end();
    return m_image;
}

void Context2D::beginPainting()
{
    if (!m_painter.isActive()) {
        m_painter.begin(&m_image);
        m_painter.setRenderHint(QPainter::Antialiasing);
        if (!m_state.clipPath.isEmpty())
            m_painter.setClipPath(m_state.clipPath);
        m_painter.setBrush(m_state.fillStyle);
        m_painter.setOpacity(m_state.globalAlpha);
        QPen pen;
        pen.setBrush(m_state.strokeStyle);
        if (pen.style() == Qt::NoPen)
            pen.setStyle(Qt::SolidLine);
        pen.setCapStyle(m_state.lineCap);
        pen.setJoinStyle(m_state.lineJoin);
        pen.setWidthF(m_state.lineWidth);
        pen.setMiterLimit(m_state.miterLimit);
        m_painter.setPen(pen);
    } else {
        if ((m_state.flags & DirtyClippingRegion) && !m_state.clipPath.isEmpty())
            m_painter.setClipPath(m_state.clipPath);
        if (m_state.flags & DirtyFillStyle)
            m_painter.setBrush(m_state.fillStyle);
        if (m_state.flags & DirtyGlobalAlpha)
            m_painter.setOpacity(m_state.globalAlpha);
        if (m_state.flags & DirtyGlobalCompositeOperation)
            m_painter.setCompositionMode(m_state.globalCompositeOperation);
        if (m_state.flags & MDirtyPen) {
            QPen pen = m_painter.pen();
            if (m_state.flags & DirtyStrokeStyle)
                pen.setBrush(m_state.strokeStyle);
            if (m_state.flags & DirtyLineWidth)
                pen.setWidthF(m_state.lineWidth);
            if (m_state.flags & DirtyLineCap)
                pen.setCapStyle(m_state.lineCap);
            if (m_state.flags & DirtyLineJoin)
                pen.setJoinStyle(m_state.lineJoin);
            if (m_state.flags & DirtyMiterLimit)
                pen.setMiterLimit(m_state.miterLimit);
            m_painter.setPen(pen);
        }
        m_state.flags = 0;
    }
}

void Context2D::clear()
{
    endPainting();
    m_image.fill(qRgba(0,0,0,0));
    scheduleChange();
}

void Context2D::reset()
{
    m_stateStack.clear();
    m_state.matrix = QMatrix();
    m_state.clipPath = QPainterPath();
    m_state.globalAlpha = 1.0;
    m_state.globalCompositeOperation = QPainter::CompositionMode_SourceOver;
    m_state.strokeStyle = Qt::black;
    m_state.fillStyle = Qt::black;
    m_state.lineWidth = 1;
    m_state.lineCap = Qt::FlatCap;
    m_state.lineJoin = Qt::MiterJoin;
    m_state.miterLimit = 10;
    m_state.shadowOffsetX = 0;
    m_state.shadowOffsetY = 0;
    m_state.shadowBlur = 0;
    m_state.shadowColor = qRgba(0, 0, 0, 0);
    m_state.flags = AllIsFullOfDirt;
    clear();
}

void Context2D::setSize(int width, int height)
{
    endPainting();
    QImage newi(width, height, QImage::Format_ARGB32_Premultiplied);
    newi.fill(qRgba(0,0,0,0));
    QPainter p(&newi);
    p.drawImage(0, 0, m_image);
    p.end();
    m_image = newi;
    scheduleChange();
}

void Context2D::setSize(const QSize &size)
{
    setSize(size.width(), size.height());
}

QSize Context2D::size() const
{
    return m_image.size();
}

void Context2D::drawImage(DomImage *image, qreal dx, qreal dy)
{
    if (!image)
        return;
    if (dx < 0) {
        qreal sx = qAbs(dx);
        qreal sy = qAbs(dy);
        qreal sw = image->width() - sx;
        qreal sh = image->height() - sy;

        drawImage(image, sx, sy, sw, sh, 0, 0, sw, sh);
    } else {
        beginPainting();
        m_painter.drawImage(QPointF(dx, dy), image->image());
        scheduleChange();
    }
}

void Context2D::drawImage(DomImage *image, qreal dx, qreal dy,
                          qreal dw, qreal dh)
{
    if (!image)
        return;
    beginPainting();
    m_painter.drawImage(QRectF(dx, dy, dw, dh).toRect(), image->image());
    scheduleChange();
}

void Context2D::drawImage(DomImage *image, qreal sx, qreal sy,
                          qreal sw, qreal sh, qreal dx, qreal dy,
                          qreal dw, qreal dh)
{
    if (!image)
        return;
    beginPainting();
    m_painter.drawImage(QRectF(dx, dy, dw, dh), image->image(),
                        QRectF(sx, sy, sw, sh));
    scheduleChange();
}

//! [2]
void Context2D::scheduleChange()
{
    if (m_changeTimerId == -1)
        m_changeTimerId = startTimer(0);
}

void Context2D::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_changeTimerId) {
        killTimer(m_changeTimerId);
        m_changeTimerId = -1;
        emit changed(endPainting());
    } else {
        QObject::timerEvent(e);
    }
}
//! [2]
