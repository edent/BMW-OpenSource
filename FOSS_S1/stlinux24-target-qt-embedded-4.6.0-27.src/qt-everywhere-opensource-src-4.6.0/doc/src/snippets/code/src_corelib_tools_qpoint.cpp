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
QPoint p;

p.setX(p.x() + 1);
p += QPoint(1, 0);
p.rx()++;
//! [0]


//! [1]
QPoint p(1, 2);
p.rx()--;   // p becomes (0, 2)
//! [1]


//! [2]
QPoint p(1, 2);
p.ry()++;   // p becomes (1, 3)
//! [2]


//! [3]
QPoint p( 3, 7);
QPoint q(-1, 4);
p += q;    // p becomes (2, 11)
//! [3]


//! [4]
QPoint p( 3, 7);
QPoint q(-1, 4);
p -= q;    // p becomes (4, 3)
//! [4]


//! [5]
QPoint p(-1, 4);
p *= 2.5;    // p becomes (-3, 10)
//! [5]


//! [6]
QPoint p(-3, 10);
p /= 2.5;           // p becomes (-1, 4)
//! [6]


//! [7]
QPoint oldPosition;

MyWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - oldPosition;
    if (point.manhattanLength() > 3)
        // the mouse has moved more than 3 pixels since the oldPosition
}
//! [7]


//! [8]
double trueLength = sqrt(pow(x(), 2) + pow(y(), 2));
//! [8]


//! [9]
QPointF p;

p.setX(p.x() + 1.0);
p += QPointF(1.0, 0.0);
p.rx()++;
//! [9]


//! [10]
 QPointF p(1.1, 2.5);
 p.rx()--;   // p becomes (0.1, 2.5)
//! [10]


//! [11]
QPointF p(1.1, 2.5);
p.ry()++;   // p becomes (1.1, 3.5)
//! [11]


//! [12]
QPointF p( 3.1, 7.1);
QPointF q(-1.0, 4.1);
p += q;    // p becomes (2.1, 11.2)
//! [12]


//! [13]
QPointF p( 3.1, 7.1);
QPointF q(-1.0, 4.1);
p -= q;    // p becomes (4.1, 3.0)
//! [13]


//! [14]
QPointF p(-1.1, 4.1);
p *= 2.5;    // p becomes (-2.75, 10.25)
//! [14]


//! [15]
QPointF p(-2.75, 10.25);
p /= 2.5;           // p becomes (-1.1, 4.1)
//! [15]
