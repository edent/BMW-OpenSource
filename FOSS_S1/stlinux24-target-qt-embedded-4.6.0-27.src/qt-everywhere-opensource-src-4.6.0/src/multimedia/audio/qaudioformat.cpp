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
#include <QDebug>
#include <QtMultimedia/qaudioformat.h>


QT_BEGIN_NAMESPACE


class QAudioFormatPrivate : public QSharedData
{
public:
    QAudioFormatPrivate()
    {
        frequency = -1;
        channels = -1;
        sampleSize = -1;
        byteOrder = QAudioFormat::Endian(QSysInfo::ByteOrder);
        sampleType = QAudioFormat::Unknown;
    }

    QAudioFormatPrivate(const QAudioFormatPrivate &other):
        QSharedData(other),
        codec(other.codec),
        byteOrder(other.byteOrder),
        sampleType(other.sampleType),
        frequency(other.frequency),
        channels(other.channels),
        sampleSize(other.sampleSize)
    {
    }

    QAudioFormatPrivate& operator=(const QAudioFormatPrivate &other)
    {
        codec = other.codec;
        byteOrder = other.byteOrder;
        sampleType = other.sampleType;
        frequency = other.frequency;
        channels = other.channels;
        sampleSize = other.sampleSize;

        return *this;
    }

    QString codec;
    QAudioFormat::Endian byteOrder;
    QAudioFormat::SampleType sampleType;
    int frequency;
    int channels;
    int sampleSize;
};

/*!
    \class QAudioFormat
    \brief The QAudioFormat class stores audio parameter information.

    \inmodule QtMultimedia
    \ingroup  multimedia
    \since 4.6

    An audio format specifies how data in an audio stream is arranged,
    i.e, how the stream is to be interpreted. The encoding itself is
    specified by the codec() used for the stream.

    In addition to the encoding, QAudioFormat contains other
    parameters that further specify how the audio data is arranged.
    These are the frequency, the number of channels, the sample size,
    the sample type, and the byte order. The following table describes
    these in more detail.

    \table
        \header
            \o Parameter
            \o Description
        \row
            \o Frequency
            \o Samples per second of audio data in Hertz.
        \row
            \o Number of channels
            \o The number of audio channels (typically one for mono
               or two for stereo)
        \row
            \o Sample size
            \o How much data is stored in each sample (typically 8
               or 16)
        \row
            \o Sample type
            \o Numerical representation of sample (typically signed integer,
               unsigned integer or float)
        \row
            \o Byte order
            \o Byte ordering of sample (typically little endian, big endian)
    \endtable

    You can obtain audio formats compatible with the audio device used
    through functions in QAudioDeviceInfo. This class also lets you
    query available parameter values for a device, so that you can set
    the parameters yourself. See the QAudioDeviceInfo class
    description for details. You need to know the format of the audio
    streams you wish to play. Qt does not set up formats for you.
*/

/*!
    Construct a new audio format.

    Values are initialized as follows:
    \list
    \o frequency()  = -1
    \o channels()   = -1
    \o sampleSize() = -1
    \o byteOrder()  = QAudioFormat::Endian(QSysInfo::ByteOrder)
    \o sampleType() = QAudioFormat::Unknown
    \c codec()      = ""
    \endlist
*/

QAudioFormat::QAudioFormat():
    d(new QAudioFormatPrivate)
{
}

/*!
    Construct a new audio format using \a other.
*/

QAudioFormat::QAudioFormat(const QAudioFormat &other):
    d(other.d)
{
}

/*!
    Destroy this audio format.
*/

QAudioFormat::~QAudioFormat()
{
}

/*!
    Assigns \a other to this QAudioFormat implementation.
*/

QAudioFormat& QAudioFormat::operator=(const QAudioFormat &other)
{
    d = other.d;
    return *this;
}

/*!
  Returns true if this QAudioFormat is equal to the \a other
  QAudioFormat; otherwise returns false.

  All elements of QAudioFormat are used for the comparison.
*/

bool QAudioFormat::operator==(const QAudioFormat &other) const
{
    return d->frequency == other.d->frequency &&
            d->channels == other.d->channels &&
            d->sampleSize == other.d->sampleSize &&
            d->byteOrder == other.d->byteOrder &&
            d->codec == other.d->codec &&
            d->sampleType == other.d->sampleType;
}

/*!
  Returns true if this QAudioFormat is not equal to the \a other
  QAudioFormat; otherwise returns false.

  All elements of QAudioFormat are used for the comparison.
*/

bool QAudioFormat::operator!=(const QAudioFormat& other) const
{
    return !(*this == other);
}

/*!
    Returns true if all of the parameters are valid.
*/

bool QAudioFormat::isValid() const
{
    return d->frequency != -1 && d->channels != -1 && d->sampleSize != -1 &&
            d->sampleType != QAudioFormat::Unknown && !d->codec.isEmpty();
}

/*!
   Sets the frequency to \a frequency.
*/

void QAudioFormat::setFrequency(int frequency)
{
    d->frequency = frequency;
}

/*!
    Returns the current frequency value.
*/

int QAudioFormat::frequency() const
{
    return d->frequency;
}

/*!
   Sets the channels to \a channels.
*/

void QAudioFormat::setChannels(int channels)
{
    d->channels = channels;
}

/*!
    Returns the current channel value.
*/

int QAudioFormat::channels() const
{
    return d->channels;
}

/*!
   Sets the sampleSize to \a sampleSize.
*/

void QAudioFormat::setSampleSize(int sampleSize)
{
    d->sampleSize = sampleSize;
}

/*!
    Returns the current sampleSize value.
*/

int QAudioFormat::sampleSize() const
{
    return d->sampleSize;
}

/*!
   Sets the codec to \a codec.

   \sa QAudioDeviceInfo::supportedCodecs()
*/

void QAudioFormat::setCodec(const QString &codec)
{
    d->codec = codec;
}

/*!
    Returns the current codec value.

   \sa QAudioDeviceInfo::supportedCodecs()
*/

QString QAudioFormat::codec() const
{
    return d->codec;
}

/*!
   Sets the byteOrder to \a byteOrder.
*/

void QAudioFormat::setByteOrder(QAudioFormat::Endian byteOrder)
{
    d->byteOrder = byteOrder;
}

/*!
    Returns the current byteOrder value.
*/

QAudioFormat::Endian QAudioFormat::byteOrder() const
{
    return d->byteOrder;
}

/*!
   Sets the sampleType to \a sampleType.
*/

void QAudioFormat::setSampleType(QAudioFormat::SampleType sampleType)
{
    d->sampleType = sampleType;
}

/*!
    Returns the current SampleType value.
*/

QAudioFormat::SampleType QAudioFormat::sampleType() const
{
    return d->sampleType;
}

/*!
    \enum QAudioFormat::SampleType

    \value Unknown       Not Set
    \value SignedInt     samples are signed integers
    \value UnSignedInt   samples are unsigned intergers
    \value Float         samples are floats
*/

/*!
    \enum QAudioFormat::Endian

    \value BigEndian     samples are big endian byte order
    \value LittleEndian  samples are little endian byte order
*/

QT_END_NAMESPACE

