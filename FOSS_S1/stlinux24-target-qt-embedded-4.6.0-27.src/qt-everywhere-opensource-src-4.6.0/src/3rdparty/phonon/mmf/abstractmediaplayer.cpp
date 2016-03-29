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

#include <QUrl>

#include "abstractmediaplayer.h"
#include "defs.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AbstractMediaPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int       NullMaxVolume = -1;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AbstractMediaPlayer::AbstractMediaPlayer() :
            m_playPending(false)
        ,   m_tickTimer(new QTimer(this))
        ,   m_mmfMaxVolume(NullMaxVolume)
{
    connect(m_tickTimer.data(), SIGNAL(timeout()), this, SLOT(tick()));
}

MMF::AbstractMediaPlayer::AbstractMediaPlayer(const AbstractPlayer& player) :
        AbstractPlayer(player)
        ,   m_playPending(false)
        ,   m_tickTimer(new QTimer(this))
        ,   m_mmfMaxVolume(NullMaxVolume)
{
    connect(m_tickTimer.data(), SIGNAL(timeout()), this, SLOT(tick()));
}

//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::play()
{
    TRACE_CONTEXT(AbstractMediaPlayer::play, EAudioApi);
    TRACE_ENTRY("state %d", privateState());

    switch (privateState()) {
    case GroundState:
        setError(NormalError);
        break;

    case LoadingState:
        m_playPending = true;
        break;

    case StoppedState:
    case PausedState:
        doPlay();
        startTickTimer();
        changeState(PlayingState);
        break;

    case PlayingState:
    case BufferingState:
    case ErrorState:
        // Do nothing
        break;

        // Protection against adding new states and forgetting to update this switch
    default:
        TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", privateState());
}

void MMF::AbstractMediaPlayer::pause()
{
    TRACE_CONTEXT(AbstractMediaPlayer::pause, EAudioApi);
    TRACE_ENTRY("state %d", privateState());

    m_playPending = false;

    switch (privateState()) {
    case GroundState:
    case LoadingState:
    case PausedState:
    case ErrorState:
        // Do nothing
        break;

    case StoppedState:
    case PlayingState:
    case BufferingState:
        doPause();
        stopTickTimer();
        changeState(PausedState);
        break;

        // Protection against adding new states and forgetting to update this switch
    default:
        TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", privateState());
}

void MMF::AbstractMediaPlayer::stop()
{
    TRACE_CONTEXT(AbstractMediaPlayer::stop, EAudioApi);
    TRACE_ENTRY("state %d", privateState());

    m_playPending = false;

    switch (privateState()) {
    case GroundState:
    case LoadingState:
    case StoppedState:
    case ErrorState:
        // Do nothing
        break;

    case PlayingState:
    case BufferingState:
    case PausedState:
        doStop();
        stopTickTimer();
        changeState(StoppedState);
        break;

        // Protection against adding new states and forgetting to update this switch
    default:
        TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", privateState());
}

void MMF::AbstractMediaPlayer::seek(qint64 ms)
{
    TRACE_CONTEXT(AbstractMediaPlayer::seek, EAudioApi);
    TRACE_ENTRY("state %d pos %Ld", state(), ms);

    switch (privateState()) {
    // Fallthrough all these
    case GroundState:
    case StoppedState:
    case PausedState:
    case PlayingState:
    case LoadingState:
    {
        const bool tickTimerWasRunning = m_tickTimer->isActive();
        stopTickTimer();

        doSeek(ms);

        if (tickTimerWasRunning) {
            startTickTimer();
        }
        break;
    }
    case BufferingState:
    // Fallthrough
    case ErrorState:
        // Do nothing
        break;
    }

    TRACE_EXIT_0();
}

bool MMF::AbstractMediaPlayer::isSeekable() const
{
    return true;
}

void MMF::AbstractMediaPlayer::doSetTickInterval(qint32 interval)
{
    TRACE_CONTEXT(AbstractMediaPlayer::doSetTickInterval, EAudioApi);
    TRACE_ENTRY("state %d m_interval %d interval %d", privateState(), tickInterval(), interval);

    m_tickTimer->setInterval(interval);

    TRACE_EXIT_0();
}

MediaSource MMF::AbstractMediaPlayer::source() const
{
    return m_source;
}

void MMF::AbstractMediaPlayer::setFileSource(const MediaSource &source, RFile& file)
{
    TRACE_CONTEXT(AbstractMediaPlayer::setFileSource, EAudioApi);
    TRACE_ENTRY("state %d source.type %d", privateState(), source.type());

    close();
    changeState(GroundState);

    // TODO: is it correct to assign even if the media type is not supported in
    // the switch statement below?
    m_source = source;

    TInt symbianErr = KErrNone;

    switch (m_source.type()) {
    case MediaSource::LocalFile: {
        symbianErr = openFile(file);
        break;
    }

    case MediaSource::Url: {
        const QUrl url(source.url());

        if (url.scheme() == QLatin1String("file")) {
            symbianErr = openFile(file);
        }
        else {
            TRACE_0("Source type not supported");
            // TODO: support network URLs
            symbianErr = KErrNotSupported;
        }

        break;
    }

    case MediaSource::Invalid:
    case MediaSource::Disc:
    case MediaSource::Stream:
        TRACE_0("Source type not supported");
        symbianErr = KErrNotSupported;
        break;

    case MediaSource::Empty:
        TRACE_0("Empty source - doing nothing");
        TRACE_EXIT_0();
        return;

        // Protection against adding new media types and forgetting to update this switch
    default:
        TRACE_PANIC(InvalidMediaTypePanic);
    }

    if (KErrNone == symbianErr) {
        changeState(LoadingState);
    } else {
        TRACE("error %d", symbianErr)
        setError(NormalError);
    }

    TRACE_EXIT_0();
}

void MMF::AbstractMediaPlayer::setNextSource(const MediaSource &source)
{
    TRACE_CONTEXT(AbstractMediaPlayer::setNextSource, EAudioApi);
    TRACE_ENTRY("state %d", privateState());

    // TODO: handle 'next source'

    m_nextSource = source;
    Q_UNUSED(source);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VolumeObserver
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::volumeChanged(qreal volume)
{
    TRACE_CONTEXT(AbstractMediaPlayer::volumeChanged, EAudioInternal);
    TRACE_ENTRY("state %d", privateState());

    AbstractPlayer::volumeChanged(volume);
    doVolumeChanged();

    TRACE_EXIT_0();
}


void MMF::AbstractMediaPlayer::doVolumeChanged()
{
    switch (privateState()) {
    case GroundState:
    case LoadingState:
    case ErrorState:
        // Do nothing
        break;

    case StoppedState:
    case PausedState:
    case PlayingState:
    case BufferingState: {
        const qreal volume = (m_volume * m_mmfMaxVolume) + 0.5;
        const int err = setDeviceVolume(volume);

        if (KErrNone != err) {
            setError(NormalError);
        }
        break;
    }

    // Protection against adding new states and forgetting to update this
    // switch
    default:
        Utils::panic(InvalidStatePanic);
    }
}


//-----------------------------------------------------------------------------
// Protected functions
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::startTickTimer()
{
    m_tickTimer->start(tickInterval());
}

void MMF::AbstractMediaPlayer::stopTickTimer()
{
    m_tickTimer->stop();
}

void MMF::AbstractMediaPlayer::maxVolumeChanged(int mmfMaxVolume)
{
    m_mmfMaxVolume = mmfMaxVolume;
    doVolumeChanged();
}

qint64 MMF::AbstractMediaPlayer::toMilliSeconds(const TTimeIntervalMicroSeconds &in)
{
    return in.Int64() / 1000;
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::tick()
{
    // For the MWC compiler, we need to qualify the base class.
    emit MMF::AbstractPlayer::tick(currentTime());
}

void MMF::AbstractMediaPlayer::changeState(PrivateState newState)
{
    TRACE_CONTEXT(AbstractMediaPlayer::changeState, EAudioInternal);

    const Phonon::State oldPhononState = phononState(privateState());
    const Phonon::State newPhononState = phononState(newState);

    // TODO: add some invariants to check that the transition is valid
    AbstractPlayer::changeState(newState);

    if (LoadingState == oldPhononState && StoppedState == newPhononState) {
        // Ensure initial volume is set on MMF API before starting playback
        doVolumeChanged();

        // Check whether play() was called while clip was being loaded.  If so,
        // playback should be started now
        if (m_playPending) {
            TRACE_0("play was called while loading; starting playback now");
            m_playPending = false;
            play();
        }
    }
}

void MMF::AbstractMediaPlayer::updateMetaData()
{
    TRACE_CONTEXT(AbstractMediaPlayer::updateMetaData, EAudioInternal);
    TRACE_ENTRY_0();

    m_metaData.clear();

    const int numberOfEntries = numberOfMetaDataEntries();
    for(int i=0; i<numberOfEntries; ++i) {
        const QPair<QString, QString> entry = metaDataEntry(i);

        // Note that we capitalize the key, as required by the Ogg Vorbis
        // metadata standard to which Phonon adheres:
        // http://xiph.org/vorbis/doc/v-comment.html
        m_metaData.insert(entry.first.toUpper(), entry.second);
    }

    emit metaDataChanged(m_metaData);

    TRACE_EXIT_0();
}

QT_END_NAMESPACE

