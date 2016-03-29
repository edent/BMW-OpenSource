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

#include "videoplayer.h"

#include "videowidget.h"

#include <QtMultimedia>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , surface(0)
    , playButton(0)
    , positionSlider(0)
{
    connect(&movie, SIGNAL(stateChanged(QMovie::MovieState)),
            this, SLOT(movieStateChanged(QMovie::MovieState)));
    connect(&movie, SIGNAL(frameChanged(int)),
            this, SLOT(frameChanged(int)));

    VideoWidget *videoWidget = new VideoWidget;
    surface = videoWidget->videoSurface();

    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playButton = new QPushButton;
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    connect(&movie, SIGNAL(frameChanged(int)),
            positionSlider, SLOT(setValue(int)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addLayout(controlLayout);

    setLayout(layout);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"));

    if (!fileName.isEmpty()) {
        surface->stop();

        movie.setFileName(fileName);

        playButton->setEnabled(true);
        positionSlider->setMaximum(movie.frameCount());

        movie.jumpToFrame(0);
    }
}

void VideoPlayer::play()
{
    switch(movie.state()) {
    case QMovie::NotRunning:
        movie.start();
        break;
    case QMovie::Paused:
        movie.setPaused(false);
        break;
    case QMovie::Running:
        movie.setPaused(true);
        break;
    }
}

void VideoPlayer::movieStateChanged(QMovie::MovieState state)
{
    switch(state) {
    case QMovie::NotRunning:
    case QMovie::Paused:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMovie::Running:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    }
}

void VideoPlayer::frameChanged(int frame)
{
    if (!presentImage(movie.currentImage())) {
        movie.stop();
        playButton->setEnabled(false);
        positionSlider->setMaximum(0);
    } else {
        positionSlider->setValue(frame);
    }
}

void VideoPlayer::setPosition(int frame)
{
    movie.jumpToFrame(frame);
}

bool VideoPlayer::presentImage(const QImage &image)
{
    QVideoFrame frame(image);

    if (!frame.isValid())
        return false;

    QVideoSurfaceFormat currentFormat = surface->surfaceFormat();

    if (frame.pixelFormat() != currentFormat.pixelFormat()
            || frame.size() != currentFormat.frameSize()) {
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());

        if (!surface->start(format))
            return false;
    }

    if (!surface->present(frame)) {
        surface->stop();

        return false;
    } else {
        return true;
    }
}
