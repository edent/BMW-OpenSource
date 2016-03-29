/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qaudiooutput_win32_p.h"

//#define DEBUG_AUDIO 1

QT_BEGIN_NAMESPACE

static CRITICAL_SECTION waveOutCriticalSection;

static const int minimumIntervalTime = 50;

QAudioOutputPrivate::QAudioOutputPrivate(const QByteArray &device, const QAudioFormat& audioFormat):
    settings(audioFormat)
{
    bytesAvailable = 0;
    buffer_size = 0;
    period_size = 0;
    m_device = device;
    totalTimeValue = 0;
    intervalTime = 1000;
    audioBuffer = 0;
    errorState = QAudio::NoError;
    deviceState = QAudio::StoppedState;
    audioSource = 0;
    pullMode = true;
    finished = false;
    InitializeCriticalSection(&waveOutCriticalSection);
}

QAudioOutputPrivate::~QAudioOutputPrivate()
{
    EnterCriticalSection(&waveOutCriticalSection);
    finished = true;
    LeaveCriticalSection(&waveOutCriticalSection);

    close();
    DeleteCriticalSection(&waveOutCriticalSection);
}

void CALLBACK QAudioOutputPrivate::waveOutProc( HWAVEOUT hWaveOut, UINT uMsg,
        DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
    Q_UNUSED(dwParam1)
    Q_UNUSED(dwParam2)
    Q_UNUSED(hWaveOut)

    QAudioOutputPrivate* qAudio;
    qAudio = (QAudioOutputPrivate*)(dwInstance);
    if(!qAudio)
        return;

    switch(uMsg) {
        case WOM_OPEN:
            qAudio->feedback();
            break;
        case WOM_CLOSE:
            return;
        case WOM_DONE:
            EnterCriticalSection(&waveOutCriticalSection);
            if(qAudio->finished || qAudio->buffer_size == 0 || qAudio->period_size == 0) {
                LeaveCriticalSection(&waveOutCriticalSection);
                return;
	    }
            qAudio->waveFreeBlockCount++;
            if(qAudio->waveFreeBlockCount >= qAudio->buffer_size/qAudio->period_size)
                qAudio->waveFreeBlockCount = qAudio->buffer_size/qAudio->period_size;
            qAudio->feedback();
            LeaveCriticalSection(&waveOutCriticalSection);
            break;
        default:
            return;
    }
}

WAVEHDR* QAudioOutputPrivate::allocateBlocks(int size, int count)
{
    int i;
    unsigned char* buffer;
    WAVEHDR* blocks;
    DWORD totalBufferSize = (size + sizeof(WAVEHDR))*count;

    if((buffer=(unsigned char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,
                    totalBufferSize)) == 0) {
        qWarning("QAudioOutput: Memory allocation error");
        return 0;
    }
    blocks = (WAVEHDR*)buffer;
    buffer += sizeof(WAVEHDR)*count;
    for(i = 0; i < count; i++) {
        blocks[i].dwBufferLength = size;
        blocks[i].lpData = (LPSTR)buffer;
        buffer += size;
    }
    return blocks;
}

void QAudioOutputPrivate::freeBlocks(WAVEHDR* blockArray)
{
    HeapFree(GetProcessHeap(), 0, blockArray);
}

QAudioFormat QAudioOutputPrivate::format() const
{
    return settings;
}

QIODevice* QAudioOutputPrivate::start(QIODevice* device)
{
    if(deviceState != QAudio::StoppedState)
        close();

    if(!pullMode && audioSource) {
        delete audioSource;
    }

    if(device) {
        //set to pull mode
        pullMode = true;
        audioSource = device;
        deviceState = QAudio::ActiveState;
    } else {
        //set to push mode
        pullMode = false;
        audioSource = new OutputPrivate(this);
        audioSource->open(QIODevice::WriteOnly|QIODevice::Unbuffered);
        deviceState = QAudio::IdleState;
    }

    if( !open() )
        return 0;

    emit stateChanged(deviceState);

    return audioSource;
}

void QAudioOutputPrivate::stop()
{
    if(deviceState == QAudio::StoppedState)
        return;
    close();
    if(!pullMode && audioSource) {
        delete audioSource;
        audioSource = 0;
    }
    emit stateChanged(deviceState);
}

