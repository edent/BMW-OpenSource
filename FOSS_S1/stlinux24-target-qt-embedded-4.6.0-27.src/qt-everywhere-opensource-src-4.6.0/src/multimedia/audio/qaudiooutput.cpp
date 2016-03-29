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
#include <QtMultimedia/qaudiooutput.h>

#include "qaudiodevicefactory_p.h"


QT_BEGIN_NAMESPACE

/*!
    \class QAudioOutput
    \brief The QAudioOutput class provides an interface for sending audio data to an audio output device.

    \inmodule QtMultimedia
    \ingroup  multimedia
    \since 4.6

    You can construct an audio output with the system's
    \l{QAudioDeviceInfo::defaultOutputDevice()}{default audio output
    device}. It is also possible to create QAudioOutput with a
    specific QAudioDeviceInfo. When you create the audio output, you
    should also send in the QAudioFormat to be used for the playback
    (see the QAudioFormat class description for details).

    To play a file:

    Starting to play an audio stream is simply a matter of calling
    start() with a QIODevice. QAudioOutput will then fetch the data it
    needs from the io device. So playing back an audio file is as
    simple as:

    \code
      QFile inputFile;
      inputFile.setFileName("/tmp/test.raw");
      inputFile.open(QIODevice::ReadOnly);

      QAudioFormat format;
      // Set up the format, eg.
      format.setFrequency(8000);
      format.setChannels(1);
      format.setSampleSize(8);
      format.setCodec("audio/pcm");
      format.setByteOrder(QAudioFormat::LittleEndian);
      format.setSampleType(QAudioFormat::UnSignedInt);

      QAudioOutput *audio = new QAudioOutput(format, this);
      connect(audio,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
      audio->start(inputFile);

    \endcode

    The file will start playing assuming that the audio system and
    output device support it. If you run out of luck, check what's
    up with the error() function.

    After the file has finished playing, we need to stop the device:

    \code
      void finishedPlaying(QAudio::State state)
      {
        if(state == QAudio::IdleState) {
          audio->stop();
          inputFile.close();
        }
      }
    \endcode

    At any given time, the QAudioOutput will be in one of four states:
    active, suspended, stopped, or idle. These states are described
    by the QAudio::State enum.
    State changes are reported through the stateChanged() signal. You
    can use this signal to, for instance, update the GUI of the
    application; the mundane example here being changing the state of
    a \c { play/pause } button. You request a state change directly
    with suspend(), stop(), reset(), resume(), and start().

    While the stream is playing, you can set a notify interval in
    milliseconds with setNotifyInterval(). This interval specifies the
    time between two emissions of the notify() signal. This is
    relative to the position in the stream, i.e., if the QAudioOutput
    is in the SuspendedState or the IdleState, the notify() signal is
    not emitted. A typical use-case would be to update a
    \l{QSlider}{slider} that allows seeking in the stream.
    If you want the time since playback started regardless of which
    states the audio output has been in, elapsedUSecs() is the function for you.

    If an error occurs, you can fetch the \l{QAudio::Error}{error
    type} with the error() function. Please see the QAudio::Error enum
    for a description of the possible errors that are reported.  When
    an error is encountered, the state changes to QAudio::StoppedState.
    You can check for errors by connecting to the stateChanged()
    signal:

    \snippet doc/src/snippets/audio/main.cpp 3

    \sa QAudioInput, QAudioDeviceInfo
*/

/*!
    Construct a new audio output and attach it to \a parent.
    The default audio output device is used with the output
    \a format parameters.
*/

QAudioOutput::QAudioOutput(const QAudioFormat &format, QObject *parent):
    QObject(parent)
{
    d = QAudioDeviceFactory::createDefaultOutputDevice(format);
    connect(d, SIGNAL(notify()), SIGNAL(notify()));
    connect(d, SIGNAL(stateChanged(QAudio::State)), SIGNAL(stateChanged(QAudio::State)));
}

/*!
    Construct a new audio output and attach it to \a parent.
    The device referenced by \a audioDevice is used with the output
    \a format parameters.
*/

