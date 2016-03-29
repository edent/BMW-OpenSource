/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qnetworkreplyimpl_p.h"
#include "qnetworkaccessbackend_p.h"
#include "qnetworkcookie.h"
#include "qabstractnetworkcache.h"
#include "QtCore/qcoreapplication.h"
#include "QtCore/qdatetime.h"
#include "QtNetwork/qsslconfiguration.h"
#include "qnetworkaccesshttpbackend_p.h"

#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

inline QNetworkReplyImplPrivate::QNetworkReplyImplPrivate()
    : backend(0), outgoingData(0), outgoingDataBuffer(0),
      copyDevice(0),
      cacheEnabled(false), cacheSaveDevice(0),
      notificationHandlingPaused(false),
      bytesDownloaded(0), lastBytesDownloaded(-1), bytesUploaded(-1),
      httpStatusCode(0),
      state(Idle)
{
}

void QNetworkReplyImplPrivate::_q_startOperation()
{
    // ensure this function is only being called once
    if (state == Working) {
        qDebug("QNetworkReplyImpl::_q_startOperation was called more than once");
        return;
    }
    state = Working;

    if (!backend) {
        error(QNetworkReplyImpl::ProtocolUnknownError,
              QCoreApplication::translate("QNetworkReply", "Protocol \"%1\" is unknown").arg(url.scheme())); // not really true!;
        finished();
        return;
    }

    backend->open();
    if (state != Finished) {
        if (operation == QNetworkAccessManager::GetOperation)
            pendingNotifications.append(NotifyDownstreamReadyWrite);

        handleNotifications();
    }
}

void QNetworkReplyImplPrivate::_q_copyReadyRead()
{
    Q_Q(QNetworkReplyImpl);
    if (state != Working)
        return;
    if (!copyDevice || !q->isOpen())
        return;

    forever {
        qint64 bytesToRead = nextDownstreamBlockSize();
        if (bytesToRead == 0)
            // we'll be called again, eventually
            break;

        bytesToRead = qBound<qint64>(1, bytesToRead, copyDevice->bytesAvailable());
        QByteArray byteData;
        byteData.resize(bytesToRead);
        qint64 bytesActuallyRead = copyDevice->read(byteData.data(), byteData.size());
        if (bytesActuallyRead == -1) {
            byteData.clear();
            backendNotify(NotifyCopyFinished);
            break;
        }

        byteData.resize(bytesActuallyRead);
        readBuffer.append(byteData);

        if (!copyDevice->isSequential() && copyDevice->atEnd()) {
            backendNotify(NotifyCopyFinished);
            bytesDownloaded += bytesActuallyRead;
            break;
        }

        bytesDownloaded += bytesActuallyRead;
    }

    if (bytesDownloaded == lastBytesDownloaded) {
        // we didn't read anything
        return;
    }

    lastBytesDownloaded = bytesDownloaded;
    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    pauseNotificationHandling();
    emit q->downloadProgress(bytesDownloaded,
                             totalSize.isNull() ? Q_INT64_C(-1) : totalSize.toLongLong());
    emit q->readyRead();
    resumeNotificationHandling();
}

void QNetworkReplyImplPrivate::_q_copyReadChannelFinished()
{
    _q_copyReadyRead();
}

void QNetworkReplyImplPrivate::_q_bufferOutgoingDataFinished()
{
    Q_Q(QNetworkReplyImpl);

    // make sure this is only called once, ever.
    //_q_bufferOutgoingData may call it or the readChannelFinished emission
    if (state != Buffering)
        return;

    // disconnect signals
    QObject::disconnect(outgoingData, SIGNAL(readyRead()), q, SLOT(_q_bufferOutgoingData()));
    QObject::disconnect(outgoingData, SIGNAL(readChannelFinished()), q, SLOT(_q_bufferOutgoingDataFinished()));

    // finally, start the request
    QMetaObject::invokeMethod(q, "_q_startOperation", Qt::QueuedConnection);
}

