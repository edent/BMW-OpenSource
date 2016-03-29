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

//#define QNETWORKACCESSHTTPBACKEND_DEBUG

#include "qnetworkaccesshttpbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkaccesscache_p.h"
#include "qabstractnetworkcache.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkrequest_p.h"
#include "qnetworkcookie_p.h"
#include "QtCore/qdatetime.h"
#include "QtNetwork/qsslconfiguration.h"

#ifndef QT_NO_HTTP

#include <string.h>             // for strchr

QT_BEGIN_NAMESPACE

enum {
    DefaultHttpPort = 80,
    DefaultHttpsPort = 443
};

class QNetworkProxy;

static QByteArray makeCacheKey(QNetworkAccessHttpBackend *backend, QNetworkProxy *proxy)
{
    QByteArray result;
    QUrl copy = backend->url();
    bool isEncrypted = copy.scheme().toLower() == QLatin1String("https");
    copy.setPort(copy.port(isEncrypted ? DefaultHttpsPort : DefaultHttpPort));
    result = copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath |
                            QUrl::RemoveQuery | QUrl::RemoveFragment);

#ifndef QT_NO_NETWORKPROXY
    if (proxy->type() != QNetworkProxy::NoProxy) {
        QUrl key;

        switch (proxy->type()) {
        case QNetworkProxy::Socks5Proxy:
            key.setScheme(QLatin1String("proxy-socks5"));
            break;

        case QNetworkProxy::HttpProxy:
        case QNetworkProxy::HttpCachingProxy:
            key.setScheme(QLatin1String("proxy-http"));
            break;

        default:
            break;
        }

        if (!key.scheme().isEmpty()) {
            key.setUserName(proxy->user());
            key.setHost(proxy->hostName());
            key.setPort(proxy->port());
            key.setEncodedQuery(result);
            result = key.toEncoded();
        }
    }
#endif

    return "http-connection:" + result;
}

static inline bool isSeparator(register char c)
{
    static const char separators[] = "()<>@,;:\\\"/[]?={}";
    return isLWS(c) || strchr(separators, c) != 0;
}

// ### merge with nextField in cookiejar.cpp
static QHash<QByteArray, QByteArray> parseHttpOptionHeader(const QByteArray &header)
{
    // The HTTP header is of the form:
    // header          = #1(directives)
    // directives      = token | value-directive
    // value-directive = token "=" (token | quoted-string)
    QHash<QByteArray, QByteArray> result;

    int pos = 0;
    while (true) {
        // skip spaces
        pos = nextNonWhitespace(header, pos);
        if (pos == header.length())
            return result;      // end of parsing

        // pos points to a non-whitespace
        int comma = header.indexOf(',', pos);
        int equal = header.indexOf('=', pos);
        if (comma == pos || equal == pos)
            // huh? Broken header.
            return result;

        // The key name is delimited by either a comma, an equal sign or the end
        // of the header, whichever comes first
        int end = comma;
        if (end == -1)
            end = header.length();
        if (equal != -1 && end > equal)
            end = equal;        // equal sign comes before comma/end
        QByteArray key = QByteArray(header.constData() + pos, end - pos).trimmed().toLower();
        pos = end + 1;

        if (equal != -1) {
            // case: token "=" (token | quoted-string)
            // skip spaces
            pos = nextNonWhitespace(header, pos);
            if (pos == header.length())
                // huh? Broken header
                return result;

            QByteArray value;
            value.reserve(header.length() - pos);
            if (header.at(pos) == '"') {
                // case: quoted-string
                // quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
                // qdtext         = <any TEXT except <">>
                // quoted-pair    = "\" CHAR
                ++pos;
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (c == '"') {
                        // end of quoted text
                        break;
                    } else if (c == '\\') {
                        ++pos;
                        if (pos >= header.length())
                            // broken header
                            return result;
                        c = header.at(pos);
                    }

                    value += c;
                    ++pos;
                }
            } else {
                // case: token
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (isSeparator(c))
                        break;
                    value += c;
                    ++pos;
                }
            }

            result.insert(key, value);

            // find the comma now:
            comma = header.indexOf(',', pos);
            if (comma == -1)
                return result;  // end of parsing
            pos = comma + 1;
        } else {
            // case: token
            // key is already set
            result.insert(key, QByteArray());
        }
    }
}

