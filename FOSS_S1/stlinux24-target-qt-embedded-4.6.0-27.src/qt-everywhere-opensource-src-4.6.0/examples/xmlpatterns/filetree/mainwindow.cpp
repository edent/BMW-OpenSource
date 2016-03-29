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
#include <QLibraryInfo>
#include <QtXmlPatterns>

#include "mainwindow.h"
#include "xmlsyntaxhighlighter.h"

//! [0]
MainWindow::MainWindow() : m_fileTree(m_namePool)
{
    setupUi(this);

    new XmlSyntaxHighlighter(fileTree->document());

    // Set up the font.
    {
        QFont font("Courier",10);
        font.setFixedPitch(true);

        fileTree->setFont(font);
        queryEdit->setFont(font);
        output->setFont(font);
    }

    const QString dir(QLibraryInfo::location(QLibraryInfo::ExamplesPath) + "/xmlpatterns/filetree");
    qDebug() << dir;

    if (QDir(dir).exists())
        loadDirectory(dir);
    else
        fileTree->setPlainText(tr("Use the Open menu entry to select a directory."));

    const QStringList queries(QDir(":/queries/", "*.xq").entryList());
    int len = queries.count();

    for (int i = 0; i < len; ++i)
        queryBox->addItem(queries.at(i));

}
//! [0]

//! [2]
void MainWindow::on_queryBox_currentIndexChanged()
{
    QFile queryFile(":/queries/" + queryBox->currentText());
    queryFile.open(QIODevice::ReadOnly);

    queryEdit->setPlainText(QString::fromLatin1(queryFile.readAll()));
    evaluateResult();
}
//! [2]

//! [3]
void MainWindow::evaluateResult()
{
    if (queryBox->currentText().isEmpty())
        return;

    QXmlQuery query(m_namePool);
    query.bindVariable("fileTree", m_fileNode);
    query.setQuery(QUrl("qrc:/queries/" + queryBox->currentText()));

    QByteArray formatterOutput;
    QBuffer buffer(&formatterOutput);
    buffer.open(QIODevice::WriteOnly);

    QXmlFormatter formatter(query, &buffer);
    query.evaluateTo(&formatter);

    output->setText(QString::fromLatin1(formatterOutput.constData()));
}
//! [3]

//! [1]
void MainWindow::on_actionOpenDirectory_triggered()
{
    const QString directoryName = QFileDialog::getExistingDirectory(this);
    if (!directoryName.isEmpty())
        loadDirectory(directoryName);
}
//! [1]

//! [4]
//! [5]
void MainWindow::loadDirectory(const QString &directory)
{
    Q_ASSERT(QDir(directory).exists());

    m_fileNode = m_fileTree.nodeFor(directory);
//! [5]

    QXmlQuery query(m_namePool);
    query.bindVariable("fileTree", m_fileNode);
    query.setQuery(QUrl("qrc:/queries/wholeTree.xq"));

    QByteArray output;
    QBuffer buffer(&output);
    buffer.open(QIODevice::WriteOnly);

    QXmlFormatter formatter(query, &buffer);
    query.evaluateTo(&formatter);

    treeInfo->setText((QString(tr("Model of %1 output as XML.")).arg(directory)));
    fileTree->setText(QString::fromLatin1(output.constData()));
    evaluateResult();
//! [6]    
}
//! [6]    
//! [4]

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About File Tree"),
                   tr("<p>Select <b>File->Open Directory</b> and "
                      "choose a directory. The directory is then "
                      "loaded into the model, and the model is "
                      "displayed on the left as XML.</p>"

                      "<p>From the query menu on the right, select "
                      "a query. The query is displayed and then run "
                      "on the model. The results are displayed below "
                      "the query.</p>"));
}


