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

#include <cmath>
#include "guide.h"
#include "colors.h"

Guide::Guide(Guide *follows)
{
    this->scaleX = 1.0;
    this->scaleY = 1.0;

    if (follows){
        while (follows->nextGuide != follows->firstGuide) // append to end
            follows = follows->nextGuide;

        follows->nextGuide = this;
        this->prevGuide = follows;
        this->firstGuide = follows->firstGuide;
        this->nextGuide = follows->firstGuide;
        this->startLength = int(follows->startLength + follows->length()) + 1;
    }
    else{
        this->prevGuide = this;
        this->firstGuide = this;
        this->nextGuide = this;
        this->startLength = 0;
    }
}

void Guide::setScale(float scaleX, float scaleY, bool all)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;

    if (all){
        Guide *next = this->nextGuide;
        while(next != this){
            next->scaleX = scaleX;
            next->scaleY = scaleY;
            next = next->nextGuide;
        }
    }
}

void Guide::setFence(const QRectF &fence, bool all)
{
    this->fence = fence;

    if (all){
        Guide *next = this->nextGuide;
        while(next != this){
            next->fence = fence;
            next = next->nextGuide;
        }
    }
}

Guide::~Guide()
{
    if (this != this->nextGuide && this->nextGuide != this->firstGuide)
        delete this->nextGuide;
}

float Guide::lengthAll()
{
    float len = length();
    Guide *next = this->nextGuide;
    while(next != this){
        len += next->length();
        next = next->nextGuide;
    }
    return len;
}

void Guide::move(DemoItem *item, QPointF &dest, float moveSpeed)
{
    QLineF walkLine(item->getGuidedPos(), dest);
    if (moveSpeed >= 0 && walkLine.length() > moveSpeed){
        // The item is too far away from it's destination point.
        // So we choose to move it towards it instead.
        float dx = walkLine.dx();
        float dy = walkLine.dy();

        if (qAbs(dx) > qAbs(dy)){
            // walk along x-axis
            if (dx != 0){
                float d = moveSpeed * dy / qAbs(dx);
                float s = dx > 0 ? moveSpeed : -moveSpeed;
                dest.setX(item->getGuidedPos().x() + s);
                dest.setY(item->getGuidedPos().y() + d);
            }
        }
        else{
            // walk along y-axis
            if (dy != 0){
                float d = moveSpeed * dx / qAbs(dy);
                float s = dy > 0 ? moveSpeed : -moveSpeed;
                dest.setX(item->getGuidedPos().x() + d);
                dest.setY(item->getGuidedPos().y() + s);
            }
        }
    }

    item->setGuidedPos(dest);
}