QNetworkAccessBackend *
QNetworkAccessHttpBackendFactory::create(QNetworkAccessManager::Operation op,
                                         const QNetworkRequest &request) const
{
    // check the operation
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PostOperation:
    case QNetworkAccessManager::HeadOperation:
    case QNetworkAccessManager::PutOperation:
    case QNetworkAccessManager::DeleteOperation:
        break;

    default:
        // no, we can't handle this request
        return 0;
    }

    QUrl url = request.url();
    QString scheme = url.scheme().toLower();
    if (scheme == QLatin1String("http") || scheme == QLatin1String("https"))
        return new QNetworkAccessHttpBackend;

    return 0;
}

static QNetworkReply::NetworkError statusCodeFromHttp(int httpStatusCode, const QUrl &url)
{
    QNetworkReply::NetworkError code;
    // we've got an error
    switch (httpStatusCode) {
    case 401:               // Authorization required
        code = QNetworkReply::AuthenticationRequiredError;
        break;

    case 403:               // Access denied
        code = QNetworkReply::ContentOperationNotPermittedError;
        break;

    case 404:               // Not Found
        code = QNetworkReply::ContentNotFoundError;
        break;

    case 405:               // Method Not Allowed
        code = QNetworkReply::ContentOperationNotPermittedError;
        break;

    case 407:
        code = QNetworkReply::ProxyAuthenticationRequiredError;
        break;

    default:
        if (httpStatusCode > 500) {
            // some kind of server error
            code = QNetworkReply::ProtocolUnknownError;
        } else if (httpStatusCode >= 400) {
            // content error we did not handle above
            code = QNetworkReply::UnknownContentError;
        } else {
            qWarning("QNetworkAccess: got HTTP status code %d which is not expected from url: \"%s\"",
                     httpStatusCode, qPrintable(url.toString()));
            code = QNetworkReply::ProtocolFailure;
        }
    }

    return code;
}

class QNetworkAccessCachedHttpConnection: public QHttpNetworkConnection,
                                      public QNetworkAccessCache::CacheableObject
{
    // Q_OBJECT
public:
    QNetworkAccessCachedHttpConnection(const QString &hostName, quint16 port, bool encrypt)
        : QHttpNetworkConnection(hostName, port, encrypt)
    {
        setExpires(true);
        setShareable(true);
    }

    virtual void dispose()
    {
#if 0  // sample code; do this right with the API
        Q_ASSERT(!isWorking());
#endif
        delete this;
    }
};

QNetworkAccessHttpBackend::QNetworkAccessHttpBackend()
    : QNetworkAccessBackend(), httpReply(0), http(0), uploadDevice(0)
#ifndef QT_NO_OPENSSL
    , pendingSslConfiguration(0), pendingIgnoreAllSslErrors(false)
#endif
{
}

QNetworkAccessHttpBackend::~QNetworkAccessHttpBackend()
{
    if (http)
        disconnectFromHttp();
#ifndef QT_NO_OPENSSL
    delete pendingSslConfiguration;
#endif
}

void QNetworkAccessHttpBackend::disconnectFromHttp()
{
    if (http) {
        // This is abut disconnecting signals, not about disconnecting TCP connections
        disconnect(http, 0, this, 0);

        // Get the object cache that stores our QHttpNetworkConnection objects
        QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getObjectCache(this);
        cache->releaseEntry(cacheKey);
    }

    // This is abut disconnecting signals, not about disconnecting TCP connections
    if (httpReply)
        disconnect(httpReply, 0, this, 0);

    http = 0;
    httpReply = 0;
    cacheKey.clear();
}

void QNetworkAccessHttpBackend::finished()
{
    if (http)
        disconnectFromHttp();
    // call parent
    QNetworkAccessBackend::finished();
}

