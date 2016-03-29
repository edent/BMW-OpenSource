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

#ifndef PHONON_MMF_ABSTRACTMEDIAPLAYER_H
#define PHONON_MMF_ABSTRACTMEDIAPLAYER_H

#include <QTimer>
#include <QScopedPointer>
#include <e32std.h>
#include "abstractplayer.h"

class RFile;

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
class AudioOutput;

/**
 * Interface via which MMF client APIs for both audio and video can be
 * accessed.
 */
class AbstractMediaPlayer : public AbstractPlayer
{
    Q_OBJECT

protected:
    AbstractMediaPlayer();
    explicit AbstractMediaPlayer(const AbstractPlayer& player);

public:
    // MediaObjectInterface
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void seek(qint64 milliseconds);
    virtual bool isSeekable() const;
    virtual MediaSource source() const;
    virtual void setFileSource(const Phonon::MediaSource&, RFile&);
    virtual void setNextSource(const MediaSource &source);

    // VolumeObserver
    virtual void volumeChanged(qreal volume);

protected:
    // AbstractPlayer
    virtual void doSetTickInterval(qint32 interval);

    virtual void doPlay() = 0;
    virtual void doPause() = 0;
    virtual void doStop() = 0;
    virtual void doSeek(qint64 pos) = 0;
    virtual int setDeviceVolume(int mmfVolume) = 0;
    virtual int openFile(RFile& file) = 0;
    virtual void close() = 0;
    virtual void changeState(PrivateState newState);

    void updateMetaData();
    virtual int numberOfMetaDataEntries() const = 0;
    virtual QPair<QString, QString> metaDataEntry(int index) const = 0;

protected:
    bool tickTimerRunning() const;
    void startTickTimer();
    void stopTickTimer();
    void maxVolumeChanged(int maxVolume);

    static qint64 toMilliSeconds(const TTimeIntervalMicroSeconds &);

private:
    void doVolumeChanged();

private Q_SLOTS:
    /**
     * Receives signal from m_tickTimer
     */
    void tick();

private:
    /**
     * This flag is set to true if play is called when the object is
     * in a Loading state.  Once loading is complete, playback will
     * be started.
     */
    bool                        m_playPending;

    QScopedPointer<QTimer>      m_tickTimer;

    int                         m_mmfMaxVolume;

    MediaSource                 m_source;
    MediaSource                 m_nextSource;

    QMultiMap<QString, QString> m_metaData;

};
}
}

QT_END_NAMESPACE

#endif

