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

#include <QtGui>

#include "mainwindow.h"
#include "xmlwriter.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *saveAction = fileMenu->addAction(tr("&Save..."));
    saveAction->setShortcut(tr("Ctrl+S"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);
    editor = new QTextEdit();

    QTextCursor cursor(editor->textCursor());
    cursor.movePosition(QTextCursor::Start); 

    QTextFrame *mainFrame = cursor.currentFrame();
    
    QTextCharFormat plainCharFormat;
    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);
/*  main frame
//! [0]
    QTextFrame *mainFrame = cursor.currentFrame();
    cursor.insertText(...);
//! [0]
*/
    cursor.insertText("Text documents are represented by the "
                      "QTextDocument class, rather than by QString objects. "
                      "Each QTextDocument object contains information about "
                      "the document's internal representation, its structure, "
                      "and keeps track of modifications to provide undo/redo "
                      "facilities. This approach allows features such as the "
                      "layout management to be delegated to specialized "
                      "classes, but also provides a focus for the framework.",
                      plainCharFormat);

//! [1]
    QTextFrameFormat frameFormat;
    frameFormat.setMargin(32);
    frameFormat.setPadding(8);
    frameFormat.setBorder(4);
//! [1]
    cursor.insertFrame(frameFormat);

/*  insert frame
//! [2]
    cursor.insertFrame(frameFormat);
    cursor.insertText(...);
//! [2]
*/
    cursor.insertText("Documents are either converted from external sources "
                      "or created from scratch using Qt. The creation process "
                      "can done by an editor widget, such as QTextEdit, or by "
                      "explicit calls to the Scribe API.", boldCharFormat);

    cursor = mainFrame->lastCursorPosition();
/*  last cursor
//! [3]
    cursor = mainFrame->lastCursorPosition();
    cursor.insertText(...);
//! [3]
*/
    cursor.insertText("There are two complementary ways to visualize the "
                      "contents of a document: as a linear buffer that is "
                      "used by editors to modify the contents, and as an "
                      "object hierarchy containing structural information "
                      "that is useful to layout engines. In the hierarchical "
                      "model, the objects generally correspond to visual "
                      "elements such as frames, tables, and lists. At a lower "
                      "level, these elements describe properties such as the "
                      "style of text used and its alignment. The linear "
                      "representation of the document is used for editing and "
                      "manipulation of the document's contents.",
                      plainCharFormat);


    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setCentralWidget(editor);
    setWindowTitle(tr("Text Document Frames"));
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save document as:"), "", tr("XML (*.xml)"));

    if (!fileName.isEmpty()) {
        if (writeXml(fileName))
            setWindowTitle(fileName);
        else
            QMessageBox::warning(this, tr("Warning"),
                tr("Failed to save the document."), QMessageBox::Cancel,
                QMessageBox::NoButton);
    }
}
bool MainWindow::writeXml(const QString &fileName)
{
    XmlWriter documentWriter(editor->document());

    QDomDocument *domDocument = documentWriter.toXml();
    QFile file(fileName);

    if (file.open(QFile::WriteOnly)) {
        QTextStream textStream(&file);
        textStream.setCodec(QTextCodec::codecForName("UTF-8"));
        
        textStream << domDocument->toString(1).toUtf8();
        file.close();
        return true;
    }
    else
        return false;
}