void QNetworkAccessHttpBackend::setupConnection()
{
#ifndef QT_NO_NETWORKPROXY
    connect(http, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
#endif
    connect(http, SIGNAL(authenticationRequired(QHttpNetworkRequest,QAuthenticator*)),
            SLOT(httpAuthenticationRequired(QHttpNetworkRequest,QAuthenticator*)));
    connect(http, SIGNAL(error(QNetworkReply::NetworkError,QString)),
            SLOT(httpError(QNetworkReply::NetworkError,QString)));
#ifndef QT_NO_OPENSSL
    connect(http, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
#endif
}

/*
    For a given httpRequest
    1) If AlwaysNetwork, return
    2) If we have a cache entry for this url populate headers so the server can return 304
    3) Calculate if response_is_fresh and if so send the cache and set loadedFromCache to true
 */
void QNetworkAccessHttpBackend::validateCache(QHttpNetworkRequest &httpRequest, bool &loadedFromCache)
{
    QNetworkRequest::CacheLoadControl CacheLoadControlAttribute =
        (QNetworkRequest::CacheLoadControl)request().attribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork).toInt();
    if (CacheLoadControlAttribute == QNetworkRequest::AlwaysNetwork) {
        // forced reload from the network
        // tell any caching proxy servers to reload too
        httpRequest.setHeaderField("Cache-Control", "no-cache");
        httpRequest.setHeaderField("Pragma", "no-cache");
        return;
    }

    QAbstractNetworkCache *nc = networkCache();
    if (!nc)
        return;                 // no local cache

    QNetworkCacheMetaData metaData = nc->metaData(url());
    if (!metaData.isValid())
        return;                 // not in cache

    if (!metaData.saveToDisk())
        return;

    QNetworkHeadersPrivate cacheHeaders;
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;
    cacheHeaders.setAllRawHeaders(metaData.rawHeaders());

    it = cacheHeaders.findRawHeader("etag");
    if (it != cacheHeaders.rawHeaders.constEnd())
        httpRequest.setHeaderField("If-None-Match", it->second);

    QDateTime lastModified = metaData.lastModified();
    if (lastModified.isValid())
        httpRequest.setHeaderField("If-Modified-Since", QNetworkHeadersPrivate::toHttpDate(lastModified));

    it = cacheHeaders.findRawHeader("Cache-Control");
    if (it != cacheHeaders.rawHeaders.constEnd()) {
        QHash<QByteArray, QByteArray> cacheControl = parseHttpOptionHeader(it->second);
        if (cacheControl.contains("must-revalidate"))
            return;
    }

    /*
     * age_value
     *      is the value of Age: header received by the cache with
     *              this response.
     * date_value
     *      is the value of the origin server's Date: header
     * request_time
     *      is the (local) time when the cache made the request
     *              that resulted in this cached response
     * response_time
     *      is the (local) time when the cache received the
     *              response
     * now
     *      is the current (local) time
     */
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int age_value = 0;
    it = cacheHeaders.findRawHeader("age");
    if (it != cacheHeaders.rawHeaders.constEnd())
        age_value = QNetworkHeadersPrivate::fromHttpDate(it->second).toTime_t();

    int date_value = 0;
    it = cacheHeaders.findRawHeader("date");
    if (it != cacheHeaders.rawHeaders.constEnd())
        date_value = QNetworkHeadersPrivate::fromHttpDate(it->second).toTime_t();

    int now = currentDateTime.toUTC().toTime_t();
    int request_time = now;
    int response_time = now;

    int apparent_age = qMax(0, response_time - date_value);
    int corrected_received_age = qMax(apparent_age, age_value);
    int response_delay = response_time - request_time;
    int corrected_initial_age = corrected_received_age + response_delay;
    int resident_time = now - response_time;
    int current_age   = corrected_initial_age + resident_time;

    // RFC 2616 13.2.4 Expiration Calculations
    QDateTime expirationDate = metaData.expirationDate();
    if (!expirationDate.isValid()) {
        if (lastModified.isValid()) {
            int diff = currentDateTime.secsTo(lastModified);
            expirationDate = lastModified;
            expirationDate.addSecs(diff / 10);
            if (httpRequest.headerField("Warning").isEmpty()) {
                QDateTime dt;
                dt.setTime_t(current_age);
                if (dt.daysTo(currentDateTime) > 1)
                    httpRequest.setHeaderField("Warning", "113");
            }
        }
    }

    int freshness_lifetime = currentDateTime.secsTo(expirationDate);
    bool response_is_fresh = (freshness_lifetime > current_age);

    if (!response_is_fresh && CacheLoadControlAttribute == QNetworkRequest::PreferNetwork)
        return;

    loadedFromCache = true;
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "response_is_fresh" << CacheLoadControlAttribute;
#endif
    if (!sendCacheContents(metaData))
        loadedFromCache = false;
}

void QNetworkAccessHttpBackend::postRequest()
{
    bool loadedFromCache = false;
    QHttpNetworkRequest httpRequest;
    switch (operation()) {
    case QNetworkAccessManager::GetOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Get);
        validateCache(httpRequest, loadedFromCache);
        break;

    case QNetworkAccessManager::HeadOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Head);
        validateCache(httpRequest, loadedFromCache);
        break;

    case QNetworkAccessManager::PostOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Post);
        httpRequest.setUploadByteDevice(createUploadByteDevice());
        break;

    case QNetworkAccessManager::PutOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Put);
        httpRequest.setUploadByteDevice(createUploadByteDevice());
        break;

    case QNetworkAccessManager::DeleteOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Delete);
        break;

    default:
        break;                  // can't happen
    }

    httpRequest.setUrl(url());

    QList<QByteArray> headers = request().rawHeaderList();
    foreach (const QByteArray &header, headers)
        httpRequest.setHeaderField(header, request().rawHeader(header));

    if (loadedFromCache) {
        // commented this out since it will be called later anyway
        // by copyFinished()
        //QNetworkAccessBackend::finished();
        return;    // no need to send the request! :)
    }

    if (request().attribute(QNetworkRequest::HttpPipeliningAllowedAttribute).toBool() == true)
        httpRequest.setPipeliningAllowed(true);

    httpReply = http->sendRequest(httpRequest);
    httpReply->setParent(this);
