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


#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudiodeviceinfo.h>
#include <QtMultimedia/qaudioengine.h>
#include <QtMultimedia/qaudioinput.h>

#include "qaudiodevicefactory_p.h"


QT_BEGIN_NAMESPACE

/*!
    \class QAudioInput
    \brief The QAudioInput class provides an interface for receiving audio data from an audio input device.

    \inmodule QtMultimedia
    \ingroup  multimedia
    \since 4.6

    You can construct an audio input with the system's
    \l{QAudioDeviceInfo::defaultInputDevice()}{default audio input
    device}. It is also possible to create QAudioInput with a
    specific QAudioDeviceInfo. When you create the audio input, you
    should also send in the QAudioFormat to be used for the recording
    (see the QAudioFormat class description for details).

    To record to a file:

    QAudioInput lets you record audio with an audio input device. The
    default constructor of this class will use the systems default
    audio device, but you can also specify a QAudioDeviceInfo for a
    specific device. You also need to pass in the QAudioFormat in
    which you wish to record.

    Starting up the QAudioInput is simply a matter of calling start()
    with a QIODevice opened for writing. For instance, to record to a
    file, you can:

    \code
    {
      QFile outputFile;
      outputFile.setFileName("/tmp/test.raw");
      outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );

      QAudioFormat format;
      // set up the format you want, eg.
      format.setFrequency(8000);
      format.setChannels(1);
      format.setSampleSize(8);
      format.setCodec("audio/pcm");
      format.setByteOrder(QAudioFormat::LittleEndian);
      format.setSampleType(QAudioFormat::UnSignedInt);

      QAudioInput *audio = new QAudioInput(format, this);
      QTimer::singleShot(3000, this, SLOT(stopRecording()));
      audio->start(outputFile);
      // Records audio for 3000ms
    }
    \endcode

    This will start recording if the format specified is supported by
    the input device (you can check this with
    QAudioDeviceInfo::isFormatSupported(). In case there are any
    snags, use the error() function to check what went wrong. We stop
    recording in the \c stopRecording() slot.

    \code
    void stopRecording()
    {
      audio->stop();
      outputFile->close();
    }
    \endcode

    At any point in time, QAudioInput will be in one of four states:
    active, suspended, stopped, or idle. These states are specified by
    the QAudio::State enum. You can request a state change directly through
    suspend(), resume(), stop(), reset(), and start(). The current
    state is reported by state(). QAudioOutput will also signal you
    when the state changes (stateChanged()).

    QAudioInput provides several ways of measuring the time that has
    passed since the start() of the recording. The \c processedUSecs()
    function returns the length of the stream in microseconds written,
    i.e., it leaves out the times the audio input was suspended or idle.
    The elapsedUSecs() function returns the time elapsed since start() was called regardless of
    which states the QAudioInput has been in.

    If an error should occur, you can fetch its reason with error().
    The possible error reasons are described by the QAudio::Error
    enum. The QAudioInput will enter the \l{QAudio::}{StoppedState} when
    an error is encountered.  Connect to the stateChanged() signal to
    handle the error:

    \snippet doc/src/snippets/audio/main.cpp 0

    \sa QAudioOutput, QAudioDeviceInfo
*/

/*!
    Construct a new audio input and attach it to \a parent.
    The default audio input device is used with the output
    \a format parameters.
*/

QAudioInput::QAudioInput(const QAudioFormat &format, QObject *parent):
    QObject(parent)
{
    d = QAudioDeviceFactory::createDefaultInputDevice(format);
    connect(d, SIGNAL(notify()), SIGNAL(notify()));
    connect(d, SIGNAL(stateChanged(QAudio::State)), SIGNAL(stateChanged(QAudio::State)));
}

/*!
    Construct a new audio input and attach it to \a parent.
    The device referenced by \a audioDevice is used with the input
    \a format parameters.
*/

QAudioInput::QAudioInput(const QAudioDeviceInfo &audioDevice, const QAudioFormat &format, QObject *parent):
    QObject(parent)
{
    d = QAudioDeviceFactory::createInputDevice(audioDevice, format);
    connect(d, SIGNAL(notify()), SIGNAL(notify()));
    connect(d, SIGNAL(stateChanged(QAudio::State)), SIGNAL(stateChanged(QAudio::State)));
}

/*!
    Destroy this audio input.
*/

QAudioInput::~QAudioInput()
{
    delete d;
}

/*!
     Uses the \a device as the QIODevice to transfer data.
     Passing a QIODevice allows the data to be transfered without any extra code.
     All that is required is to open the QIODevice.

     \sa QIODevice
*/

