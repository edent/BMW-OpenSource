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

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

#include <QtCore/qendian.h>
#include <QtCore/qbuffer.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>

#include <QtMultimedia/qaudiodeviceinfo.h>
#include <QtMultimedia/qaudiooutput.h>

#include "qaudio_mac_p.h"
#include "qaudiooutput_mac_p.h"


QT_BEGIN_NAMESPACE


namespace
{

static const int default_buffer_size = 8 * 1024;


class QAudioOutputBuffer : public QObject
{
    Q_OBJECT

public:
    QAudioOutputBuffer(int bufferSize, int maxPeriodSize, QAudioFormat const& audioFormat):
        m_deviceError(false),
        m_maxPeriodSize(maxPeriodSize),
        m_device(0)
    {
        m_buffer = new QAudioRingBuffer(bufferSize + (bufferSize % maxPeriodSize == 0 ? 0 : maxPeriodSize - (bufferSize % maxPeriodSize)));
        m_bytesPerFrame = (audioFormat.sampleSize() / 8) * audioFormat.channels();
        m_periodTime = m_maxPeriodSize / m_bytesPerFrame * 1000 / audioFormat.frequency();

        m_fillTimer = new QTimer(this);
        connect(m_fillTimer, SIGNAL(timeout()), SLOT(fillBuffer()));
    }

    ~QAudioOutputBuffer()
    {
        delete m_buffer;
    }

    qint64 readFrames(char* data, qint64 maxFrames)
    {
        bool    wecan = true;
        qint64  framesRead = 0;

        while (wecan && framesRead < maxFrames) {
            QAudioRingBuffer::Region region = m_buffer->acquireReadRegion((maxFrames - framesRead) * m_bytesPerFrame);

            if (region.second > 0) {
                region.second -= region.second % m_bytesPerFrame;
                memcpy(data + (framesRead * m_bytesPerFrame), region.first, region.second);
                framesRead += region.second / m_bytesPerFrame;
            }
            else
                wecan = false;

            m_buffer->releaseReadRegion(region);
        }

        if (framesRead == 0 && m_deviceError)
            framesRead = -1;

        return framesRead;
    }

    qint64 writeBytes(const char* data, qint64 maxSize)
    {
        bool    wecan = true;
        qint64  bytesWritten = 0;

        maxSize -= maxSize % m_bytesPerFrame;
        while (wecan && bytesWritten < maxSize) {
            QAudioRingBuffer::Region region = m_buffer->acquireWriteRegion(maxSize - bytesWritten);

            if (region.second > 0) {
                memcpy(region.first, data + bytesWritten, region.second);
                bytesWritten += region.second;
            }
            else
                wecan = false;

            m_buffer->releaseWriteRegion(region);
        }

        if (bytesWritten > 0)
            emit readyRead();

        return bytesWritten;
    }

    int available() const
    {
        return m_buffer->free();
    }

    void reset()
    {
        m_buffer->reset();
        m_deviceError = false;
    }

    void setPrefetchDevice(QIODevice* device)
    {
        if (m_device != device) {
            m_device = device;
            if (m_device != 0)
                fillBuffer();
        }
    }

    void startFillTimer()
    {
        if (m_device != 0)
            m_fillTimer->start(m_buffer->size() / 2 / m_maxPeriodSize * m_periodTime);
    }

    void stopFillTimer()
    {
        m_fillTimer->stop();
    }

signals:
    void readyRead();

private slots:
    void fillBuffer()
    {
        const int free = m_buffer->free();
        const int writeSize = free - (free % m_maxPeriodSize);

        if (writeSize > 0) {
            bool    wecan = true;
            int     filled = 0;

            while (!m_deviceError && wecan && filled < writeSize) {
                QAudioRingBuffer::Region region = m_buffer->acquireWriteRegion(writeSize - filled);

                if (region.second > 0) {
                    region.second = m_device->read(region.first, region.second);
                    if (region.second > 0)
                        filled += region.second;
                    else if (region.second == 0)
                        wecan = false;
                    else if (region.second < 0) {
                        m_fillTimer->stop();
                        region.second = 0;
                        m_deviceError = true;
                    }
                }
                else
                    wecan = false;

                m_buffer->releaseWriteRegion(region);
            }

            if (filled > 0)
                emit readyRead();
        }
    }

private:
    bool        m_deviceError;
    int         m_maxPeriodSize;
    int         m_bytesPerFrame;
    int         m_periodTime;
    QIODevice*  m_device;
    QTimer*     m_fillTimer;
    QAudioRingBuffer*  m_buffer;
};


}

