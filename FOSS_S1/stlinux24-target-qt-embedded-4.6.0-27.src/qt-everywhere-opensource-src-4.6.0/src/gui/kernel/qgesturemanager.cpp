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

#include "private/qgesturemanager_p.h"
#include "private/qstandardgestures_p.h"
#include "private/qwidget_p.h"
#include "private/qgesture_p.h"
#include "private/qgraphicsitem_p.h"
#include "private/qevent_p.h"
#include "qgesture.h"
#include "qevent.h"
#include "qgraphicsitem.h"

#ifdef Q_WS_MAC
#include "qmacgesturerecognizer_mac_p.h"
#endif
#if defined(Q_OS_WIN) && !defined(QT_NO_NATIVE_GESTURES)
#include "qwinnativepangesturerecognizer_win_p.h"
#endif

#include "qdebug.h"

// #define GESTURE_DEBUG
#ifndef GESTURE_DEBUG
# define DEBUG if (0) qDebug
#else
# define DEBUG qDebug
#endif

QT_BEGIN_NAMESPACE

QGestureManager *qt_gestureManager = 0;

QGestureManager* QGestureManager::instance()
{
    if (!qt_gestureManager)
        qt_gestureManager = new QGestureManager(qApp);
    return qt_gestureManager;
}

QGestureManager::QGestureManager(QObject *parent)
    : QObject(parent), state(NotGesture), m_lastCustomGestureId(0)
{
    qRegisterMetaType<Qt::GestureState>();

#if defined(Q_WS_MAC)
    registerGestureRecognizer(new QMacSwipeGestureRecognizer);
    registerGestureRecognizer(new QMacPinchGestureRecognizer);
  #if defined(QT_MAC_USE_COCOA)
    registerGestureRecognizer(new QMacPanGestureRecognizer);
  #endif
#else
    registerGestureRecognizer(new QPanGestureRecognizer);
    registerGestureRecognizer(new QPinchGestureRecognizer);
    registerGestureRecognizer(new QSwipeGestureRecognizer);
    registerGestureRecognizer(new QTapGestureRecognizer);
#endif
#if defined(Q_OS_WIN)
  #if !defined(QT_NO_NATIVE_GESTURES)
    registerGestureRecognizer(new QWinNativePanGestureRecognizer);
  #endif
#else
    registerGestureRecognizer(new QTapAndHoldGestureRecognizer);
#endif
}

QGestureManager::~QGestureManager()
{
    qDeleteAll(m_recognizers.values());
    foreach (QGestureRecognizer *recognizer, m_obsoleteGestures.keys()) {
        qDeleteAll(m_obsoleteGestures.value(recognizer));
        delete recognizer;
    }
    m_obsoleteGestures.clear();
}

Qt::GestureType QGestureManager::registerGestureRecognizer(QGestureRecognizer *recognizer)
{
    QGesture *dummy = recognizer->create(0);
    if (!dummy) {
        qWarning("QGestureManager::registerGestureRecognizer: "
                 "the recognizer fails to create a gesture object, skipping registration.");
        return Qt::GestureType(0);
    }
    Qt::GestureType type = dummy->gestureType();
    if (type == Qt::CustomGesture) {
        // generate a new custom gesture id
        ++m_lastCustomGestureId;
        type = Qt::GestureType(Qt::CustomGesture + m_lastCustomGestureId);
    }
    m_recognizers.insertMulti(type, recognizer);
    delete dummy;
    return type;
}

