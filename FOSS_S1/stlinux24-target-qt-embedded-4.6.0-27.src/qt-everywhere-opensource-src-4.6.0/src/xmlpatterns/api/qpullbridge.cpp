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

#include <QVariant>

#include "qabstractxmlnodemodel_p.h"
#include "qitemmappingiterator_p.h"
#include "qitem_p.h"
#include "qxmlname.h"
#include "qxmlquery_p.h"

#include "qpullbridge_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/*!
  \brief Bridges a QPatternist::SequenceIterator to QAbstractXmlPullProvider.
  \class QPatternist::PullBridge
  \internal
  \reentrant
  \ingroup xml-tools

  The approach of this class is rather straight forward since QPatternist::SequenceIterator
  and QAbstractXmlPullProvider are conceptually similar. While QPatternist::SequenceIterator only
  delivers top level items(since it's not an event stream, it's a list of items), PullBridge
  needs to recursively iterate the children of nodes too, which is achieved through the
  stack m_iterators.
 */

AbstractXmlPullProvider::Event PullBridge::next()
{
    m_index = m_iterators.top().second->next();

    if(!m_index.isNull())
    {
        Item item(m_index);

        if(item && item.isAtomicValue())
            m_current = AtomicValue;
        else
        {
            Q_ASSERT(item.isNode());

            switch(m_index.kind())
            {
                case QXmlNodeModelIndex::Attribute:
                {
                    m_current = Attribute;
                    break;
                }
                case QXmlNodeModelIndex::Comment:
                {
                    m_current = Comment;
                    break;
                }
                case QXmlNodeModelIndex::Element:
                {
                    m_iterators.push(qMakePair(StartElement, m_index.iterate(QXmlNodeModelIndex::AxisChild)));
                    m_current = StartElement;
                    break;
                }
                case QXmlNodeModelIndex::Document:
                {
                    m_iterators.push(qMakePair(StartDocument, m_index.iterate(QXmlNodeModelIndex::AxisChild)));
                    m_current = StartDocument;
                    break;
                }
                case QXmlNodeModelIndex::Namespace:
                {
                    m_current = Namespace;
                    break;
                }
                case QXmlNodeModelIndex::ProcessingInstruction:
                {
                    m_current = ProcessingInstruction;
                    break;
                }
                case QXmlNodeModelIndex::Text:
                {
                    m_current = Text;
                    break;
                }
            }
        }
    }
    else
    {
        if(m_iterators.isEmpty())
            m_current = EndOfInput;
        else
        {
            switch(m_iterators.top().first)
            {
                case StartOfInput:
                {
                    m_current = EndOfInput;
                    break;
                }
                case StartElement:
                {
                    m_current = EndElement;
                    m_iterators.pop();
                    break;
                }
                case StartDocument:
                {
                    m_current = EndDocument;
                    m_iterators.pop();
                    break;
                }
                default:
                {
                    Q_ASSERT_X(false, Q_FUNC_INFO,
                               "Invalid value.");
                    m_current = EndOfInput;
                }
            }
        }

    }

    return m_current;
}

AbstractXmlPullProvider::Event PullBridge::current() const
{
    return m_current;
}

QXmlNodeModelIndex PullBridge::index() const
{
    return m_index;
}

QSourceLocation PullBridge::sourceLocation() const
{
    return m_index.model()->sourceLocation(m_index);
}

QXmlName PullBridge::name() const
{
    return m_index.name();
}

QVariant PullBridge::atomicValue() const
{
    return QVariant();
}

QString PullBridge::stringValue() const
{
    return QString();
}

QHash<QXmlName, QString> PullBridge::attributes()
{
    Q_ASSERT(m_current == StartElement);

    QHash<QXmlName, QString> attributes;

    QXmlNodeModelIndex::Iterator::Ptr it = m_index.iterate(QXmlNodeModelIndex::AxisAttribute);
    QXmlNodeModelIndex index = it->next();
    while (!index.isNull()) {
        const Item attribute(index);
        attributes.insert(index.name(), index.stringValue());

        index = it->next();
    }

    return attributes;
}

QHash<QXmlName, QXmlItem> PullBridge::attributeItems()
{
    Q_ASSERT(m_current == StartElement);

    QHash<QXmlName, QXmlItem> attributes;

    QXmlNodeModelIndex::Iterator::Ptr it = m_index.iterate(QXmlNodeModelIndex::AxisAttribute);
    QXmlNodeModelIndex index = it->next();
    while (!index.isNull()) {
        const Item attribute(index);
        attributes.insert(index.name(), QXmlItem(index));

        index = it->next();
    }

    return attributes;
}

QT_END_NAMESPACE

