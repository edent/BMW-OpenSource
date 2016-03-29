/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qacceltreeresourceloader_p.h"
#include "qxmlschema.h"
#include "qxmlschema_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

QXmlSchemaPrivate::QXmlSchemaPrivate(const QXmlNamePool &namePool)
    : m_namePool(namePool)
    , m_userMessageHandler(0)
    , m_uriResolver(0)
    , m_userNetworkAccessManager(0)
    , m_schemaContext(new QPatternist::XsdSchemaContext(m_namePool.d))
    , m_schemaParserContext(new QPatternist::XsdSchemaParserContext(m_namePool.d, m_schemaContext))
    , m_schemaIsValid(false)
{
    m_networkAccessManager = new QPatternist::ReferenceCountedValue<QNetworkAccessManager>(new QNetworkAccessManager());
    m_messageHandler = new QPatternist::ReferenceCountedValue<QAbstractMessageHandler>(new QPatternist::ColoringMessageHandler());
}

QXmlSchemaPrivate::QXmlSchemaPrivate(const QPatternist::XsdSchemaContext::Ptr &schemaContext)
    : m_namePool(QXmlNamePool(schemaContext->namePool().data()))
    , m_userMessageHandler(0)
    , m_uriResolver(0)
    , m_userNetworkAccessManager(0)
    , m_schemaContext(schemaContext)
    , m_schemaParserContext(new QPatternist::XsdSchemaParserContext(m_namePool.d, m_schemaContext))
    , m_schemaIsValid(false)
{
    m_networkAccessManager = new QPatternist::ReferenceCountedValue<QNetworkAccessManager>(new QNetworkAccessManager());
    m_messageHandler = new QPatternist::ReferenceCountedValue<QAbstractMessageHandler>(new QPatternist::ColoringMessageHandler());
}

QXmlSchemaPrivate::QXmlSchemaPrivate(const QXmlSchemaPrivate &other)
    : QSharedData(other)
{
    m_namePool = other.m_namePool;
    m_userMessageHandler = other.m_userMessageHandler;
    m_uriResolver = other.m_uriResolver;
    m_userNetworkAccessManager = other.m_userNetworkAccessManager;
    m_messageHandler = other.m_messageHandler;
    m_networkAccessManager = other.m_networkAccessManager;

    m_schemaContext = other.m_schemaContext;
    m_schemaParserContext = other.m_schemaParserContext;
    m_schemaIsValid = other.m_schemaIsValid;
    m_documentUri = other.m_documentUri;
}

void QXmlSchemaPrivate::load(const QUrl &source, const QString &targetNamespace)
{
    m_documentUri = QPatternist::XPathHelper::normalizeQueryURI(source);

    m_schemaContext->setMessageHandler(messageHandler());
    m_schemaContext->setUriResolver(uriResolver());
    m_schemaContext->setNetworkAccessManager(networkAccessManager());

    const QPatternist::AutoPtr<QNetworkReply> reply(QPatternist::AccelTreeResourceLoader::load(source, m_schemaContext->networkAccessManager(),
                                                                                               m_schemaContext, QPatternist::AccelTreeResourceLoader::ContinueOnError));
    if (reply)
        load(reply.data(), source, targetNamespace);
}

void QXmlSchemaPrivate::load(const QByteArray &data, const QUrl &documentUri, const QString &targetNamespace)
{
    QByteArray localData(data);

    QBuffer buffer(&localData);
    buffer.open(QIODevice::ReadOnly);

    load(&buffer, documentUri, targetNamespace);
}

void QXmlSchemaPrivate::load(QIODevice *source, const QUrl &documentUri, const QString &targetNamespace)
{
    m_schemaParserContext = QPatternist::XsdSchemaParserContext::Ptr(new QPatternist::XsdSchemaParserContext(m_namePool.d, m_schemaContext));
    m_schemaIsValid = false;

    if (!source) {
        qWarning("A null QIODevice pointer cannot be passed.");
        return;
    }

    if (!source->isReadable()) {
        qWarning("The device must be readable.");
        return;
    }

    m_documentUri = QPatternist::XPathHelper::normalizeQueryURI(documentUri);
    m_schemaContext->setMessageHandler(messageHandler());
    m_schemaContext->setUriResolver(uriResolver());
    m_schemaContext->setNetworkAccessManager(networkAccessManager());

    QPatternist::XsdSchemaParser parser(m_schemaContext, m_schemaParserContext, source);
    parser.setDocumentURI(documentUri);
    parser.setTargetNamespace(targetNamespace);

    try {
        parser.parse();
        m_schemaParserContext->resolver()->resolve();

        m_schemaIsValid = true;
    } catch (QPatternist::Exception exception) {
        m_schemaIsValid = false;
    }
}

bool QXmlSchemaPrivate::isValid() const
{
    return m_schemaIsValid;
}

QXmlNamePool QXmlSchemaPrivate::namePool() const
{
    return m_namePool;
}

QUrl QXmlSchemaPrivate::documentUri() const
{
    return m_documentUri;
}

void QXmlSchemaPrivate::setMessageHandler(QAbstractMessageHandler *handler)
{
    m_userMessageHandler = handler;
}

QAbstractMessageHandler *QXmlSchemaPrivate::messageHandler() const
{
    if (m_userMessageHandler)
        return m_userMessageHandler;

    return m_messageHandler.data()->value;
}

void QXmlSchemaPrivate::setUriResolver(const QAbstractUriResolver *resolver)
{
    m_uriResolver = resolver;
}

const QAbstractUriResolver *QXmlSchemaPrivate::uriResolver() const
{
    return m_uriResolver;
}

void QXmlSchemaPrivate::setNetworkAccessManager(QNetworkAccessManager *networkmanager)
{
    m_userNetworkAccessManager = networkmanager;
}

QNetworkAccessManager *QXmlSchemaPrivate::networkAccessManager() const
{
    if (m_userNetworkAccessManager)
        return m_userNetworkAccessManager;

    return m_networkAccessManager.data()->value;
}

QT_END_NAMESPACE