void QGestureManager::unregisterGestureRecognizer(Qt::GestureType type)
{
    QList<QGestureRecognizer *> list = m_recognizers.values(type);
    m_recognizers.remove(type);
    foreach (QGesture *g, m_gestureToRecognizer.keys()) {
        QGestureRecognizer *recognizer = m_gestureToRecognizer.value(g);
        if (list.contains(recognizer)) {
            m_deletedRecognizers.insert(g, recognizer);
            m_gestureToRecognizer.remove(g);
        }
    }

    foreach (QGestureRecognizer *recognizer, list) {
        QList<QGesture *> obsoleteGestures;
        QMap<ObjectGesture, QList<QGesture *> >::Iterator iter = m_objectGestures.begin();
        while (iter != m_objectGestures.end()) {
            ObjectGesture objectGesture = iter.key();
            if (objectGesture.gesture == type)
                obsoleteGestures << iter.value();
            ++iter;
        }
        m_obsoleteGestures.insert(recognizer, obsoleteGestures);
    }
}

void QGestureManager::cleanupCachedGestures(QObject *target, Qt::GestureType type)
{
    QMap<ObjectGesture, QList<QGesture *> >::Iterator iter = m_objectGestures.begin();
    while (iter != m_objectGestures.end()) {
        ObjectGesture objectGesture = iter.key();
        if (objectGesture.gesture == type && target == objectGesture.object.data()) {
            qDeleteAll(iter.value());
            iter = m_objectGestures.erase(iter);
        } else {
            ++iter;
        }
    }
}

// get or create a QGesture object that will represent the state for a given object, used by the recognizer
QGesture *QGestureManager::getState(QObject *object, QGestureRecognizer *recognizer, Qt::GestureType type)
{
    // if the widget is being deleted we should be carefull and not to
    // create a new state, as it will create QWeakPointer which doesnt work
    // from the destructor.
    if (object->isWidgetType()) {
        if (static_cast<QWidget *>(object)->d_func()->data.in_destructor)
            return 0;
    } else if (QGesture *g = qobject_cast<QGesture *>(object)) {
        return g;
#ifndef QT_NO_GRAPHICSVIEW
    } else {
        Q_ASSERT(qobject_cast<QGraphicsObject *>(object));
#endif
    }

    QList<QGesture *> states =
            m_objectGestures.value(QGestureManager::ObjectGesture(object, type));
    // check if the QGesture for this recognizer has already been created
    foreach (QGesture *state, states) {
        if (m_gestureToRecognizer.value(state) == recognizer)
            return state;
    }

    Q_ASSERT(recognizer);
    QGesture *state = recognizer->create(object);
    if (!state)
        return 0;
    state->setParent(this);
    if (state->gestureType() == Qt::CustomGesture) {
        // if the recognizer didn't fill in the gesture type, then this
        // is a custom gesture with autogenerated id and we fill it.
        state->d_func()->gestureType = type;
#if defined(GESTURE_DEBUG)
        state->setObjectName(QString::number((int)type));
#endif
    }
    m_objectGestures[QGestureManager::ObjectGesture(object, type)].append(state);
    m_gestureToRecognizer[state] = recognizer;
    m_gestureOwners[state] = object;

    return state;
}