void QNetworkReplyImplPrivate::_q_bufferOutgoingData()
{
    Q_Q(QNetworkReplyImpl);

    if (!outgoingDataBuffer) {
        // first call, create our buffer
        outgoingDataBuffer = new QRingBuffer();

        QObject::connect(outgoingData, SIGNAL(readyRead()), q, SLOT(_q_bufferOutgoingData()));
        QObject::connect(outgoingData, SIGNAL(readChannelFinished()), q, SLOT(_q_bufferOutgoingDataFinished()));
    }

    qint64 bytesBuffered = 0;
    qint64 bytesToBuffer = 0;

    // read data into our buffer
    forever {
        bytesToBuffer = outgoingData->bytesAvailable();
        // unknown? just try 2 kB, this also ensures we always try to read the EOF
        if (bytesToBuffer <= 0)
            bytesToBuffer = 2*1024;

        char *dst = outgoingDataBuffer->reserve(bytesToBuffer);
        bytesBuffered = outgoingData->read(dst, bytesToBuffer);

        if (bytesBuffered == -1) {
            // EOF has been reached.
            outgoingDataBuffer->chop(bytesToBuffer);

            _q_bufferOutgoingDataFinished();
            break;
        } else if (bytesBuffered == 0) {
            // nothing read right now, just wait until we get called again
            outgoingDataBuffer->chop(bytesToBuffer);

            break;
        } else {
            // don't break, try to read() again
            outgoingDataBuffer->chop(bytesToBuffer - bytesBuffered);
        }
    }
}


void QNetworkReplyImplPrivate::setup(QNetworkAccessManager::Operation op, const QNetworkRequest &req,
                                     QIODevice *data)
{
    Q_Q(QNetworkReplyImpl);

    outgoingData = data;
    request = req;
    url = request.url();
    operation = op;

    if (outgoingData && backend) {
        // there is data to be uploaded, e.g. HTTP POST.

        if (!backend->needsResetableUploadData() || !outgoingData->isSequential()) {
            // backend does not need upload buffering or
            // fixed size non-sequential
            // just start the operation
            QMetaObject::invokeMethod(q, "_q_startOperation", Qt::QueuedConnection);
        } else {
            bool bufferingDisallowed =
                    req.attribute(QNetworkRequest::DoNotBufferUploadDataAttribute,
                                             false).toBool();

            if (bufferingDisallowed) {
                // if a valid content-length header for the request was supplied, we can disable buffering
                // if not, we will buffer anyway
                if (req.header(QNetworkRequest::ContentLengthHeader).isValid()) {
                    QMetaObject::invokeMethod(q, "_q_startOperation", Qt::QueuedConnection);
                } else {
                    state = Buffering;
                    QMetaObject::invokeMethod(q, "_q_bufferOutgoingData", Qt::QueuedConnection);
                }
            } else {
                // _q_startOperation will be called when the buffering has finished.
                state = Buffering;
                QMetaObject::invokeMethod(q, "_q_bufferOutgoingData", Qt::QueuedConnection);
            }
        }
    } else {
        // No outgoing data (e.g. HTTP GET request)
        // or no backend
        // if no backend, _q_startOperation will handle the error of this
        QMetaObject::invokeMethod(q, "_q_startOperation", Qt::QueuedConnection);
    }

    q->QIODevice::open(QIODevice::ReadOnly);
}

void QNetworkReplyImplPrivate::backendNotify(InternalNotifications notification)
{
    Q_Q(QNetworkReplyImpl);
    if (!pendingNotifications.contains(notification))
        pendingNotifications.enqueue(notification);

    if (pendingNotifications.size() == 1)
        QCoreApplication::postEvent(q, new QEvent(QEvent::NetworkReplyUpdated));
}

void QNetworkReplyImplPrivate::handleNotifications()
{
    if (notificationHandlingPaused)
        return;

    NotificationQueue current = pendingNotifications;
    pendingNotifications.clear();

    if (state != Working)
        return;

    while (!current.isEmpty()) {
        InternalNotifications notification = current.dequeue();
        switch (notification) {
        case NotifyDownstreamReadyWrite:
            if (copyDevice)
                _q_copyReadyRead();
            else
                backend->downstreamReadyWrite();
            break;

        case NotifyCloseDownstreamChannel:
            backend->closeDownstreamChannel();
            break;

        case NotifyCopyFinished: {
            QIODevice *dev = copyDevice;
            copyDevice = 0;
            backend->copyFinished(dev);
            break;
        }
        }
    }
}

// Do not handle the notifications while we are emitting downloadProgress
// or readyRead
void QNetworkReplyImplPrivate::pauseNotificationHandling()
{
    notificationHandlingPaused = true;
}

