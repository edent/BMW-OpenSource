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
#include "xmlsyntaxhighlighter.h"

//! [4]
class MessageHandler : public QAbstractMessageHandler
{
    public:
        MessageHandler()
            : QAbstractMessageHandler(0)
        {
        }

        QString statusMessage() const
        {
            return m_description;
        }

        int line() const
        {
            return m_sourceLocation.line();
        }

        int column() const
        {
            return m_sourceLocation.column();
        }

    protected:
        virtual void handleMessage(QtMsgType type, const QString &description,
                                   const QUrl &identifier, const QSourceLocation &sourceLocation)
        {
            Q_UNUSED(type);
            Q_UNUSED(identifier);

            m_messageType = type;
            m_description = description;
            m_sourceLocation = sourceLocation;
        }

    private:
        QtMsgType m_messageType;
        QString m_description;
        QSourceLocation m_sourceLocation;
};
//! [4]

//! [0]
MainWindow::MainWindow()
{
    setupUi(this);

    new XmlSyntaxHighlighter(schemaView->document());
    new XmlSyntaxHighlighter(instanceEdit->document());

    schemaSelection->addItem(tr("Contact Schema"));
    schemaSelection->addItem(tr("Recipe Schema"));
    schemaSelection->addItem(tr("Order Schema"));

    instanceSelection->addItem(tr("Valid Contact Instance"));
    instanceSelection->addItem(tr("Invalid Contact Instance"));

    connect(schemaSelection, SIGNAL(currentIndexChanged(int)), SLOT(schemaSelected(int)));
    connect(instanceSelection, SIGNAL(currentIndexChanged(int)), SLOT(instanceSelected(int)));
    connect(validateButton, SIGNAL(clicked()), SLOT(validate()));
    connect(instanceEdit, SIGNAL(textChanged()), SLOT(textChanged()));

    validationStatus->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    schemaSelected(0);
    instanceSelected(0);
}
//! [0]

//! [1]
void MainWindow::schemaSelected(int index)
{
    instanceSelection->clear();
    if (index == 0) {
        instanceSelection->addItem(tr("Valid Contact Instance"));
        instanceSelection->addItem(tr("Invalid Contact Instance"));
    } else if (index == 1) {
        instanceSelection->addItem(tr("Valid Recipe Instance"));
        instanceSelection->addItem(tr("Invalid Recipe Instance"));
    } else if (index == 2) {
        instanceSelection->addItem(tr("Valid Order Instance"));
        instanceSelection->addItem(tr("Invalid Order Instance"));
    }
    textChanged();

    QFile schemaFile(QString(":/schema_%1.xsd").arg(index));
    schemaFile.open(QIODevice::ReadOnly);
    const QString schemaText(QString::fromUtf8(schemaFile.readAll()));
    schemaView->setPlainText(schemaText);

    validate();
}
//! [1]

//! [2]
void MainWindow::instanceSelected(int index)
{
    QFile instanceFile(QString(":/instance_%1.xml").arg((2*schemaSelection->currentIndex()) + index));
    instanceFile.open(QIODevice::ReadOnly);
    const QString instanceText(QString::fromUtf8(instanceFile.readAll()));
    instanceEdit->setPlainText(instanceText);

    validate();
}
//! [2]

//! [3]
void MainWindow::validate()
{
    const QByteArray schemaData = schemaView->toPlainText().toUtf8();
    const QByteArray instanceData = instanceEdit->toPlainText().toUtf8();

    MessageHandler messageHandler;

    QXmlSchema schema;
    schema.setMessageHandler(&messageHandler);

    schema.load(schemaData);

    bool errorOccurred = false;
    if (!schema.isValid()) {
        errorOccurred = true;
    } else {
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(instanceData))
            errorOccurred = true;
    }

    if (errorOccurred) {
        validationStatus->setText(messageHandler.statusMessage());
        moveCursor(messageHandler.line(), messageHandler.column());
    } else {
        validationStatus->setText(tr("validation successful"));
    }

    const QString styleSheet = QString("QLabel {background: %1; padding: 3px}")
                                      .arg(errorOccurred ? QColor(Qt::red).lighter(160).name() :
                                                           QColor(Qt::green).lighter(160).name());
    validationStatus->setStyleSheet(styleSheet);
}
//! [3]

void MainWindow::textChanged()
{
    instanceEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>());
}

void MainWindow::moveCursor(int line, int column)
{
    instanceEdit->moveCursor(QTextCursor::Start);
    for (int i = 1; i < line; ++i)
        instanceEdit->moveCursor(QTextCursor::Down);

    for (int i = 1; i < column; ++i)
        instanceEdit->moveCursor(QTextCursor::Right);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    const QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = instanceEdit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    instanceEdit->setExtraSelections(extraSelections);

    instanceEdit->setFocus();
}