class MacOutputDevice : public QIODevice
{
    Q_OBJECT

public:
    MacOutputDevice(QAudioOutputBuffer* audioBuffer, QObject* parent):
        QIODevice(parent),
        m_audioBuffer(audioBuffer)
    {
        open(QIODevice::WriteOnly | QIODevice::Unbuffered);
    }

    qint64 readData(char* data, qint64 len)
    {
        Q_UNUSED(data);
        Q_UNUSED(len);

        return 0;
    }

    qint64 writeData(const char* data, qint64 len)
    {
        return m_audioBuffer->writeBytes(data, len);
    }

    bool isSequential() const
    {
        return true;
    }

private:
    QAudioOutputBuffer*    m_audioBuffer;
};


QAudioOutputPrivate::QAudioOutputPrivate(const QByteArray& device, const QAudioFormat& format):
    audioFormat(format)
{
    QDataStream ds(device);
    quint32 did, mode;

    ds >> did >> mode;

    if (QAudio::Mode(mode) == QAudio::AudioInput)
        errorCode = QAudio::OpenError;
    else {
        isOpen = false;
        audioDeviceId = AudioDeviceID(did);
        audioUnit = 0;
        audioIO = 0;
        startTime = 0;
        totalFrames = 0;
        audioBuffer = 0;
        internalBufferSize = default_buffer_size;
        clockFrequency = AudioGetHostClockFrequency() / 1000;
        errorCode = QAudio::NoError;
        stateCode = QAudio::StoppedState;
        audioThreadState = Stopped;

        intervalTimer = new QTimer(this);
        intervalTimer->setInterval(1000);
        connect(intervalTimer, SIGNAL(timeout()), SIGNAL(notify()));
    }
}

QAudioOutputPrivate::~QAudioOutputPrivate()
{
    close();
}

bool QAudioOutputPrivate::open()
{
    if (errorCode != QAudio::NoError)
        return false;

    if (isOpen)
        return true;

    ComponentDescription    cd;
    cd.componentType = kAudioUnitType_Output;
    cd.componentSubType = kAudioUnitSubType_HALOutput;
    cd.componentManufacturer = kAudioUnitManufacturer_Apple;
    cd.componentFlags = 0;
    cd.componentFlagsMask = 0;

    // Open
    Component cp = FindNextComponent(NULL, &cd);
    if (cp == 0) {
        qWarning() << "QAudioOutput: Failed to find HAL Output component";
        return false;
    }

    if (OpenAComponent(cp, &audioUnit) != noErr) {
        qWarning() << "QAudioOutput: Unable to Open Output Component";
        return false;
    }

    // register callback
    AURenderCallbackStruct  cb;
    cb.inputProc = renderCallback;
    cb.inputProcRefCon = this;

    if (AudioUnitSetProperty(audioUnit,
                               kAudioUnitProperty_SetRenderCallback,
                               kAudioUnitScope_Global,
                               0,
                               &cb,
                               sizeof(cb)) != noErr) {
        qWarning() << "QAudioOutput: Failed to set AudioUnit callback";
        return false;
    }

    // Set Audio Device
    if (AudioUnitSetProperty(audioUnit,
                                kAudioOutputUnitProperty_CurrentDevice,
                                kAudioUnitScope_Global,
                                0,
                                &audioDeviceId,
                                sizeof(audioDeviceId)) != noErr) {
        qWarning() << "QAudioOutput: Unable to use configured device";
        return false;
    }

    // Set stream format
    streamFormat = toAudioStreamBasicDescription(audioFormat);

    UInt32 size = sizeof(deviceFormat);
    if (AudioUnitGetProperty(audioUnit,
                                kAudioUnitProperty_StreamFormat,
                                kAudioUnitScope_Input,
                                0,
                                &deviceFormat,
                                &size) != noErr) {
        qWarning() << "QAudioOutput: Unable to retrieve device format";
        return false;
    }

    if (AudioUnitSetProperty(audioUnit,
                                kAudioUnitProperty_StreamFormat,
                                kAudioUnitScope_Input,
                                0,
                                &streamFormat,
                                sizeof(streamFormat)) != noErr) {
        qWarning() << "QAudioOutput: Unable to Set Stream information";
        return false;
    }

    // Allocate buffer
    UInt32 numberOfFrames = 0;
    size = sizeof(UInt32);
    if (AudioUnitGetProperty(audioUnit,
                                kAudioDevicePropertyBufferFrameSize,
                                kAudioUnitScope_Global,
                                0,
                                &numberOfFrames,
                                &size) != noErr) {
        qWarning() << "QAudioInput: Failed to get audio period size";
        return false;
    }

    periodSizeBytes = (numberOfFrames * streamFormat.mSampleRate / deviceFormat.mSampleRate) * 
                        streamFormat.mBytesPerFrame;
    if (internalBufferSize < periodSizeBytes * 2)
        internalBufferSize = periodSizeBytes * 2;
    else
        internalBufferSize -= internalBufferSize % streamFormat.mBytesPerFrame;

    audioBuffer = new QAudioOutputBuffer(internalBufferSize, periodSizeBytes, audioFormat);
    connect(audioBuffer, SIGNAL(readyRead()), SLOT(inputReady()));  // Pull

    audioIO = new MacOutputDevice(audioBuffer, this);

    // Init
    if (AudioUnitInitialize(audioUnit)) {
        qWarning() << "QAudioOutput: Failed to initialize AudioUnit";
        return false;
    }

    isOpen = true;

    return true;
}

