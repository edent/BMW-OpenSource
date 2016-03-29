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

#include "qhttpnetworkconnection_p.h"
#include "qhttpnetworkconnectionchannel_p.h"
#include "private/qnoncontiguousbytedevice_p.h"

#include <qpair.h>
#include <qdebug.h>

#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslkey.h>
#    include <QtNetwork/qsslcipher.h>
#    include <QtNetwork/qsslconfiguration.h>
#endif

QT_BEGIN_NAMESPACE

// TODO: Put channel specific stuff here so it does not polute qhttpnetworkconnection.cpp

void QHttpNetworkConnectionChannel::init()
{
#ifndef QT_NO_OPENSSL
    if (connection->d_func()->encrypt)
        socket = new QSslSocket;
    else
        socket = new QTcpSocket;
#else
    socket = new QTcpSocket;
#endif

    // limit the socket read buffer size. we will read everything into
    // the QHttpNetworkReply anyway, so let's grow only that and not
    // here and there.
    socket->setReadBufferSize(64*1024);

    QObject::connect(socket, SIGNAL(bytesWritten(qint64)),
                     this, SLOT(_q_bytesWritten(qint64)),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(connected()),
                     this, SLOT(_q_connected()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(readyRead()),
                     this, SLOT(_q_readyRead()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(disconnected()),
                     this, SLOT(_q_disconnected()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(_q_error(QAbstractSocket::SocketError)),
                     Qt::DirectConnection);
#ifndef QT_NO_NETWORKPROXY
    QObject::connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
                     this, SLOT(_q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
                     Qt::DirectConnection);
#endif

#ifndef QT_NO_OPENSSL
    QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
    if (sslSocket) {
        // won't be a sslSocket if encrypt is false
        QObject::connect(sslSocket, SIGNAL(encrypted()),
                         this, SLOT(_q_encrypted()),
                         Qt::DirectConnection);
        QObject::connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)),
                         this, SLOT(_q_sslErrors(const QList<QSslError>&)),
                         Qt::DirectConnection);
        QObject::connect(sslSocket, SIGNAL(encryptedBytesWritten(qint64)),
                         this, SLOT(_q_encryptedBytesWritten(qint64)),
                         Qt::DirectConnection);
    }
#endif
}


void QHttpNetworkConnectionChannel::close()
{
    socket->blockSignals(true);
    socket->close();
    socket->blockSignals(false);
    state = QHttpNetworkConnectionChannel::IdleState;
}


