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

#include "certificateinfo.h"
#include "sslclient.h"
#include "ui_sslclient.h"
#include "ui_sslerrors.h"

#include <QtGui/QScrollBar>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>
#include <QtNetwork/QSslCipher>

SslClient::SslClient(QWidget *parent)
    : QWidget(parent), socket(0), padLock(0), executingDialog(false)
{
    form = new Ui_Form;
    form->setupUi(this);
    form->hostNameEdit->setSelection(0, form->hostNameEdit->text().size());
    form->sessionOutput->setHtml(tr("&lt;not connected&gt;"));

    connect(form->hostNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateEnabledState()));
    connect(form->connectButton, SIGNAL(clicked()),
            this, SLOT(secureConnect()));
    connect(form->sendButton, SIGNAL(clicked()),
            this, SLOT(sendData()));
}

SslClient::~SslClient()
{
    delete form;
}

void SslClient::updateEnabledState()
{
    bool unconnected = !socket || socket->state() == QAbstractSocket::UnconnectedState;

    form->hostNameEdit->setReadOnly(!unconnected);
    form->hostNameEdit->setFocusPolicy(unconnected ? Qt::StrongFocus : Qt::NoFocus);

    form->hostNameLabel->setEnabled(unconnected);
    form->portBox->setEnabled(unconnected);
    form->portLabel->setEnabled(unconnected);
    form->connectButton->setEnabled(unconnected && !form->hostNameEdit->text().isEmpty());

    bool connected = socket && socket->state() == QAbstractSocket::ConnectedState;
    form->sessionBox->setEnabled(connected);
    form->sessionOutput->setEnabled(connected);
    form->sessionInput->setEnabled(connected);
    form->sessionInputLabel->setEnabled(connected);
    form->sendButton->setEnabled(connected);
}

void SslClient::secureConnect()
{
    if (!socket) {
        socket = new QSslSocket(this);
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        connect(socket, SIGNAL(encrypted()),
                this, SLOT(socketEncrypted()));
        connect(socket, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(sslErrors(QList<QSslError>)));
        connect(socket, SIGNAL(readyRead()),
                this, SLOT(socketReadyRead()));
    }

    socket->connectToHostEncrypted(form->hostNameEdit->text(), form->portBox->value());
    updateEnabledState();
}

void SslClient::socketStateChanged(QAbstractSocket::SocketState state)
{
    if (executingDialog)
        return;

    updateEnabledState();
    if (state == QAbstractSocket::UnconnectedState) {
        form->hostNameEdit->setPalette(QPalette());
        form->hostNameEdit->setFocus();
        form->cipherLabel->setText(tr("<none>"));
        if (padLock)
            padLock->hide();
        socket->deleteLater();
        socket = 0;
    }
}

void SslClient::socketEncrypted()
{
    if (!socket)
        return;                 // might have disconnected already

    form->sessionOutput->clear();
    form->sessionInput->setFocus();

    QPalette palette;
    palette.setColor(QPalette::Base, QColor(255, 255, 192));
    form->hostNameEdit->setPalette(palette);

    QSslCipher ciph = socket->sessionCipher();
    QString cipher = QString("%1, %2 (%3/%4)").arg(ciph.authenticationMethod())
                     .arg(ciph.name()).arg(ciph.usedBits()).arg(ciph.supportedBits());;
    form->cipherLabel->setText(cipher);

    if (!padLock) {
        padLock = new QToolButton;
        padLock->setIcon(QIcon(":/encrypted.png"));
#ifndef QT_NO_CURSOR
        padLock->setCursor(Qt::ArrowCursor);
#endif
        padLock->setToolTip(tr("Display encryption details."));

        int extent = form->hostNameEdit->height() - 2;
        padLock->resize(extent, extent);
        padLock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

        QHBoxLayout *layout = new QHBoxLayout(form->hostNameEdit);
        layout->setMargin(form->hostNameEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth));
        layout->setSpacing(0);
        layout->addStretch();
        layout->addWidget(padLock);

        form->hostNameEdit->setLayout(layout);

        connect(padLock, SIGNAL(clicked()),
                this, SLOT(displayCertificateInfo()));
    } else {
        padLock->show();
    }
}

void SslClient::socketReadyRead()
{
    appendString(QString::fromUtf8(socket->readAll()));
}

void SslClient::sendData()
{
    QString input = form->sessionInput->text();
    appendString(input + "\n");
    socket->write(input.toUtf8() + "\r\n");
    form->sessionInput->clear();
}

void SslClient::sslErrors(const QList<QSslError> &errors)
{
    QDialog errorDialog(this);
    Ui_SslErrors ui;
    ui.setupUi(&errorDialog);
    connect(ui.certificateChainButton, SIGNAL(clicked()),
            this, SLOT(displayCertificateInfo()));

    foreach (const QSslError &error, errors)
        ui.sslErrorList->addItem(error.errorString());

    executingDialog = true;
    if (errorDialog.exec() == QDialog::Accepted)
        socket->ignoreSslErrors();
    executingDialog = false;

    // did the socket state change?
    if (socket->state() != QAbstractSocket::ConnectedState)
        socketStateChanged(socket->state());
}

void SslClient::displayCertificateInfo()
{
    CertificateInfo *info = new CertificateInfo(this);
    info->setCertificateChain(socket->peerCertificateChain());
    info->exec();
    info->deleteLater();
}

void SslClient::appendString(const QString &line)
{
    QTextCursor cursor(form->sessionOutput->textCursor());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(line);
    form->sessionOutput->verticalScrollBar()->setValue(form->sessionOutput->verticalScrollBar()->maximum());
}
