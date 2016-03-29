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

#include "qxsdinstancereader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdInstanceReader::XsdInstanceReader(const QAbstractXmlNodeModel *model, const XsdSchemaContext::Ptr &context)
    : m_context(context)
    , m_model(model->iterate(model->root(QXmlNodeModelIndex()), QXmlNodeModelIndex::AxisChild))
{
}

bool XsdInstanceReader::atEnd() const
{
    return (m_model.current() == AbstractXmlPullProvider::EndOfInput);
}

void XsdInstanceReader::readNext()
{
    m_model.next();

    if (m_model.current() == AbstractXmlPullProvider::StartElement) {
        m_cachedAttributes = m_model.attributes();
        m_cachedAttributeItems = m_model.attributeItems();
        m_cachedSourceLocation = m_model.sourceLocation();
        m_cachedItem = QXmlItem(m_model.index());
    }
}

bool XsdInstanceReader::isStartElement() const
{
    return (m_model.current() == AbstractXmlPullProvider::StartElement);
}

bool XsdInstanceReader::isEndElement() const
{
    return (m_model.current() == AbstractXmlPullProvider::EndElement);
}

bool XsdInstanceReader::hasChildText() const
{
    const QXmlNodeModelIndex index = m_model.index();
    QXmlNodeModelIndex::Iterator::Ptr it = index.model()->iterate(index, QXmlNodeModelIndex::AxisChild);

    QXmlNodeModelIndex currentIndex = it->next();
    while (!currentIndex.isNull()) {
        if (currentIndex.kind() == QXmlNodeModelIndex::Text)
            return true;

        currentIndex = it->next();
    }

    return false;
}

bool XsdInstanceReader::hasChildElement() const
{
    const QXmlNodeModelIndex index = m_model.index();
    QXmlNodeModelIndex::Iterator::Ptr it = index.model()->iterate(index, QXmlNodeModelIndex::AxisChild);

    QXmlNodeModelIndex currentIndex = it->next();
    while (!currentIndex.isNull()) {
        if (currentIndex.kind() == QXmlNodeModelIndex::Element)
            return true;

        currentIndex = it->next();
    }

    return false;
}

QXmlName XsdInstanceReader::name() const
{
    return m_model.name();
}

QXmlName XsdInstanceReader::convertToQName(const QString &name) const
{
    const int pos = name.indexOf(QLatin1Char(':'));

    QXmlName::PrefixCode prefixCode = 0;
    QXmlName::NamespaceCode namespaceCode;
    QXmlName::LocalNameCode localNameCode;
    if (pos != -1) {
        prefixCode = m_context->namePool()->allocatePrefix(name.left(pos));
        namespaceCode = m_cachedItem.toNodeModelIndex().namespaceForPrefix(prefixCode);
        localNameCode = m_context->namePool()->allocateLocalName(name.mid(pos + 1));
    } else {
        prefixCode = StandardPrefixes::empty;
        namespaceCode = m_cachedItem.toNodeModelIndex().namespaceForPrefix(prefixCode);
        if (namespaceCode == -1)
            namespaceCode = StandardNamespaces::empty;
        localNameCode = m_context->namePool()->allocateLocalName(name);
    }

    return QXmlName(namespaceCode, localNameCode, prefixCode);
}

bool XsdInstanceReader::hasAttribute(const QXmlName &name) const
{
    return m_cachedAttributes.contains(name);
}

QString XsdInstanceReader::attribute(const QXmlName &name) const
{
    Q_ASSERT(m_cachedAttributes.contains(name));

    return m_cachedAttributes.value(name);
}

QSet<QXmlName> XsdInstanceReader::attributeNames() const
{
    return m_cachedAttributes.keys().toSet();
}

QString XsdInstanceReader::text() const
{
    const QXmlNodeModelIndex index = m_model.index();
    QXmlNodeModelIndex::Iterator::Ptr it = index.model()->iterate(index, QXmlNodeModelIndex::AxisChild);

    QString result;

    QXmlNodeModelIndex currentIndex = it->next();
    while (!currentIndex.isNull()) {
        if (currentIndex.kind() == QXmlNodeModelIndex::Text) {
            result.append(Item(currentIndex).stringValue());
        }

        currentIndex = it->next();
    }

    return result;
}

QXmlItem XsdInstanceReader::item() const
{
    return m_cachedItem;
}

QXmlItem XsdInstanceReader::attributeItem(const QXmlName &name) const
{
    return m_cachedAttributeItems.value(name);
}

QSourceLocation XsdInstanceReader::sourceLocation() const
{
    return m_cachedSourceLocation;
}

QVector<QXmlName> XsdInstanceReader::namespaceBindings(const QXmlNodeModelIndex &index) const
{
    return index.namespaceBindings();
}

QT_END_NAMESPACE