bool QAudioOutputPrivate::open()
{
#ifdef DEBUG_AUDIO
    QTime now(QTime::currentTime());
    qDebug()<<now.second()<<"s "<<now.msec()<<"ms :open()";
#endif
    if(buffer_size == 0) {
        // Default buffer size, 200ms, default period size is 40ms
        buffer_size = settings.frequency()*settings.channels()*(settings.sampleSize()/8)*0.2;
	period_size = buffer_size/5;
    } else {
        period_size = buffer_size/5;
    }
    waveBlocks = allocateBlocks(period_size, buffer_size/period_size);

    EnterCriticalSection(&waveOutCriticalSection);
    waveFreeBlockCount = buffer_size/period_size;
    LeaveCriticalSection(&waveOutCriticalSection);

    waveCurrentBlock = 0;

    if(audioBuffer == 0)
        audioBuffer = new char[buffer_size];

    timeStamp.restart();
    elapsedTimeOffset = 0;

    wfx.nSamplesPerSec = settings.frequency();
    wfx.wBitsPerSample = settings.sampleSize();
    wfx.nChannels = settings.channels();
    wfx.cbSize = 0;

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    UINT_PTR devId = WAVE_MAPPER;

    WAVEOUTCAPS woc;
    unsigned long iNumDevs,ii;
    iNumDevs = waveOutGetNumDevs();
    for(ii=0;ii<iNumDevs;ii++) {
        if(waveOutGetDevCaps(ii, &woc, sizeof(WAVEOUTCAPS))
	    == MMSYSERR_NOERROR) {
	    QString tmp;
	    tmp = QString::fromUtf16((const unsigned short*)woc.szPname);
            if(tmp.compare(QLatin1String(m_device)) == 0) {
	        devId = ii;
		break;
	    }
	}
    }

    if(waveOutOpen(&hWaveOut, devId, &wfx,
                (DWORD_PTR)&waveOutProc,
                (DWORD_PTR) this,
                CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        errorState = QAudio::OpenError;
        deviceState = QAudio::StoppedState;
        emit stateChanged(deviceState);
        qWarning("QAudioOutput: open error");
        return false;
    }

    totalTimeValue = 0;
    timeStampOpened.restart();
    elapsedTimeOffset = 0;

    errorState = QAudio::NoError;
    if(pullMode) {
        deviceState = QAudio::ActiveState;
        QTimer::singleShot(10, this, SLOT(feedback()));
    } else
        deviceState = QAudio::IdleState;

    return true;
}

void QAudioOutputPrivate::close()
{
    if(deviceState == QAudio::StoppedState)
        return;

    deviceState = QAudio::StoppedState;
    int delay = (buffer_size-bytesFree())*1000/(settings.frequency()
                  *settings.channels()*(settings.sampleSize()/8));
    waveOutReset(hWaveOut);
    Sleep(delay+10);

    freeBlocks(waveBlocks);
    waveOutClose(hWaveOut);
    delete [] audioBuffer;
    audioBuffer = 0;
    buffer_size = 0;
}

int QAudioOutputPrivate::bytesFree() const
{
    int buf;
    buf = waveFreeBlockCount*period_size;

    return buf;
}

int QAudioOutputPrivate::periodSize() const
{
    return period_size;
}

void QAudioOutputPrivate::setBufferSize(int value)
{
    if(deviceState == QAudio::StoppedState)
        buffer_size = value;
}

int QAudioOutputPrivate::bufferSize() const
{
    return buffer_size;
}

void QAudioOutputPrivate::setNotifyInterval(int ms)
{
    if(ms >= minimumIntervalTime)
        intervalTime = ms;
    else
        intervalTime = minimumIntervalTime;
}

int QAudioOutputPrivate::notifyInterval() const
{
    return intervalTime;
}

qint64 QAudioOutputPrivate::processedUSecs() const
{
    return totalTimeValue;
}

qint64 QAudioOutputPrivate::write( const char *data, qint64 len )
{
    // Write out some audio data

    char* p = (char*)data;
    int l = (int)len;

    WAVEHDR* current;
    int remain;
    current = &waveBlocks[waveCurrentBlock];
    while(l > 0) {
        EnterCriticalSection(&waveOutCriticalSection);
        if(waveFreeBlockCount==0) {
            LeaveCriticalSection(&waveOutCriticalSection);
            break;
        }
        LeaveCriticalSection(&waveOutCriticalSection);

        if(current->dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

        if(l < period_size)
            remain = l;
        else
            remain = period_size;
        memcpy(current->lpData, p, remain);

        l -= remain;
        p += remain;
        current->dwBufferLength = remain;
        waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));

        EnterCriticalSection(&waveOutCriticalSection);
        waveFreeBlockCount--;
        LeaveCriticalSection(&waveOutCriticalSection);
#ifdef DEBUG_AUDIO
        EnterCriticalSection(&waveOutCriticalSection);
        qDebug("write out l=%d, waveFreeBlockCount=%d",
                current->dwBufferLength,waveFreeBlockCount);
        LeaveCriticalSection(&waveOutCriticalSection);
#endif
        totalTimeValue += current->dwBufferLength
            /(settings.channels()*(settings.sampleSize()/8))
            *1000000/settings.frequency();;
        waveCurrentBlock++;
        waveCurrentBlock %= buffer_size/period_size;
        current = &waveBlocks[waveCurrentBlock];
        current->dwUser = 0;
    }
    return (len-l);
}