bool QGestureManager::filterEventThroughContexts(const QMultiHash<QObject *,
                                                 Qt::GestureType> &contexts,
                                                 QEvent *event)
{
    QSet<QGesture *> triggeredGestures;
    QSet<QGesture *> finishedGestures;
    QSet<QGesture *> newMaybeGestures;
    QSet<QGesture *> canceledGestures;
    QSet<QGesture *> notGestures;

    // TODO: sort contexts by the gesture type and check if one of the contexts
    //       is already active.

    bool ret = false;

    // filter the event through recognizers
    typedef QHash<QObject *, Qt::GestureType>::const_iterator ContextIterator;
    for (ContextIterator cit = contexts.begin(), ce = contexts.end(); cit != ce; ++cit) {
        Qt::GestureType gestureType = cit.value();
        QMap<Qt::GestureType, QGestureRecognizer *>::const_iterator
                rit = m_recognizers.lowerBound(gestureType),
                re = m_recognizers.upperBound(gestureType);
        for (; rit != re; ++rit) {
            QGestureRecognizer *recognizer = rit.value();
            QObject *target = cit.key();
            QGesture *state = getState(target, recognizer, gestureType);
            if (!state)
                continue;
            QGestureRecognizer::Result result = recognizer->recognize(state, target, event);
            QGestureRecognizer::Result type = result & QGestureRecognizer::ResultState_Mask;
            result &= QGestureRecognizer::ResultHint_Mask;
            if (type == QGestureRecognizer::TriggerGesture) {
                DEBUG() << "QGestureManager:Recognizer: gesture triggered: " << state;
                triggeredGestures << state;
            } else if (type == QGestureRecognizer::FinishGesture) {
                DEBUG() << "QGestureManager:Recognizer: gesture finished: " << state;
                finishedGestures << state;
            } else if (type == QGestureRecognizer::MayBeGesture) {
                DEBUG() << "QGestureManager:Recognizer: maybe gesture: " << state;
                newMaybeGestures << state;
            } else if (type == QGestureRecognizer::CancelGesture) {
                DEBUG() << "QGestureManager:Recognizer: not gesture: " << state;
                notGestures << state;
            } else if (type == QGestureRecognizer::Ignore) {
                DEBUG() << "QGestureManager:Recognizer: ignored the event: " << state;
            } else {
                DEBUG() << "QGestureManager:Recognizer: hm, lets assume the recognizer"
                        << "ignored the event: " << state;
            }
            if (result & QGestureRecognizer::ConsumeEventHint) {
                DEBUG() << "QGestureManager: we were asked to consume the event: "
                        << state;
                ret = true;
            }
        }
    }

    QSet<QGesture *> startedGestures = triggeredGestures - m_activeGestures;
    triggeredGestures &= m_activeGestures;

    // check if a running gesture switched back to maybe state
    QSet<QGesture *> activeToMaybeGestures = m_activeGestures & newMaybeGestures;

    // check if a running gesture switched back to not gesture state,
    // i.e. were canceled
    QSet<QGesture *> activeToCancelGestures = m_activeGestures & notGestures;
    canceledGestures += activeToCancelGestures;

    // start timers for new gestures in maybe state
    foreach (QGesture *state, newMaybeGestures) {
        QBasicTimer &timer = m_maybeGestures[state];
        if (!timer.isActive())
            timer.start(3000, this);
    }
    // kill timers for gestures that were in maybe state
    QSet<QGesture *> notMaybeGestures = (startedGestures | triggeredGestures
                                         | finishedGestures | canceledGestures
                                         | notGestures);
    foreach(QGesture *gesture, notMaybeGestures) {
        QHash<QGesture *, QBasicTimer>::iterator it =
                m_maybeGestures.find(gesture);
        if (it != m_maybeGestures.end()) {
            it.value().stop();
            m_maybeGestures.erase(it);
        }
    }

    Q_ASSERT((startedGestures & finishedGestures).isEmpty());
    Q_ASSERT((startedGestures & newMaybeGestures).isEmpty());
    Q_ASSERT((startedGestures & canceledGestures).isEmpty());
    Q_ASSERT((finishedGestures & newMaybeGestures).isEmpty());
    Q_ASSERT((finishedGestures & canceledGestures).isEmpty());
    Q_ASSERT((canceledGestures & newMaybeGestures).isEmpty());

    QSet<QGesture *> notStarted = finishedGestures - m_activeGestures;
    if (!notStarted.isEmpty()) {
        // there are some gestures that claim to be finished, but never started.
        // probably those are "singleshot" gestures so we'll fake the started state.
        foreach (QGesture *gesture, notStarted)
            gesture->d_func()->state = Qt::GestureStarted;
        QSet<QGesture *> undeliveredGestures;
        deliverEvents(notStarted, &undeliveredGestures);
        finishedGestures -= undeliveredGestures;
    }

    m_activeGestures += startedGestures;
    // sanity check: all triggered gestures should already be in active gestures list
    Q_ASSERT((m_activeGestures & triggeredGestures).size() == triggeredGestures.size());
    m_activeGestures -= finishedGestures;
    m_activeGestures -= activeToMaybeGestures;
    m_activeGestures -= canceledGestures;

    // set the proper gesture state on each gesture
    foreach (QGesture *gesture, startedGestures)
        gesture->d_func()->state = Qt::GestureStarted;
    foreach (QGesture *gesture, triggeredGestures)
        gesture->d_func()->state = Qt::GestureUpdated;
    foreach (QGesture *gesture, finishedGestures)
        gesture->d_func()->state = Qt::GestureFinished;
    foreach (QGesture *gesture, canceledGestures)
        gesture->d_func()->state = Qt::GestureCanceled;
    foreach (QGesture *gesture, activeToMaybeGestures)
        gesture->d_func()->state = Qt::GestureFinished;

    if (!m_activeGestures.isEmpty() || !m_maybeGestures.isEmpty() ||
        !startedGestures.isEmpty() || !triggeredGestures.isEmpty() ||
        !finishedGestures.isEmpty() || !canceledGestures.isEmpty()) {
        DEBUG() << "QGestureManager::filterEventThroughContexts:"
                << "\n\tactiveGestures:" << m_activeGestures
                << "\n\tmaybeGestures:" << m_maybeGestures.keys()
                << "\n\tstarted:" << startedGestures
                << "\n\ttriggered:" << triggeredGestures
                << "\n\tfinished:" << finishedGestures
                << "\n\tcanceled:" << canceledGestures;
    }

    QSet<QGesture *> undeliveredGestures;
    deliverEvents(startedGestures+triggeredGestures+finishedGestures+canceledGestures,
                  &undeliveredGestures);

    foreach (QGesture *g, startedGestures) {
        if (undeliveredGestures.contains(g))
            continue;
        if (g->gestureCancelPolicy() == QGesture::CancelAllInContext) {
            DEBUG() << "lets try to cancel some";
            // find gestures in context in Qt::GestureStarted or Qt::GestureUpdated state and cancel them
            cancelGesturesForChildren(g);
        }
    }

    m_activeGestures -= undeliveredGestures;

    // reset gestures that ended
    QSet<QGesture *> endedGestures =
            finishedGestures + canceledGestures + undeliveredGestures;
    foreach (QGesture *gesture, endedGestures) {
        recycle(gesture);
        m_gestureTargets.remove(gesture);
    }
    return ret;
}

