/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ancestormovemonitor.h"
#include "utils.h"
#include "videooutput.h"

#include <QCoreApplication>

QT_BEGIN_NAMESPACE

using namespace Phonon::MMF;

/*! \class Phonon::MMF::AncestorMoveMonitor
  \internal
  \brief Class which installs a global event filter, and listens for move
  events which may affect the absolute position of widgets registered with
  the monitor
  See QTBUG-4956
*/


/*! \class Phonon::MMF::VideoOutputObserver
    \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

AncestorMoveMonitor::AncestorMoveMonitor(QObject *parent)
    :   QObject(parent)
{
    QCoreApplication::instance()->installEventFilter(this);
}

AncestorMoveMonitor::~AncestorMoveMonitor()
{
    QCoreApplication::instance()->removeEventFilter(this);
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void AncestorMoveMonitor::registerTarget(VideoOutput *target)
{
    TRACE_CONTEXT(AncestorMoveMonitor::registerTarget, EVideoInternal);
    TRACE_ENTRY("target 0x%08x", target);

    // First un-register the target, in case this is being called as a result
    // of re-parenting.  This is not the most efficient way to update the
    // target hash, but since this is not likely to be a frequent operation,
    // simplicity is preferred over outright speed.  In any case, re-parenting
    // of the video widget leads to re-creation of native windows, which is
    // likely to take far more processing than any implementation of this
    // function.
    unRegisterTarget(target);

    QWidget *ancestor = target->parentWidget();
    while(ancestor) {
        const Hash::iterator it = m_hash.find(ancestor);
        if(m_hash.end() == it) {
            TargetList targetList;
            targetList.append(target);
            m_hash.insert(ancestor, targetList);
        } else {
            TargetList& targetList = it.value();
            Q_ASSERT(targetList.indexOf(target) == -1);
            targetList.append(target);
        }
        ancestor = ancestor->parentWidget();
    }

    dump();

    TRACE_EXIT_0();
}

void AncestorMoveMonitor::unRegisterTarget(VideoOutput *target)
{
    TRACE_CONTEXT(AncestorMoveMonitor::unRegisterTarget, EVideoInternal);
    TRACE_ENTRY("target 0x%08x", target);

    Hash::iterator it = m_hash.begin();
    while(it != m_hash.end()) {
        TargetList& targetList = it.value();
        const int index = targetList.indexOf(target);
        if(index != -1)
            targetList.removeAt(index);
        if(targetList.count())
            ++it;
        else
            it = m_hash.erase(it);
    }

    dump();

    TRACE_EXIT_0();
}

bool AncestorMoveMonitor::eventFilter(QObject *watched, QEvent *event)
{
    TRACE_CONTEXT(AncestorMoveMonitor::eventFilter, EVideoInternal);

    if(event->type() == QEvent::Move || event->type() == QEvent::ParentChange) {

        //TRACE_ENTRY("watched 0x%08x event.type %d", watched, event->type());

        const Hash::const_iterator it = m_hash.find(watched);
        if(it != m_hash.end()) {
            const TargetList& targetList = it.value();
            VideoOutput* target = 0;
            foreach(target, targetList) {
                switch (event->type()) {

                case QEvent::Move:
                    // Notify the target that its ancestor has moved
                    target->ancestorMoved();
                    break;

                case QEvent::ParentChange:
                    // Update ancestor list for the target
                    registerTarget(target);
                    break;

                default:
                    Q_ASSERT(false);
                }
            }
        }

        //TRACE_EXIT_0();
    }

    // The event is never consumed by this filter
    return false;
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void AncestorMoveMonitor::dump()
{
#ifndef QT_NO_DEBUG
    TRACE_CONTEXT(AncestorMoveMonitor::dump, EVideoInternal);
    for(Hash::const_iterator it = m_hash.begin();
        it != m_hash.end(); ++it) {
        const QObject *ancestor = it.key();
        TRACE("ancestor 0x%08x", ancestor);
        const TargetList& targetList = it.value();
        VideoOutput* target = 0;
        foreach(target, targetList) {
            TRACE("    target 0x%08x", target);
        }
    }
#endif
}



QT_END_NAMESPACE

