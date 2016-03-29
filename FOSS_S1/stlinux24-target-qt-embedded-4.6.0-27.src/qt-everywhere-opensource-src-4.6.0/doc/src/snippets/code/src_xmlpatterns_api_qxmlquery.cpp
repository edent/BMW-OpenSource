/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), value);
//! [0]


{
//! [1]
    QByteArray myDocument;
    QBuffer buffer(&myDocument); // This is a QIODevice.
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("myDocument", &buffer);
    query.setQuery("doc($myDocument)");
//! [1]
}


{
    QIODevice *device = 0;
//! [2]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), device);
//! [2]

}

{
    QIODevice *myOutputDevice = 0;
//! [3]
    QFile xq("myquery.xq");

    QXmlQuery query;
    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlSerializer serializer(query, myOutputDevice);
    query.evaluateTo(&serializer);
//! [3]
}

{
    QIODevice *myOutputDevice = 0;
//! [4]
    QFile xq("myquery.xq");
    QString fileName("the filename");
    QString publisherName("the publisher");
    qlonglong year = 1234;

    QXmlQuery query;

    query.bindVariable("file", QVariant(fileName));
    query.bindVariable("publisher", QVariant(publisherName));
    query.bindVariable("year", QVariant(year));

    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlSerializer serializer(query, myOutputDevice);
    query.evaluateTo(&serializer);
//! [4]
}

{
//! [5]
    QFile xq("myquery.xq");
    QString fileName("the filename");
    QString publisherName("the publisher");
    qlonglong year = 1234;

    QXmlQuery query;

    query.bindVariable("file", QVariant(fileName));
    query.bindVariable("publisher", QVariant(publisherName));
    query.bindVariable("year", QVariant(year));

    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull()) {
        if (item.isAtomicValue()) {
            QVariant v = item.toAtomicValue();
            switch (v.type()) {
                case QVariant::LongLong:
                    // xs:integer
                    break;
                case QVariant::String:
                    // xs:string
                    break;
                default:
                    // error
                    break;
            }
        }
        else if (item.isNode()) {
            QXmlNodeModelIndex i = item.toNodeModelIndex();
            // process node
        }
        item = result.next();
    }
//! [5]
}

{
//! [6]
    QFile xq("myquery.xq");

    QXmlQuery query;
    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull()) {
        if (item.isAtomicValue()) {
            QVariant v = item.toAtomicValue();
            switch (v.type()) {
                case QVariant::LongLong:
                    // xs:integer
                    break;
                case QVariant::String:
                    // xs:string
                    break;
                default:
                    if (v.userType() == qMetaTypeId<QXmlName>()) {
                        QXmlName n = qVariantValue<QXmlName>(v);
                        // process QXmlName n...
                    }
                    else {
                        // error
                    }
                    break;
            }
        }
        else if (item.isNode()) {
            QXmlNodeModelIndex i = item.toNodeModelIndex();
            // process node
        }
        item = result.next();
    }
//! [6]
}

{
    QIODevice *out = 0;
//! [7]
    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(QUrl("myInput.xml"));
    query.setQuery(QUrl("myStylesheet.xsl"));
    query.evaluateTo(out);
//! [7]
}
