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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVector>
#include <QtXmlPatterns/QSimpleXmlNodeModel>

class FileTree : public QSimpleXmlNodeModel
{
 public:
    FileTree(const QXmlNamePool &namePool);

    QXmlNodeModelIndex nodeFor(const QString &fileName) const;

    //! [0]
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex&, const QXmlNodeModelIndex&) const;
    virtual QXmlName name(const QXmlNodeModelIndex &node) const;
    virtual QUrl documentUri(const QXmlNodeModelIndex &node) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &node) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &node) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &node) const;
    //! [0]
 protected:
    //! [1]
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex &element) const;
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex&) const;
    //! [1]

 private:
    //! [4]
    enum Type {
        File,
        Directory,
        AttributeFileName,
        AttributeFilePath,
        AttributeSize,
        AttributeMIMEType,
        AttributeSuffix
    };
    //! [4]

    inline QXmlNodeModelIndex nextSibling(const QXmlNodeModelIndex &nodeIndex,
                                          const QFileInfo &from,
                                          qint8 offset) const;
    inline const QFileInfo &toFileInfo(const QXmlNodeModelIndex &index) const;
    inline QXmlNodeModelIndex toNodeIndex(const QFileInfo &index,
                                          Type attributeName) const;
    inline QXmlNodeModelIndex toNodeIndex(const QFileInfo &index) const;

    /*
     One possible improvement is to use a hash, and use the &*&value()
     trick to get a pointer, which would be stored in data() instead
     of the index.
    */
    //! [2]
    mutable QVector<QFileInfo>  m_fileInfos;
    const QDir::Filters         m_filterAllowAll;
    const QDir::SortFlags       m_sortFlags;
    QVector<QXmlName>           m_names;
    //! [2]
};

    //! [3]
    //! [3]
