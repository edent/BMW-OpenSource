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

#include "querymainwindow.h"
#include "xmlsyntaxhighlighter.h"

//! [0]
QueryMainWindow::QueryMainWindow() : ui_defaultQueries(0)
{
    setupUi(this);

    new XmlSyntaxHighlighter(qFindChild<QTextEdit*>(this, "inputTextEdit")->document());
    new XmlSyntaxHighlighter(qFindChild<QTextEdit*>(this, "outputTextEdit")->document());

    ui_defaultQueries = qFindChild<QComboBox*>(this, "defaultQueries");
    QMetaObject::connectSlotsByName(this);
    connect(ui_defaultQueries, SIGNAL(currentIndexChanged(int)), SLOT(displayQuery(int)));

    loadInputFile();
    const QStringList queries(QDir(":/files/", "*.xq").entryList());
    int len = queries.count();
    for(int i = 0; i < len; ++i)
        ui_defaultQueries->addItem(queries.at(i));
}
//! [0]


//! [1]
void QueryMainWindow::displayQuery(int index)
{
    QFile queryFile(QString(":files/") + ui_defaultQueries->itemText(index));
    queryFile.open(QIODevice::ReadOnly);
    const QString query(QString::fromLatin1(queryFile.readAll()));
    qFindChild<QTextEdit*>(this, "queryTextEdit")->setPlainText(query);

    evaluate(query);
}
//! [1]


void QueryMainWindow::loadInputFile()
{
    QFile forView;
    forView.setFileName(":/files/cookbook.xml");
    if (!forView.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this,
            tr("Unable to open file"), forView.errorString());
        return;
    }

    QTextStream in(&forView);
    QString inputDocument = in.readAll();
    qFindChild<QTextEdit*>(this, "inputTextEdit")->setPlainText(inputDocument);
}


//! [2]
void QueryMainWindow::evaluate(const QString &str)
{
    QFile sourceDocument;
    sourceDocument.setFileName(":/files/cookbook.xml");
    sourceDocument.open(QIODevice::ReadOnly);

    QByteArray outArray;
    QBuffer buffer(&outArray);
    buffer.open(QIODevice::ReadWrite);

    QXmlQuery query;
    query.bindVariable("inputDocument", &sourceDocument);
    query.setQuery(str);
    if (!query.isValid())
        return;

    QXmlFormatter formatter(query, &buffer);
    if (!query.evaluateTo(&formatter))
        return;

    buffer.close();
    qFindChild<QTextEdit*>(this, "outputTextEdit")->setPlainText(QString::fromUtf8(outArray.constData()));

}
//! [2]

