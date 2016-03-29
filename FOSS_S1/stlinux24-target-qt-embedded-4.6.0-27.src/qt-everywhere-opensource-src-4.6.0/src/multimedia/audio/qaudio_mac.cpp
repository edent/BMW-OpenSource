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


#include "qaudio_mac_p.h"

QT_BEGIN_NAMESPACE

// Debugging
QDebug operator<<(QDebug dbg, const QAudioFormat& audioFormat)
{
    dbg.nospace() << "QAudioFormat(" <<
            audioFormat.frequency() << "," <<
            audioFormat.channels() << "," <<
            audioFormat.sampleSize()<< "," <<
            audioFormat.codec() << "," <<
            audioFormat.byteOrder() << "," <<
            audioFormat.sampleType() << ")";

    return dbg.space();
}


// Conversion
QAudioFormat toQAudioFormat(AudioStreamBasicDescription const& sf)
{
    QAudioFormat    audioFormat;

    audioFormat.setFrequency(sf.mSampleRate);
    audioFormat.setChannels(sf.mChannelsPerFrame);
    audioFormat.setSampleSize(sf.mBitsPerChannel);
    audioFormat.setCodec(QString::fromLatin1("audio/pcm"));
    audioFormat.setByteOrder(sf.mFormatFlags & kLinearPCMFormatFlagIsBigEndian != 0 ? QAudioFormat::BigEndian : QAudioFormat::LittleEndian);
    QAudioFormat::SampleType type = QAudioFormat::UnSignedInt;
    if ((sf.mFormatFlags & kLinearPCMFormatFlagIsSignedInteger) != 0)
        type = QAudioFormat::SignedInt;
    else if ((sf.mFormatFlags & kLinearPCMFormatFlagIsFloat) != 0)
        type = QAudioFormat::Float;
    audioFormat.setSampleType(type);

    return audioFormat;
}

AudioStreamBasicDescription toAudioStreamBasicDescription(QAudioFormat const& audioFormat)
{
    AudioStreamBasicDescription sf;

    sf.mFormatFlags         = kAudioFormatFlagIsPacked;
    sf.mSampleRate          = audioFormat.frequency();
    sf.mFramesPerPacket     = 1;
    sf.mChannelsPerFrame    = audioFormat.channels();
    sf.mBitsPerChannel      = audioFormat.sampleSize();
    sf.mBytesPerFrame       = sf.mChannelsPerFrame * (sf.mBitsPerChannel / 8);
    sf.mBytesPerPacket      = sf.mFramesPerPacket * sf.mBytesPerFrame;
    sf.mFormatID            = kAudioFormatLinearPCM;

    switch (audioFormat.sampleType()) {
    case QAudioFormat::SignedInt: sf.mFormatFlags |= kAudioFormatFlagIsSignedInteger; break;
    case QAudioFormat::UnSignedInt: /* default */ break;
    case QAudioFormat::Float: sf.mFormatFlags |= kAudioFormatFlagIsFloat; break;
    case QAudioFormat::Unknown:  default: break;
    }

    return sf;
}

// QAudioRingBuffer
QAudioRingBuffer::QAudioRingBuffer(int bufferSize):
    m_bufferSize(bufferSize)
{
    m_buffer = new char[m_bufferSize];
    reset();
}

QAudioRingBuffer::~QAudioRingBuffer()
{
    delete m_buffer;
}

int QAudioRingBuffer::used() const
{
    return m_bufferUsed;
}

int QAudioRingBuffer::free() const
{
    return m_bufferSize - m_bufferUsed;
}

int QAudioRingBuffer::size() const
{
    return m_bufferSize;
}

void QAudioRingBuffer::reset()
{
    m_readPos = 0;
    m_writePos = 0;
    m_bufferUsed = 0;
}

QT_END_NAMESPACE


