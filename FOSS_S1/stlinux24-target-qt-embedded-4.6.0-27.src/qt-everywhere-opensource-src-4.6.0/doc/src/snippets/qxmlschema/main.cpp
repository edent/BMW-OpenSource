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

#include <QtCore>
#include <QtXmlPatterns>

class Schema
{
    public:
        void loadFromUrl() const;
        void loadFromFile() const;
        void loadFromData() const;
};

void Schema::loadFromUrl() const
{
//! [0]
    QUrl url("http://www.schema-example.org/myschema.xsd");

    QXmlSchema schema;
    if (schema.load(url) == true)
        qDebug() << "schema is valid";
    else
        qDebug() << "schema is invalid";
//! [0]
}

void Schema::loadFromFile() const
{
//! [1]
    QFile file("myschema.xsd");
    file.open(QIODevice::ReadOnly);

    QXmlSchema schema;
    schema.load(&file, QUrl::fromLocalFile(file.fileName()));

    if (schema.isValid())
        qDebug() << "schema is valid";
    else
        qDebug() << "schema is invalid";
//! [1]
}

void Schema::loadFromData() const
{
//! [2]
    QByteArray data( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<xsd:schema"
                     "        xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""
                     "        xmlns=\"http://qt.nokia.com/xmlschematest\""
                     "        targetNamespace=\"http://qt.nokia.com/xmlschematest\""
                     "        version=\"1.0\""
                     "        elementFormDefault=\"qualified\">"
                     "</xsd:schema>" );

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QXmlSchema schema;
    schema.load(&buffer);

    if (schema.isValid())
        qDebug() << "schema is valid";
    else
        qDebug() << "schema is invalid";
//! [2]
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    Schema schema;

    schema.loadFromUrl();
    schema.loadFromFile();
    schema.loadFromData();

    return 0;
}