bool QHttpNetworkConnectionChannel::sendRequest()
{
    switch (state) {
    case QHttpNetworkConnectionChannel::IdleState: { // write the header
        if (!ensureConnection()) {
            // wait for the connection (and encryption) to be done
            // sendRequest will be called again from either
            // _q_connected or _q_encrypted
            return false;
        }
        written = 0; // excluding the header
        bytesTotal = 0;
        if (reply) {
            reply->d_func()->clear();
            reply->d_func()->connection = connection;
            reply->d_func()->autoDecompress = request.d->autoDecompress;
            reply->d_func()->pipeliningUsed = false;
        }
        state = QHttpNetworkConnectionChannel::WritingState;
        pendingEncrypt = false;
        // if the url contains authentication parameters, use the new ones
        // both channels will use the new authentication parameters
        if (!request.url().userInfo().isEmpty()) {
            QUrl url = request.url();
            QAuthenticator &auth = authenticator;
            if (url.userName() != auth.user()
                || (!url.password().isEmpty() && url.password() != auth.password())) {
                auth.setUser(url.userName());
                auth.setPassword(url.password());
                connection->d_func()->copyCredentials(connection->d_func()->indexOf(socket), &auth, false);
            }
            // clear the userinfo,  since we use the same request for resending
            // userinfo in url can conflict with the one in the authenticator
            url.setUserInfo(QString());
            request.setUrl(url);
        }
        connection->d_func()->createAuthorization(socket, request);
#ifndef QT_NO_NETWORKPROXY
        QByteArray header = QHttpNetworkRequestPrivate::header(request,
            (connection->d_func()->networkProxy.type() != QNetworkProxy::NoProxy));
#else
        QByteArray header = QHttpNetworkRequestPrivate::header(request, false);
#endif
        socket->write(header);
        QNonContiguousByteDevice* uploadByteDevice = request.uploadByteDevice();
        if (uploadByteDevice) {
            // connect the signals so this function gets called again
            QObject::connect(uploadByteDevice, SIGNAL(readyRead()),this, SLOT(_q_uploadDataReadyRead()));

            bytesTotal = request.contentLength();
        } else {
            state = QHttpNetworkConnectionChannel::WaitingState;
            sendRequest();
            break;
        }
        // write the initial chunk together with the headers
        // fall through
    }
    case QHttpNetworkConnectionChannel::WritingState:
    {
        // write the data
        QNonContiguousByteDevice* uploadByteDevice = request.uploadByteDevice();
        if (!uploadByteDevice || bytesTotal == written) {
            if (uploadByteDevice)
                emit reply->dataSendProgress(written, bytesTotal);
            state = QHttpNetworkConnectionChannel::WaitingState; // now wait for response
            sendRequest();
            break;
        }

        // only feed the QTcpSocket buffer when there is less than 32 kB in it
        const qint64 socketBufferFill = 32*1024;
        const qint64 socketWriteMaxSize = 16*1024;


#ifndef QT_NO_OPENSSL
        QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
        // if it is really an ssl socket, check more than just bytesToWrite()
        while ((socket->bytesToWrite() + (sslSocket ? sslSocket->encryptedBytesToWrite() : 0))
                <= socketBufferFill && bytesTotal != written)
#else
        while (socket->bytesToWrite() <= socketBufferFill
               && bytesTotal != written)
#endif
        {
            // get pointer to upload data
            qint64 currentReadSize;
            qint64 desiredReadSize = qMin(socketWriteMaxSize, bytesTotal - written);
            const char *readPointer = uploadByteDevice->readPointer(desiredReadSize, currentReadSize);

            if (currentReadSize == -1) {
                // premature eof happened
                connection->d_func()->emitReplyError(socket, reply, QNetworkReply::UnknownNetworkError);
                return false;
                break;
            } else if (readPointer == 0 || currentReadSize == 0) {
                // nothing to read currently, break the loop
                break;
            } else {
                qint64 currentWriteSize = socket->write(readPointer, currentReadSize);
                if (currentWriteSize == -1 || currentWriteSize != currentReadSize) {
                    // socket broke down
                    connection->d_func()->emitReplyError(socket, reply, QNetworkReply::UnknownNetworkError);
                    return false;
                } else {
                    written += currentWriteSize;
                    uploadByteDevice->advanceReadPointer(currentWriteSize);

                    emit reply->dataSendProgress(written, bytesTotal);

                    if (written == bytesTotal) {
                        // make sure this function is called once again
                        state = QHttpNetworkConnectionChannel::WaitingState;
                        sendRequest();
                        break;
                    }
                }
            }
        }
        break;
    }

    case QHttpNetworkConnectionChannel::WaitingState:
    {
        QNonContiguousByteDevice* uploadByteDevice = request.uploadByteDevice();
        if (uploadByteDevice) {
            QObject::disconnect(uploadByteDevice, SIGNAL(readyRead()), this, SLOT(_q_uploadDataReadyRead()));
        }

        // HTTP pipelining
        connection->d_func()->fillPipeline(socket);
        socket->flush();

        // ensure we try to receive a reply in all cases, even if _q_readyRead_ hat not been called
        // this is needed if the sends an reply before we have finished sending the request. In that
        // case receiveReply had been called before but ignored the server reply
        receiveReply();
        break;
    }
    case QHttpNetworkConnectionChannel::ReadingState:
    case QHttpNetworkConnectionChannel::Wait4AuthState:
        // ignore _q_bytesWritten in these states
        // fall through
    default:
        break;
    }
    return true;
}