// Resume notification handling
void QNetworkReplyImplPrivate::resumeNotificationHandling()
{
    Q_Q(QNetworkReplyImpl);
    notificationHandlingPaused = false;
    if (pendingNotifications.size() >= 1)
        QCoreApplication::postEvent(q, new QEvent(QEvent::NetworkReplyUpdated));
}

QAbstractNetworkCache *QNetworkReplyImplPrivate::networkCache() const
{
    if (!backend)
        return 0;
    return backend->networkCache();
}

void QNetworkReplyImplPrivate::createCache()
{
    // check if we can save and if we're allowed to
    if (!networkCache()
        || !request.attribute(QNetworkRequest::CacheSaveControlAttribute, true).toBool()
        || request.attribute(QNetworkRequest::CacheLoadControlAttribute,
                             QNetworkRequest::PreferNetwork).toInt()
            == QNetworkRequest::AlwaysNetwork)
        return;
    cacheEnabled = true;
}

bool QNetworkReplyImplPrivate::isCachingEnabled() const
{
    return (cacheEnabled && networkCache() != 0);
}

void QNetworkReplyImplPrivate::setCachingEnabled(bool enable)
{
    if (!enable && !cacheEnabled)
        return;                 // nothing to do
    if (enable && cacheEnabled)
        return;                 // nothing to do either!

    if (enable) {
        if (bytesDownloaded) {
            // refuse to enable in this case
            qCritical("QNetworkReplyImpl: backend error: caching was enabled after some bytes had been written");
            return;
        }

        createCache();
    } else {
        // someone told us to turn on, then back off?
        // ok... but you should make up your mind
        qDebug("QNetworkReplyImpl: setCachingEnabled(true) called after setCachingEnabled(false) -- "
               "backend %s probably needs to be fixed",
               backend->metaObject()->className());
        networkCache()->remove(url);
        cacheSaveDevice = 0;
        cacheEnabled = false;
    }
}

void QNetworkReplyImplPrivate::completeCacheSave()
{
    if (cacheEnabled && errorCode != QNetworkReplyImpl::NoError) {
        networkCache()->remove(url);
    } else if (cacheEnabled && cacheSaveDevice) {
        networkCache()->insert(cacheSaveDevice);
    }
    cacheSaveDevice = 0;
    cacheEnabled = false;
}

void QNetworkReplyImplPrivate::emitUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    Q_Q(QNetworkReplyImpl);
    bytesUploaded = bytesSent;
    pauseNotificationHandling();
    emit q->uploadProgress(bytesSent, bytesTotal);
    resumeNotificationHandling();
}


qint64 QNetworkReplyImplPrivate::nextDownstreamBlockSize() const
{
    enum { DesiredBufferSize = 32 * 1024 };
    if (readBufferMaxSize == 0)
        return DesiredBufferSize;

    return qMax<qint64>(0, readBufferMaxSize - readBuffer.byteAmount());
}

// we received downstream data and send this to the cache
// and to our readBuffer (which in turn gets read by the user of QNetworkReply)
void QNetworkReplyImplPrivate::appendDownstreamData(QByteDataBuffer &data)
{
    Q_Q(QNetworkReplyImpl);
    if (!q->isOpen())
        return;

    if (cacheEnabled && !cacheSaveDevice) {
        // save the meta data
        QNetworkCacheMetaData metaData;
        metaData.setUrl(url);
        metaData = backend->fetchCacheMetaData(metaData);

        // save the redirect request also in the cache
        QVariant redirectionTarget = q->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirectionTarget.isValid()) {
            QNetworkCacheMetaData::AttributesMap attributes = metaData.attributes();
            attributes.insert(QNetworkRequest::RedirectionTargetAttribute, redirectionTarget);
            metaData.setAttributes(attributes);
        }

        cacheSaveDevice = networkCache()->prepare(metaData);

        if (!cacheSaveDevice || (cacheSaveDevice && !cacheSaveDevice->isOpen())) {
            if (cacheSaveDevice && !cacheSaveDevice->isOpen())
                qCritical("QNetworkReplyImpl: network cache returned a device that is not open -- "
                      "class %s probably needs to be fixed",
                      networkCache()->metaObject()->className());

            networkCache()->remove(url);
            cacheSaveDevice = 0;
            cacheEnabled = false;
        }
    }

    qint64 bytesWritten = 0;
    for (int i = 0; i < data.bufferCount(); i++) {
        QByteArray item = data[i];

        if (cacheSaveDevice)
            cacheSaveDevice->write(item.constData(), item.size());
        readBuffer.append(item);

        bytesWritten += item.size();
    }
    data.clear();

    bytesDownloaded += bytesWritten;
    lastBytesDownloaded = bytesDownloaded;

    QPointer<QNetworkReplyImpl> qq = q;

    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    pauseNotificationHandling();
    emit q->downloadProgress(bytesDownloaded,
                             totalSize.isNull() ? Q_INT64_C(-1) : totalSize.toLongLong());
    // important: At the point of this readyRead(), the data parameter list must be empty,
    // else implicit sharing will trigger memcpy when the user is reading data!
    emit q->readyRead();

    // hopefully we haven't been deleted here
    if (!qq.isNull()) {
        resumeNotificationHandling();
        // do we still have room in the buffer?
        if (nextDownstreamBlockSize() > 0)
            backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);
    }
}

