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
#include <QVBoxLayout>

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include "audiooutput.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SECONDS     1
#define FREQ        600
#define SYSTEM_FREQ 44100

Generator::Generator(QObject *parent)
    :QIODevice( parent )
{
    finished = false;
    buffer = new char[SECONDS*SYSTEM_FREQ*4+1000];
    t=buffer;
    len=fillData(t,FREQ,SECONDS); /* mono FREQHz sine */
    pos   = 0;
    total = len;
}

Generator::~Generator()
{
    delete [] buffer;
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    close();
}

int Generator::putShort(char *t, unsigned int value)
{
    *(unsigned char *)(t++)=value&255;
    *(unsigned char *)(t)=(value/256)&255;
    return 2;
}

int Generator::fillData(char *start, int frequency, int seconds)
{
    int i, len=0;
    int value;
    for(i=0; i<seconds*SYSTEM_FREQ; i++) {
        value=(int)(32767.0*sin(2.0*M_PI*((double)(i))*(double)(frequency)/SYSTEM_FREQ));
        putShort(start, value);
        start += 4;
        len+=2;
    }
    return len;
}

qint64 Generator::readData(char *data, qint64 maxlen)
{
    int len = maxlen;
    if(len > 16384)
        len = 16384;

    if(len < (SECONDS*SYSTEM_FREQ*2)-pos) {
        // Normal
        memcpy(data,t+pos,len);
        pos+=len;
        return len;
    } else {
        // Whats left and reset to start
        qint64 left = (SECONDS*SYSTEM_FREQ*2)-pos;
        memcpy(data,t+pos,left);
        pos=0;
        return left;
    }
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

AudioTest::AudioTest()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click for Push Mode"));
    connect(button,SIGNAL(clicked()),SLOT(toggle()));
    layout->addWidget(button);

    button2 = new QPushButton(this);
    button2->setText(tr("Click To Suspend"));
    connect(button2,SIGNAL(clicked()),SLOT(togglePlay()));
    layout->addWidget(button2);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    buffer = new char[BUFFER_SIZE];

    gen = new Generator(this);

    pullMode = true;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),SLOT(writeMore()));

    gen->start();

    settings.setFrequency(SYSTEM_FREQ);
    settings.setChannels(1);
    settings.setSampleSize(16);
    settings.setCodec("audio/pcm");
    settings.setByteOrder(QAudioFormat::LittleEndian);
    settings.setSampleType(QAudioFormat::SignedInt);
    audioOutput = new QAudioOutput(settings,this);
    connect(audioOutput,SIGNAL(notify()),SLOT(status()));
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));

    audioOutput->start(gen);
}

AudioTest::~AudioTest()
{
    delete [] buffer;
}

void AudioTest::deviceChanged(int idx)
{
    timer->stop();
    gen->stop();
    audioOutput->stop();
    audioOutput->disconnect(this);
    delete audioOutput;

    device = deviceBox->itemData(idx).value<QAudioDeviceInfo>();
    audioOutput = new QAudioOutput(device,settings,this);
    connect(audioOutput,SIGNAL(notify()),SLOT(status()));
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    gen->start();
    audioOutput->start(gen);
}

void AudioTest::status()
{
    qWarning()<<"byteFree = "<<audioOutput->bytesFree()<<" bytes, elapsedUSecs = "<<audioOutput->elapsedUSecs()<<", processedUSecs = "<<audioOutput->processedUSecs();
}

void AudioTest::writeMore()
{
    if(!audioOutput)
        return;

    if(audioOutput->state() == QAudio::StoppedState)
        return;

    int    l;
    int    out;

    int chunks = audioOutput->bytesFree()/audioOutput->periodSize();
    while(chunks) {
       l = gen->read(buffer,audioOutput->periodSize());
       if(l > 0)
           out = output->write(buffer,l);
       if(l != audioOutput->periodSize())
	   break;
       chunks--;
    }
}

void AudioTest::toggle()
{
    // Change between pull and push modes

    timer->stop();
    audioOutput->stop();

    if (pullMode) {
        button->setText("Click for Pull Mode");
        output = audioOutput->start();
        pullMode = false;
        timer->start(20);
    } else {
        button->setText("Click for Push Mode");
        pullMode = true;
        audioOutput->start(gen);
    }
}

void AudioTest::togglePlay()
{
    // toggle suspend/resume
    if(audioOutput->state() == QAudio::SuspendedState) {
        qWarning()<<"status: Suspended, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::ActiveState) {
        qWarning()<<"status: Active, suspend()";
        audioOutput->suspend();
        button2->setText("Click To Resume");
    } else if (audioOutput->state() == QAudio::StoppedState) {
        qWarning()<<"status: Stopped, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::IdleState) {
        qWarning()<<"status: IdleState";
    }
}

void AudioTest::state(QAudio::State state)
{
    qWarning()<<" state="<<state;
}
