/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QVector>
#include <QtDebug>

#include <QCoreApplication>
#include <QMetaProperty>
#include <QXmlQuery>
#include <QXmlResultItems>

#include "qobjectxmlmodel.h"

QT_BEGIN_NAMESPACE

/*
<metaObjects>
    <metaObject className="QObject"/>
    <metaObject className="QWidget" superClass="QObject">
    </metaObject>
    ...
</metaObjects>
<QObject objectName="MyWidget" property1="..." property2="..."> <!-- This is root() -->
    <QObject objectName="MyFOO" property1="..."/>
    ....
</QObject>
*/

QObjectXmlModel::QObjectXmlModel(QObject *const object, const QXmlNamePool &np)
    : QSimpleXmlNodeModel(np),
      m_baseURI(QUrl::fromLocalFile(QCoreApplication::applicationFilePath())),
      m_root(object),
      m_allMetaObjects(allMetaObjects())
{
    Q_ASSERT(m_baseURI.isValid());
}

//! [5]
QXmlNodeModelIndex QObjectXmlModel::qObjectSibling(const int pos, const QXmlNodeModelIndex &n) const
{
    Q_ASSERT(pos == 1 || pos == -1);
    Q_ASSERT(asQObject(n));

    const QObject *parent = asQObject(n)->parent();
    if (parent) {
        const QList<QObject *> &children = parent->children();
        const int siblingPos = children.indexOf(asQObject(n)) + pos;

        if (siblingPos >= 0 && siblingPos < children.count())
            return createIndex(children.at(siblingPos));
        else
            return QXmlNodeModelIndex();
    }
    else
        return QXmlNodeModelIndex();
}
//! [5]

//! [1]
QObjectXmlModel::QObjectNodeType QObjectXmlModel::toNodeType(const QXmlNodeModelIndex &n)
{
    return QObjectNodeType(n.additionalData() & (15 << 26));
}
//! [1]

//! [9]
QObjectXmlModel::AllMetaObjects QObjectXmlModel::allMetaObjects() const
{
    QXmlQuery query(namePool());
    query.bindVariable("root", root());
    query.setQuery("declare variable $root external;"
                   "$root/descendant-or-self::QObject");
    Q_ASSERT(query.isValid());

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem i(result.next());

    AllMetaObjects objects;
    while (!i.isNull()) {
        const QMetaObject *moo = asQObject(i.toNodeModelIndex())->metaObject();
        while (moo) {
            if (!objects.contains(moo))
                objects.append(moo);
            moo = moo->superClass();
        }
        i = result.next();
    }

    Q_ASSERT(!objects.contains(0));
    return objects;
}
//! [9]

QXmlNodeModelIndex QObjectXmlModel::metaObjectSibling(const int pos, const QXmlNodeModelIndex &n) const
{
    Q_ASSERT(pos == 1 || pos == -1);
    Q_ASSERT(!n.isNull());

    const int indexOf = m_allMetaObjects.indexOf(static_cast<const QMetaObject *>(n.internalPointer())) + pos;

    if (indexOf >= 0 && indexOf < m_allMetaObjects.count())
        return createIndex(const_cast<QMetaObject *>(m_allMetaObjects.at(indexOf)), MetaObject);
    else
        return QXmlNodeModelIndex();
}

//! [2]
QXmlNodeModelIndex QObjectXmlModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &n) const
{
    switch (toNodeType(n))
    {
        case IsQObject:
        {
            switch (axis)
            {
                case Parent:
                    return createIndex(asQObject(n)->parent());

                case FirstChild:
                {
                    if (!asQObject(n) || asQObject(n)->children().isEmpty())
                        return QXmlNodeModelIndex();
                    else
                        return createIndex(asQObject(n)->children().first());
                }
                
                case NextSibling:
                    return qObjectSibling(1, n);

//! [10]                    
                case PreviousSibling:
                {
                    if (asQObject(n) == m_root)
                        return createIndex(qint64(0), MetaObjects);
                    else
                        return qObjectSibling(-1, n);
                }
//! [10]                    
            }
            Q_ASSERT(false);
        }

//! [7]
        case QObjectClassName:
        case QObjectProperty:
        {
            Q_ASSERT(axis == Parent);
            return createIndex(asQObject(n));
        }
//! [7]
//! [2]
//! [3]

//! [11]        
        case MetaObjects:
        {
            switch (axis)
            {
                case Parent:
                    return QXmlNodeModelIndex();
                case PreviousSibling:
                    return QXmlNodeModelIndex();
                case NextSibling:
                    return root();
                case FirstChild:
                {
                    return createIndex(const_cast<QMetaObject*>(m_allMetaObjects.first()),MetaObject);
                }
            }
            Q_ASSERT(false);
        }
//! [11]        

        case MetaObject:
        {
            switch (axis)
            {
                case FirstChild:
                    return QXmlNodeModelIndex();
                case Parent:
                    return createIndex(qint64(0), MetaObjects);
                case PreviousSibling:
                    return metaObjectSibling(-1, n);
                case NextSibling:
                    return metaObjectSibling(1, n);
            }
        }

        case MetaObjectClassName:
        case MetaObjectSuperClass:
        {
            Q_ASSERT(axis == Parent);
            return createIndex(asQObject(n), MetaObject);
        }
//! [3]
//! [4]
    }

    Q_ASSERT(false);
    return QXmlNodeModelIndex();
}
//! [4]