// Cancel all gestures of children of the widget that original is associated with
void QGestureManager::cancelGesturesForChildren(QGesture *original)
{
    Q_ASSERT(original);
    QWidget *originatingWidget = m_gestureTargets.value(original);
    Q_ASSERT(originatingWidget);

    // iterate over all active gestures and all maybe gestures
    // for each find the owner
    // if the owner is part of our sub-hierarchy, cancel it.

    QSet<QGesture*> cancelledGestures;
    QSet<QGesture*>::Iterator iter = m_activeGestures.begin();
    while (iter != m_activeGestures.end()) {
        QWidget *widget = m_gestureTargets.value(*iter);
        // note that we don't touch the gestures for our originatingWidget
        if (widget != originatingWidget && originatingWidget->isAncestorOf(widget)) {
            DEBUG() << "  found a gesture to cancel" << (*iter);
            (*iter)->d_func()->state = Qt::GestureCanceled;
            cancelledGestures << *iter;
            iter = m_activeGestures.erase(iter);
        } else {
            ++iter;
        }
    }

    // TODO handle 'maybe' gestures too

    // sort them per target widget by cherry picking from almostCanceledGestures and delivering
    QSet<QGesture *> almostCanceledGestures = cancelledGestures;
    while (!almostCanceledGestures.isEmpty()) {
        QWidget *target = 0;
        QSet<QGesture*> gestures;
        iter = almostCanceledGestures.begin();
        // sort per target widget
        while (iter != almostCanceledGestures.end()) {
            QWidget *widget = m_gestureTargets.value(*iter);
            if (target == 0)
                target = widget;
            if (target == widget) {
                gestures << *iter;
                iter = almostCanceledGestures.erase(iter);
            } else {
                ++iter;
            }
        }
        Q_ASSERT(target);

        QSet<QGesture*> undeliveredGestures;
        deliverEvents(gestures, &undeliveredGestures);
    }

    for (iter = cancelledGestures.begin(); iter != cancelledGestures.end(); ++iter)
        recycle(*iter);
}