#ifndef QT_NO_OPENSSL
    if (pendingSslConfiguration)
        httpReply->setSslConfiguration(*pendingSslConfiguration);
    if (pendingIgnoreAllSslErrors)
        httpReply->ignoreSslErrors();
    httpReply->ignoreSslErrors(pendingIgnoreSslErrorsList);
#endif

    connect(httpReply, SIGNAL(readyRead()), SLOT(replyReadyRead()));
    connect(httpReply, SIGNAL(finished()), SLOT(replyFinished()));
    connect(httpReply, SIGNAL(finishedWithError(QNetworkReply::NetworkError,QString)),
            SLOT(httpError(QNetworkReply::NetworkError,QString)));
    connect(httpReply, SIGNAL(headerChanged()), SLOT(replyHeaderChanged()));
}

void QNetworkAccessHttpBackend::invalidateCache()
{
    QAbstractNetworkCache *nc = networkCache();
    if (nc)
        nc->remove(url());
}

void QNetworkAccessHttpBackend::open()
{
    QUrl url = request().url();
    bool encrypt = url.scheme().toLower() == QLatin1String("https");
    setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, encrypt);

    // set the port number in the reply if it wasn't set
    url.setPort(url.port(encrypt ? DefaultHttpsPort : DefaultHttpPort));

    QNetworkProxy *theProxy = 0;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy transparentProxy, cacheProxy;

    foreach (const QNetworkProxy &p, proxyList()) {
        // use the first proxy that works
        // for non-encrypted connections, any transparent or HTTP proxy
        // for encrypted, only transparent proxies
        if (!encrypt
            && (p.capabilities() & QNetworkProxy::CachingCapability)
            && (p.type() == QNetworkProxy::HttpProxy ||
                p.type() == QNetworkProxy::HttpCachingProxy)) {
            cacheProxy = p;
            transparentProxy = QNetworkProxy::NoProxy;
            theProxy = &cacheProxy;
            break;
        }
        if (p.isTransparentProxy()) {
            transparentProxy = p;
            cacheProxy = QNetworkProxy::NoProxy;
            theProxy = &transparentProxy;
            break;
        }
    }

    // check if at least one of the proxies
    if (transparentProxy.type() == QNetworkProxy::DefaultProxy &&
        cacheProxy.type() == QNetworkProxy::DefaultProxy) {
        // unsuitable proxies
        error(QNetworkReply::ProxyNotFoundError,
              tr("No suitable proxy found"));
        finished();
        return;
    }
