/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [snippet]
QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
{
    switch (c) {
    case MediaObjectClass:
        return new MediaObject(parent);
    case VolumeFaderEffectClass:
        return new VolumeFaderEffect(parent);
    case AudioOutputClass:
        return new AudioOutput(parent);
    case AudioDataOutputClass:
        return new AudioDataOutput(parent);
    case VisualizationClass:
        return new Visualization(parent);
    case VideoDataOutputClass:
        return new VideoDataOutput(parent);
    case EffectClass:
        return new Effect(args[0].toInt(), parent);
    case VideoWidgetClass:
        return new VideoWidget(qobject_cast<QWidget *>(parent));
    }
    return 0;
}

QSet<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    QSet<int> set;
    switch(type)
    {
    case Phonon::AudioOutputDeviceType:
        // use AudioDeviceEnumerator to list ALSA and OSS devices
        set << 10000 << 10001;
        break;
    case Phonon::AudioCaptureDeviceType:
        set << 20000 << 20001;
        break;
    case Phonon::VideoOutputDeviceType:
        break;
    case Phonon::VideoCaptureDeviceType:
        set << 30000 << 30001;
        break;
    case Phonon::VisualizationType:
    case Phonon::AudioCodecType:
    case Phonon::VideoCodecType:
    case Phonon::ContainerFormatType:
        break;
    case Phonon::EffectType:
        set << 0x7F000001;
        break;
    }
    return set;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    QHash<QByteArray, QVariant> ret;
    switch (type) {
    case Phonon::AudioOutputDeviceType:
        switch (index) {
        case 10000:
            ret.insert("name", QLatin1String("internal Soundcard"));
            break;
        case 10001:
            ret.insert("name", QLatin1String("USB Headset"));
            ret.insert("icon", KIcon("usb-headset"));
            ret.insert("available", false);
            break;
        }
        break;
    case Phonon::AudioCaptureDeviceType:
        switch (index) {
        case 20000:
            ret.insert("name", QLatin1String("Soundcard"));
            ret.insert("description", QLatin1String("first description"));
            break;
        case 20001:
            ret.insert("name", QLatin1String("DV"));
            ret.insert("description", QLatin1String("second description"));
            break;
        }
        break;
    case Phonon::VideoOutputDeviceType:
        break;
    case Phonon::VideoCaptureDeviceType:
        switch (index) {
        case 30000:
            ret.insert("name", QLatin1String("USB Webcam"));
            ret.insert("description", QLatin1String("first description"));
            break;
        case 30001:
            ret.insert("name", QLatin1String("DV"));
            ret.insert("description", QLatin1String("second description"));
            break;
        }
        break;
    case Phonon::VisualizationType:
        break;
    case Phonon::AudioCodecType:
        break;
    case Phonon::VideoCodecType:
        break;
    case Phonon::ContainerFormatType:
        break;
    case Phonon::EffectType:
        switch (index) {
        case 0x7F000001:
            ret.insert("name", QLatin1String("Delay"));
            ret.insert("description", QLatin1String("Simple delay effect with time, feedback and level controls."));
            break;
        }
        break;
    }
    return ret;
}
//! [snippet]
