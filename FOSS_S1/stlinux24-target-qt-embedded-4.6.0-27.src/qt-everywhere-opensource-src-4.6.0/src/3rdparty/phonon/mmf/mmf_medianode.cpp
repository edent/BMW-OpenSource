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

#include "mediaobject.h"

#include "mmf_medianode.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::MediaNode
  \internal
*/

MMF::MediaNode::MediaNode(QObject *parent) : QObject::QObject(parent)
                                           , m_source(0)
                                           , m_target(0)
                                           , m_isApplied(false)
{
}

bool MMF::MediaNode::connectMediaNode(MediaNode *target)
{
    m_target = target;
    m_target->setSource(this);

    return applyNodesOnMediaObject(target);
}

bool MMF::MediaNode::disconnectMediaNode(MediaNode *target)
{
    Q_UNUSED(target);
    m_target = 0;
    m_isApplied = false;
    return true;
}

void MMF::MediaNode::setSource(MediaNode *source)
{
    m_source = source;
}

MMF::MediaNode *MMF::MediaNode::source() const
{
    return m_source;
}

MMF::MediaNode *MMF::MediaNode::target() const
{
    return m_target;
}

bool MMF::MediaNode::applyNodesOnMediaObject(MediaNode *)
{
    // Algorithmically, this can be expressed in a more efficient way by
    // exercising available assumptions, but it complicates code for input
    // data(length of the graph) which typically is very small.

    // First, we go to the very beginning of the graph.
    MMF::MediaNode *current = this;
    do {
        MediaNode *const candidate = current->source();
        if (candidate)
            current = candidate;
        else
            break;
    }
    while (current);

    // Now we do two things, while walking to the other end:
    // 1. Find the MediaObject, if present
    // 2. Collect a list of all unapplied MediaNodes

    QList<MediaNode *> unapplied;
    MMF::MediaObject *mo = 0;

    do {
        if (!current->m_isApplied)
            unapplied.append(current);

        if (!mo)
            mo = qobject_cast<MMF::MediaObject *>(current);

        current = current->target();
    }
    while (current);

    // Now, lets activate all the objects, if we found the MediaObject.

    if (mo) {
        for (int i = 0; i < unapplied.count(); ++i) {
            MediaNode *const at = unapplied.at(i);

            // We don't want to apply MediaObject on itself.
            if (at != mo)
                at->activateOnMediaObject(mo);
        }
    }

    return true;
}

QT_END_NAMESPACE

