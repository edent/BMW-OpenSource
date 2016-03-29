/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "main.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

QT_USE_NAMESPACE

int main(int argc, char **argv)
{
    enum ExitCode
    {
        Valid = 0,
        Invalid,
        ParseError
    };

    enum ExecutionMode
    {
        InvalidMode,
        SchemaOnlyMode,
        InstanceOnlyMode,
        SchemaAndInstanceMode
    };

    const QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QLatin1String("xmlpatternsvalidator"));

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 2 && arguments.size() != 3) {
        qDebug() << QXmlPatternistCLI::tr("usage: xmlpatternsvalidator (<schema url> | <instance url> <schema url> | <instance url>)");
        return ParseError;
    }

    // parse command line arguments
    ExecutionMode mode = InvalidMode;

    QUrl schemaUri;
    QUrl instanceUri;

    {
        QUrl url = arguments[1];

        if (url.isRelative())
            url = QUrl::fromLocalFile(arguments[1]);

        if (arguments.size() == 2) {
            // either it is a schema or instance document

            if (arguments[1].toLower().endsWith(QLatin1String(".xsd"))) {
                schemaUri = url;
                mode = SchemaOnlyMode;
            } else {
                // as we could validate all types of xml documents, don't check the extension here
                instanceUri = url;
                mode = InstanceOnlyMode;
            }
        } else if (arguments.size() == 3) {
            instanceUri = url;
            schemaUri = arguments[2];

            if (schemaUri.isRelative())
                schemaUri = QUrl::fromLocalFile(schemaUri.toString());

            mode = SchemaAndInstanceMode;
        }
    }

    // Validate schema
    QXmlSchema schema;
    if (InstanceOnlyMode != mode) {
        schema.load(schemaUri);
        if (!schema.isValid())
            return Invalid;
    }

    if (SchemaOnlyMode == mode)
        return Valid;

    // Validate instance
    QXmlSchemaValidator validator(schema);
    if (validator.validate(instanceUri))
        return Valid;

    return Invalid;
}