#endif

    // check if we have an open connection to this host
    cacheKey = makeCacheKey(this, theProxy);
    QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getObjectCache(this);
    // the http object is actually a QHttpNetworkConnection
    http = static_cast<QNetworkAccessCachedHttpConnection *>(cache->requestEntryNow(cacheKey));
    if (http == 0) {
        // no entry in cache; create an object
        // the http object is actually a QHttpNetworkConnection
        http = new QNetworkAccessCachedHttpConnection(url.host(), url.port(), encrypt);

#ifndef QT_NO_NETWORKPROXY
        http->setTransparentProxy(transparentProxy);
        http->setCacheProxy(cacheProxy);
#endif

        // cache the QHttpNetworkConnection corresponding to this cache key
        cache->addEntry(cacheKey, http);
    }

    setupConnection();
    postRequest();
}

void QNetworkAccessHttpBackend::closeDownstreamChannel()
{
    // this indicates that the user closed the stream while the reply isn't finished yet
}

bool QNetworkAccessHttpBackend::waitForDownstreamReadyRead(int msecs)
{
    Q_ASSERT(http);

    if (httpReply->bytesAvailable()) {
        readFromHttp();
        return true;
    }

    if (msecs == 0) {
        // no bytes available in the socket and no waiting
        return false;
    }

    // ### FIXME
    qCritical("QNetworkAccess: HTTP backend does not support waitForReadyRead()");
    return false;
}


void QNetworkAccessHttpBackend::downstreamReadyWrite()
{
    readFromHttp();
    if (httpReply && httpReply->bytesAvailable() == 0 && httpReply->isFinished())
        replyFinished();
}

void QNetworkAccessHttpBackend::replyReadyRead()
{
    readFromHttp();
}

void QNetworkAccessHttpBackend::readFromHttp()
{
    if (!httpReply)
        return;

    // We read possibly more than nextDownstreamBlockSize(), but
    // this is not a critical thing since it is already in the
    // memory anyway

    QByteDataBuffer list;

    while (httpReply->bytesAvailable() != 0 && nextDownstreamBlockSize() != 0 && nextDownstreamBlockSize() > list.byteAmount()) {
        QByteArray data = httpReply->readAny();
        list.append(data);
    }

    if (!list.isEmpty())
      writeDownstreamData(list);
}

void QNetworkAccessHttpBackend::replyFinished()
{
    if (httpReply->bytesAvailable())
        // we haven't read everything yet. Wait some more.
        return;

    int statusCode = httpReply->statusCode();
    if (statusCode >= 400) {
        // it's an error reply
        QString msg = QLatin1String(QT_TRANSLATE_NOOP("QNetworkReply",
                                                      "Error downloading %1 - server replied: %2"));
        msg = msg.arg(url().toString(), httpReply->reasonPhrase());
        error(statusCodeFromHttp(httpReply->statusCode(), httpReply->url()), msg);
    }

#ifndef QT_NO_OPENSSL
    // store the SSL configuration now
    // once we call finished(), we won't have access to httpReply anymore
    QSslConfiguration sslConfig = httpReply->sslConfiguration();
    if (pendingSslConfiguration) {
        *pendingSslConfiguration = sslConfig;
    } else if (!sslConfig.isNull()) {
        QT_TRY {
            pendingSslConfiguration = new QSslConfiguration(sslConfig);
        } QT_CATCH(...) {
            qWarning("QNetworkAccess: could not allocate a QSslConfiguration object for a SSL connection.");
        }
    }
#endif

    finished();
}

void QNetworkAccessHttpBackend::checkForRedirect(const int statusCode)
{
    switch (statusCode) {
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // What do we do about the caching of the HTML note?
        // The response to a 303 MUST NOT be cached, while the response to
        // all of the others is cacheable if the headers indicate it to be
        QByteArray header = rawHeader("location");
        QUrl url = QUrl::fromEncoded(header);
        if (!url.isValid())
            url = QUrl(QLatin1String(header));
        redirectionRequested(url);
    }
}

