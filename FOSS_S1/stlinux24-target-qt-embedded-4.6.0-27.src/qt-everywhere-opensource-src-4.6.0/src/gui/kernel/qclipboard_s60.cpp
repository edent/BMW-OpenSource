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

#include "qclipboard.h"

#ifndef QT_NO_CLIPBOARD

#include "qapplication.h"
#include "qbitmap.h"
#include "qdatetime.h"
#include "qbuffer.h"
#include "qwidget.h"
#include "qevent.h"
#include "private/qcore_symbian_p.h"
#include <QtDebug>

// Symbian's clipboard
#include <baclipb.h>
QT_BEGIN_NAMESPACE

//###  Mime Type mapping to UIDs

const TUid KQtCbDataStream = {0x2001B2DD};


class QClipboardData
{
public:
    QClipboardData();
   ~QClipboardData();

    void setSource(QMimeData* s)
    {
        if (s == src)
            return;
        delete src;
        src = s;
    }
    QMimeData* source()
    { return src; }
    bool connected()
    { return connection; }
    void clear();

private:
    QMimeData* src;
    bool connection;
};

QClipboardData::QClipboardData():src(0),connection(true)
{
    clear();
}

QClipboardData::~QClipboardData()
{
    connection = false;
    delete src;
}

void QClipboardData::clear()
{
    QMimeData* newSrc = new QMimeData;
    delete src;
    src = newSrc;
}

static QClipboardData *internalCbData = 0;

static void cleanupClipboardData()
{
    delete internalCbData;
    internalCbData = 0;
}

static QClipboardData *clipboardData()
{
    if (internalCbData == 0) {
        internalCbData = new QClipboardData;
        if (internalCbData)
        {
            if (!internalCbData->connected())
            {
                delete internalCbData;
                internalCbData = 0;
            }
            else
            {
                qAddPostRoutine(cleanupClipboardData);
            }
        }
    }
    return internalCbData;
}

void writeToStreamLX(const QMimeData* aData, RWriteStream& aStream)
{
    // This function both leaves and throws exceptions. There must be no destructor
    // dependencies between cleanup styles, and no cleanup stack dependencies on stacked objects.
    QStringList headers = aData->formats();
    aStream << TCardinality(headers.count());
    for (QStringList::const_iterator iter= headers.constBegin();iter != headers.constEnd();iter++)
    {
        HBufC* stringData = TPtrC(reinterpret_cast<const TUint16*>((*iter).utf16())).AllocLC();
        QByteArray ba = aData->data((*iter));
        qDebug() << "copy to clipboard mime: " << *iter << " data: " << ba;
        // mime type
        aStream << TCardinality(stringData->Size());
        aStream << *(stringData);
        // mime data
        aStream << TCardinality(ba.size());
        aStream.WriteL(reinterpret_cast<const uchar*>(ba.constData()),ba.size());
        CleanupStack::PopAndDestroy(stringData);
    }
}

void readFromStreamLX(QMimeData* aData,RReadStream& aStream)
{
    // This function both leaves and throws exceptions. There must be no destructor
    // dependencies between cleanup styles, and no cleanup stack dependencies on stacked objects.
    TCardinality mimeTypeCount;
    aStream >> mimeTypeCount;
    for (int i = 0; i< mimeTypeCount;i++)
    {
        // mime type
        TCardinality mimeTypeSize;
        aStream >> mimeTypeSize;
        HBufC* mimeTypeBuf = HBufC::NewLC(aStream,mimeTypeSize);
        QString mimeType = QString::fromUtf16(mimeTypeBuf->Des().Ptr(),mimeTypeBuf->Length());
        CleanupStack::PopAndDestroy(mimeTypeBuf);
        // mime data
        TCardinality dataSize;
        aStream >> dataSize;
        QByteArray ba;
        ba.reserve(dataSize);
        aStream.ReadL(reinterpret_cast<uchar*>(ba.data_ptr()->data),dataSize);
        ba.data_ptr()->size = dataSize;
        qDebug() << "paste from clipboard mime: " << mimeType << " data: " << ba;
        aData->setData(mimeType,ba);
    }
}


/*****************************************************************************
  QClipboard member functions
 *****************************************************************************/

void QClipboard::clear(Mode mode)
{
    setText(QString(), mode);
}
const QMimeData* QClipboard::mimeData(Mode mode) const
{
    if (mode != Clipboard) return 0;
    QClipboardData *d = clipboardData();
    if (d)
    {
        TRAPD(err,{
            RFs fs = qt_s60GetRFs();
            CClipboard* cb = CClipboard::NewForReadingLC(fs);
            Q_ASSERT(cb);
            RStoreReadStream stream;
            TStreamId stid = (cb->StreamDictionary()).At(KQtCbDataStream);
            stream.OpenLC(cb->Store(),stid);
            QT_TRYCATCH_LEAVING(readFromStreamLX(d->source(),stream));
            CleanupStack::PopAndDestroy(2,cb);
            return d->source();
        });
        if (err != KErrNone){
            qDebug()<< "clipboard is empty/err: " << err;
        }

    }
    return 0;
}


void QClipboard::setMimeData(QMimeData* src, Mode mode)
{
    if (mode != Clipboard) return;
    QClipboardData *d = clipboardData();
    if (d)
    {
        TRAPD(err,{
            RFs fs = qt_s60GetRFs();
            CClipboard* cb = CClipboard::NewForWritingLC(fs);
            RStoreWriteStream  stream;
            TStreamId stid = stream.CreateLC(cb->Store());
            QT_TRYCATCH_LEAVING(writeToStreamLX(src,stream));
            d->setSource(src);
            stream.CommitL();
            (cb->StreamDictionary()).AssignL(KQtCbDataStream,stid);
            cb->CommitL();
            CleanupStack::PopAndDestroy(2,cb);
        });
        if (err != KErrNone){
            qDebug()<< "clipboard write err :" << err;
        }
    }
    emitChanged(QClipboard::Clipboard);
}

bool QClipboard::supportsMode(Mode mode) const
{
    return (mode == Clipboard);
}

bool QClipboard::ownsMode(Mode mode) const
{
    if (mode == Clipboard)
        qWarning("QClipboard::ownsClipboard: UNIMPLEMENTED!");
    return false;
}

bool QClipboard::event(QEvent * /* e */)
{
    return true;
}

void QClipboard::connectNotify( const char * )
{
}

void QClipboard::ownerDestroyed()
{
}
QT_END_NAMESPACE
#endif // QT_NO_CLIPBOARD
