/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtGui>

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QAudioInput>

class Window2 : public QWidget
{
    Q_OBJECT

public slots:
//![0]
    void stateChanged(QAudio::State newState)
    {
        switch(newState) {
            case QAudio::StopState:
            if (input->error() != QAudio::NoError) {
                // Error handling
            } else {

            }
            break;
//![0]
        default:
            ;
        }
    }

private:
    QAudioInput *input;

};

class Window : public QWidget
{
    Q_OBJECT

public:
    Window()
    {
        output = new QAudioOutput;
        connect(output, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(stateChanged(QAudio::State)));
    }

private:
    void setupFormat()
    {
//![1]
        QAudioFormat format;
        format.setFrequency(44100);
//![1]
        format.setChannels(2);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
//![2]
        format.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

        if (!info.isFormatSupported(format))
            format = info.nearestFormat(format);
//![2]
    }

public slots:
//![3]
    void stateChanged(QAudio::State newState)
    {
        switch (newState) {
            case QAudio::StopState:
                if (output->error() != QAudio::NoError) {
                    // Do your error handlin
                } else {
                    // Normal stop
                }
                break;
//![3]

            // Handle 
            case QAudio::ActiveState:
                // Handle active state...
                break;
            break;
        default:
            ;
        }
    }

private:
    QAudioOutput *output;
};

int main(int argv, char **args)
{
    QApplication app(argv, args);

    Window window;
    window.show();

    return app.exec();        
}


#include "main.moc"

