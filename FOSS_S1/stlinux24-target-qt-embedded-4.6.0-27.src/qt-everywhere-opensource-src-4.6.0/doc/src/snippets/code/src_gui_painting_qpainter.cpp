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

//! [0]
void SimpleExampleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Qt");
}
//! [0]


//! [1]
void MyWidget::paintEvent(QPaintEvent *)
{
    QPainter p;
    p.begin(this);
    p.drawLine(...);        // drawing code
    p.end();
}
//! [1]


//! [2]
void MyWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawLine(...);        // drawing code
}
//! [2]


//! [3]
painter->begin(0); // impossible - paint device cannot be 0

QPixmap image(0, 0);
painter->begin(&image); // impossible - image.isNull() == true;

painter->begin(myWidget);
painter2->begin(myWidget); // impossible - only one painter at a time
//! [3]


//! [4]
void QPainter::rotate(qreal angle)
{
    QMatrix matrix;
    matrix.rotate(angle);
    setWorldMatrix(matrix, true);
}
//! [4]


//! [5]
QPainterPath path;
path.moveTo(20, 80);
path.lineTo(20, 30);
path.cubicTo(80, 0, 50, 50, 80, 80);

QPainter painter(this);
painter.drawPath(path);
//! [5]


//! [6]
QLineF line(10.0, 80.0, 90.0, 20.0);

QPainter(this);
painter.drawLine(line);
//! [6]


//! [7]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);

QPainter painter(this);
painter.drawRect(rectangle);
//! [7]


//! [8]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);

QPainter painter(this);
painter.drawRoundedRect(rectangle, 20.0, 15.0);
//! [8]


//! [9]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);

QPainter painter(this);
painter.drawEllipse(rectangle);
//! [9]


//! [10]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);
int startAngle = 30 * 16;
int spanAngle = 120 * 16;

QPainter painter(this);
painter.drawArc(rectangle, startAngle, spanAngle);
//! [10]


//! [11]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);
int startAngle = 30 * 16;
int spanAngle = 120 * 16;

QPainter painter(this);
painter.drawPie(rectangle, startAngle, spanAngle);
//! [11]


//! [12]
QRectF rectangle(10.0, 20.0, 80.0, 60.0);
int startAngle = 30 * 16;
int spanAngle = 120 * 16;

QPainter painter(this);
painter.drawChord(rect, startAngle, spanAngle);
//! [12]


//! [13]
static const QPointF points[3] = {
    QPointF(10.0, 80.0),
    QPointF(20.0, 10.0),
    QPointF(80.0, 30.0),
};

QPainter painter(this);
painter.drawPolyline(points, 3);
//! [13]


//! [14]
static const QPointF points[4] = {
    QPointF(10.0, 80.0),
    QPointF(20.0, 10.0),
    QPointF(80.0, 30.0),
    QPointF(90.0, 70.0)
};

QPainter painter(this);
painter.drawPolygon(points, 4);
//! [14]


//! [15]
static const QPointF points[4] = {
    QPointF(10.0, 80.0),
    QPointF(20.0, 10.0),
    QPointF(80.0, 30.0),
    QPointF(90.0, 70.0)
};

QPainter painter(this);
painter.drawConvexPolygon(points, 4);
//! [15]


//! [16]
QRectF target(10.0, 20.0, 80.0, 60.0);
QRectF source(0.0, 0.0, 70.0, 40.0);
QPixmap pixmap(":myPixmap.png");

QPainter(this);
painter.drawPixmap(target, image, source);
//! [16]


//! [17]
QPainter painter(this);
painter.drawText(rect, Qt::AlignCenter, tr("Qt by\nNokia"));
//! [17]


//! [18]
QPicture picture;
QPointF point(10.0, 20.0)
picture.load("drawing.pic");

QPainter painter(this);
painter.drawPicture(0, 0, picture);
//! [18]


//! [19]
fillRect(rectangle, background()).
//! [19]


//! [20]
QRectF target(10.0, 20.0, 80.0, 60.0);
QRectF source(0.0, 0.0, 70.0, 40.0);
QImage image(":/images/myImage.png");

QPainter painter(this);
painter.drawImage(target, image, source);
//! [20]


//! [21]
QPainter painter(this);
painter.fillRect(0, 0, 128, 128, Qt::green);
painter.beginNativePainting();

glEnable(GL_SCISSOR_TEST);
glScissor(0, 0, 64, 64);

glClearColor(1, 0, 0, 1);
glClear(GL_COLOR_BUFFER_BIT);

glDisable(GL_SCISSOR_TEST);

painter.endNativePainting();
//! [21]
