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

#include "demoitemanimation.h"
#include "demoitem.h"
#include "colors.h"

DemoItemAnimation::DemoItemAnimation(DemoItem *item, INOROUT inOrOut)
{
    this->opacityAt0 = 1.0;
    this->opacityAt1 = 1.0;
    this->startDelay = 0;
    this->inOrOut = inOrOut;
    this->hideOnFinished = false;
    this->forcePlay = false;
    this->timeline = new QTimeLine(5000);
    this->timeline->setFrameRange(0, 2000);
    this->timeline->setUpdateInterval(int(1000.0/Colors::fps));
    this->moveOnPlay = false;
    setTimeLine(this->timeline);
    setItem(item);
}

DemoItemAnimation::~DemoItemAnimation()
{
    // Do not delete demoitem. It is not
    // owned by an animation
    delete this->timeline;
}

void DemoItemAnimation::prepare()
{
    this->demoItem()->prepare();
}

void DemoItemAnimation::setStartPos(const QPointF &pos){
    this->startPos = pos;
}

void DemoItemAnimation::setDuration(int duration)
{
    duration = int(duration * Colors::animSpeed);
    this->timeline->setDuration(duration);
    this->moveOnPlay = true;
}

void DemoItemAnimation::setCurrentTime(int ms)
{
    this->timeline->setCurrentTime(ms);
}

bool DemoItemAnimation::notOwnerOfItem()
{
    return this != demoItem()->currentAnimation;
}

void DemoItemAnimation::play(bool fromStart, bool force)
{
    this->fromStart = fromStart;
    this->forcePlay = force;

    QPointF currPos = this->demoItem()->pos();

    // If the item that this animation controls in currently under the
    // control of another animation, stop that animation first
    if (this->demoItem()->currentAnimation)
        this->demoItem()->currentAnimation->timeline->stop();
    this->demoItem()->currentAnimation = this;
    this->timeline->stop();

    if (Colors::noAnimations && !this->forcePlay){
        this->timeline->setCurrentTime(1);
        this->demoItem()->setPos(this->posAt(1));
    }
    else{
        if (this->demoItem()->isVisible())
            // If the item is already visible, start the animation from
            // the items current position rather than from start.
            this->setPosAt(0.0, currPos);
        else
            this->setPosAt(0.0, this->startPos);

        if (this->fromStart){
            this->timeline->setCurrentTime(0);
            this->demoItem()->setPos(this->posAt(0));
        }
    }

    if (this->inOrOut == ANIM_IN)
        this->demoItem()->setRecursiveVisible(true);

    if (this->startDelay){
        QTimer::singleShot(this->startDelay, this, SLOT(playWithoutDelay()));
        return;
    }
    else
        this->playWithoutDelay();
}

void DemoItemAnimation::playWithoutDelay()
{
    if (this->moveOnPlay && !(Colors::noAnimations && !this->forcePlay))
        this->timeline->start();
    this->demoItem()->animationStarted(this->inOrOut);
}

void DemoItemAnimation::stop(bool reset)
{
    this->timeline->stop();
    if (reset)
        this->demoItem()->setPos(this->posAt(0));
    if (this->hideOnFinished && !this->moveOnPlay)
        this->demoItem()->setRecursiveVisible(false);
    this->demoItem()->animationStopped(this->inOrOut);
}

void DemoItemAnimation::setRepeat(int nr)
{
    this->timeline->setLoopCount(nr);
}

void DemoItemAnimation::playReverse()
{
}

bool DemoItemAnimation::running()
{
    return (this->timeLine()->state() == QTimeLine::Running);
}

bool DemoItemAnimation::runningOrItemLocked()
{
    return (this->running() || this->demoItem()->locked);
}

void DemoItemAnimation::lockItem(bool state)
{
    this->demoItem()->locked = state;
}

DemoItem *DemoItemAnimation::demoItem()
{
    return (DemoItem *) this->item();
}

void DemoItemAnimation::setOpacityAt0(qreal opacity)
{
    this->opacityAt0 = opacity;
}

void DemoItemAnimation::setOpacityAt1(qreal opacity)
{
    this->opacityAt1 = opacity;
}

void DemoItemAnimation::setOpacity(qreal step)
{
    DemoItem *demoItem = (DemoItem *) item();
    demoItem->opacity = this->opacityAt0 + step * step * step * (this->opacityAt1 - this->opacityAt0);
}

void DemoItemAnimation::afterAnimationStep(qreal step)
{
    if (step == 1.0f){
        if (this->timeline->loopCount() > 0){
            // animation finished.
            if (this->hideOnFinished)
                this->demoItem()->setRecursiveVisible(false);
            this->demoItem()->animationStopped(this->inOrOut);
        }
    } else if (Colors::noAnimations && !this->forcePlay){
        // The animation is not at end, but
        // the animations should not play, so go to end.
        this->setStep(1.0f); // will make this method being called recursive.
    }
}