void QAudioOutputPrivate::close()
{
    if (audioUnit != 0) {
        AudioOutputUnitStop(audioUnit);
        AudioUnitUninitialize(audioUnit);
        CloseComponent(audioUnit);
    }

    delete audioBuffer;
}

QAudioFormat QAudioOutputPrivate::format() const
{
    return audioFormat;
}

QIODevice* QAudioOutputPrivate::start(QIODevice* device)
{
    QIODevice*  op = device;

    if (!open()) {
        stateCode = QAudio::StoppedState;
        errorCode = QAudio::OpenError;
        return audioIO;
    }

    reset();
    audioBuffer->reset();
    audioBuffer->setPrefetchDevice(op);

    if (op == 0) {
        op = audioIO;
        stateCode = QAudio::IdleState;
    }
    else
        stateCode = QAudio::ActiveState;

    // Start
    errorCode = QAudio::NoError;
    totalFrames = 0;
    startTime = AudioGetCurrentHostTime();

    if (stateCode == QAudio::ActiveState)
        audioThreadStart();

    emit stateChanged(stateCode);

    return op;
}

void QAudioOutputPrivate::stop()
{
    QMutexLocker    lock(&mutex);
    if (stateCode != QAudio::StoppedState) {
        audioThreadDrain();

        stateCode = QAudio::StoppedState;
        errorCode = QAudio::NoError;
        QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection, Q_ARG(QAudio::State, stateCode));
    }
}

void QAudioOutputPrivate::reset()
{
    QMutexLocker    lock(&mutex);
    if (stateCode != QAudio::StoppedState) {
        audioThreadStop();

        stateCode = QAudio::StoppedState;
        errorCode = QAudio::NoError;
        QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection, Q_ARG(QAudio::State, stateCode));
    }
}

void QAudioOutputPrivate::suspend()
{
    QMutexLocker    lock(&mutex);
    if (stateCode == QAudio::ActiveState || stateCode == QAudio::IdleState) {
        audioThreadStop();

        stateCode = QAudio::SuspendedState;
        errorCode = QAudio::NoError;
        QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection, Q_ARG(QAudio::State, stateCode));
    }
}

void QAudioOutputPrivate::resume()
{
    QMutexLocker    lock(&mutex);
    if (stateCode == QAudio::SuspendedState) {
        audioThreadStart();

        stateCode = QAudio::ActiveState;
        errorCode = QAudio::NoError;
        QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection, Q_ARG(QAudio::State, stateCode));
    }
}

int QAudioOutputPrivate::bytesFree() const
{
    return audioBuffer->available();
}

int QAudioOutputPrivate::periodSize() const
{
    return periodSizeBytes;
}

void QAudioOutputPrivate::setBufferSize(int bs)
{
    if (stateCode == QAudio::StoppedState)
        internalBufferSize = bs;
}

int QAudioOutputPrivate::bufferSize() const
{
    return internalBufferSize;
}

void QAudioOutputPrivate::setNotifyInterval(int milliSeconds)
{
    intervalTimer->setInterval(milliSeconds);
}

int QAudioOutputPrivate::notifyInterval() const
{
    return intervalTimer->interval();
}

