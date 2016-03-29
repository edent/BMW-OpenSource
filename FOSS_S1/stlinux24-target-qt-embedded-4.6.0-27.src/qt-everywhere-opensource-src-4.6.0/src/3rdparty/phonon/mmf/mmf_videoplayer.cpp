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

#include <QApplication>    // for QApplication::activeWindow
#include <QUrl>
#include <QTimer>
#include <QWidget>

#include <coemain.h>    // for CCoeEnv
#include <coecntrl.h>

#include "mmf_videoplayer.h"
#include "utils.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VideoPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoPlayer::VideoPlayer()
        :   m_wsSession(CCoeEnv::Static()->WsSession())
        ,   m_screenDevice(*CCoeEnv::Static()->ScreenDevice())
        ,   m_window(0)
        ,   m_totalTime(0)
        ,   m_mmfOutputChangePending(false)
{
    construct();
}

MMF::VideoPlayer::VideoPlayer(const AbstractPlayer& player)
        :   AbstractMediaPlayer(player)
        ,   m_wsSession(CCoeEnv::Static()->WsSession())
        ,   m_screenDevice(*CCoeEnv::Static()->ScreenDevice())
        ,   m_window(0)
        ,   m_totalTime(0)
        ,   m_mmfOutputChangePending(false)
{
    construct();
}