void QNetworkAccessHttpBackend::replyHeaderChanged()
{
    setAttribute(QNetworkRequest::HttpPipeliningWasUsedAttribute, httpReply->isPipeliningUsed());

    // reconstruct the HTTP header
    QList<QPair<QByteArray, QByteArray> > headerMap = httpReply->header();
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = headerMap.constBegin(),
                                                        end = headerMap.constEnd();
    QByteArray header;

    for (; it != end; ++it) {
        QByteArray value = rawHeader(it->first);
        if (!value.isEmpty()) {
            if (it->first.toLower() == "set-cookie")
                value += "\n";
            else
                value += ", ";
        }
        value += it->second;
        setRawHeader(it->first, value);
    }

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, httpReply->statusCode());
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, httpReply->reasonPhrase());

    // is it a redirection?
    const int statusCode = httpReply->statusCode();
    checkForRedirect(statusCode);

    if (statusCode >= 500 && statusCode < 600) {
        QAbstractNetworkCache *nc = networkCache();
        if (nc) {
            QNetworkCacheMetaData metaData = nc->metaData(url());
            QNetworkHeadersPrivate cacheHeaders;
            cacheHeaders.setAllRawHeaders(metaData.rawHeaders());
            QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;
            it = cacheHeaders.findRawHeader("Cache-Control");
            bool mustReValidate = false;
            if (it != cacheHeaders.rawHeaders.constEnd()) {
                QHash<QByteArray, QByteArray> cacheControl = parseHttpOptionHeader(it->second);
                if (cacheControl.contains("must-revalidate"))
                    mustReValidate = true;
            }
            if (!mustReValidate && sendCacheContents(metaData))
                return;
        }
    }

    if (statusCode == 304) {
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        qDebug() << "Received a 304 from" << url();
#endif
        QAbstractNetworkCache *nc = networkCache();
        if (nc) {
            QNetworkCacheMetaData oldMetaData = nc->metaData(url());
            QNetworkCacheMetaData metaData = fetchCacheMetaData(oldMetaData);
            if (oldMetaData != metaData)
                nc->updateMetaData(metaData);
            if (sendCacheContents(metaData))
                return;
        }
    }


    if (statusCode != 304 && statusCode != 303) {
        if (!isCachingEnabled())
            setCachingEnabled(true);
    }
    metaDataChanged();
}

void QNetworkAccessHttpBackend::httpAuthenticationRequired(const QHttpNetworkRequest &,
                                                           QAuthenticator *auth)
{
    authenticationRequired(auth);
}

void QNetworkAccessHttpBackend::httpError(QNetworkReply::NetworkError errorCode,
                                          const QString &errorString)
{
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "http error!" << errorCode << errorString;
#endif
#if 0
    static const QNetworkReply::NetworkError conversionTable[] = {
        QNetworkReply::ConnectionRefusedError,
        QNetworkReply::RemoteHostClosedError,
        QNetworkReply::HostNotFoundError,
        QNetworkReply::UnknownNetworkError, // SocketAccessError
        QNetworkReply::UnknownNetworkError, // SocketResourceError
        QNetworkReply::TimeoutError,        // SocketTimeoutError
        QNetworkReply::UnknownNetworkError, // DatagramTooLargeError
        QNetworkReply::UnknownNetworkError, // NetworkError
        QNetworkReply::UnknownNetworkError, // AddressInUseError
        QNetworkReply::UnknownNetworkError, // SocketAddressNotAvailableError
        QNetworkReply::UnknownNetworkError, // UnsupportedSocketOperationError
        QNetworkReply::UnknownNetworkError, // UnfinishedSocketOperationError
        QNetworkReply::ProxyAuthenticationRequiredError
    };
    QNetworkReply::NetworkError code;
    if (int(errorCode) >= 0 &&
        uint(errorCode) < (sizeof conversionTable / sizeof conversionTable[0]))
        code = conversionTable[errorCode];
    else
        code = QNetworkReply::UnknownNetworkError;
#endif
    error(errorCode, errorString);
    finished();
}

/*
    A simple web page that can be used to test us: http://www.procata.com/cachetest/
 */
