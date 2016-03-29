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

#ifndef QHTTPNETWORKCONNECTIONCHANNEL_H
#define QHTTPNETWORKCONNECTIONCHANNEL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qabstractsocket.h>

#include <private/qobject_p.h>
#include <qauthenticator.h>
#include <qnetworkproxy.h>
#include <qbuffer.h>

#include <private/qhttpnetworkheader_p.h>
#include <private/qhttpnetworkrequest_p.h>
#include <private/qhttpnetworkreply_p.h>


#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslsocket.h>
#    include <QtNetwork/qsslerror.h>
#else
#   include <QtNetwork/qtcpsocket.h>
#endif

QT_BEGIN_NAMESPACE

class QHttpNetworkRequest;
class QHttpNetworkReply;
class QByteArray;
class QHttpNetworkConnection;

#ifndef HttpMessagePair
typedef QPair<QHttpNetworkRequest, QHttpNetworkReply*> HttpMessagePair;
#endif

class QHttpNetworkConnectionChannel : public QObject {
    Q_OBJECT
public:
    enum ChannelState {
        IdleState = 0,          // ready to send request
        ConnectingState = 1,    // connecting to host
        WritingState = 2,       // writing the data
        WaitingState = 4,       // waiting for reply
        ReadingState = 8,       // reading the reply
        Wait4AuthState = 0x10,  // blocked for send till the current authentication slot is done
        BusyState = (ConnectingState|WritingState|WaitingState|ReadingState|Wait4AuthState)
    };
    QAbstractSocket *socket;
    ChannelState state;
    QHttpNetworkRequest request; // current request
    QHttpNetworkReply *reply; // current reply for this request
    qint64 written;
    qint64 bytesTotal;
    bool resendCurrent;
    int lastStatus; // last status received on this channel
    bool pendingEncrypt; // for https (send after encrypted)
    int reconnectAttempts; // maximum 2 reconnection attempts
    QAuthenticatorPrivate::Method authMehtod;
    QAuthenticatorPrivate::Method proxyAuthMehtod;
    QAuthenticator authenticator;
    QAuthenticator proxyAuthenticator;
#ifndef QT_NO_OPENSSL
    bool ignoreAllSslErrors;
    QList<QSslError> ignoreSslErrorsList;
#endif

    // HTTP pipelining -> http://en.wikipedia.org/wiki/Http_pipelining
    enum PipeliningSupport {
        PipeliningSupportUnknown, // default for a new connection
        PipeliningProbablySupported, // after having received a server response that indicates support
        PipeliningNotSupported // currently not used
    };
    PipeliningSupport pipeliningSupported;
    QList<HttpMessagePair> alreadyPipelinedRequests;


    QHttpNetworkConnectionChannel() : socket(0), state(IdleState), reply(0), written(0), bytesTotal(0), resendCurrent(false),
    lastStatus(0), pendingEncrypt(false), reconnectAttempts(2),
    authMehtod(QAuthenticatorPrivate::None), proxyAuthMehtod(QAuthenticatorPrivate::None)
#ifndef QT_NO_OPENSSL
    , ignoreAllSslErrors(false)
#endif
    , pipeliningSupported(PipeliningSupportUnknown)
    , connection(0)
    {}

    void setConnection(QHttpNetworkConnection *c) {connection = c;}
    QHttpNetworkConnection *connection;

    void init();
    void close();

    bool sendRequest();
    void receiveReply();

    bool ensureConnection();

    bool expand(bool dataComplete);
    void allDone(); // reply header + body have been read
    void handleStatus(); // called from allDone()

    void pipelineInto(HttpMessagePair &pair);
    void requeueCurrentlyPipelinedRequests();
    void detectPipeliningSupport();

    void closeAndResendCurrentRequest();

    void eatWhitespace();

    bool isSocketBusy() const;
    bool isSocketWriting() const;
    bool isSocketWaiting() const;
    bool isSocketReading() const;

    protected slots:
    void _q_bytesWritten(qint64 bytes); // proceed sending
    void _q_readyRead(); // pending data to read
    void _q_disconnected(); // disconnected from host
    void _q_connected(); // start sending request
    void _q_error(QAbstractSocket::SocketError); // error from socket
#ifndef QT_NO_NETWORKPROXY
    void _q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth); // from transparent proxy
#endif

    void _q_uploadDataReadyRead();

#ifndef QT_NO_OPENSSL
    void _q_encrypted(); // start sending request (https)
    void _q_sslErrors(const QList<QSslError> &errors); // ssl errors from the socket
    void _q_encryptedBytesWritten(qint64 bytes); // proceed sending
#endif
};



QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif
