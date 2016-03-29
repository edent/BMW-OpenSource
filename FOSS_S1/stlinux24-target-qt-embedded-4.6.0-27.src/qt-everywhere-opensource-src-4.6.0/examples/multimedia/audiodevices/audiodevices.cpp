/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#include <QAudioDeviceInfo>

#include "audiodevices.h"

AudioDevicesBase::AudioDevicesBase( QMainWindow *parent, Qt::WFlags f )
{
    Q_UNUSED(parent)
    Q_UNUSED(f)
    setupUi( this );
}

AudioDevicesBase::~AudioDevicesBase() {}


AudioTest::AudioTest( QMainWindow *parent, Qt::WFlags f )
    : AudioDevicesBase( parent, f )
{
    nearestFreq->setDisabled(true);
    nearestChannel->setDisabled(true);
    nearestCodec->setDisabled(true);
    nearestSampleSize->setDisabled(true);
    nearestSampleType->setDisabled(true);
    nearestEndian->setDisabled(true);
    logOutput->setDisabled(true);

    mode = QAudio::AudioOutput;
    modeBox->addItem("Input");
    modeBox->addItem("Output");

    connect(testButton,SIGNAL(clicked()),SLOT(test()));
    connect(modeBox,SIGNAL(activated(int)),SLOT(modeChanged(int)));
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    connect(frequencyBox,SIGNAL(activated(int)),SLOT(freqChanged(int)));
    connect(channelsBox,SIGNAL(activated(int)),SLOT(channelChanged(int)));
    connect(codecsBox,SIGNAL(activated(int)),SLOT(codecChanged(int)));
    connect(sampleSizesBox,SIGNAL(activated(int)),SLOT(sampleSizeChanged(int)));
    connect(sampleTypesBox,SIGNAL(activated(int)),SLOT(sampleTypeChanged(int)));
    connect(endianBox,SIGNAL(activated(int)),SLOT(endianChanged(int)));

    modeBox->setCurrentIndex(0);
    modeChanged(0);
    deviceBox->setCurrentIndex(0);
    deviceChanged(0);
}

AudioTest::~AudioTest()
{
}

void AudioTest::test()
{
    // tries to set all the settings picked.
    logOutput->clear();
    logOutput->append("NOTE: an invalid codec audio/test exists for testing, to get a fail condition.");

    if (!deviceInfo.isNull()) {
        if (deviceInfo.isFormatSupported(settings)) {
            logOutput->append("Success");
            nearestFreq->setText("");
            nearestChannel->setText("");
            nearestCodec->setText("");
            nearestSampleSize->setText("");
            nearestSampleType->setText("");
            nearestEndian->setText("");
        } else {
            QAudioFormat nearest = deviceInfo.nearestFormat(settings);
            logOutput->append(tr("Failed"));
            nearestFreq->setText(QString("%1").arg(nearest.frequency()));
            nearestChannel->setText(QString("%1").arg(nearest.channels()));
            nearestCodec->setText(nearest.codec());
            nearestSampleSize->setText(QString("%1").arg(nearest.sampleSize()));

            switch(nearest.sampleType()) {
                case QAudioFormat::SignedInt:
                    nearestSampleType->setText("SignedInt");
                    break;
                case QAudioFormat::UnSignedInt:
                    nearestSampleType->setText("UnSignedInt");
                    break;
                case QAudioFormat::Float:
                    nearestSampleType->setText("Float");
                    break;
                case QAudioFormat::Unknown:
                    nearestSampleType->setText("Unknown");
            }
            switch(nearest.byteOrder()) {
                case QAudioFormat::LittleEndian:
                    nearestEndian->setText("LittleEndian");
                    break;
                case QAudioFormat::BigEndian:
                    nearestEndian->setText("BigEndian");
            }
        }
    }
    else
        logOutput->append("No Device");
}

void AudioTest::modeChanged(int idx)
{
    // mode has changed
    if(idx == 0)
        mode=QAudio::AudioInput;
    else
        mode=QAudio::AudioOutput;

    deviceBox->clear();
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(mode))
        deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
}