QAudioOutput::QAudioOutput(const QAudioDeviceInfo &audioDevice, const QAudioFormat &format, QObject *parent):
    QObject(parent)
{
    d = QAudioDeviceFactory::createOutputDevice(audioDevice, format);
    connect(d, SIGNAL(notify()), SIGNAL(notify()));
    connect(d, SIGNAL(stateChanged(QAudio::State)), SIGNAL(stateChanged(QAudio::State)));
}

/*!
    Destroys this audio output.
*/

QAudioOutput::~QAudioOutput()
{
    delete d;
}

/*!
    Returns the QAudioFormat being used.

*/

QAudioFormat QAudioOutput::format() const
{
    return d->format();
}

/*!
    Uses the \a device as the QIODevice to transfer data.
    Passing a QIODevice allows the data to be transfered without any extra code.
    All that is required is to open the QIODevice.

    \sa QIODevice
*/

void QAudioOutput::start(QIODevice* device)
{
    /*
    -If currently not StoppedState, stop.
    -If previous start was push mode, delete internal QIODevice.
    -open audio output.
    -If ok, NoError and ActiveState, else OpenError and StoppedState
    -emit stateChanged()
    */
    d->start(device);
}

/*!
    Returns a pointer to the QIODevice being used to handle the data
    transfer. This QIODevice can be used to write() audio data directly.

    \sa QIODevice
*/

QIODevice* QAudioOutput::start()
{
    /*
    -If currently not StoppedState, stop.
    -If no internal QIODevice, create one.
    -open audio output.
    -If ok, NoError and IdleState, else OpenError and StoppedState
    -emit stateChanged()
    -return internal QIODevice
    */
    return d->start(0);
}

/*!
    Stops the audio output.
*/

void QAudioOutput::stop()
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

void QAudioOutput::reset()
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

void QAudioOutput::suspend()
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

void QAudioOutput::resume()
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
    Returns the free space available in bytes in the audio buffer.
*/

int QAudioOutput::bytesFree() const
{
    /*
    -If not ActiveState|IdleState, return 0
    -return space available in audio buffer in bytes
    */
    return d->bytesFree();
}

/*!
    Returns the period size in bytes.

    Note: This is the recommended write size in bytes.
*/

int QAudioOutput::periodSize() const
{
    return d->periodSize();
}

/*!
    Sets the audio buffer size to \a value in bytes.

    Note: This function can be called anytime before start(), calls to this
    are ignored after start(). It should not be assumed that the buffer size
    set is the actual buffer size used, calling bufferSize() anytime after start()
    will return the actual buffer size being used.
*/

void QAudioOutput::setBufferSize(int value)
{
    d->setBufferSize(value);
}

/*!
    Returns the audio buffer size in bytes.

    If called before start(), returns platform default value.
    If called before start() but setBufferSize() was called prior, returns value set by setBufferSize().
    If called after start(), returns the actual buffer size being used. This may not be what was set previously
    by setBufferSize().

*/

int QAudioOutput::bufferSize() const
{
    return d->bufferSize();
}

/*!
    Sets the interval for notify() signal to be emitted.
    This is based on the \a ms of audio data processed
    not on actual real-time. The resolution of the timer is platform specific.
*/

void QAudioOutput::setNotifyInterval(int ms)
{
    d->setNotifyInterval(ms);
}

/*!
    Returns the notify interval in milliseconds.
*/

int QAudioOutput::notifyInterval() const
{
    return d->notifyInterval();
}

/*!
    Returns the amount of audio data processed since start()
    was called in microseconds.
*/

qint64 QAudioOutput::processedUSecs() const
{
    return d->processedUSecs();
}

/*!
    Returns the microseconds since start() was called, including time in Idle and
    Suspend states.
*/

qint64 QAudioOutput::elapsedUSecs() const
{
    return d->elapsedUSecs();
}

/*!
    Returns the error state.
*/

QAudio::Error QAudioOutput::error() const
{
    return d->error();
}

/*!
    Returns the state of audio processing.
*/

QAudio::State QAudioOutput::state() const
{
    return d->state();
}

/*!
    \fn QAudioOutput::stateChanged(QAudio::State state)
    This signal is emitted when the device \a state has changed.
    This is the current state of the audio output.
*/

/*!
    \fn QAudioOutput::notify()
    This signal is emitted when x ms of audio data has been processed
    the interval set by setNotifyInterval(x).
*/

QT_END_NAMESPACE
