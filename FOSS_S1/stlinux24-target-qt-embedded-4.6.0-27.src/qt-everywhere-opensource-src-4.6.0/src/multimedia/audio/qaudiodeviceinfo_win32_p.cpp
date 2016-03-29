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


#include <windows.h>
#include <mmsystem.h>
#include "qaudiodeviceinfo_win32_p.h"

QT_BEGIN_NAMESPACE

// For mingw toolchain mmsystem.h only defines half the defines, so add if needed.
#ifndef WAVE_FORMAT_44M08
#define WAVE_FORMAT_44M08 0x00000100
#define WAVE_FORMAT_44S08 0x00000200
#define WAVE_FORMAT_44M16 0x00000400
#define WAVE_FORMAT_44S16 0x00000800
#define WAVE_FORMAT_48M08 0x00001000
#define WAVE_FORMAT_48S08 0x00002000
#define WAVE_FORMAT_48M16 0x00004000
#define WAVE_FORMAT_48S16 0x00008000
#define WAVE_FORMAT_96M08 0x00010000
#define WAVE_FORMAT_96S08 0x00020000
#define WAVE_FORMAT_96M16 0x00040000
#define WAVE_FORMAT_96S16 0x00080000
#endif


QAudioDeviceInfoInternal::QAudioDeviceInfoInternal(QByteArray dev, QAudio::Mode mode)
{
    device = QLatin1String(dev);
    this->mode = mode;
}

QAudioDeviceInfoInternal::~QAudioDeviceInfoInternal()
{
    close();
}

bool QAudioDeviceInfoInternal::isFormatSupported(const QAudioFormat& format) const
{
    return testSettings(format);
}

QAudioFormat QAudioDeviceInfoInternal::preferredFormat() const
{
    QAudioFormat nearest;
    if(mode == QAudio::AudioOutput) {
        nearest.setFrequency(44100);
        nearest.setChannels(2);
        nearest.setByteOrder(QAudioFormat::LittleEndian);
        nearest.setSampleType(QAudioFormat::SignedInt);
        nearest.setSampleSize(16);
        nearest.setCodec(QLatin1String("audio/pcm"));
    } else {
        nearest.setFrequency(11025);
        nearest.setChannels(1);
        nearest.setByteOrder(QAudioFormat::LittleEndian);
        nearest.setSampleType(QAudioFormat::SignedInt);
        nearest.setSampleSize(8);
        nearest.setCodec(QLatin1String("audio/pcm"));
    }
    return nearest;
}

QAudioFormat QAudioDeviceInfoInternal::nearestFormat(const QAudioFormat& format) const
{
    if(testSettings(format))
        return format;
    else
        return preferredFormat();
}

QString QAudioDeviceInfoInternal::deviceName() const
{
    return device;
}

QStringList QAudioDeviceInfoInternal::codecList()
{
    updateLists();
    return codecz;
}

QList<int> QAudioDeviceInfoInternal::frequencyList()
{
    updateLists();
    return freqz;
}

QList<int> QAudioDeviceInfoInternal::channelsList()
{
    updateLists();
    return channelz;
}

QList<int> QAudioDeviceInfoInternal::sampleSizeList()
{
    updateLists();
    return sizez;
}

QList<QAudioFormat::Endian> QAudioDeviceInfoInternal::byteOrderList()
{
    updateLists();
    return byteOrderz;
}

QList<QAudioFormat::SampleType> QAudioDeviceInfoInternal::sampleTypeList()
{
    updateLists();
    return typez;
}


bool QAudioDeviceInfoInternal::open()
{
    return true;
}

void QAudioDeviceInfoInternal::close()
{
}

bool QAudioDeviceInfoInternal::testSettings(const QAudioFormat& format) const
{
    // Set nearest to closest settings that do work.
    // See if what is in settings will work (return value).

    bool failed = false;

    // For now, just accept only audio/pcm codec
    if(!format.codec().startsWith(QLatin1String("audio/pcm")))
        failed = true;

    if(!failed && !(format.channels() == 1 || format.channels() == 2))
        failed = true;

    if(!failed) {
        if(!(format.frequency() == 8000 || format.frequency() == 11025 || format.frequency() == 22050 ||
	   format.frequency() == 44100 || format.frequency() == 48000 || format.frequency() == 96000))
	    failed = true;
    }

    if(!failed && !(format.sampleSize() == 8 || format.sampleSize() == 16))
        failed = true;

    if(!failed) {
        // settings work
        return true;
    }
    return false;
}

