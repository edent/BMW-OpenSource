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

#include <QtCore>

void parseHtmlFile(QTextStream &out, const QString &fileName) {
    QFile file(fileName);

    out << "Analysis of HTML file: " << fileName << endl;

    if (!file.open(QIODevice::ReadOnly)) {
        out << "  Couldn't open the file." << endl << endl << endl;
        return;
    }

//! [0]
    QXmlStreamReader reader(&file);
//! [0]

//! [1]
    int paragraphCount = 0;
    QStringList links;
    QString title;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == "title")
                title = reader.readElementText();
            else if(reader.name() == "a")
                links.append(reader.attributes().value("href").toString());
            else if(reader.name() == "p")
                ++paragraphCount;
        }
    }
//! [1]

//! [2]
    if (reader.hasError()) {
        out << "  The HTML file isn't well-formed: " << reader.errorString()
            << endl << endl << endl;
        return;
    }
//! [2]

    out << "  Title: \"" << title << "\"" << endl
        << "  Number of paragraphs: " << paragraphCount << endl
        << "  Number of links: " << links.size() << endl
        << "  Showing first few links:" << endl;

    while(links.size() > 5)
        links.removeLast();

    foreach(QString link, links)
        out << "    " << link << endl;
    out << endl << endl;
}

int main(int argc, char **argv)
{
    // intialize QtCore application
    QCoreApplication app(argc, argv);

    // get a list of all html files in the current directory
    QStringList filter;
    filter << "*.htm";
    filter << "*.html";
    QStringList htmlFiles = QDir::current().entryList(filter, QDir::Files);

    QTextStream out(stdout);

    if (htmlFiles.isEmpty()) {
        out << "No html files available.";
        return 1;
    }

    // parse each html file and write the result to file/stream
    foreach(QString file, htmlFiles)
        parseHtmlFile(out, file);

    return 0;
}