void QGestureManager::cleanupGesturesForRemovedRecognizer(QGesture *gesture)
{
    QGestureRecognizer *recognizer = m_deletedRecognizers.value(gesture);
    Q_ASSERT(recognizer);
    m_deletedRecognizers.remove(gesture);
    if (m_deletedRecognizers.keys(recognizer).isEmpty()) {
        // no more active gestures, cleanup!
        qDeleteAll(m_obsoleteGestures.value(recognizer));
        m_obsoleteGestures.remove(recognizer);
        delete recognizer;
    }
}

// return true if accepted (consumed)
bool QGestureManager::filterEvent(QWidget *receiver, QEvent *event)
{
    QSet<Qt::GestureType> types;
    QMultiHash<QObject *, Qt::GestureType> contexts;
    QWidget *w = receiver;
    typedef QMap<Qt::GestureType, Qt::GestureFlags>::const_iterator ContextIterator;
    if (!w->d_func()->gestureContext.isEmpty()) {
        for(ContextIterator it = w->d_func()->gestureContext.begin(),
            e = w->d_func()->gestureContext.end(); it != e; ++it) {
            types.insert(it.key());
            contexts.insertMulti(w, it.key());
        }
    }
    // find all gesture contexts for the widget tree
    w = w->isWindow() ? 0 : w->parentWidget();
    while (w)
    {
        for (ContextIterator it = w->d_func()->gestureContext.begin(),
             e = w->d_func()->gestureContext.end(); it != e; ++it) {
            if (!(it.value() & Qt::DontStartGestureOnChildren)) {
                if (!types.contains(it.key())) {
                    types.insert(it.key());
                    contexts.insertMulti(w, it.key());
                }
            }
        }
        if (w->isWindow())
            break;
        w = w->parentWidget();
    }
    return filterEventThroughContexts(contexts, event);
}

#ifndef QT_NO_GRAPHICSVIEW
bool QGestureManager::filterEvent(QGraphicsObject *receiver, QEvent *event)
{
    QSet<Qt::GestureType> types;
    QMultiHash<QObject *, Qt::GestureType> contexts;
    QGraphicsObject *item = receiver;
    if (!item->QGraphicsItem::d_func()->gestureContext.isEmpty()) {
        typedef QMap<Qt::GestureType, Qt::GestureFlags>::const_iterator ContextIterator;
        for(ContextIterator it = item->QGraphicsItem::d_func()->gestureContext.begin(),
            e = item->QGraphicsItem::d_func()->gestureContext.end(); it != e; ++it) {
            types.insert(it.key());
            contexts.insertMulti(item, it.key());
        }
    }
    // find all gesture contexts for the graphics object tree
    item = item->parentObject();
    while (item)
    {
        typedef QMap<Qt::GestureType, Qt::GestureFlags>::const_iterator ContextIterator;
        for (ContextIterator it = item->QGraphicsItem::d_func()->gestureContext.begin(),
             e = item->QGraphicsItem::d_func()->gestureContext.end(); it != e; ++it) {
            if (!(it.value() & Qt::DontStartGestureOnChildren)) {
                if (!types.contains(it.key())) {
                    types.insert(it.key());
                    contexts.insertMulti(item, it.key());
                }
            }
        }
        item = item->parentObject();
    }
    return filterEventThroughContexts(contexts, event);
}
#endif