void QAudioOutputPrivate::resume()
{
    if(deviceState == QAudio::SuspendedState) {
        deviceState = QAudio::ActiveState;
        errorState = QAudio::NoError;
        waveOutRestart(hWaveOut);
        QTimer::singleShot(10, this, SLOT(feedback()));
        emit stateChanged(deviceState);
    }
}

void QAudioOutputPrivate::suspend()
{
    if(deviceState == QAudio::ActiveState) {
        waveOutPause(hWaveOut);
        deviceState = QAudio::SuspendedState;
        errorState = QAudio::NoError;
        emit stateChanged(deviceState);
    }
}

void QAudioOutputPrivate::feedback()
{
#ifdef DEBUG_AUDIO
    QTime now(QTime::currentTime());
    qDebug()<<now.second()<<"s "<<now.msec()<<"ms :feedback()";
#endif
    bytesAvailable = bytesFree();

    if(!(deviceState==QAudio::StoppedState||deviceState==QAudio::SuspendedState)) {
        if(bytesAvailable >= period_size)
            QMetaObject::invokeMethod(this, "deviceReady", Qt::QueuedConnection);
    }
}

bool QAudioOutputPrivate::deviceReady()
{
    if(pullMode) {
        int chunks = bytesAvailable/period_size;
#ifdef DEBUG_AUDIO
        qDebug()<<"deviceReady() avail="<<bytesAvailable<<" bytes, period size="<<period_size<<" bytes";
        qDebug()<<"deviceReady() no. of chunks that can fit ="<<chunks<<", chunks in bytes ="<<chunks*period_size;
#endif
        bool startup = false;
        if(totalTimeValue == 0)
	    startup = true;

	bool full=false;
	EnterCriticalSection(&waveOutCriticalSection);
	if(waveFreeBlockCount==0) full = true;
	LeaveCriticalSection(&waveOutCriticalSection);
	if (full){
#ifdef DEBUG_AUDIO
            qDebug() << "Skipping data as unable to write";
#endif
	    if((timeStamp.elapsed() + elapsedTimeOffset) > intervalTime ) {
                emit notify();
		elapsedTimeOffset = timeStamp.elapsed() + elapsedTimeOffset - intervalTime;
		timeStamp.restart();
	    }
	    return true;
	}

        if(startup)
	    waveOutPause(hWaveOut);
        int input = period_size*chunks;
        int l = audioSource->read(audioBuffer,input);
        if(l > 0) {
            int out= write(audioBuffer,l);
            if(out > 0)
                deviceState = QAudio::ActiveState;
	    if(startup)
	        waveOutRestart(hWaveOut);
        } else if(l == 0) {
            bytesAvailable = bytesFree();

            int check = 0;
            EnterCriticalSection(&waveOutCriticalSection);
            check = waveFreeBlockCount;
            LeaveCriticalSection(&waveOutCriticalSection);
            if(check == buffer_size/period_size) {
                errorState = QAudio::UnderrunError;
                deviceState = QAudio::IdleState;
                emit stateChanged(deviceState);
            }

        } else if(l < 0) {
            bytesAvailable = bytesFree();
            errorState = QAudio::IOError;
        }
    }
    if(deviceState != QAudio::ActiveState)
        return true;

    if((timeStamp.elapsed() + elapsedTimeOffset) > intervalTime) {
        emit notify();
	elapsedTimeOffset = timeStamp.elapsed() + elapsedTimeOffset - intervalTime;
        timeStamp.restart();
    }

    return true;
}

qint64 QAudioOutputPrivate::elapsedUSecs() const
{
    if (deviceState == QAudio::StoppedState)
        return 0;

    return timeStampOpened.elapsed()*1000;
}

QAudio::Error QAudioOutputPrivate::error() const
{
    return errorState;
}

QAudio::State QAudioOutputPrivate::state() const
{
    return deviceState;
}

void QAudioOutputPrivate::reset()
{
    close();
}

OutputPrivate::OutputPrivate(QAudioOutputPrivate* audio)
{
    audioDevice = qobject_cast<QAudioOutputPrivate*>(audio);
}

OutputPrivate::~OutputPrivate() {}

qint64 OutputPrivate::readData( char* data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)

    return 0;
}

qint64 OutputPrivate::writeData(const char* data, qint64 len)
{
    int retry = 0;
    qint64 written = 0;

    if((audioDevice->deviceState == QAudio::ActiveState)
            ||(audioDevice->deviceState == QAudio::IdleState)) {
        qint64 l = len;
        while(written < l) {
            int chunk = audioDevice->write(data+written,(l-written));
            if(chunk <= 0)
                retry++;
            else
                written+=chunk;

            if(retry > 10)
                return written;
        }
        audioDevice->deviceState = QAudio::ActiveState;
    }
    return written;
}

QT_END_NAMESPACE