// this is used when it was fetched from the cache, right?
void QNetworkReplyImplPrivate::appendDownstreamData(QIODevice *data)
{
    Q_Q(QNetworkReplyImpl);
    if (!q->isOpen())
        return;

    // read until EOF from data
    if (copyDevice) {
        qCritical("QNetworkReplyImpl: copy from QIODevice already in progress -- "
                  "backend probly needs to be fixed");
        return;
    }

    copyDevice = data;
    q->connect(copyDevice, SIGNAL(readyRead()), SLOT(_q_copyReadyRead()));
    q->connect(copyDevice, SIGNAL(readChannelFinished()), SLOT(_q_copyReadChannelFinished()));

    // start the copy:
    _q_copyReadyRead();
}

void QNetworkReplyImplPrivate::finished()
{
    Q_Q(QNetworkReplyImpl);
    if (state == Finished || state == Aborted)
        return;

    state = Finished;
    pendingNotifications.clear();

    pauseNotificationHandling();
    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    if (totalSize.isNull() || totalSize == -1) {
        emit q->downloadProgress(bytesDownloaded, bytesDownloaded);
    }

    if (bytesUploaded == -1 && (outgoingData || outgoingDataBuffer))
        emit q->uploadProgress(0, 0);
    resumeNotificationHandling();

    completeCacheSave();

    // note: might not be a good idea, since users could decide to delete us
    // which would delete the backend too...
    // maybe we should protect the backend
    pauseNotificationHandling();
    emit q->readChannelFinished();
    emit q->finished();
    resumeNotificationHandling();
}

void QNetworkReplyImplPrivate::error(QNetworkReplyImpl::NetworkError code, const QString &errorMessage)
{
    Q_Q(QNetworkReplyImpl);

    errorCode = code;
    q->setErrorString(errorMessage);

    // note: might not be a good idea, since users could decide to delete us
    // which would delete the backend too...
    // maybe we should protect the backend
    emit q->error(code);
}

void QNetworkReplyImplPrivate::metaDataChanged()
{
    Q_Q(QNetworkReplyImpl);
    // do we have cookies?
    if (cookedHeaders.contains(QNetworkRequest::SetCookieHeader) && !manager.isNull()) {
        QList<QNetworkCookie> cookies =
            qvariant_cast<QList<QNetworkCookie> >(cookedHeaders.value(QNetworkRequest::SetCookieHeader));
        QNetworkCookieJar *jar = manager->cookieJar();
        if (jar)
            jar->setCookiesFromUrl(cookies, url);
    }
    emit q->metaDataChanged();
}

void QNetworkReplyImplPrivate::redirectionRequested(const QUrl &target)
{
    attributes.insert(QNetworkRequest::RedirectionTargetAttribute, target);
}

void QNetworkReplyImplPrivate::sslErrors(const QList<QSslError> &errors)
{
#ifndef QT_NO_OPENSSL
    Q_Q(QNetworkReplyImpl);
    emit q->sslErrors(errors);
#else
    Q_UNUSED(errors);
#endif
}

bool QNetworkReplyImplPrivate::isFinished() const
{
    return (state == Finished || state == Aborted);
}

