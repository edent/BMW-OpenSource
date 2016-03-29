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

#include "abstractplayer.h"
#include "defs.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AbstractPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AbstractPlayer::AbstractPlayer()
        :   m_videoOutput(0)
        ,   m_volume(InitialVolume)
        ,   m_state(GroundState)
        ,   m_error(NoError)
        ,   m_tickInterval(DefaultTickInterval)
        ,   m_transitionTime(0)
        ,   m_prefinishMark(0)
{

}

MMF::AbstractPlayer::AbstractPlayer(const AbstractPlayer& player)
        :   m_videoOutput(player.m_videoOutput)
        ,   m_volume(player.m_volume)
        ,   m_state(GroundState)
        ,   m_error(NoError)
        ,   m_tickInterval(player.tickInterval())
        ,   m_transitionTime(player.transitionTime())
        ,   m_prefinishMark(player.prefinishMark())
{

}

//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

qint32 MMF::AbstractPlayer::tickInterval() const
{
    return m_tickInterval;
}

void MMF::AbstractPlayer::setTickInterval(qint32 interval)
{
    m_tickInterval = interval;
    doSetTickInterval(interval);
}

qint32 MMF::AbstractPlayer::prefinishMark() const
{
    return m_prefinishMark;
}

void MMF::AbstractPlayer::setPrefinishMark(qint32 mark)
{
    m_prefinishMark = mark;
}

qint32 MMF::AbstractPlayer::transitionTime() const
{
    return m_transitionTime;
}

void MMF::AbstractPlayer::setTransitionTime(qint32 time)
{
    m_transitionTime = time;
}


//-----------------------------------------------------------------------------
// VolumeObserver
//-----------------------------------------------------------------------------

void MMF::AbstractPlayer::volumeChanged(qreal volume)
{
    m_volume = volume;
}


//-----------------------------------------------------------------------------
// Video output
//-----------------------------------------------------------------------------

void MMF::AbstractPlayer::setVideoOutput(VideoOutput* videoOutput)
{
    m_videoOutput = videoOutput;
    videoOutputChanged();
}

void MMF::AbstractPlayer::videoOutputChanged()
{
    // Default behaviour is empty - overridden by VideoPlayer
}

void MMF::AbstractPlayer::setError(Phonon::ErrorType error,
                                   const QString &errorMessage)
{
    TRACE_CONTEXT(AbstractPlayer::setError, EAudioInternal);
    TRACE_ENTRY("state %d error %d", m_state, error);

    m_error = error;
    m_errorString = errorMessage;
    changeState(ErrorState);

    TRACE_EXIT_0();
}

Phonon::ErrorType MMF::AbstractPlayer::errorType() const
{
    const Phonon::ErrorType result = (ErrorState == m_state)
                                     ? errorType() : NoError;
    return result;
}

QString MMF::AbstractPlayer::errorString() const
{
    return m_errorString;
}

Phonon::State MMF::AbstractPlayer::phononState() const
{
    return phononState(m_state);
}

Phonon::State MMF::AbstractPlayer::phononState(PrivateState state)
{
    const Phonon::State phononState =
        GroundState == state
        ?    Phonon::LoadingState
        :    static_cast<Phonon::State>(state);

    return phononState;
}

AbstractPlayer::PrivateState AbstractPlayer::privateState() const
{
    return m_state;
}

Phonon::State MMF::AbstractPlayer::state() const
{
    return phononState(m_state);
}

void MMF::AbstractPlayer::setState(PrivateState newState)
{
    m_state = newState;
}

void MMF::AbstractPlayer::changeState(PrivateState newState)
{
    TRACE_CONTEXT(AbstractPlayer::changeState, EAudioInternal);
    TRACE_ENTRY("state %d newState %d", privateState(), newState);

    // TODO: add some invariants to check that the transition is valid

    const Phonon::State oldPhononState = phononState(privateState());

    // We need to change the state before we emit stateChanged(), because
    // some user code, for instance the mediaplayer, switch on MediaObject's
    // state.
    setState(newState);

    const Phonon::State newPhononState = phononState(newState);

    if (oldPhononState != newPhononState) {
        TRACE("emit stateChanged(%d, %d)", newPhononState, oldPhononState);
        emit stateChanged(newPhononState, oldPhononState);
    }

    TRACE_EXIT_0();
}

QT_END_NAMESPACE