void QAudioInput::start(QIODevice* device)
{
    /*
       -If currently not StoppedState, stop
       -If previous start was push mode, delete internal QIODevice.
       -open audio input.
       If ok, NoError and ActiveState, else OpenError and StoppedState.
       -emit stateChanged()
    */
    d->start(device);
}

/*!
    Returns a pointer to the QIODevice being used to handle the data
    transfer. This QIODevice can be used to read() audio data
    directly.

    \sa QIODevice
*/

QIODevice* QAudioInput::start()
{
    /*
    -If currently not StoppedState, stop
    -If no internal QIODevice, create one.
    -open audio input.
    -If ok, NoError and IdleState, else OpenError and StoppedState
    -emit stateChanged()
    -return internal QIODevice
    */
    return d->start(0);
}

/*!
    Returns the QAudioFormat being used.
*/

QAudioFormat QAudioInput::format() const
{
    return d->format();
}

/*!
    Stops the audio input.
*/

void QAudioInput::stop()
{
    /*
    -If StoppedState, return
    -set to StoppedState
    -detach from audio device
    -emit stateChanged()
    */
    d->stop();
}

/*!
    Drops all audio data in the buffers, resets buffers to zero.
*/

void QAudioInput::reset()
{
    /*
    -drop all buffered audio, set buffers to zero.
    -call stop()
    */
    d->reset();
}

/*!
    Stops processing audio data, preserving buffered audio data.
*/

void QAudioInput::suspend()
{
    /*
    -If not ActiveState|IdleState, return
    -stop processing audio, saving all buffered audio data
    -set NoError and SuspendedState
    -emit stateChanged()
    */
    d->suspend();
}

/*!
    Resumes processing audio data after a suspend().
*/

void QAudioInput::resume()
{
    /*
    -If SuspendedState, return
    -resume audio
    -(PULL MODE): set ActiveState, NoError
    -(PUSH MODE): set IdleState, NoError
    -kick start audio if needed
    -emit stateChanged()
    */
     d->resume();
}

/*!
    Sets the audio buffer size to \a value milliseconds.

    Note: This function can be called anytime before start(), calls to this
    are ignored after start(). It should not be assumed that the buffer size
    set is the actual buffer size used, calling bufferSize() anytime after start()
    will return the actual buffer size being used.

*/

void QAudioInput::setBufferSize(int value)
{
    d->setBufferSize(value);
}

/*!
    Returns the audio buffer size in milliseconds.

    If called before start(), returns platform default value.
    If called before start() but setBufferSize() was called prior, returns value set by setBufferSize().
    If called after start(), returns the actual buffer size being used. This may not be what was set previously
    by setBufferSize().

*/

int QAudioInput::bufferSize() const
{
    return d->bufferSize();
}

/*!
    Returns the amount of audio data available to read in bytes.
*/

int QAudioInput::bytesReady() const
{
    /*
    -If not ActiveState|IdleState, return 0
    -return amount of audio data available to read
    */
    return d->bytesReady();
}

/*!
    Returns the period size in bytes.

    Note: This is the recommended read size in bytes.
*/

int QAudioInput::periodSize() const
{
    return d->periodSize();
}

/*!
    Sets the interval for notify() signal to be emitted.
    This is based on the \a ms of audio data processed
    not on actual real-time. The resolution of the timer is platform specific.
*/

void QAudioInput::setNotifyInterval(int ms)
{
    d->setNotifyInterval(ms);
}

/*!
    Returns the notify interval in milliseconds.
*/

int QAudioInput::notifyInterval() const
{
    return d->notifyInterval();
}

/*!
    Returns the amount of audio data processed since start()
    was called in microseconds.
*/

qint64 QAudioInput::processedUSecs() const
{
    return d->processedUSecs();
}

/*!
    Returns the microseconds since start() was called, including time in Idle and
    Suspend states.
*/

qint64 QAudioInput::elapsedUSecs() const
{
    return d->elapsedUSecs();
}

/*!
    Returns the error state.
*/

QAudio::Error QAudioInput::error() const
{
    return d->error();
}

/*!
    Returns the state of audio processing.
*/

QAudio::State QAudioInput::state() const
{
    return d->state();
}

/*!
    \fn QAudioInput::stateChanged(QAudio::State state)
    This signal is emitted when the device \a state has changed.
*/

/*!
    \fn QAudioInput::notify()
    This signal is emitted when x ms of audio data has been processed
    the interval set by setNotifyInterval(x).
*/

QT_END_NAMESPACE

