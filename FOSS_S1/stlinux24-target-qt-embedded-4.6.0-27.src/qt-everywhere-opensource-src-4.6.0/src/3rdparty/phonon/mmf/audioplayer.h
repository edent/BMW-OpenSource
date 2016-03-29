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

#ifndef PHONON_MMF_AUDIOPLAYER_H
#define PHONON_MMF_AUDIOPLAYER_H

#include "abstractmediaplayer.h"

class CDrmPlayerUtility;
class TTimeIntervalMicroSeconds;

#ifdef QT_PHONON_MMF_AUDIO_DRM
#include <drmaudiosampleplayer.h>
typedef CDrmPlayerUtility CPlayerType;
typedef MDrmAudioPlayerCallback MPlayerObserverType;
#else
#include <mdaaudiosampleplayer.h>
typedef CMdaAudioPlayerUtility CPlayerType;
typedef MMdaAudioPlayerCallback MPlayerObserverType;
#endif

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
/**
 * @short Wrapper over MMF audio client utility
 */
class AudioPlayer   :   public AbstractMediaPlayer
                    ,   public MPlayerObserverType    // typedef
#ifdef QT_PHONON_MMF_AUDIO_DRM
                    ,   public MAudioLoadingObserver
#endif
{
    Q_OBJECT

public:
    AudioPlayer();
    explicit AudioPlayer(const AbstractPlayer& player);
    virtual ~AudioPlayer();

    // AbstractMediaPlayer
    virtual void doPlay();
    virtual void doPause();
    virtual void doStop();
    virtual void doSeek(qint64 milliseconds);
    virtual int setDeviceVolume(int mmfVolume);
    virtual int openFile(RFile& file);
    virtual void close();

    // MediaObjectInterface
    virtual bool hasVideo() const;
    virtual qint64 currentTime() const;
    virtual qint64 totalTime() const;

#ifdef QT_PHONON_MMF_AUDIO_DRM
    // MDrmAudioPlayerCallback
    virtual void MdapcInitComplete(TInt aError,
                                   const TTimeIntervalMicroSeconds &aDuration);
    virtual void MdapcPlayComplete(TInt aError);

    // MAudioLoadingObserver
    virtual void MaloLoadingStarted();
    virtual void MaloLoadingComplete();
#else
    // MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError,
                                  const TTimeIntervalMicroSeconds &aDuration);
    virtual void MapcPlayComplete(TInt aError);
#endif

    /**
     * This class owns the pointer.
     */
    CPlayerType *player() const;

private:
    void construct();

    // AbstractMediaPlayer
    virtual int numberOfMetaDataEntries() const;
    virtual QPair<QString, QString> metaDataEntry(int index) const;

private:
    /**
     * Using CPlayerType typedef in order to be able to easily switch between
     * CMdaAudioPlayerUtility and CDrmPlayerUtility
     */
    QScopedPointer<CPlayerType> m_player;
};
}
}

QT_END_NAMESPACE

#endif