bool QNetworkAccessHttpBackend::sendCacheContents(const QNetworkCacheMetaData &metaData)
{
    setCachingEnabled(false);
    if (!metaData.isValid())
        return false;

    QAbstractNetworkCache *nc = networkCache();
    Q_ASSERT(nc);
    QIODevice *contents = nc->data(url());
    if (!contents) {
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        qDebug() << "Can not send cache, the contents are 0" << url();
#endif
        return false;
    }
    contents->setParent(this);

    QNetworkCacheMetaData::AttributesMap attributes = metaData.attributes();
    int status = attributes.value(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status < 100)
        status = 200;           // fake it

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, attributes.value(QNetworkRequest::HttpReasonPhraseAttribute));
    setAttribute(QNetworkRequest::SourceIsFromCacheAttribute, true);

    QNetworkCacheMetaData::RawHeaderList rawHeaders = metaData.rawHeaders();
    QNetworkCacheMetaData::RawHeaderList::ConstIterator it = rawHeaders.constBegin(),
                                                       end = rawHeaders.constEnd();
    for ( ; it != end; ++it)
        setRawHeader(it->first, it->second);

    checkForRedirect(status);

    emit metaDataChanged();

    // invoke this asynchronously, else Arora/QtDemoBrowser don't like cached downloads
    // see task 250221 / 251801
    qRegisterMetaType<QIODevice*>("QIODevice*");
    QMetaObject::invokeMethod(this, "writeDownstreamData", Qt::QueuedConnection, Q_ARG(QIODevice*, contents));


#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "Successfully sent cache:" << url() << contents->size() << "bytes";
#endif
    if (httpReply)
        disconnect(httpReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    return true;
}

void QNetworkAccessHttpBackend::copyFinished(QIODevice *dev)
{
    delete dev;
    finished();
}

#ifndef QT_NO_OPENSSL
void QNetworkAccessHttpBackend::ignoreSslErrors()
{
    if (httpReply)
        httpReply->ignoreSslErrors();
    else
        pendingIgnoreAllSslErrors = true;
}

void QNetworkAccessHttpBackend::ignoreSslErrors(const QList<QSslError> &errors)
{
    if (httpReply) {
        httpReply->ignoreSslErrors(errors);
    } else {
        // the pending list is set if QNetworkReply::ignoreSslErrors(const QList<QSslError> &errors)
        // is called before QNetworkAccessManager::get() (or post(), etc.)
        pendingIgnoreSslErrorsList = errors;
    }
}

void QNetworkAccessHttpBackend::fetchSslConfiguration(QSslConfiguration &config) const
{
    if (httpReply)
        config = httpReply->sslConfiguration();
    else if (pendingSslConfiguration)
        config = *pendingSslConfiguration;
}

void QNetworkAccessHttpBackend::setSslConfiguration(const QSslConfiguration &newconfig)
{
    if (httpReply)
        httpReply->setSslConfiguration(newconfig);
    else if (pendingSslConfiguration)
        *pendingSslConfiguration = newconfig;
    else
        pendingSslConfiguration = new QSslConfiguration(newconfig);
}
#endif

