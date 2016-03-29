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

#ifndef QGESTURE_P_H
#define QGESTURE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qrect.h"
#include "qpoint.h"
#include "qdatetime.h"
#include "qgesture.h"
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

class QGesturePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGesture)

public:
    QGesturePrivate()
        : gestureType(Qt::CustomGesture), state(Qt::NoGesture),
          isHotSpotSet(false), gestureCancelPolicy(0)

    {
    }

    Qt::GestureType gestureType;
    Qt::GestureState state;
    QPointF hotSpot;
    uint isHotSpotSet : 1;
    uint gestureCancelPolicy : 2;
};

class QPanGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QPanGesture)

public:
    QPanGesturePrivate()
        : acceleration(0)
    {
    }

    QPointF lastOffset;
    QPointF offset;
    QPoint startPosition;
    qreal acceleration;
};

class QPinchGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QPinchGesture)

public:
    QPinchGesturePrivate()
        : totalChangeFlags(0), changeFlags(0),
          totalScaleFactor(0), lastScaleFactor(0), scaleFactor(0),
          totalRotationAngle(0), lastRotationAngle(0), rotationAngle(0),
          isNewSequence(true)
    {
    }

    QPinchGesture::ChangeFlags totalChangeFlags;
    QPinchGesture::ChangeFlags changeFlags;

    QPointF startCenterPoint;
    QPointF lastCenterPoint;
    QPointF centerPoint;

    qreal totalScaleFactor;
    qreal lastScaleFactor;
    qreal scaleFactor;

    qreal totalRotationAngle;
    qreal lastRotationAngle;
    qreal rotationAngle;

    bool isNewSequence;
    QPointF startPosition[2];
};

class QSwipeGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QSwipeGesture)

public:
    QSwipeGesturePrivate()
        : horizontalDirection(QSwipeGesture::NoDirection),
          verticalDirection(QSwipeGesture::NoDirection),
          swipeAngle(0),
          started(false), speed(0)
    {
    }

    QSwipeGesture::SwipeDirection horizontalDirection;
    QSwipeGesture::SwipeDirection verticalDirection;
    qreal swipeAngle;

    QPoint lastPositions[3];
    bool started;
    qreal speed;
    QTime time;
};

class QTapGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QTapGesture)

public:
    QTapGesturePrivate()
    {
    }

    QPointF position;
};

class QTapAndHoldGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QTapAndHoldGesture)

public:
    QTapAndHoldGesturePrivate()
        : timerId(0)
    {
    }

    QPointF position;
    int timerId;
};

QT_END_NAMESPACE

#endif // QGESTURE_P_H
