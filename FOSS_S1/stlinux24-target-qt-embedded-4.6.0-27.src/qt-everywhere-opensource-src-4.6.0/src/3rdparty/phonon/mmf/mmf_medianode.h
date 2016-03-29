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

#ifndef PHONON_MMF_MEDIANODE_H
#define PHONON_MMF_MEDIANODE_H

#include <QObject>
#include <Phonon/EffectInterface>
#include "audioplayer.h"

QT_BEGIN_NAMESPACE

/**
 * @file mmf_medianode.h mmf_medianode.cpp
 *
 * This file starts with mmf_ in order to avoid clash with Phonon's
 * medianode.h. The GStreamer backend has a file named medianode.h, but it
 * isn't compiled with ABLD build system, which have problems with separating
 * user and system include paths.
 */

namespace Phonon
{
namespace MMF
{
class MediaObject;

/**
 * @short Base class for all nodes in the MMF backend.
 *
 * MediaNode is the base class for all nodes in the chain for MMF. Currently
 * they are:
 *
 * - MediaObject: a source of media
 * - AbstractEffect: supplying audio effects
 * - AudioOutput: pretty much a dummy interface, but is also MediaNode in order
 *   to simplify connection/disconnection.
 *
 * MediaNode provides spectatability into the chain, and also allows the
 * connection code to be written in a polymorphic manner, instead of putting it
 * all in the Backend class. Due to that MMF has no concept of chaining, the
 * order of the nodes in the graph has no meaning.
 */
class MediaNode : public QObject
{
    Q_OBJECT
public:
    MediaNode(QObject *parent);

    virtual bool connectMediaNode(MediaNode *target);
    virtual bool disconnectMediaNode(MediaNode *target);
    void setSource(MediaNode *source);

    MediaNode *source() const;
    MediaNode *target() const;

protected:
    /**
     * When connectMediaNode() is called and a MediaObject is part of
     * the its graph, this function will be called for each MediaNode in the
     * graph for which it hasn't been called yet.
     *
     * The caller guarantees that @p mo is always non-null.
     */
    virtual bool activateOnMediaObject(MediaObject *mo) = 0;

private:
    /**
     * Finds a MediaObject anywhere in the graph @p target is apart of, and
     * calls activateOnMediaObject() for all MediaNodes in the graph for which
     * it hasn't been applied to already.
     */
    bool applyNodesOnMediaObject(MediaNode *target);

    MediaNode * m_source;
    MediaNode * m_target;
    bool        m_isApplied;
};
}
}

QT_END_NAMESPACE

#endif

