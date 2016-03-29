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

#include "qmemoryvideobuffer_p.h"

#include <private/qabstractvideobuffer_p.h>
#include <qbytearray.h>

QT_BEGIN_NAMESPACE

class QMemoryVideoBufferPrivate : public QAbstractVideoBufferPrivate
{
public:
    QMemoryVideoBufferPrivate()
        : bytesPerLine(0)
        , mapMode(QAbstractVideoBuffer::NotMapped)
    {
    }

    int bytesPerLine;
    QAbstractVideoBuffer::MapMode mapMode;
    QByteArray data;
};

/*!
    \class QMemoryVideoBuffer
    \brief The QMemoryVideoBuffer class provides a system memory allocated video data buffer.
    \internal

    QMemoryVideoBuffer is the default video buffer for allocating system memory.  It may be used to
    allocate memory for a QVideoFrame without implementing your own QAbstractVideoBuffer.
*/

/*!
    Constructs a video buffer with an image stride of \a bytesPerLine from a byte \a array.
*/
QMemoryVideoBuffer::QMemoryVideoBuffer(const QByteArray &array, int bytesPerLine)
    : QAbstractVideoBuffer(*new QMemoryVideoBufferPrivate, NoHandle)
{
    Q_D(QMemoryVideoBuffer);

    d->data = array;
    d->bytesPerLine = bytesPerLine;
}

/*!
    Destroys a system memory allocated video buffer.
*/
QMemoryVideoBuffer::~QMemoryVideoBuffer()
{
}

/*!
    \reimp
*/
QAbstractVideoBuffer::MapMode QMemoryVideoBuffer::mapMode() const
{
    return d_func()->mapMode;
}

/*!
    \reimp
*/
uchar *QMemoryVideoBuffer::map(MapMode mode, int *numBytes, int *bytesPerLine)
{
    Q_D(QMemoryVideoBuffer);

    if (d->mapMode == NotMapped && d->data.data() && mode != NotMapped) {
        d->mapMode = mode;

        if (numBytes)
            *numBytes = d->data.size();

        if (bytesPerLine)
            *bytesPerLine = d->bytesPerLine;

        return reinterpret_cast<uchar *>(d->data.data());
    } else {
        return 0;
    }
}

/*!
    \reimp
*/
void QMemoryVideoBuffer::unmap()
{
    d_func()->mapMode = NotMapped;
}

QT_END_NAMESPACE
