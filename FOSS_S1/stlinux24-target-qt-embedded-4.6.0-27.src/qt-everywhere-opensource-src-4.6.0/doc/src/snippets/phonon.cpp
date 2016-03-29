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
    

Window::Window()
{
    {
//![0]
    Phonon::MediaObject *music =
        Phonon::createPlayer(Phonon::MusicCategory,
                             Phonon::MediaSource("/path/mysong.wav"));
    music->play();
//![0]
    }

    {
    QWidget *parentWidget = new QWidget;
    QUrl url("Myfancymusic");
//![1]
    Phonon::VideoPlayer *player =
        new Phonon::VideoPlayer(Phonon::VideoCategory, parentWidget);
    player->play(url);
//![1]
    }

    {
//![2]
    Phonon::MediaObject *mediaObject = new Phonon::MediaObject(this);
    mediaObject->setCurrentSource(Phonon::MediaSource("/mymusic/barbiegirl.wav"));
    Phonon::AudioOutput *audioOutput =
        new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::Path path = Phonon::createPath(mediaObject, audioOutput);
//![2]
    
//![3]
    Phonon::Effect *effect =
        new Phonon::Effect(
            Phonon::BackendCapabilities::availableAudioEffects()[0], this);
    path.insertEffect(effect);
//![3]    
    }

    {
//![4]
    Phonon::MediaObject *mediaObject = new Phonon::MediaObject(this);

    Phonon::VideoWidget *videoWidget = new Phonon::VideoWidget(this);
    Phonon::createPath(mediaObject, videoWidget);

    Phonon::AudioOutput *audioOutput =
        new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(mediaObject, audioOutput);
//![4]
//![5]
    mediaObject->play();
//![5]
    }
}