void QAudioDeviceInfoInternal::updateLists()
{
    // redo all lists based on current settings
    bool base = false;
    bool match = false;
    DWORD fmt = NULL;
    QString tmp;

    if(device.compare(QLatin1String("default")) == 0)
        base = true;

    if(mode == QAudio::AudioOutput) {
        WAVEOUTCAPS woc;
	unsigned long iNumDevs,i;
	iNumDevs = waveOutGetNumDevs();
	for(i=0;i<iNumDevs;i++) {
	    if(waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))
	        == MMSYSERR_NOERROR) {
		tmp = QString::fromUtf16((const unsigned short*)woc.szPname);
		if(tmp.compare(device) == 0) {
		    match = true;
		    fmt = woc.dwFormats;
		    break;
		}
		if(base) {
		    match = true;
		    fmt = woc.dwFormats;
		    break;
		}
	    }
	}
    } else {
        WAVEINCAPS woc;
	unsigned long iNumDevs,i;
	iNumDevs = waveInGetNumDevs();
	for(i=0;i<iNumDevs;i++) {
	    if(waveInGetDevCaps(i, &woc, sizeof(WAVEINCAPS))
	        == MMSYSERR_NOERROR) {
		tmp = QString::fromUtf16((const unsigned short*)woc.szPname);
		if(tmp.compare(device) == 0) {
		    match = true;
		    fmt = woc.dwFormats;
		    break;
		}
		if(base) {
		    match = true;
		    fmt = woc.dwFormats;
		    break;
		}
	    }
	}
    }
    sizez.clear();
    freqz.clear();
    channelz.clear();
    byteOrderz.clear();
    typez.clear();
    codecz.clear();

    if(match) {
        if((fmt && WAVE_FORMAT_1M08)
           || (fmt && WAVE_FORMAT_1S08)
	   || (fmt && WAVE_FORMAT_2M08)
	   || (fmt && WAVE_FORMAT_2S08)
	   || (fmt && WAVE_FORMAT_4M08)
	   || (fmt && WAVE_FORMAT_4S08)
#ifndef Q_OS_WINCE
	   || (fmt && WAVE_FORMAT_48M08)
	   || (fmt && WAVE_FORMAT_48S08)
	   || (fmt && WAVE_FORMAT_96M08)
	   || (fmt && WAVE_FORMAT_96S08)
#endif
       ) {
            sizez.append(8);
	}
        if((fmt && WAVE_FORMAT_1M16)
           || (fmt && WAVE_FORMAT_1S16)
	   || (fmt && WAVE_FORMAT_2M16)
	   || (fmt && WAVE_FORMAT_2S16)
	   || (fmt && WAVE_FORMAT_4M16)
	   || (fmt && WAVE_FORMAT_4S16)
#ifndef Q_OS_WINCE
	   || (fmt && WAVE_FORMAT_48M16)
	   || (fmt && WAVE_FORMAT_48S16)
	   || (fmt && WAVE_FORMAT_96M16)
	   || (fmt && WAVE_FORMAT_96S16)
#endif
       ) {
            sizez.append(16);
	}
        if((fmt && WAVE_FORMAT_1M08)
           || (fmt && WAVE_FORMAT_1S08)
	   || (fmt && WAVE_FORMAT_1M16)
	   || (fmt && WAVE_FORMAT_1S16)) {
            freqz.append(11025);
	}
        if((fmt && WAVE_FORMAT_2M08)
           || (fmt && WAVE_FORMAT_2S08)
	   || (fmt && WAVE_FORMAT_2M16)
	   || (fmt && WAVE_FORMAT_2S16)) {
            freqz.append(22050);
	}
        if((fmt && WAVE_FORMAT_4M08)
           || (fmt && WAVE_FORMAT_4S08)
	   || (fmt && WAVE_FORMAT_4M16)
	   || (fmt && WAVE_FORMAT_4S16)) {
            freqz.append(44100);
	}
#ifndef Q_OS_WINCE
        if((fmt && WAVE_FORMAT_48M08)
           || (fmt && WAVE_FORMAT_48S08)
	   || (fmt && WAVE_FORMAT_48M16)
	   || (fmt && WAVE_FORMAT_48S16)) {
            freqz.append(48000);
	}
        if((fmt && WAVE_FORMAT_96M08)
           || (fmt && WAVE_FORMAT_96S08)
	   || (fmt && WAVE_FORMAT_96M16)
	   || (fmt && WAVE_FORMAT_96S16)) {
            freqz.append(96000);
        }
#endif
	channelz.append(1);
	channelz.append(2);

	byteOrderz.append(QAudioFormat::LittleEndian);

	typez.append(QAudioFormat::SignedInt);
	typez.append(QAudioFormat::UnSignedInt);

	codecz.append(QLatin1String("audio/pcm"));
    }
}

QList<QByteArray> QAudioDeviceInfoInternal::availableDevices(QAudio::Mode mode)
{
    Q_UNUSED(mode)

    QList<QByteArray> devices;

    if(mode == QAudio::AudioOutput) {
        WAVEOUTCAPS woc;
	unsigned long iNumDevs,i;
	iNumDevs = waveOutGetNumDevs();
	for(i=0;i<iNumDevs;i++) {
	    if(waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))
	        == MMSYSERR_NOERROR) {
	        devices.append(QString::fromUtf16((const unsigned short*)woc.szPname).toLocal8Bit().constData());
	    }
	}
    } else {
        WAVEINCAPS woc;
	unsigned long iNumDevs,i;
	iNumDevs = waveInGetNumDevs();
	for(i=0;i<iNumDevs;i++) {
	    if(waveInGetDevCaps(i, &woc, sizeof(WAVEINCAPS))
	        == MMSYSERR_NOERROR) {
	        devices.append(QString::fromUtf16((const unsigned short*)woc.szPname).toLocal8Bit().constData());
	    }
	}

    }
    if(devices.count() > 0)
        devices.append("default");

    return devices;
}

QByteArray QAudioDeviceInfoInternal::defaultOutputDevice()
{
    return QByteArray("default");
}

QByteArray QAudioDeviceInfoInternal::defaultInputDevice()
{
    return QByteArray("default");
}

QT_END_NAMESPACE
