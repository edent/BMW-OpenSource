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

#ifndef Patternist_QObjectNodeModel_H
#define Patternist_QObjectNodeModel_H

#include <QSimpleXmlNodeModel>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QObject;
class PropertyToAtomicValue;

/**
 * @short Delegates QtCore's QObject into Patternist's QAbstractXmlNodeModel.
 * known as pre/post numbering.
 *
 * QObjectXmlModel sets the toggle on QXmlNodeModelIndex to @c true, if it
 * represents a property of the QObject. That is, if the QXmlNodeModelIndex is
 * an attribute.
 *
 * @author Frans Englich <frans.englich@nokia.com>
 */
class QObjectXmlModel : public QSimpleXmlNodeModel
{
  public:
    QObjectXmlModel(QObject *const object, const QXmlNamePool &np);

    QXmlNodeModelIndex root() const;

//! [0]
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &n1, const QXmlNodeModelIndex &n2) const;
    virtual QXmlName name(const QXmlNodeModelIndex &n) const;
    virtual QUrl documentUri(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &n) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &n) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex&) const;
//! [0]

  private:
    /**
     * The highest three bits are used to signify whether the node index
     * is an artificial node.
     *
     * @short if QXmlNodeModelIndex::additionalData() has the
     * QObjectPropery flag set, then the QXmlNodeModelIndex is an
     * attribute of the QObject element, and the remaining bits form
     * an offset to the QObject property that the QXmlNodeModelIndex
     * refers to.
     *
     */
//! [3]
    enum QObjectNodeType
    {
        IsQObject               = 0,
        QObjectProperty         = 1 << 26,
        MetaObjects             = 2 << 26,
        MetaObject              = 3 << 26,
        MetaObjectClassName     = 4 << 26,
        MetaObjectSuperClass    = 5 << 26,
        QObjectClassName        = 6 << 26
    };
//! [3]

//! [1]
    typedef QVector<const QMetaObject *> AllMetaObjects;
//! [1]
    AllMetaObjects allMetaObjects() const;

    static QObjectNodeType toNodeType(const QXmlNodeModelIndex &n);
    static bool isTypeSupported(QVariant::Type type);
    static inline QObject *asQObject(const QXmlNodeModelIndex &n);
    static inline bool isProperty(const QXmlNodeModelIndex n);
    static inline QMetaProperty toMetaProperty(const QXmlNodeModelIndex &n);
    /**
     * Returns the ancestors of @p n. Does therefore not include
     * @p n.
     */
    inline QXmlNodeModelIndex::List ancestors(const QXmlNodeModelIndex n) const;
    QXmlNodeModelIndex qObjectSibling(const int pos,
                                      const QXmlNodeModelIndex &n) const;
    QXmlNodeModelIndex metaObjectSibling(const int pos,
                                         const QXmlNodeModelIndex &n) const;

//! [2]
    const QUrl              m_baseURI;
    QObject *const          m_root;
//! [4]
    const AllMetaObjects    m_allMetaObjects;
//! [4]
//! [2]
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
