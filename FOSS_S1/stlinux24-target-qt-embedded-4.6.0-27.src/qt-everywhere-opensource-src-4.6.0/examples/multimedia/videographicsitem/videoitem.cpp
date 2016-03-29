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

#include "videoitem.h"

#include <QtMultimedia>

VideoItem::VideoItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , imageFormat(QImage::Format_Invalid)
    , framePainted(false)
{
}

VideoItem::~VideoItem()
{
}

QRectF VideoItem::boundingRect() const
{
    return QRectF(QPointF(0,0), surfaceFormat().sizeHint());
}

void VideoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -boundingRect().height());
        }

        painter->drawImage(boundingRect(), QImage(
                currentFrame.bits(),
                imageSize.width(),
                imageSize.height(),
                imageFormat));

        painter->setTransform(oldTransform);

        framePainted = true;

        currentFrame.unmap();
    }
}

QList<QVideoFrame::PixelFormat> VideoItem::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoItem::start(const QVideoSurfaceFormat &format)
{
    if (isFormatSupported(format)) {
        imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
        imageSize = format.frameSize();
        framePainted = true;

        QAbstractVideoSurface::start(format);

        prepareGeometryChange();

        return true;
    } else {
        return false;
    }
}

void VideoItem::stop()
{
    currentFrame = QVideoFrame();
    framePainted = false;

    QAbstractVideoSurface::stop();
}

bool VideoItem::present(const QVideoFrame &frame)
{
    if (!framePainted) {
        if (!QAbstractVideoSurface::isActive())
            setError(StoppedError);

        return false;
    } else {
        currentFrame = frame;
        framePainted = false;

        update();

        return true;
    }
}
