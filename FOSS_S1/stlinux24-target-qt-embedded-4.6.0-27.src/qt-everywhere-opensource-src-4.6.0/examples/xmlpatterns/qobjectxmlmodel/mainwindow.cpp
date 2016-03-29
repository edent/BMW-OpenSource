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

#include <QtGui>
#include <QtXmlPatterns>

#include "mainwindow.h"
#include "qobjectxmlmodel.h"
#include "xmlsyntaxhighlighter.h"

MainWindow::MainWindow()
{
    setupUi(this);

    new XmlSyntaxHighlighter(wholeTreeOutput->document());

    /* Setup the font. */
    {
        QFont font("Courier");
        font.setFixedPitch(true);

        wholeTree->setFont(font);
        wholeTreeOutput->setFont(font);
        htmlQueryEdit->setFont(font);
    }

    QXmlNamePool namePool;
    QObjectXmlModel qObjectModel(this, namePool);
    QXmlQuery query(namePool);

    /* The QObject tree as XML view. */
    {
        query.bindVariable("root", qObjectModel.root());
        query.setQuery(QUrl("qrc:/queries/wholeTree.xq"));

        Q_ASSERT(query.isValid());
        QByteArray output;
        QBuffer buffer(&output);
        buffer.open(QIODevice::WriteOnly);

        /* Let's the use the formatter, so it's a bit easier to read. */
        QXmlFormatter serializer(query, &buffer);

        query.evaluateTo(&serializer);
        buffer.close();

        {
            QFile queryFile(":/queries/wholeTree.xq");
            queryFile.open(QIODevice::ReadOnly);
            wholeTree->setPlainText(QString::fromUtf8(queryFile.readAll()));
            wholeTreeOutput->setPlainText(QString::fromUtf8(output.constData()));
        }
    }

    /* The QObject occurrence statistics as HTML view. */
    {
        query.setQuery(QUrl("qrc:/queries/statisticsInHTML.xq"));
        Q_ASSERT(query.isValid());

        QByteArray output;
        QBuffer buffer(&output);
        buffer.open(QIODevice::WriteOnly);

        /* Let's the use the serializer, so we gain a bit of speed. */
        QXmlSerializer serializer(query, &buffer);

        query.evaluateTo(&serializer);
        buffer.close();

        {
            QFile queryFile(":/queries/statisticsInHTML.xq");
            queryFile.open(QIODevice::ReadOnly);
            htmlQueryEdit->setPlainText(QString::fromUtf8(queryFile.readAll()));
            htmlOutput->setHtml(QString(output));
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("About QObject XML Model"),
                       tr("<p>The <b>QObject XML Model</b> example shows "
                          "how to use XQuery on top of data of your choice "
                          "without converting it to an XML document.</p>"
                          "<p>In this example a QSimpleXmlNodeModel subclass "
                          "makes it possible to query a QObject tree using "
                          "XQuery and retrieve the result as pointers to "
                          "QObjects, or as XML.</p>"
                          "<p>A possible use case of this could be to write "
                          "an application that tests a graphical interface "
                          "against Human Interface Guidelines, or that "
                          "queries an application's data which is modeled "
                          "using a QObject tree and dynamic properties."));
}