bool QGestureManager::filterEvent(QGesture *state, QEvent *event)
{
    QMultiHash<QObject *, Qt::GestureType> contexts;
    contexts.insert(state, state->gestureType());
    return filterEventThroughContexts(contexts, event);
}

void QGestureManager::getGestureTargets(const QSet<QGesture*> &gestures,
    QMap<QWidget *, QList<QGesture *> > *conflicts,
    QMap<QWidget *, QList<QGesture *> > *normal)
{
    typedef QHash<Qt::GestureType, QHash<QWidget *, QGesture *> > GestureByTypes;
    GestureByTypes gestureByTypes;

    // sort gestures by types
    foreach (QGesture *gesture, gestures) {
        QWidget *receiver = m_gestureTargets.value(gesture, 0);
        Q_ASSERT(receiver);
        gestureByTypes[gesture->gestureType()].insert(receiver, gesture);
    }

    // for each gesture type
    foreach (Qt::GestureType type, gestureByTypes.keys()) {
        QHash<QWidget *, QGesture *> gestures = gestureByTypes.value(type);
        foreach (QWidget *widget, gestures.keys()) {
            QWidget *w = widget->parentWidget();
            while (w) {
                QMap<Qt::GestureType, Qt::GestureFlags>::const_iterator it
                        = w->d_func()->gestureContext.find(type);
                if (it != w->d_func()->gestureContext.end()) {
                    // i.e. 'w' listens to gesture 'type'
                    Qt::GestureFlags flags = it.value();
                    if (!(it.value() & Qt::DontStartGestureOnChildren) && w != widget) {
                        // conflicting gesture!
                        (*conflicts)[widget].append(gestures[widget]);
                        break;
                    }
                }
                if (w->isWindow()) {
                    w = 0;
                    break;
                }
                w = w->parentWidget();
            }
            if (!w)
                (*normal)[widget].append(gestures[widget]);
        }
    }
}