//! [6]
QVector<QXmlNodeModelIndex> QObjectXmlModel::attributes(const QXmlNodeModelIndex& n) const
{
     QVector<QXmlNodeModelIndex> result;
     QObject *const object = asQObject(n);

     switch(toNodeType(n))
     {
        case IsQObject:
        {
            const QMetaObject *const metaObject = object->metaObject();
            const int count = metaObject->propertyCount();
            result.append(createIndex(object, QObjectClassName));

            for (int i = 0; i < count; ++i) {
                const QMetaProperty qmp(metaObject->property(i));
                const int ii = metaObject->indexOfProperty(qmp.name());
                if (i == ii)
                    result.append(createIndex(object, QObjectProperty | i));
            }
            return result;
        }
//! [6]

        case MetaObject:
        {
            result.append(createIndex(object, MetaObjectClassName));
            result.append(createIndex(object, MetaObjectSuperClass));
            return result;
        }
//! [8]
        default:
            return QVector<QXmlNodeModelIndex>();
     }
}
//! [8]

QObject *QObjectXmlModel::asQObject(const QXmlNodeModelIndex &n)
{
    return static_cast<QObject *>(n.internalPointer());
}

bool QObjectXmlModel::isProperty(const QXmlNodeModelIndex n)
{
    return n.additionalData() & QObjectProperty;
}

QUrl QObjectXmlModel::documentUri(const QXmlNodeModelIndex& ) const
{
    return m_baseURI;
}

QXmlNodeModelIndex::NodeKind QObjectXmlModel::kind(const QXmlNodeModelIndex& n) const
{
    switch (toNodeType(n))
    {
        case IsQObject:
        case MetaObject:
        case MetaObjects:
            return QXmlNodeModelIndex::Element;

        case QObjectProperty:
        case MetaObjectClassName:
        case MetaObjectSuperClass:
        case QObjectClassName:
            return QXmlNodeModelIndex::Attribute;
    }

    Q_ASSERT(false);
    return QXmlNodeModelIndex::Element;
}

QXmlNodeModelIndex::DocumentOrder QObjectXmlModel::compareOrder(const QXmlNodeModelIndex& , const QXmlNodeModelIndex& ) const
{
    return QXmlNodeModelIndex::Follows; // TODO
}

//! [0]
QXmlNodeModelIndex QObjectXmlModel::root() const
{
    return createIndex(m_root);
}
//! [0]

QXmlNodeModelIndex QObjectXmlModel::root(const QXmlNodeModelIndex& n) const
{
    QObject *p = asQObject(n);
    Q_ASSERT(p);

    do {
        QObject *const candidate = p->parent();
        if (candidate)
            p = candidate;
        else
            break;
    }
    while (true);

    return createIndex(p);
}

/*!
  We simply throw all of them into a QList and
  return an iterator over it.
 */
QXmlNodeModelIndex::List QObjectXmlModel::ancestors(const QXmlNodeModelIndex n) const
{
    const QObject *p = asQObject(n);
    Q_ASSERT(p);

    QXmlNodeModelIndex::List result;
    do {
        QObject *const candidate = p->parent();
        if (candidate) {
            result.append(createIndex(candidate, 0));
            p = candidate;
        }
        else
            break;
    }
    while (true);

    return result;
}

QMetaProperty QObjectXmlModel::toMetaProperty(const QXmlNodeModelIndex &n)
{
    const int propertyOffset = n.additionalData() & (~QObjectProperty);
    const QObject *const qo = asQObject(n);
    return qo->metaObject()->property(propertyOffset);
}

QXmlName QObjectXmlModel::name(const QXmlNodeModelIndex &n) const
{
    switch (toNodeType(n))
    {
        case IsQObject:
            return QXmlName(namePool(), QLatin1String("QObject"));
        case MetaObject:
            return QXmlName(namePool(), QLatin1String("metaObject"));
        case QObjectClassName:
        case MetaObjectClassName:
            return QXmlName(namePool(), QLatin1String("className"));
        case QObjectProperty:
            return QXmlName(namePool(), toMetaProperty(n).name());
        case MetaObjects:
            return QXmlName(namePool(), QLatin1String("metaObjects"));
        case MetaObjectSuperClass:
            return QXmlName(namePool(), QLatin1String("superClass"));
    }

    Q_ASSERT(false);
    return QXmlName();
}

QVariant QObjectXmlModel::typedValue(const QXmlNodeModelIndex &n) const
{
    switch (toNodeType(n))
    {
        case QObjectProperty:
        {
           const QVariant &candidate = toMetaProperty(n).read(asQObject(n));
           if (isTypeSupported(candidate.type()))
               return candidate;
           else
               return QVariant();
        }
        
        case MetaObjectClassName:
            return QVariant(static_cast<QMetaObject*>(n.internalPointer())->className());
            
        case MetaObjectSuperClass:
        {
            const QMetaObject *const superClass = static_cast<QMetaObject*>(n.internalPointer())->superClass();
            if (superClass)
                return QVariant(superClass->className());
            else
                return QVariant();
        }
        
        case QObjectClassName:
            return QVariant(asQObject(n)->metaObject()->className());
            
        default:
            return QVariant();
    }
}

/*!
 Returns \c true if QVariants of type \a type can be used
 in QtXmlPatterns, otherwise \c false.
 */
bool QObjectXmlModel::isTypeSupported(QVariant::Type type)
{
    /* See data/qatomicvalue.cpp too. */
    switch (type)
    {
        /* Fallthrough all these. */
        case QVariant::Char:
        case QVariant::String:
        case QVariant::Url:
        case QVariant::Bool:
        case QVariant::ByteArray:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Date:
        case QVariant::DateTime:
        case QVariant::Time:
        case QVariant::Double:
            return true;
        default:
            return false;
    }
}

QT_END_NAMESPACE
