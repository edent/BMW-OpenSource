/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "lifecycle.h"
#include "stickman.h"
#include "node.h"
#include "animation.h"
#include "graphicsview.h"

#include <QtCore>
#include <QtGui>

class KeyPressTransition: public QSignalTransition
{
public:
    KeyPressTransition(GraphicsView *receiver, Qt::Key key)
        : QSignalTransition(receiver, SIGNAL(keyPressed(int))), m_key(key)
    {
    }
    KeyPressTransition(GraphicsView *receiver, Qt::Key key, QAbstractState *target)
        : QSignalTransition(receiver, SIGNAL(keyPressed(int))), m_key(key)
    {
        setTargetState(target);
    }

    virtual bool eventTest(QEvent *e)
    {
        if (QSignalTransition::eventTest(e)) {
            QVariant key = static_cast<QStateMachine::SignalEvent*>(e)->arguments().at(0);
            return (key.toInt() == int(m_key));
        } 

        return false;
    }
private:
    Qt::Key m_key;
};

//! [4]
class LightningStrikesTransition: public QEventTransition
{
public:
    LightningStrikesTransition(QAbstractState *target)
        : QEventTransition(this, QEvent::Timer)
    {
        setTargetState(target);
        qsrand((uint)QDateTime::currentDateTime().toTime_t());
        startTimer(1000);
    }

    virtual bool eventTest(QEvent *e)
    {
        return QEventTransition::eventTest(e) && ((qrand() % 50) == 0);
    }
};
//! [4]

LifeCycle::LifeCycle(StickMan *stickMan, GraphicsView *keyReceiver)
    : m_stickMan(stickMan), m_keyReceiver(keyReceiver)
{
    // Create animation group to be used for all transitions
    m_animationGroup = new QParallelAnimationGroup();
    const int stickManNodeCount = m_stickMan->nodeCount();
    for (int i=0; i<stickManNodeCount; ++i) {
        QPropertyAnimation *pa = new QPropertyAnimation(m_stickMan->node(i), "pos");
        m_animationGroup->addAnimation(pa);    
    }

    // Set up intial state graph
//! [3]
    m_machine = new QStateMachine();
    m_machine->addDefaultAnimation(m_animationGroup);
//! [3]

    m_alive = new QState(m_machine);
    m_alive->setObjectName("alive");
    
    // Make it blink when lightning strikes before entering dead animation
    QState *lightningBlink = new QState(m_machine);    
    lightningBlink->assignProperty(m_stickMan->scene(), "backgroundBrush", Qt::white);
    lightningBlink->assignProperty(m_stickMan, "penColor", Qt::black);
    lightningBlink->assignProperty(m_stickMan, "fillColor", Qt::white);
    lightningBlink->assignProperty(m_stickMan, "isDead", true);
    
//! [5]
    QTimer *timer = new QTimer(lightningBlink);
    timer->setSingleShot(true);
    timer->setInterval(100);
    QObject::connect(lightningBlink, SIGNAL(entered()), timer, SLOT(start()));
    QObject::connect(lightningBlink, SIGNAL(exited()), timer, SLOT(stop()));
//! [5]
  
    m_dead = new QState(m_machine);
    m_dead->assignProperty(m_stickMan->scene(), "backgroundBrush", Qt::black);
    m_dead->assignProperty(m_stickMan, "penColor", Qt::white);
    m_dead->assignProperty(m_stickMan, "fillColor", Qt::black);
    m_dead->setObjectName("dead");
           
    // Idle state (sets no properties)
    m_idle = new QState(m_alive);
    m_idle->setObjectName("idle");

    m_alive->setInitialState(m_idle);

    // Lightning strikes at random
    m_alive->addTransition(new LightningStrikesTransition(lightningBlink));
//! [0]
    lightningBlink->addTransition(timer, SIGNAL(timeout()), m_dead);
//! [0]

    m_machine->setInitialState(m_alive);
}

void LifeCycle::setDeathAnimation(const QString &fileName)
{
    QState *deathAnimation = makeState(m_dead, fileName);
    m_dead->setInitialState(deathAnimation);
}

void LifeCycle::start()
{
    m_machine->start();
}

void LifeCycle::addActivity(const QString &fileName, Qt::Key key)
{
    QState *state = makeState(m_alive, fileName);
    m_alive->addTransition(new KeyPressTransition(m_keyReceiver, key, state));
}

QState *LifeCycle::makeState(QState *parentState, const QString &animationFileName)
{
    QState *topLevel = new QState(parentState);
    
    Animation animation;
    {
        QFile file(animationFileName);
        if (file.open(QIODevice::ReadOnly))
            animation.load(&file);
    }

    const int frameCount = animation.totalFrames();
    QState *previousState = 0;
    for (int i=0; i<frameCount; ++i) {
        animation.setCurrentFrame(i);

//! [1]
        QState *frameState = new QState(topLevel);                       
        const int nodeCount = animation.nodeCount();
        for (int j=0; j<nodeCount; ++j)
            frameState->assignProperty(m_stickMan->node(j), "pos", animation.nodePos(j));
//! [1]

        frameState->setObjectName(QString::fromLatin1("frame %0").arg(i));
        if (previousState == 0)
            topLevel->setInitialState(frameState);
        else
//! [2]
            previousState->addTransition(previousState, SIGNAL(propertiesAssigned()), frameState);
//! [2]
        
        previousState = frameState;
    }

    // Loop
    previousState->addTransition(previousState, SIGNAL(propertiesAssigned()), topLevel->initialState());

    return topLevel;

}

LifeCycle::~LifeCycle()
{
    delete m_machine;
    delete m_animationGroup;
}