void MMF::VideoPlayer::construct()
{
    TRACE_CONTEXT(VideoPlayer::VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    if (m_videoOutput)
        m_videoOutput->setObserver(this);

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    // Ignore return value - first call must always return true
    getNativeWindowSystemHandles();

    // TODO: is this the correct way to handle errors which occur when
    // creating a Symbian object in the constructor of a Qt object?

    // TODO: check whether videoOutput is visible?  If not, then the
    // corresponding window will not be active, meaning that the
    // clipping region will be set to empty and the video will not be
    // visible.  If this is the case, we should set m_mmfOutputChangePending
    // and respond to future showEvents from the videoOutput widget.

    TRAPD(err,
        m_player.reset(CVideoPlayerUtility::NewL
            (
                 *this,
                 priority, preference,
                 m_wsSession, m_screenDevice,
                 *m_window,
                 m_rect, m_rect
            ))
        );

    if (KErrNone != err)
        changeState(ErrorState);

    TRACE_EXIT_0();
}

MMF::VideoPlayer::~VideoPlayer()
{
    TRACE_CONTEXT(VideoPlayer::~VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    if (m_videoOutput)
        m_videoOutput->setObserver(0);

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::doPlay()
{
    TRACE_CONTEXT(VideoPlayer::doPlay, EVideoApi);

    // See comment in updateMmfOutput
    if (m_mmfOutputChangePending) {
        TRACE_0("MMF output change pending - pushing now");
        updateMmfOutput();
    }

    m_player->Play();
}

void MMF::VideoPlayer::doPause()
{
    TRACE_CONTEXT(VideoPlayer::doPause, EVideoApi);

    TRAPD(err, m_player->PauseL());
    if (KErrNone != err) {
        TRACE("PauseL error %d", err);
        setError(NormalError);
    }
}

void MMF::VideoPlayer::doStop()
{
    m_player->Stop();
}

void MMF::VideoPlayer::doSeek(qint64 ms)
{
    TRACE_CONTEXT(VideoPlayer::doSeek, EVideoApi);

    bool wasPlaying = false;
    if (state() == PlayingState) {
        // The call to SetPositionL does not have any effect if playback is
        // ongoing, so we pause before seeking.
        doPause();
        wasPlaying = true;
    }

    TRAPD(err, m_player->SetPositionL(TTimeIntervalMicroSeconds(ms * 1000)));

    if (KErrNone == err) {
        if (wasPlaying)
            doPlay();
    }
    else {
        TRACE("SetPositionL error %d", err);
        setError(NormalError);
    }
}

int MMF::VideoPlayer::setDeviceVolume(int mmfVolume)
{
    TRAPD(err, m_player->SetVolumeL(mmfVolume));
    return err;
}

int MMF::VideoPlayer::openFile(RFile& file)
{
    TRAPD(err, m_player->OpenFileL(file));
    return err;
}

void MMF::VideoPlayer::close()
{
    m_player->Close();
}

bool MMF::VideoPlayer::hasVideo() const
{
    return true;
}

qint64 MMF::VideoPlayer::currentTime() const
{
    TRACE_CONTEXT(VideoPlayer::currentTime, EVideoApi);

    TTimeIntervalMicroSeconds us;
    TRAPD(err, us = m_player->PositionL())

    qint64 result = 0;

    if (KErrNone == err) {
        result = toMilliSeconds(us);
    } else {
        TRACE("PositionL error %d", err);

        // If we don't cast away constness here, we simply have to ignore
        // the error.
        const_cast<VideoPlayer*>(this)->setError(NormalError);
    }

    return result;
}

qint64 MMF::VideoPlayer::totalTime() const
{
    return m_totalTime;
}


//-----------------------------------------------------------------------------
// MVideoPlayerUtilityObserver callbacks
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::MvpuoOpenComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoOpenComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

    if (KErrNone == aError)
        m_player->Prepare();
    else
        setError(NormalError);

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPrepareComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoPrepareComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

    TRAPD(err, doPrepareCompleteL(aError));

    if (KErrNone == err) {
        maxVolumeChanged(m_player->MaxVolume());

        if (m_videoOutput)
            m_videoOutput->setFrameSize(m_frameSize);

        // See comment in updateMmfOutput
        if (m_mmfOutputChangePending) {
            TRACE_0("MMF output change pending - pushing now");
            updateMmfOutput();
        }

        emit totalTimeChanged(totalTime());
        changeState(StoppedState);
    } else {
        setError(NormalError);
    }

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::doPrepareCompleteL(TInt aError)
{
    User::LeaveIfError(aError);

    // Get frame size
    TSize size;
    m_player->VideoFrameSizeL(size);
    m_frameSize = QSize(size.iWidth, size.iHeight);

    // Get duration
    m_totalTime = toMilliSeconds(m_player->DurationL());
}


void MMF::VideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoFrameReady, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    // TODO
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);   // suppress warnings in release builds

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPlayComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoPlayComplete, EVideoApi)
    TRACE_ENTRY("state %d error %d", state(), aError);

    Q_UNUSED(aError);   // suppress warnings in release builds
    changeState(StoppedState);

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoEvent(const TMMFEvent &aEvent)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoEvent, EVideoApi);
    TRACE_ENTRY("state %d", state());

    // TODO
    Q_UNUSED(aEvent);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VideoOutputObserver
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::videoOutputRegionChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputRegionChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    const bool changed = getNativeWindowSystemHandles();

    // See comment in updateMmfOutput
    if (changed) {
        if (state() == LoadingState)
            m_mmfOutputChangePending = true;
        else
            updateMmfOutput();
    }

    TRACE_EXIT_0();
}


#ifndef QT_NO_DEBUG

// The following code is for debugging problems related to video visibility.  It allows
// the VideoPlayer instance to query the window server in order to determine the
// DSA drawing region for the video window.

class CDummyAO : public CActive
{
public:
    CDummyAO() : CActive(CActive::EPriorityStandard) { CActiveScheduler::Add(this); }
    void RunL() { }
    void DoCancel() { }
    TRequestStatus& Status() { return iStatus; }
    void SetActive() { CActive::SetActive(); }
};

void getDsaRegion(RWsSession &session, const RWindowBase &window)
{
    RDirectScreenAccess dsa(session);
    TInt err = dsa.Construct();
    CDummyAO ao;
    RRegion* region;
    err = dsa.Request(region, ao.Status(), window);
    ao.SetActive();
    dsa.Close();
    ao.Cancel();
    if (region) {
        qDebug() << "Phonon::MMF::getDsaRegion count" << region->Count();
        for (int i=0; i<region->Count(); ++i) {
            const TRect& rect = region->RectangleList()[i];
            qDebug() << "Phonon::MMF::getDsaRegion rect"
                << rect.iTl.iX << rect.iTl.iY << rect.iBr.iX << rect.iBr.iY;
        }
        region->Close();
    }
}

