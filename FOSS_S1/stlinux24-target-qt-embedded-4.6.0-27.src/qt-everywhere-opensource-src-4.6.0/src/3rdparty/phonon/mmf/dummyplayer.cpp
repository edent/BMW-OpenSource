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

#include "dummyplayer.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::DummyPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::DummyPlayer::DummyPlayer()
{

}

MMF::DummyPlayer::DummyPlayer(const AbstractPlayer& player)
        :   AbstractPlayer(player)
{

}


//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::DummyPlayer::play()
{

}

void MMF::DummyPlayer::pause()
{

}

void MMF::DummyPlayer::stop()
{

}

void MMF::DummyPlayer::seek(qint64)
{

}

bool MMF::DummyPlayer::hasVideo() const
{
    return false;
}

bool MMF::DummyPlayer::isSeekable() const
{
    return false;
}

Phonon::State MMF::DummyPlayer::state() const
{
    return Phonon::LoadingState;
}

qint64 MMF::DummyPlayer::currentTime() const
{
    return 0;
}

Phonon::ErrorType MMF::DummyPlayer::errorType() const
{
    return Phonon::NoError;
}

qint64 MMF::DummyPlayer::totalTime() const
{
    return 0;
}

MediaSource MMF::DummyPlayer::source() const
{
    return MediaSource();
}

void MMF::DummyPlayer::setFileSource(const Phonon::MediaSource &, RFile &)
{

}

void MMF::DummyPlayer::setNextSource(const MediaSource &)
{

}


//-----------------------------------------------------------------------------
// AbstractPlayer
//-----------------------------------------------------------------------------

void MMF::DummyPlayer::doSetTickInterval(qint32)
{

}

QT_END_NAMESPACE

