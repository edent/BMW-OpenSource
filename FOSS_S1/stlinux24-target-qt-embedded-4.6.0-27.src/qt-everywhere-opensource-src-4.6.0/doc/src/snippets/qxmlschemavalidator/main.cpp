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

class SchemaValidator
{
    public:
        void validateFromUrl() const;
        void validateFromFile() const;
        void validateFromData() const;
        void validateComplete() const;

    private:
        QXmlSchema getSchema() const;
};

void SchemaValidator::validateFromUrl() const
{
//! [0]
    const QXmlSchema schema = getSchema();

    const QUrl url("http://www.schema-example.org/test.xml");

    QXmlSchemaValidator validator(schema);
    if (validator.validate(url))
        qDebug() << "instance document is valid";
    else
        qDebug() << "instance document is invalid";
//! [0]
}

void SchemaValidator::validateFromFile() const
{
//! [1]
    const QXmlSchema schema = getSchema();

    QFile file("test.xml");
    file.open(QIODevice::ReadOnly);

    QXmlSchemaValidator validator(schema);
    if (validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
        qDebug() << "instance document is valid";
    else
        qDebug() << "instance document is invalid";
//! [1]
}

void SchemaValidator::validateFromData() const
{
//! [2]
    const QXmlSchema schema = getSchema();

    QByteArray data("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    "<test></test>");

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QXmlSchemaValidator validator(schema);
    if (validator.validate(&buffer))
        qDebug() << "instance document is valid";
    else
        qDebug() << "instance document is invalid";
//! [2]
}

QXmlSchema SchemaValidator::getSchema() const
{
    QByteArray data("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    "<xsd:schema"
                    "        xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""
                    "        xmlns=\"http://qt.nokia.com/xmlschematest\""
                    "        targetNamespace=\"http://qt.nokia.com/xmlschematest\""
                    "        version=\"1.0\""
                    "        elementFormDefault=\"qualified\">"
                    "</xsd:schema>");

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QXmlSchema schema;
    schema.load(&buffer);

    return schema;
}

void SchemaValidator::validateComplete() const
{
//! [3]
    QUrl schemaUrl("file:///home/user/schema.xsd");

    QXmlSchema schema;
    schema.load(schemaUrl);

    if (schema.isValid()) {
        QFile file("test.xml");
        file.open(QIODevice::ReadOnly);

        QXmlSchemaValidator validator(schema);
        if (validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
            qDebug() << "instance document is valid";
        else
            qDebug() << "instance document is invalid";
    }
//! [3]
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    SchemaValidator validator;

    validator.validateFromUrl();
    validator.validateFromFile();
    validator.validateFromData();
    validator.validateComplete();

    return 0;
}
