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

#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>

/*
    This class exists for the sole purpose of creating a translation context.
 */
class PrettyPrint
{
    Q_DECLARE_TR_FUNCTIONS(PrettyPrint)
};

int main(int argc, char *argv[])
{
    enum ExitCode
    {
        Success,
        ParseFailure,
        ArgumentError,
        WriteError,
        FileFailure
    };

    QCoreApplication app(argc, argv);

    QTextStream errorStream(stderr);

    if (argc != 2)
    {
        errorStream << PrettyPrint::tr(
                       "Usage: prettyprint <path to XML file>\n");
        return ArgumentError;
    }

    QString inputFilePath(QCoreApplication::arguments().at(1));
    QFile inputFile(inputFilePath);

    if (!QFile::exists(inputFilePath))
    {
        errorStream << PrettyPrint::tr(
                       "File %1 does not exist.\n").arg(inputFilePath);
        return FileFailure;

    } else if (!inputFile.open(QIODevice::ReadOnly)) {
        errorStream << PrettyPrint::tr(
                       "Failed to open file %1.\n").arg(inputFilePath);
        return FileFailure;
    }

    QFile outputFile;
    if (!outputFile.open(stdout, QIODevice::WriteOnly))
    {
        QTextStream(stderr) << PrettyPrint::tr("Failed to open stdout.");
        return WriteError;
    }

    QXmlStreamReader reader(&inputFile);
    int indentation = 0;
    QHash<int,QPair<int, int> > indentationStack;

    while (!reader.atEnd())
    {
        reader.readNext();
        if (reader.isStartElement()) {
            indentationStack[indentation] = QPair<int,int>(
                reader.lineNumber(), reader.columnNumber());
            indentation += 1;
        } else if (reader.isEndElement()) {
            indentationStack.remove(indentation);
            indentation -= 1;
        }

        if (reader.error())
        {
            errorStream << PrettyPrint::tr(
                           "Error: %1 in file %2 at line %3, column %4.\n").arg(
                               reader.errorString(), inputFilePath,
                               QString::number(reader.lineNumber()),
                               QString::number(reader.columnNumber()));
            if (indentationStack.contains(indentation-1)) {
                int line = indentationStack[indentation-1].first;
                int column = indentationStack[indentation-1].second;
                errorStream << PrettyPrint::tr(
                               "Opened at line %1, column %2.\n").arg(
                                   QString::number(line),
                                   QString::number(column));
            }
            return ParseFailure;

        } else if (reader.isStartElement() && !reader.name().isEmpty()) {
            outputFile.write(QByteArray().fill(' ', indentation));
            outputFile.write(reader.name().toString().toLocal8Bit());
            outputFile.write(QString(" line %1, column %2\n").arg(
                reader.lineNumber()).arg(reader.columnNumber()).toLocal8Bit());
        }
    }

    return Success;
}