qint64 QAudioOutputPrivate::processedUSecs() const
{
    return totalFrames * 1000000 / audioFormat.frequency();
}

qint64 QAudioOutputPrivate::elapsedUSecs() const
{
    if (stateCode == QAudio::StoppedState)
        return 0;

    return (AudioGetCurrentHostTime() - startTime) / (clockFrequency / 1000);
}

QAudio::Error QAudioOutputPrivate::error() const
{
    return errorCode;
}

QAudio::State QAudioOutputPrivate::state() const
{
    return stateCode;
}

void QAudioOutputPrivate::audioThreadStart()
{
    startTimers();
    audioThreadState = Running;
    AudioOutputUnitStart(audioUnit);
}

void QAudioOutputPrivate::audioThreadStop()
{
    stopTimers();
    if (audioThreadState.testAndSetAcquire(Running, Stopped))
        threadFinished.wait(&mutex);
}

void QAudioOutputPrivate::audioThreadDrain()
{
    stopTimers();
    if (audioThreadState.testAndSetAcquire(Running, Draining))
        threadFinished.wait(&mutex);
}

void QAudioOutputPrivate::audioDeviceStop()
{
    AudioOutputUnitStop(audioUnit);
    audioThreadState = Stopped;
    threadFinished.wakeOne();
}

void QAudioOutputPrivate::audioDeviceIdle()
{
    QMutexLocker    lock(&mutex);
    if (stateCode == QAudio::ActiveState) {
        audioDeviceStop();

        errorCode = QAudio::UnderrunError;
        stateCode = QAudio::IdleState;
        QMetaObject::invokeMethod(this, "deviceStopped", Qt::QueuedConnection);
    }
}

void QAudioOutputPrivate::audioDeviceError()
{
    QMutexLocker    lock(&mutex);
    if (stateCode == QAudio::ActiveState) {
        audioDeviceStop();

        errorCode = QAudio::IOError;
        stateCode = QAudio::StoppedState;
        QMetaObject::invokeMethod(this, "deviceStopped", Qt::QueuedConnection);
    }
}

void QAudioOutputPrivate::startTimers()
{
    audioBuffer->startFillTimer();
    intervalTimer->start();
}

void QAudioOutputPrivate::stopTimers()
{
    audioBuffer->stopFillTimer();
    intervalTimer->stop();
}


void QAudioOutputPrivate::deviceStopped()
{
    intervalTimer->stop();
    emit stateChanged(stateCode);
}

void QAudioOutputPrivate::inputReady()
{
    QMutexLocker    lock(&mutex);
    if (stateCode == QAudio::IdleState) {
        audioThreadStart();

        stateCode = QAudio::ActiveState;
        errorCode = QAudio::NoError;

        QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection, Q_ARG(QAudio::State, stateCode));
    }
}


OSStatus QAudioOutputPrivate::renderCallback(void* inRefCon,
                                    AudioUnitRenderActionFlags* ioActionFlags,
                                    const AudioTimeStamp* inTimeStamp,
                                    UInt32 inBusNumber,
                                    UInt32 inNumberFrames,
                                    AudioBufferList* ioData)
{
    Q_UNUSED(ioActionFlags)
    Q_UNUSED(inTimeStamp)
    Q_UNUSED(inBusNumber)
    Q_UNUSED(inNumberFrames)

    QAudioOutputPrivate* d = static_cast<QAudioOutputPrivate*>(inRefCon);

    const int threadState = d->audioThreadState.fetchAndAddAcquire(0);
    if (threadState == Stopped) {
        ioData->mBuffers[0].mDataByteSize = 0;
        d->audioDeviceStop();
    }
    else {
        const UInt32    bytesPerFrame = d->streamFormat.mBytesPerFrame;
        qint64          framesRead;

        framesRead = d->audioBuffer->readFrames((char*)ioData->mBuffers[0].mData,
                                                 ioData->mBuffers[0].mDataByteSize / bytesPerFrame);

        if (framesRead > 0) {
            ioData->mBuffers[0].mDataByteSize = framesRead * bytesPerFrame;
            d->totalFrames += framesRead;
        }
        else {
            ioData->mBuffers[0].mDataByteSize = 0;
            if (framesRead == 0) {
                if (threadState == Draining)
                    d->audioDeviceStop();
                else
                    d->audioDeviceIdle();
            }
            else
                d->audioDeviceError();
        }
    }

    return noErr;
}


QT_END_NAMESPACE

#include "qaudiooutput_mac_p.moc"