void QGestureManager::deliverEvents(const QSet<QGesture *> &gestures,
                                    QSet<QGesture *> *undeliveredGestures)
{
    if (gestures.isEmpty())
        return;

    typedef QMap<QWidget *, QList<QGesture *> > GesturesPerWidget;
    GesturesPerWidget conflictedGestures;
    GesturesPerWidget normalStartedGestures;

    QSet<QGesture *> startedGestures;
    // first figure out the initial receivers of gestures
    for (QSet<QGesture *>::const_iterator it = gestures.begin(),
         e = gestures.end(); it != e; ++it) {
        QGesture *gesture = *it;
        QWidget *target = m_gestureTargets.value(gesture, 0);
        if (!target) {
            // the gesture has just started and doesn't have a target yet.
            Q_ASSERT(gesture->state() == Qt::GestureStarted);
            if (gesture->hasHotSpot()) {
                // guess the target widget using the hotspot of the gesture
                QPoint pt = gesture->hotSpot().toPoint();
                if (QWidget *w = qApp->topLevelAt(pt)) {
                    target = w->childAt(w->mapFromGlobal(pt));
                }
            } else {
                // or use the context of the gesture
                QObject *context = m_gestureOwners.value(gesture, 0);
                if (context->isWidgetType())
                    target = static_cast<QWidget *>(context);
            }
            if (target)
                m_gestureTargets.insert(gesture, target);
        }

        Qt::GestureType gestureType = gesture->gestureType();
        Q_ASSERT(gestureType != Qt::CustomGesture);

        if (target) {
            if (gesture->state() == Qt::GestureStarted) {
                startedGestures.insert(gesture);
            } else {
                normalStartedGestures[target].append(gesture);
            }
        } else {
            DEBUG() << "QGestureManager::deliverEvent: could not find the target for gesture"
                    << gesture->gestureType();
            qWarning("QGestureManager::deliverEvent: could not find the target for gesture");
            undeliveredGestures->insert(gesture);
        }
    }

    getGestureTargets(startedGestures, &conflictedGestures, &normalStartedGestures);
    DEBUG() << "QGestureManager::deliverEvents:"
            << "\nstarted: " << startedGestures
            << "\nconflicted: " << conflictedGestures
            << "\nnormal: " << normalStartedGestures
            << "\n";

    // if there are conflicting gestures, send the GestureOverride event
    for (GesturesPerWidget::const_iterator it = conflictedGestures.begin(),
        e = conflictedGestures.end(); it != e; ++it) {
        QWidget *receiver = it.key();
        QList<QGesture *> gestures = it.value();
        DEBUG() << "QGestureManager::deliverEvents: sending GestureOverride to"
                << receiver
                << "gestures:" << gestures;
        QGestureEvent event(gestures);
        event.t = QEvent::GestureOverride;
        // mark event and individual gestures as ignored
        event.ignore();
        foreach(QGesture *g, gestures)
            event.setAccepted(g, false);

        QApplication::sendEvent(receiver, &event);
        bool eventAccepted = event.isAccepted();
        foreach(QGesture *gesture, event.gestures()) {
            if (eventAccepted || event.isAccepted(gesture)) {
                QWidget *w = event.d_func()->targetWidgets.value(gesture->gestureType(), 0);
                Q_ASSERT(w);
                DEBUG() << "override event: gesture was accepted:" << gesture << w;
                QList<QGesture *> &gestures = normalStartedGestures[w];
                gestures.append(gesture);
                // override the target
                m_gestureTargets[gesture] = w;
            } else {
                DEBUG() << "override event: gesture wasn't accepted. putting back:" << gesture;
                QList<QGesture *> &gestures = normalStartedGestures[receiver];
                gestures.append(gesture);
            }
        }
    }

    // delivering gestures that are not in conflicted state
    for (GesturesPerWidget::const_iterator it = normalStartedGestures.begin(),
        e = normalStartedGestures.end(); it != e; ++it) {
        if (!it.value().isEmpty()) {
            DEBUG() << "QGestureManager::deliverEvents: sending to" << it.key()
                    << "gestures:" << it.value();
            QGestureEvent event(it.value());
            QApplication::sendEvent(it.key(), &event);
            bool eventAccepted = event.isAccepted();
            foreach (QGesture *gesture, event.gestures()) {
                if (gesture->state() == Qt::GestureStarted &&
                    (eventAccepted || event.isAccepted(gesture))) {
                    QWidget *w = event.d_func()->targetWidgets.value(gesture->gestureType(), 0);
                    Q_ASSERT(w);
                    DEBUG() << "started gesture was delivered and accepted by" << w;
                    m_gestureTargets[gesture] = w;
                }
            }
        }
    }
}

void QGestureManager::timerEvent(QTimerEvent *event)
{
    QHash<QGesture *, QBasicTimer>::iterator it = m_maybeGestures.begin(),
                                             e = m_maybeGestures.end();
    for (; it != e; ) {
        QBasicTimer &timer = it.value();
        Q_ASSERT(timer.isActive());
        if (timer.timerId() == event->timerId()) {
            timer.stop();
            QGesture *gesture = it.key();
            it = m_maybeGestures.erase(it);
            DEBUG() << "QGestureManager::timerEvent: gesture stopped due to timeout:"
                    << gesture;
            recycle(gesture);
        } else {
            ++it;
        }
    }
}

void QGestureManager::recycle(QGesture *gesture)
{
    QGestureRecognizer *recognizer = m_gestureToRecognizer.value(gesture, 0);
    if (recognizer) {
        gesture->setGestureCancelPolicy(QGesture::CancelNone);
        recognizer->reset(gesture);
    } else {
        cleanupGesturesForRemovedRecognizer(gesture);
    }
}

QT_END_NAMESPACE

#include "moc_qgesturemanager_p.cpp"