QNetworkReplyImpl::QNetworkReplyImpl(QObject *parent)
    : QNetworkReply(*new QNetworkReplyImplPrivate, parent)
{
}

QNetworkReplyImpl::~QNetworkReplyImpl()
{
    Q_D(QNetworkReplyImpl);
    if (d->isCachingEnabled())
        d->networkCache()->remove(url());
    if (d->outgoingDataBuffer)
        delete d->outgoingDataBuffer;
}

void QNetworkReplyImpl::abort()
{
    Q_D(QNetworkReplyImpl);
    if (d->state == QNetworkReplyImplPrivate::Aborted)
        return;

    // stop both upload and download
    if (d->outgoingData)
        disconnect(d->outgoingData, 0, this, 0);
    if (d->copyDevice)
        disconnect(d->copyDevice, 0, this, 0);

    QNetworkReply::close();

    if (d->state != QNetworkReplyImplPrivate::Finished) {
        // emit signals
        d->error(OperationCanceledError, tr("Operation canceled"));
        d->finished();
    }
    d->state = QNetworkReplyImplPrivate::Aborted;

    // finished may access the backend
    if (d->backend) {
        d->backend->deleteLater();
        d->backend = 0;
    }
}

void QNetworkReplyImpl::close()
{
    Q_D(QNetworkReplyImpl);
    if (d->state == QNetworkReplyImplPrivate::Aborted ||
        d->state == QNetworkReplyImplPrivate::Finished)
        return;

    // stop the download
    if (d->backend)
        d->backend->closeDownstreamChannel();
    if (d->copyDevice)
        disconnect(d->copyDevice, 0, this, 0);

    QNetworkReply::close();

    // emit signals
    d->error(OperationCanceledError, tr("Operation canceled"));
    d->finished();
}

/*!
    Returns the number of bytes available for reading with
    QIODevice::read(). The number of bytes available may grow until
    the finished() signal is emitted.
*/
qint64 QNetworkReplyImpl::bytesAvailable() const
{
    return QNetworkReply::bytesAvailable() + d_func()->readBuffer.byteAmount();
}

void QNetworkReplyImpl::setReadBufferSize(qint64 size)
{
    Q_D(QNetworkReplyImpl);
    if (size > d->readBufferMaxSize &&
        size > d->readBuffer.byteAmount())
        d->backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);

    QNetworkReply::setReadBufferSize(size);
}

#ifndef QT_NO_OPENSSL
QSslConfiguration QNetworkReplyImpl::sslConfigurationImplementation() const
{
    Q_D(const QNetworkReplyImpl);
    QSslConfiguration config;
    if (d->backend)
        d->backend->fetchSslConfiguration(config);
    return config;
}

void QNetworkReplyImpl::setSslConfigurationImplementation(const QSslConfiguration &config)
{
    Q_D(QNetworkReplyImpl);
    if (d->backend && !config.isNull())
        d->backend->setSslConfiguration(config);
}

void QNetworkReplyImpl::ignoreSslErrors()
{
    Q_D(QNetworkReplyImpl);
    if (d->backend)
        d->backend->ignoreSslErrors();
}

void QNetworkReplyImpl::ignoreSslErrorsImplementation(const QList<QSslError> &errors)
{
    Q_D(QNetworkReplyImpl);
    if (d->backend)
        d->backend->ignoreSslErrors(errors);
}
#endif  // QT_NO_OPENSSL

/*!
    \internal
*/
qint64 QNetworkReplyImpl::readData(char *data, qint64 maxlen)
{
    Q_D(QNetworkReplyImpl);
    if (d->readBuffer.isEmpty())
        return d->state == QNetworkReplyImplPrivate::Finished ? -1 : 0;

    d->backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);
    if (maxlen == 1) {
        // optimization for getChar()
        *data = d->readBuffer.getChar();
        return 1;
    }

    maxlen = qMin<qint64>(maxlen, d->readBuffer.byteAmount());
    return d->readBuffer.read(data, maxlen);
}

/*!
   \internal Reimplemented for internal purposes
*/
bool QNetworkReplyImpl::event(QEvent *e)
{
    if (e->type() == QEvent::NetworkReplyUpdated) {
        d_func()->handleNotifications();
        return true;
    }

    return QObject::event(e);
}

QT_END_NAMESPACE

#include "moc_qnetworkreplyimpl_p.cpp"