void QHttpNetworkConnectionChannel::receiveReply()
{
    Q_ASSERT(socket);

    qint64 bytes = 0;
    QAbstractSocket::SocketState socketState = socket->state();

    // connection might be closed to signal the end of data
    if (socketState == QAbstractSocket::UnconnectedState) {
        if (!socket->bytesAvailable()) {
            if (reply && reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                this->state = QHttpNetworkConnectionChannel::IdleState;
                allDone();
            } else {
                // try to reconnect/resend before sending an error.
                if (reconnectAttempts-- > 0) {
                    closeAndResendCurrentRequest();
                } else if (reply) {
                    reply->d_func()->errorString = connection->d_func()->errorDetail(QNetworkReply::RemoteHostClosedError, socket);
                    emit reply->finishedWithError(QNetworkReply::RemoteHostClosedError, reply->d_func()->errorString);
                    QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
                }
            }
        }
    }

    // read loop for the response
    while (socket->bytesAvailable()) {
        QHttpNetworkReplyPrivate::ReplyState state = reply ? reply->d_func()->state : QHttpNetworkReplyPrivate::AllDoneState;
        switch (state) {
        case QHttpNetworkReplyPrivate::NothingDoneState:
        case QHttpNetworkReplyPrivate::ReadingStatusState: {
            eatWhitespace();
            qint64 statusBytes = reply->d_func()->readStatus(socket);
            if (statusBytes == -1 && reconnectAttempts <= 0) {
                // too many errors reading/receiving/parsing the status, close the socket and emit error
                close();
                reply->d_func()->errorString = connection->d_func()->errorDetail(QNetworkReply::ProtocolFailure, socket);
                emit reply->finishedWithError(QNetworkReply::ProtocolFailure, reply->d_func()->errorString);
                QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
                break;
            } else if (statusBytes == -1) {
                reconnectAttempts--;
                reply->d_func()->clear();
                closeAndResendCurrentRequest();
                break;
            }
            bytes += statusBytes;
            lastStatus = reply->d_func()->statusCode;
            break;
        }
        case QHttpNetworkReplyPrivate::ReadingHeaderState:
            bytes += reply->d_func()->readHeader(socket);
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                if (reply->d_func()->isGzipped() && reply->d_func()->autoDecompress) {
                    // remove the Content-Length from header
                    reply->d_func()->removeAutoDecompressHeader();
                } else {
                    reply->d_func()->autoDecompress = false;
                }
                if (reply && reply->d_func()->statusCode == 100) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::ReadingStatusState;
                    break; // ignore
                }
                if (reply->d_func()->shouldEmitSignals())
                    emit reply->headerChanged();
                if (!reply->d_func()->expectContent()) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                    this->state = QHttpNetworkConnectionChannel::IdleState;
                    allDone();
                    return;
                }
            }
            break;
        case QHttpNetworkReplyPrivate::ReadingDataState: {
            if (!reply->d_func()->isChunked() && !reply->d_func()->autoDecompress
                && reply->d_func()->bodyLength > 0) {
                // bulk files like images should fulfill these properties and
                // we can therefore save on memory copying
                bytes = reply->d_func()->readBodyFast(socket, &reply->d_func()->responseData);
                reply->d_func()->totalProgress += bytes;
                if (reply->d_func()->shouldEmitSignals()) {
                    QPointer<QHttpNetworkReply> replyPointer = reply;
                    emit reply->readyRead();
                    // make sure that the reply is valid
                    if (replyPointer.isNull())
                        return;
                    emit reply->dataReadProgress(reply->d_func()->totalProgress, reply->d_func()->bodyLength);
                    // make sure that the reply is valid
                    if (replyPointer.isNull())
                        return;
                }
            }
            else
            {
                // use the traditional slower reading (for compressed encoding, chunked encoding,
                // no content-length etc)
                QByteDataBuffer byteDatas;
                bytes = reply->d_func()->readBody(socket, &byteDatas);
                if (bytes) {
                    if (reply->d_func()->autoDecompress)
                        reply->d_func()->appendCompressedReplyData(byteDatas);
                    else
                        reply->d_func()->appendUncompressedReplyData(byteDatas);

                    if (!reply->d_func()->autoDecompress) {
                        reply->d_func()->totalProgress += bytes;
                        if (reply->d_func()->shouldEmitSignals()) {
                            QPointer<QHttpNetworkReply> replyPointer = reply;
                            // important: At the point of this readyRead(), the byteDatas list must be empty,
                            // else implicit sharing will trigger memcpy when the user is reading data!
                            emit reply->readyRead();
                            // make sure that the reply is valid
                            if (replyPointer.isNull())
                                return;
                            emit reply->dataReadProgress(reply->d_func()->totalProgress, reply->d_func()->bodyLength);
                            // make sure that the reply is valid
                           if (replyPointer.isNull())
                                return;
                        }
                    }
#ifndef QT_NO_COMPRESS
                    else if (!expand(false)) { // expand a chunk if possible
                        return; // ### expand failed
                    }
#endif
                }
            }
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState)
                break;
            // everything done, fall through
            }
      case QHttpNetworkReplyPrivate::AllDoneState:
            this->state = QHttpNetworkConnectionChannel::IdleState;
            allDone();
            break;
        default:
            break;
        }
    }
}

