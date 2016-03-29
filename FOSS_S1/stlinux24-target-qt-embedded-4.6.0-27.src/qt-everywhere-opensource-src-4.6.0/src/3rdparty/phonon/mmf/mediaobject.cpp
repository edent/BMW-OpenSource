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

#include "audiooutput.h"
#include "audioplayer.h"
#include "defs.h"
#include "dummyplayer.h"
#include "utils.h"
#include "utils.h"
#include "mmf_videoplayer.h"
#include "videowidget.h"

#include "mediaobject.h"

#include <QDir>
#include <QUrl>

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::MediaObject
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::MediaObject::MediaObject(QObject *parent) : MMF::MediaNode::MediaNode(parent)
                                               , m_recognizerOpened(false)
{
    m_player.reset(new DummyPlayer());

    TRACE_CONTEXT(MediaObject::MediaObject, EAudioApi);
    TRACE_ENTRY_0();

    Q_UNUSED(parent);

    TRACE_EXIT_0();
}

MMF::MediaObject::~MediaObject()
{
    TRACE_CONTEXT(MediaObject::~MediaObject, EAudioApi);
    TRACE_ENTRY_0();

    m_file.Close();
    m_fileServer.Close();
    m_recognizer.Close();

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Recognizer
//-----------------------------------------------------------------------------

bool MMF::MediaObject::openRecognizer()
{
    TRACE_CONTEXT(MediaObject::openRecognizer, EAudioInternal);

    if (!m_recognizerOpened) {
        TInt err = m_recognizer.Connect();
        if (KErrNone != err) {
            TRACE("RApaLsSession::Connect error %d", err);
            return false;
        }

        err = m_fileServer.Connect();
        if (KErrNone != err) {
            TRACE("RFs::Connect error %d", err);
            return false;
        }

        // This must be called in order to be able to share file handles with
        // the recognizer server (see fileMediaType function).
        err = m_fileServer.ShareProtected();
        if (KErrNone != err) {
            TRACE("RFs::ShareProtected error %d", err);
            return false;
        }

        m_recognizerOpened = true;
    }

    return true;
}

MMF::MediaType MMF::MediaObject::fileMediaType
(const QString& fileName)
{
    TRACE_CONTEXT(MediaObject::fileMediaType, EAudioInternal);

    MediaType result = MediaTypeUnknown;

    if (openRecognizer()) {

        const QHBufC fileNameSymbian(QDir::toNativeSeparators(fileName));

        m_file.Close();
        TInt err = m_file.Open(m_fileServer, *fileNameSymbian, EFileRead | EFileShareReadersOnly);

        if (KErrNone == err) {
            TDataRecognitionResult recognizerResult;
            err = m_recognizer.RecognizeData(m_file, recognizerResult);
            if (KErrNone == err) {
                const TPtrC mimeType = recognizerResult.iDataType.Des();
                result = Utils::mimeTypeToMediaType(mimeType);
            } else {
                TRACE("RApaLsSession::RecognizeData filename %S error %d", fileNameSymbian.data(), err);
            }
        } else {
            TRACE("RFile::Open filename %S error %d", fileNameSymbian.data(), err);
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::MediaObject::play()
{
    m_player->play();
}

void MMF::MediaObject::pause()
{
    m_player->pause();
}

void MMF::MediaObject::stop()
{
    m_player->stop();
}

void MMF::MediaObject::seek(qint64 ms)
{
    m_player->seek(ms);

    if (state() == PausedState or state() == PlayingState) {
        emit tick(currentTime());
    }
}

qint32 MMF::MediaObject::tickInterval() const
{
    return m_player->tickInterval();
}

void MMF::MediaObject::setTickInterval(qint32 interval)
{
    m_player->setTickInterval(interval);
}

bool MMF::MediaObject::hasVideo() const
{
    return m_player->hasVideo();
}

bool MMF::MediaObject::isSeekable() const
{
    return m_player->isSeekable();
}

Phonon::State MMF::MediaObject::state() const
{
    return m_player->state();
}

qint64 MMF::MediaObject::currentTime() const
{
    return m_player->currentTime();
}

QString MMF::MediaObject::errorString() const
{
    return m_player->errorString();
}

Phonon::ErrorType MMF::MediaObject::errorType() const
{
    return m_player->errorType();
}

qint64 MMF::MediaObject::totalTime() const
{
    return m_player->totalTime();
}

MediaSource MMF::MediaObject::source() const
{
    return m_player->source();
}

void MMF::MediaObject::setSource(const MediaSource &source)
{
    createPlayer(source);

    // This is a hack to work around KErrInUse from MMF client utility
    // OpenFileL calls
    m_player->setFileSource(source, m_file);

    emit currentSourceChanged(source);
}

void MMF::MediaObject::createPlayer(const MediaSource &source)
{
    TRACE_CONTEXT(MediaObject::createPlayer, EAudioApi);
    TRACE_ENTRY("state %d source.type %d", state(), source.type());
    TRACE_ENTRY("source.type %d", source.type());

    MediaType mediaType = MediaTypeUnknown;

    AbstractPlayer* oldPlayer = m_player.data();

    const bool oldPlayerHasVideo = oldPlayer->hasVideo();
    const bool oldPlayerSeekable = oldPlayer->isSeekable();

    Phonon::ErrorType error = NoError;
    QString errorMessage;

    // Determine media type
    switch (source.type()) {
    case MediaSource::LocalFile:
        mediaType = fileMediaType(source.fileName());
        break;

    case MediaSource::Url:
        {
            const QUrl url(source.url());
            if (url.scheme() == QLatin1String("file")) {
                mediaType = fileMediaType(url.toLocalFile());
            }
            else {
                errorMessage = QLatin1String("Network streaming not supported yet");
                error = NormalError;
            }
        }
        break;

    case MediaSource::Invalid:
    case MediaSource::Disc:
    case MediaSource::Stream:
        TRACE_0("Unsupported media type");
        error = NormalError;
        break;

    case MediaSource::Empty:
        TRACE_0("Empty media source");
        break;
    }

    AbstractPlayer* newPlayer = 0;

    // Construct newPlayer using oldPlayer (if not 0) in order to copy
    // parameters (volume, prefinishMark, transitionTime) which may have
    // been set on oldPlayer.

    switch (mediaType) {
    case MediaTypeUnknown:
        TRACE_0("Media type could not be determined");
        if (oldPlayer) {
            newPlayer = new DummyPlayer(*oldPlayer);
        } else {
            newPlayer = new DummyPlayer();
        }

        error = NormalError;
        errorMessage = tr("Media type could not be determined");
        break;

    case MediaTypeAudio:
        if (oldPlayer) {
            newPlayer = new AudioPlayer(*oldPlayer);
        } else {
            newPlayer = new AudioPlayer();
        }
        break;

    case MediaTypeVideo:
        if (oldPlayer) {
            newPlayer = new VideoPlayer(*oldPlayer);
        } else {
            newPlayer = new VideoPlayer();
        }
        break;
    }

    m_player.reset(newPlayer);

    if (oldPlayerHasVideo != hasVideo()) {
        emit hasVideoChanged(hasVideo());
    }

    if (oldPlayerSeekable != isSeekable()) {
        emit seekableChanged(isSeekable());
    }

    connect(m_player.data(), SIGNAL(totalTimeChanged(qint64)), SIGNAL(totalTimeChanged(qint64)));
    connect(m_player.data(), SIGNAL(stateChanged(Phonon::State, Phonon::State)), SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    connect(m_player.data(), SIGNAL(finished()), SIGNAL(finished()));
    connect(m_player.data(), SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
    connect(m_player.data(), SIGNAL(metaDataChanged(const QMultiMap<QString, QString>&)), SIGNAL(metaDataChanged(const QMultiMap<QString, QString>&)));

    // We need to call setError() after doing the connects, otherwise the
    // error won't be received.
    if (error != NoError) {
        Q_ASSERT(m_player);
        m_player->setError(error, errorMessage);
    }

    TRACE_EXIT_0();
}

void MMF::MediaObject::setNextSource(const MediaSource &source)
{
    m_player->setNextSource(source);
}

qint32 MMF::MediaObject::prefinishMark() const
{
    return m_player->prefinishMark();
}

void MMF::MediaObject::setPrefinishMark(qint32 mark)
{
    m_player->setPrefinishMark(mark);
}

qint32 MMF::MediaObject::transitionTime() const
{
    return m_player->transitionTime();
}

void MMF::MediaObject::setTransitionTime(qint32 time)
{
    m_player->setTransitionTime(time);
}


//-----------------------------------------------------------------------------
// VolumeObserver
//-----------------------------------------------------------------------------

void MMF::MediaObject::volumeChanged(qreal volume)
{
    m_player->volumeChanged(volume);
}

//-----------------------------------------------------------------------------
// Video output
//-----------------------------------------------------------------------------

void MMF::MediaObject::setVideoOutput(VideoOutput* videoOutput)
{
    m_player->setVideoOutput(videoOutput);
}


AbstractPlayer *MMF::MediaObject::abstractPlayer() const
{
    return m_player.data();
}

bool MMF::MediaObject::activateOnMediaObject(MediaObject *)
{
    // Guess what, we do nothing.
    return true;
}

QT_END_NAMESPACE