#endif // _DEBUG

void MMF::VideoPlayer::updateMmfOutput()
{
    TRACE_CONTEXT(VideoPlayer::updateMmfOutput, EVideoInternal);
    TRACE_ENTRY_0();

    // Calling SetDisplayWindowL is a no-op unless the MMF controller has
    // been loaded, so we shouldn't do it.  Instead, the
    // m_mmfOutputChangePending flag is used to record the fact that we
    // need to call SetDisplayWindowL, and this is checked in
    // MvpuoPrepareComplete, at which point the MMF controller has been
    // loaded.

#ifndef QT_NO_DEBUG
    getDsaRegion(m_wsSession, *m_window);
#endif

    TRAPD(err,
        m_player->SetDisplayWindowL
        (
            m_wsSession, m_screenDevice,
            *m_window,
            m_rect, m_rect
        )
    );

    if (KErrNone != err) {
        TRACE("SetDisplayWindowL error %d", err);
        setError(NormalError);
    }

    m_mmfOutputChangePending = false;

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::videoOutputChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputChanged, EVideoInternal);
    TRACE_ENTRY_0();

    if (m_videoOutput) {
        m_videoOutput->setObserver(this);
        m_videoOutput->setFrameSize(m_frameSize);
    }

    videoOutputRegionChanged();

    TRACE_EXIT_0();
}

bool MMF::VideoPlayer::getNativeWindowSystemHandles()
{
    TRACE_CONTEXT(VideoPlayer::getNativeWindowSystemHandles, EVideoInternal);
    TRACE_ENTRY_0();

    CCoeControl *control = 0;

    if (m_videoOutput)
        // Create native window
        control = m_videoOutput->winId();
    else
        // Get top-level window
        control = QApplication::activeWindow()->effectiveWinId();

#ifndef QT_NO_DEBUG
    if (m_videoOutput) {
        QScopedPointer<ObjectDump::QDumper> dumper(new ObjectDump::QDumper);
        dumper->setPrefix("Phonon::MMF"); // to aid searchability of logs
        ObjectDump::addDefaultAnnotators(*dumper);
        TRACE_0("Dumping VideoOutput:");
        dumper->dumpObject(*m_videoOutput);
    }
    else {
        TRACE_0("m_videoOutput is null - dumping top-level control info:");
        TRACE("control %08x", control);
        TRACE("control.parent %08x", control->Parent());
        TRACE("control.isVisible %d", control->IsVisible());
        TRACE("control.rect %d,%d %dx%d",
            control->Position().iX, control->Position().iY,
            control->Size().iWidth, control->Size().iHeight);
        TRACE("control.ownsWindow %d", control->OwnsWindow());
    }
#endif

    RWindowBase *const window = control->DrawableWindow();
    const TRect rect(window->AbsPosition(), window->Size());

    TRACE("rect                  %d %d - %d %d",
        rect.iTl.iX, rect.iTl.iY,
        rect.iBr.iX, rect.iBr.iY);

    bool changed = false;

    if (window != m_window || rect != m_rect) {
        m_window = window;
        m_rect = rect;
        changed = true;
    }

    TRACE_RETURN("changed %d", changed);
}

int MMF::VideoPlayer::numberOfMetaDataEntries() const
{
    int numberOfEntries = 0;
    TRAP_IGNORE(numberOfEntries = m_player->NumberOfMetaDataEntriesL());
    return numberOfEntries;
}

QPair<QString, QString> MMF::VideoPlayer::metaDataEntry(int index) const
{
    CMMFMetaDataEntry *entry = 0;
    QT_TRAP_THROWING(entry = m_player->MetaDataEntryL(index));
    return QPair<QString, QString>(qt_TDesC2QString(entry->Name()), qt_TDesC2QString(entry->Value()));
}

QT_END_NAMESPACE