bool QHttpNetworkConnectionChannel::ensureConnection()
{
    // make sure that this socket is in a connected state, if not initiate
    // connection to the host.
    if (socket->state() != QAbstractSocket::ConnectedState) {
        // connect to the host if not already connected.
        // resend this request after we receive the disconnected signal
        if (socket->state() == QAbstractSocket::ClosingState) {
            resendCurrent = true;
            return false;
        }
        state = QHttpNetworkConnectionChannel::ConnectingState;
        pendingEncrypt = connection->d_func()->encrypt;

        // reset state
        pipeliningSupported = PipeliningSupportUnknown;

        // This workaround is needed since we use QAuthenticator for NTLM authentication. The "phase == Done"
        // is the usual criteria for emitting authentication signals. The "phase" is set to "Done" when the
        // last header for Authorization is generated by the QAuthenticator. Basic & Digest logic does not
        // check the "phase" for generating the Authorization header. NTLM authentication is a two stage
        // process & needs the "phase". To make sure the QAuthenticator uses the current username/password
        // the phase is reset to Start.
        QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(authenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;
        priv = QAuthenticatorPrivate::getPrivate(proxyAuthenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;

        QString connectHost = connection->d_func()->hostName;
        qint16 connectPort = connection->d_func()->port;

#ifndef QT_NO_NETWORKPROXY
        // HTTPS always use transparent proxy.
        if (connection->d_func()->networkProxy.type() != QNetworkProxy::NoProxy && !connection->d_func()->encrypt) {
            connectHost = connection->d_func()->networkProxy.hostName();
            connectPort = connection->d_func()->networkProxy.port();
        }
#endif
        if (connection->d_func()->encrypt) {
#ifndef QT_NO_OPENSSL
            QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
            sslSocket->connectToHostEncrypted(connectHost, connectPort);
            if (ignoreAllSslErrors)
                sslSocket->ignoreSslErrors();
            sslSocket->ignoreSslErrors(ignoreSslErrorsList);
#else
            connection->d_func()->emitReplyError(socket, reply, QNetworkReply::ProtocolUnknownError);
#endif
        } else {
            socket->connectToHost(connectHost, connectPort);
        }
        return false;
    }
    return true;
}


#ifndef QT_NO_COMPRESS
bool QHttpNetworkConnectionChannel::expand(bool dataComplete)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    qint64 total = reply->d_func()->compressedData.size();
    if (total >= CHUNK || dataComplete) {
         // uncompress the data
        QByteArray content, inflated;
        content = reply->d_func()->compressedData;
        reply->d_func()->compressedData.clear();

        int ret = Z_OK;
        if (content.size())
            ret = reply->d_func()->gunzipBodyPartially(content, inflated);
        int retCheck = (dataComplete) ? Z_STREAM_END : Z_OK;
        if (ret >= retCheck) {
            if (inflated.size()) {
                reply->d_func()->totalProgress += inflated.size();
                reply->d_func()->appendUncompressedReplyData(inflated);
                if (reply->d_func()->shouldEmitSignals()) {
                    QPointer<QHttpNetworkReply> replyPointer = reply;
                    // important: At the point of this readyRead(), inflated must be cleared,
                    // else implicit sharing will trigger memcpy when the user is reading data!
                    emit reply->readyRead();
                    // make sure that the reply is valid
                    if (replyPointer.isNull())
                        return true;
                    emit reply->dataReadProgress(reply->d_func()->totalProgress, 0);
                    // make sure that the reply is valid
                    if (replyPointer.isNull())
                        return true;

                }
            }
        } else {
            connection->d_func()->emitReplyError(socket, reply, QNetworkReply::ProtocolFailure);
            return false;
        }
    }
    return true;
}
#endif


void QHttpNetworkConnectionChannel::allDone()
{
#ifndef QT_NO_COMPRESS
    // expand the whole data.
    if (reply->d_func()->expectContent() && reply->d_func()->autoDecompress && !reply->d_func()->streamEnd)
        expand(true); // ### if expand returns false, its an error
#endif
    // while handling 401 & 407, we might reset the status code, so save this.
    bool emitFinished = reply->d_func()->shouldEmitSignals();
    handleStatus();
    // ### at this point there should be no more data on the socket
    // close if server requested
    if (reply->d_func()->isConnectionCloseEnabled())
        close();
    // queue the finished signal, this is required since we might send new requests from
    // slot connected to it. The socket will not fire readyRead signal, if we are already
    // in the slot connected to readyRead
    if (emitFinished)
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
    // reset the reconnection attempts after we receive a complete reply.
    // in case of failures, each channel will attempt two reconnects before emitting error.
    reconnectAttempts = 2;

    detectPipeliningSupport();

    // move next from pipeline to current request
    if (!alreadyPipelinedRequests.isEmpty()) {
        if (resendCurrent || reply->d_func()->isConnectionCloseEnabled() || socket->state() != QAbstractSocket::ConnectedState) {
            // move the pipelined ones back to the main queue
            requeueCurrentlyPipelinedRequests();
            close();
        } else {
            // there were requests pipelined in and we can continue
            HttpMessagePair messagePair = alreadyPipelinedRequests.takeFirst();

            request = messagePair.first;
            reply = messagePair.second;
            state = QHttpNetworkConnectionChannel::ReadingState;
            resendCurrent = false;

            written = 0; // message body, excluding the header, irrelevant here
            bytesTotal = 0; // message body total, excluding the header, irrelevant here

            // pipeline even more
            connection->d_func()->fillPipeline(socket);

            // continue reading
            receiveReply();
        }
    } else if (alreadyPipelinedRequests.isEmpty() && socket->bytesAvailable() > 0) {
        eatWhitespace();
        // this is weird. we had nothing pipelined but still bytes available. better close it.
        if (socket->bytesAvailable() > 0)
            close();
        QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
    } else if (alreadyPipelinedRequests.isEmpty()) {
        QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

void QHttpNetworkConnectionChannel::detectPipeliningSupport()
{
    // detect HTTP Pipelining support
    QByteArray serverHeaderField = reply->headerField("Server");
    if (
            // check for broken servers in server reply header
            // this is adapted from http://mxr.mozilla.org/firefox/ident?i=SupportsPipelining
            (!serverHeaderField.contains("Microsoft-IIS/4."))
            && (!serverHeaderField.contains("Microsoft-IIS/5."))
            && (!serverHeaderField.contains("Netscape-Enterprise/3."))
            // check for HTTP/1.1
            && (reply->d_func()->majorVersion == 1 && reply->d_func()->minorVersion == 1)
            // check for not having connection close
            && (!reply->d_func()->isConnectionCloseEnabled())
            // check if it is still connected
            && (socket->state() == QAbstractSocket::ConnectedState)
            ) {
        pipeliningSupported = QHttpNetworkConnectionChannel::PipeliningProbablySupported;
    } else {
        pipeliningSupported = QHttpNetworkConnectionChannel::PipeliningSupportUnknown;
    }
}

// called when the connection broke and we need to queue some pipelined requests again
void QHttpNetworkConnectionChannel::requeueCurrentlyPipelinedRequests()
{
    for (int i = 0; i < alreadyPipelinedRequests.length(); i++)
        connection->d_func()->requeueRequest(alreadyPipelinedRequests.at(i));
    alreadyPipelinedRequests.clear();

    QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
}

void QHttpNetworkConnectionChannel::eatWhitespace()
{
    char c;
    while (socket->bytesAvailable()) {
        if (socket->peek(&c, 1) != 1)
            return;

        // read all whitespace and line endings
        if (c == 11 || c == '\n' || c == '\r' || c == ' ' || c == 31) {
            socket->read(&c, 1);
            continue;
        } else {
            break;
        }
    }
}

void QHttpNetworkConnectionChannel::handleStatus()
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    int statusCode = reply->statusCode();
    bool resend = false;

    switch (statusCode) {
    case 401: // auth required
    case 407: // proxy auth required
        if (connection->d_func()->handleAuthenticateChallenge(socket, reply, (statusCode == 407), resend)) {
            if (resend) {
                QNonContiguousByteDevice* uploadByteDevice = request.uploadByteDevice();
                if (uploadByteDevice) {
                    if (uploadByteDevice->reset()) {
                        written = 0;
                    } else {
                        connection->d_func()->emitReplyError(socket, reply, QNetworkReply::ContentReSendError);
                        break;
                    }
                }

                reply->d_func()->eraseData();

                if (alreadyPipelinedRequests.isEmpty()) {
                    // this does a re-send without closing the connection
                    resendCurrent = true;
                    QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
                } else {
                    // we had requests pipelined.. better close the connection in closeAndResendCurrentRequest
                    closeAndResendCurrentRequest();
                    QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
                }
            }
        } else {
            emit reply->headerChanged();
            emit reply->readyRead();
            QNetworkReply::NetworkError errorCode = (statusCode == 407)
                ? QNetworkReply::ProxyAuthenticationRequiredError
                : QNetworkReply::AuthenticationRequiredError;
            reply->d_func()->errorString = connection->d_func()->errorDetail(errorCode, socket);
            emit connection->error(errorCode, reply->d_func()->errorString);
            emit reply->finished();
        }
        break;
    default:
        QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

void  QHttpNetworkConnectionChannel::pipelineInto(HttpMessagePair &pair)
{
    // this is only called for simple GET

    QHttpNetworkRequest &request = pair.first;
    QHttpNetworkReply *reply = pair.second;
    if (reply) {
        reply->d_func()->clear();
        reply->d_func()->connection = connection;
        reply->d_func()->autoDecompress = request.d->autoDecompress;
        reply->d_func()->pipeliningUsed = true;
    }

#ifndef QT_NO_NETWORKPROXY
    QByteArray header = QHttpNetworkRequestPrivate::header(request,
                                                           (connection->d_func()->networkProxy.type() != QNetworkProxy::NoProxy));
#else
    QByteArray header = QHttpNetworkRequestPrivate::header(request, false);
#endif
    socket->write(header);

    alreadyPipelinedRequests.append(pair);
}

void QHttpNetworkConnectionChannel::closeAndResendCurrentRequest()
{
    requeueCurrentlyPipelinedRequests();
    close();
    resendCurrent = true;
    QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
}

bool QHttpNetworkConnectionChannel::isSocketBusy() const
{
    return (state & QHttpNetworkConnectionChannel::BusyState);
}

bool QHttpNetworkConnectionChannel::isSocketWriting() const
{
    return (state & QHttpNetworkConnectionChannel::WritingState);
}

bool QHttpNetworkConnectionChannel::isSocketWaiting() const
{
    return (state & QHttpNetworkConnectionChannel::WaitingState);
}

bool QHttpNetworkConnectionChannel::isSocketReading() const
{
    return (state & QHttpNetworkConnectionChannel::ReadingState);
}

//private slots
void QHttpNetworkConnectionChannel::_q_readyRead()
{
    if (isSocketWaiting() || isSocketReading()) {
        state = QHttpNetworkConnectionChannel::ReadingState;
        if (reply)
            receiveReply();
    }
}

void QHttpNetworkConnectionChannel::_q_bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    // bytes have been written to the socket. write even more of them :)
    if (isSocketWriting())
        sendRequest();
    // otherwise we do nothing
}

void QHttpNetworkConnectionChannel::_q_disconnected()
{
    // read the available data before closing
    if (isSocketWaiting() || isSocketReading()) {
        state = QHttpNetworkConnectionChannel::ReadingState;
        if (reply)
            receiveReply();
    } else if (state == QHttpNetworkConnectionChannel::IdleState && resendCurrent) {
        // re-sending request because the socket was in ClosingState
        QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
    }
    state = QHttpNetworkConnectionChannel::IdleState;

    requeueCurrentlyPipelinedRequests();
    close();
}


void QHttpNetworkConnectionChannel::_q_connected()
{
    // improve performance since we get the request sent by the kernel ASAP
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    pipeliningSupported = QHttpNetworkConnectionChannel::PipeliningSupportUnknown;

    // ### FIXME: if the server closes the connection unexpectedly, we shouldn't send the same broken request again!
    //channels[i].reconnectAttempts = 2;
    if (!pendingEncrypt) {
        state = QHttpNetworkConnectionChannel::IdleState;
        if (reply)
            sendRequest();
        else
            close();
    }
}


void QHttpNetworkConnectionChannel::_q_error(QAbstractSocket::SocketError socketError)
{
    if (!socket)
        return;
    bool send2Reply = false;
    QNetworkReply::NetworkError errorCode = QNetworkReply::UnknownNetworkError;

    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        errorCode = QNetworkReply::HostNotFoundError;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorCode = QNetworkReply::ConnectionRefusedError;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        // try to reconnect/resend before sending an error.
        // while "Reading" the _q_disconnected() will handle this.
        if (state != QHttpNetworkConnectionChannel::IdleState && state != QHttpNetworkConnectionChannel::ReadingState) {
            if (reconnectAttempts-- > 0) {
                closeAndResendCurrentRequest();
                return;
            } else {
                send2Reply = true;
                errorCode = QNetworkReply::RemoteHostClosedError;
            }
        } else {
            return;
        }
        break;
    case QAbstractSocket::SocketTimeoutError:
        // try to reconnect/resend before sending an error.
        if (state == QHttpNetworkConnectionChannel::WritingState && (reconnectAttempts-- > 0)) {
            closeAndResendCurrentRequest();
            return;
        }
        send2Reply = true;
        errorCode = QNetworkReply::TimeoutError;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorCode = QNetworkReply::ProxyAuthenticationRequiredError;
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorCode = QNetworkReply::SslHandshakeFailedError;
        break;
    default:
        // all other errors are treated as NetworkError
        errorCode = QNetworkReply::UnknownNetworkError;
        break;
    }
    QPointer<QObject> that = connection;
    QString errorString = connection->d_func()->errorDetail(errorCode, socket);
    if (send2Reply) {
        if (reply) {
            reply->d_func()->errorString = errorString;
            // this error matters only to this reply
            emit reply->finishedWithError(errorCode, errorString);
        }
        // send the next request
        QMetaObject::invokeMethod(that, "_q_startNextRequest", Qt::QueuedConnection);
    } else {
        // the failure affects all requests.
        emit connection->error(errorCode, errorString);
    }
    if (that) //signal emission triggered event loop
        close();
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnectionChannel::_q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator* auth)
{
    connection->d_func()->emitProxyAuthenticationRequired(this, proxy, auth);
}
#endif

void QHttpNetworkConnectionChannel::_q_uploadDataReadyRead()
{
    sendRequest();
}

#ifndef QT_NO_OPENSSL
void QHttpNetworkConnectionChannel::_q_encrypted()
{
    if (!socket)
        return; // ### error
    state = QHttpNetworkConnectionChannel::IdleState;
    sendRequest();
}

void QHttpNetworkConnectionChannel::_q_sslErrors(const QList<QSslError> &errors)
{
    if (!socket)
        return;
    //QNetworkReply::NetworkError errorCode = QNetworkReply::ProtocolFailure;
    emit connection->sslErrors(errors);
}

void QHttpNetworkConnectionChannel::_q_encryptedBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    // bytes have been written to the socket. write even more of them :)
    if (isSocketWriting())
        sendRequest();
    // otherwise we do nothing
}
#endif

QT_END_NAMESPACE

#include "moc_qhttpnetworkconnectionchannel_p.cpp"

#endif // QT_NO_HTTP
