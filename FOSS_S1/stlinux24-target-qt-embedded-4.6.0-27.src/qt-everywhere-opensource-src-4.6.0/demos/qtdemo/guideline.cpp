/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "guideline.h"
#include <cmath>

GuideLine::GuideLine(const QLineF &line, Guide *follows) : Guide(follows)
{
    this->line = line;
}

GuideLine::GuideLine(const QPointF &end, Guide *follows) : Guide(follows)
{
    if (follows)
        this->line = QLineF(prevGuide->endPos(), end);
    else
        this->line = QLineF(QPointF(0, 0), end);
}

float GuideLine::length()
{
    return line.length();
}

QPointF GuideLine::startPos()
{
    return QPointF(this->line.p1().x() * scaleX, this->line.p1().y() * scaleY);
}

QPointF GuideLine::endPos()
{
    return QPointF(this->line.p2().x() * scaleX, this->line.p2().y() * scaleY);
}

void GuideLine::guide(DemoItem *item, float moveSpeed)
{
    float frame = item->guideFrame - this->startLength;
    float endX = (this->line.p1().x() + (frame * this->line.dx() / this->length())) * scaleX;
    float endY = (this->line.p1().y() + (frame * this->line.dy() / this->length())) * scaleY;
    QPointF pos(endX, endY);
    this->move(item, pos, moveSpeed);
}

