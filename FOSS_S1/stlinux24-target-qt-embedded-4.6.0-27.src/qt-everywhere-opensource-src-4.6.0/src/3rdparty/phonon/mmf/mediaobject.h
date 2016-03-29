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

#ifndef PHONON_MMF_MEDIAOBJECT_H
#define PHONON_MMF_MEDIAOBJECT_H

#include <Phonon/MediaSource>
#include <Phonon/MediaObjectInterface>
#include <QScopedPointer>
#include <QTimer>

// For recognizer
#include <apgcli.h>

#include "abstractplayer.h"
#include "mmf_medianode.h"
#include "defs.h"
#include "volumeobserver.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
class AbstractPlayer;
class VideoOutput;

/**
 * @short Facade class which wraps MMF client utility instance
 */
class MediaObject : public MediaNode
                  , public MediaObjectInterface
                  , public VolumeObserver
{
    Q_OBJECT
    Q_INTERFACES(Phonon::MediaObjectInterface)

public:
    MediaObject(QObject *parent);
    virtual ~MediaObject();

    // MediaObjectInterface
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void seek(qint64 milliseconds);
    virtual qint32 tickInterval() const;
    virtual void setTickInterval(qint32 interval);
    virtual bool hasVideo() const;
    virtual bool isSeekable() const;
    virtual qint64 currentTime() const;
    virtual Phonon::State state() const;
    virtual QString errorString() const;
    virtual Phonon::ErrorType errorType() const;
    virtual qint64 totalTime() const;
    virtual MediaSource source() const;
    virtual void setSource(const MediaSource &);
    virtual void setNextSource(const MediaSource &source);
    virtual qint32 prefinishMark() const;
    virtual void setPrefinishMark(qint32);
    virtual qint32 transitionTime() const;
    virtual void setTransitionTime(qint32);

    // VolumeObserver
    void volumeChanged(qreal volume);

    /**
     * This class owns the AbstractPlayer, and will delete it upon
     * destruction.
     */
    AbstractPlayer *abstractPlayer() const;

    void setVideoOutput(VideoOutput* videoOutput);

    virtual bool activateOnMediaObject(MediaObject *);

Q_SIGNALS:
    void totalTimeChanged(qint64 length);
    void hasVideoChanged(bool hasVideo);
    void seekableChanged(bool seekable);
    // TODO: emit bufferStatus from MediaObject
    void bufferStatus(int);
    // TODO: emit aboutToFinish from MediaObject
    void aboutToFinish();
    // TODO: emit prefinishMarkReached from MediaObject
    void prefinishMarkReached(qint32);
    // TODO: emit metaDataChanged from MediaObject
    void metaDataChanged(const QMultiMap<QString, QString>& metaData);
    void currentSourceChanged(const MediaSource& source);
    void stateChanged(Phonon::State oldState,
                      Phonon::State newState);
    void finished();
    void tick(qint64 time);

private:
    void createPlayer(const MediaSource &source);
    bool openRecognizer();

    // Audio / video media type recognition
    MediaType fileMediaType(const QString& fileName);
    // TODO: urlMediaType function

    static qint64 toMilliSeconds(const TTimeIntervalMicroSeconds &);

private:

    // Audio / video media type recognition
    bool                                m_recognizerOpened;
    RApaLsSession                       m_recognizer;
    RFs                                 m_fileServer;

    // Storing the file handle here to work around KErrInUse error
    // from MMF player utility OpenFileL functions
    RFile                               m_file;

    QScopedPointer<AbstractPlayer>      m_player;

};
}
}

QT_END_NAMESPACE

#endif
