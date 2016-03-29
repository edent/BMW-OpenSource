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

#ifndef PHONON_MMF_ABSTRACTPLAYER_H
#define PHONON_MMF_ABSTRACTPLAYER_H

#include <Phonon/phononnamespace.h>
#include <Phonon/MediaSource.h>

#include <QObject>

#include "volumeobserver.h"

#include "videooutput.h"

class RFile;

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
class VideoOutput;

/**
 * @short Interface which abstracts from MediaObject the current
 * media type
 *
 * This may be:
 *  -   Nothing, in which case this interface is implemented by
 *      DummyPlayer
 *  -   Audio, in which case the implementation is AudioPlayer
 *  -   Video, in which case the implementation is VideoPlayer
 */
class AbstractPlayer : public QObject
                     , public VolumeObserver
{
    // Required although this class has no signals or slots
    // Without this, qobject_cast will fail
    Q_OBJECT

public:
    AbstractPlayer();
    explicit AbstractPlayer(const AbstractPlayer& player);

    // MediaObjectInterface (implemented)
    qint32 tickInterval() const;
    void setTickInterval(qint32);
    void setTransitionTime(qint32);
    qint32 transitionTime() const;
    void setPrefinishMark(qint32);
    qint32 prefinishMark() const;

    // MediaObjectInterface (abstract)
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seek(qint64 milliseconds) = 0;
    virtual bool hasVideo() const = 0;
    virtual bool isSeekable() const = 0;
    virtual qint64 currentTime() const = 0;
    virtual Phonon::ErrorType errorType() const;
    virtual QString errorString() const;
    virtual qint64 totalTime() const = 0;
    virtual Phonon::MediaSource source() const = 0;
    // This is a temporary hack to work around KErrInUse from MMF
    // client utility OpenFileL calls
    //virtual void setSource(const Phonon::MediaSource &) = 0;
    virtual void setFileSource(const Phonon::MediaSource&, RFile&) = 0;
    virtual void setNextSource(const Phonon::MediaSource &) = 0;

    // VolumeObserver
    virtual void volumeChanged(qreal volume);

    void setVideoOutput(VideoOutput* videoOutput);

    /**
     * Records error and changes state to ErrorState
     */
    void setError(Phonon::ErrorType error,
                  const QString &errorMessage = QString());

    Phonon::State state() const;

Q_SIGNALS:
    void totalTimeChanged(qint64 length);
    void finished();
    void tick(qint64 time);
    void stateChanged(Phonon::State oldState,
                      Phonon::State newState);
    void metaDataChanged(const QMultiMap<QString, QString>& metaData);

protected:
    /**
     * Defined private state enumeration in order to add GroundState
     */
    enum PrivateState {
        LoadingState    = Phonon::LoadingState,
        StoppedState    = Phonon::StoppedState,
        PlayingState    = Phonon::PlayingState,
        BufferingState  = Phonon::BufferingState,
        PausedState     = Phonon::PausedState,
        ErrorState      = Phonon::ErrorState,
        GroundState
    };

    /**
     * Converts PrivateState into the corresponding Phonon::State
     */
    Phonon::State phononState() const;

    /**
     * Converts PrivateState into the corresponding Phonon::State
     */
    static Phonon::State phononState(PrivateState state);

    virtual void videoOutputChanged();

    PrivateState privateState() const;

    /**
     * Changes state and emits stateChanged()
     */
    virtual void changeState(PrivateState newState);

    /**
     * Modifies m_state directly. Typically you want to call changeState(),
     * which performs the business logic.
     */
    void setState(PrivateState newState);

private:
    virtual void doSetTickInterval(qint32 interval) = 0;

protected:
    // Not owned
    VideoOutput*                m_videoOutput;

    qreal                       m_volume;

private:
    PrivateState                m_state;
    Phonon::ErrorType           m_error;
    QString                     m_errorString;
    qint32                      m_tickInterval;
    qint32                      m_transitionTime;
    qint32                      m_prefinishMark;

};
}
}

QT_END_NAMESPACE

#endif

