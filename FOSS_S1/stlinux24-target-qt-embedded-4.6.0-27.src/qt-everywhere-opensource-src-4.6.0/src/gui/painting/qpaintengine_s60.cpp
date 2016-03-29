/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include <private/qpaintengine_s60_p.h>
#include <private/qpixmap_s60_p.h>
#include <private/qt_s60_p.h>

QT_BEGIN_NAMESPACE

class QS60PaintEnginePrivate : public QRasterPaintEnginePrivate
{
public:
    QS60PaintEnginePrivate(QS60PaintEngine *engine) { Q_UNUSED(engine); }
};

QS60PaintEngine::QS60PaintEngine(QPaintDevice *device, QS60PixmapData *data)
    : QRasterPaintEngine(*(new QS60PaintEnginePrivate(this)), device), pixmapData(data)
{
}

bool QS60PaintEngine::begin(QPaintDevice *device)
{
    pixmapData->beginDataAccess();
    return QRasterPaintEngine::begin(device);
}

bool QS60PaintEngine::end()
{
    bool ret = QRasterPaintEngine::end();
    pixmapData->endDataAccess();
    return ret;
}

void QS60PaintEngine::drawPixmap(const QPointF &p, const QPixmap &pm)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawPixmap(p, pm);
    srcData->endDataAccess();
}

void QS60PaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawPixmap(r, pm, sr);
    srcData->endDataAccess();
}

void QS60PaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawTiledPixmap(r, pm, sr);
    srcData->endDataAccess();
}

void QS60PaintEngine::prepare(QImage *image)
{
    QRasterBuffer *buffer = d_func()->rasterBuffer.data();
    if (buffer)
        buffer->prepare(image);
}

QT_END_NAMESPACE