void AudioTest::deviceChanged(int idx)
{
    if (deviceBox->count() == 0)
        return;

    // device has changed
    deviceInfo = deviceBox->itemData(idx).value<QAudioDeviceInfo>();

    frequencyBox->clear();
    QList<int> freqz = deviceInfo.supportedFrequencies();
    for(int i = 0; i < freqz.size(); ++i)
        frequencyBox->addItem(QString("%1").arg(freqz.at(i)));
    if(freqz.size())
        settings.setFrequency(freqz.at(0));

    channelsBox->clear();
    QList<int> chz = deviceInfo.supportedChannels();
    for(int i = 0; i < chz.size(); ++i)
        channelsBox->addItem(QString("%1").arg(chz.at(i)));
    if(chz.size())
        settings.setChannels(chz.at(0));

    codecsBox->clear();
    QStringList codecz = deviceInfo.supportedCodecs();
    for(int i = 0; i < codecz.size(); ++i)
        codecsBox->addItem(QString("%1").arg(codecz.at(i)));
    if(codecz.size())
        settings.setCodec(codecz.at(0));
    // Add false to create failed condition!
    codecsBox->addItem("audio/test");

    sampleSizesBox->clear();
    QList<int> sampleSizez = deviceInfo.supportedSampleSizes();
    for(int i = 0; i < sampleSizez.size(); ++i)
        sampleSizesBox->addItem(QString("%1").arg(sampleSizez.at(i)));
    if(sampleSizez.size())
        settings.setSampleSize(sampleSizez.at(0));

    sampleTypesBox->clear();
    QList<QAudioFormat::SampleType> sampleTypez = deviceInfo.supportedSampleTypes();
    for(int i = 0; i < sampleTypez.size(); ++i) {
        switch(sampleTypez.at(i)) {
            case QAudioFormat::SignedInt:
                sampleTypesBox->addItem("SignedInt");
                break;
            case QAudioFormat::UnSignedInt:
                sampleTypesBox->addItem("UnSignedInt");
                break;
            case QAudioFormat::Float:
                sampleTypesBox->addItem("Float");
                break;
            case QAudioFormat::Unknown:
                sampleTypesBox->addItem("Unknown");
        }
	if(sampleTypez.size())
            settings.setSampleType(sampleTypez.at(0));
    }

    endianBox->clear();
    QList<QAudioFormat::Endian> endianz = deviceInfo.supportedByteOrders();
    for(int i = 0; i < endianz.size(); ++i) {
        switch(endianz.at(i)) {
            case QAudioFormat::LittleEndian:
                endianBox->addItem("Little Endian");
                break;
            case QAudioFormat::BigEndian:
                endianBox->addItem("Big Endian");
                break;
        }
    }
    if(endianz.size())
        settings.setByteOrder(endianz.at(0));
}

void AudioTest::freqChanged(int idx)
{
    // freq has changed
    settings.setFrequency(frequencyBox->itemText(idx).toInt());
}

void AudioTest::channelChanged(int idx)
{
    settings.setChannels(channelsBox->itemText(idx).toInt());
}

void AudioTest::codecChanged(int idx)
{
    settings.setCodec(codecsBox->itemText(idx));
}

void AudioTest::sampleSizeChanged(int idx)
{
    settings.setSampleSize(sampleSizesBox->itemText(idx).toInt());
}

void AudioTest::sampleTypeChanged(int idx)
{
    switch(sampleTypesBox->itemText(idx).toInt()) {
        case QAudioFormat::SignedInt:
            settings.setSampleType(QAudioFormat::SignedInt);
            break;
        case QAudioFormat::UnSignedInt:
            settings.setSampleType(QAudioFormat::UnSignedInt);
            break;
        case QAudioFormat::Float:
            settings.setSampleType(QAudioFormat::Float);
    }
}

void AudioTest::endianChanged(int idx)
{
    switch(endianBox->itemText(idx).toInt()) {
        case QAudioFormat::LittleEndian:
            settings.setByteOrder(QAudioFormat::LittleEndian);
            break;
        case QAudioFormat::BigEndian:
            settings.setByteOrder(QAudioFormat::BigEndian);
    }
}