QNetworkCacheMetaData QNetworkAccessHttpBackend::fetchCacheMetaData(const QNetworkCacheMetaData &oldMetaData) const
{
    QNetworkCacheMetaData metaData = oldMetaData;

    QNetworkHeadersPrivate cacheHeaders;
    cacheHeaders.setAllRawHeaders(metaData.rawHeaders());
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;

    QList<QByteArray> newHeaders = rawHeaderList();
    foreach (QByteArray header, newHeaders) {
        QByteArray originalHeader = header;
        header = header.toLower();
        bool hop_by_hop =
            (header == "connection"
             || header == "keep-alive"
             || header == "proxy-authenticate"
             || header == "proxy-authorization"
             || header == "te"
             || header == "trailers"
             || header == "transfer-encoding"
             || header ==  "upgrade");
        if (hop_by_hop)
            continue;

        // for 4.6.0, we were planning to not store the date header in the
        // cached resource; through that we planned to reduce the number
        // of writes to disk when using a QNetworkDiskCache (i.e. don't
        // write to disk when only the date changes).
        // However, without the date we cannot calculate the age of the page
        // anymore. Consider a proper fix of that problem for 4.6.1.
        //if (header == "date")
            //continue;

        // Don't store Warning 1xx headers
        if (header == "warning") {
            QByteArray v = rawHeader(header);
            if (v.length() == 3
                && v[0] == '1'
                && v[1] >= '0' && v[1] <= '9'
                && v[2] >= '0' && v[2] <= '9')
                continue;
        }

        it = cacheHeaders.findRawHeader(header);
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            // Match the behavior of Firefox and assume Cache-Control: "no-transform"
            if (header == "content-encoding"
                || header == "content-range"
                || header == "content-type")
                continue;

            // For MS servers that send "Content-Length: 0" on 304 responses
            // ignore this too
            if (header == "content-length")
                continue;
        }

#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        QByteArray n = rawHeader(header);
        QByteArray o;
        if (it != cacheHeaders.rawHeaders.constEnd())
            o = (*it).second;
        if (n != o && header != "date") {
            qDebug() << "replacing" << header;
            qDebug() << "new" << n;
            qDebug() << "old" << o;
        }
#endif
        cacheHeaders.setRawHeader(originalHeader, rawHeader(header));
    }
    metaData.setRawHeaders(cacheHeaders.rawHeaders);

    bool checkExpired = true;

    QHash<QByteArray, QByteArray> cacheControl;
    it = cacheHeaders.findRawHeader("Cache-Control");
    if (it != cacheHeaders.rawHeaders.constEnd()) {
        cacheControl = parseHttpOptionHeader(it->second);
        QByteArray maxAge = cacheControl.value("max-age");
        if (!maxAge.isEmpty()) {
            checkExpired = false;
            QDateTime dt = QDateTime::currentDateTime();
            dt = dt.addSecs(maxAge.toInt());
            metaData.setExpirationDate(dt);
        }
    }
    if (checkExpired) {
        it = cacheHeaders.findRawHeader("expires");
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            QDateTime expiredDateTime = QNetworkHeadersPrivate::fromHttpDate(it->second);
            metaData.setExpirationDate(expiredDateTime);
        }
    }

    it = cacheHeaders.findRawHeader("last-modified");
    if (it != cacheHeaders.rawHeaders.constEnd())
        metaData.setLastModified(QNetworkHeadersPrivate::fromHttpDate(it->second));

    bool canDiskCache;
    // only cache GET replies by default, all other replies (POST, PUT, DELETE)
    //  are not cacheable by default (according to RFC 2616 section 9)
    if (httpReply->request().operation() == QHttpNetworkRequest::Get) {

        canDiskCache = true;
        // 14.32
        // HTTP/1.1 caches SHOULD treat "Pragma: no-cache" as if the client
        // had sent "Cache-Control: no-cache".
        it = cacheHeaders.findRawHeader("pragma");
        if (it != cacheHeaders.rawHeaders.constEnd()
            && it->second == "no-cache")
            canDiskCache = false;

        // HTTP/1.1. Check the Cache-Control header
        if (cacheControl.contains("no-cache"))
            canDiskCache = false;
        else if (cacheControl.contains("no-store"))
            canDiskCache = false;

    // responses to POST might be cacheable
    } else if (httpReply->request().operation() == QHttpNetworkRequest::Post) {

        canDiskCache = false;
        // some pages contain "expires:" and "cache-control: no-cache" field,
        // so we only might cache POST requests if we get "cache-control: max-age ..."
        if (cacheControl.contains("max-age"))
            canDiskCache = true;

    // responses to PUT and DELETE are not cacheable
    } else {
        canDiskCache = false;
    }

    metaData.setSaveToDisk(canDiskCache);
    int statusCode = httpReply->statusCode();
    QNetworkCacheMetaData::AttributesMap attributes;
    if (statusCode != 304) {
        // update the status code
        attributes.insert(QNetworkRequest::HttpStatusCodeAttribute, statusCode);
        attributes.insert(QNetworkRequest::HttpReasonPhraseAttribute, httpReply->reasonPhrase());
    } else {
        // this is a redirection, keep the attributes intact
        attributes = oldMetaData.attributes();
    }
    metaData.setAttributes(attributes);
    return metaData;
}

QT_END_NAMESPACE

#endif // QT_NO_HTTP
