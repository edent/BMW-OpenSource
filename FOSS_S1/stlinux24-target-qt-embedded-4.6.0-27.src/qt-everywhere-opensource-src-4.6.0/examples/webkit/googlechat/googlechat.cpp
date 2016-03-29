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
#include <QtWebKit>
#include <QSslSocket>

#include "googlechat.h"

#define GOOGLECHAT_URL "http://talkgadget.google.com/talkgadget/m"

GoogleChat::GoogleChat(): QWidget() {
    form.setupUi(this);
    setFixedSize(320, 480);

    form.userNameEdit->setFocus();
    connect(form.userNameEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.userNameEdit, SIGNAL(returnPressed()), SLOT(inputPassword()));

    connect(form.passwordEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.passwordEdit, SIGNAL(returnPressed()), SLOT(doLogin()));

    form.loginButton->setEnabled(false);
    connect(form.loginButton, SIGNAL(clicked()), SLOT(doLogin()));

    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(initialPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    form.webView->setUrl((QUrl(GOOGLECHAT_URL)));
    form.webView->setContextMenuPolicy(Qt::PreventContextMenu);

    showStatus("Wait...");
}

void GoogleChat::showStatus(const QString &msg) {
    form.statusLabel->setText(msg);
    form.stackedWidget->setCurrentIndex(0);
}

void GoogleChat::showError(const QString &msg) {
    form.progressBar->hide();
    showStatus(QString("Error: %1").arg(msg));
}

QString GoogleChat::evalJS(const QString &js) {
    QWebFrame *frame = form.webView->page()->mainFrame();
    return frame->evaluateJavaScript(js).toString();
}

void GoogleChat::adjustLoginButton() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    form.loginButton->setEnabled(ok);
}

void GoogleChat::inputPassword() {
    if (!form.userNameEdit->text().isEmpty())
        form.passwordEdit->setFocus();
}

void GoogleChat::doLogin() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    if (!ok)
        return;

    form.progressBar->setValue(0);
    form.progressBar->show();
    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(loginPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    showStatus("Logging in...");

    QString userEmail = userName + "@gmail.com";
    evalJS(QString("document.getElementById('Email').value = \"%1\";").arg(userEmail));
    evalJS(QString("document.getElementById('Passwd').value = \"%1\";").arg(password));
    evalJS("document.getElementById('gaia_loginform').submit();");
}

void GoogleChat::initialPage(bool ok) {
    if (!QSslSocket::supportsSsl()) {
        showError("This example requires SSL support.");
        return;
    }

    if (ok) {
        QString s1 = evalJS("document.getElementById('Email').name");
        QString s2 = evalJS("document.getElementById('Passwd').name");
        QString s3 = evalJS("document.getElementById('gaia_loginform').id");
        if (s1 == "Email" && s2 == "Passwd" && s3 == "gaia_loginform") {
            form.stackedWidget->setCurrentIndex(1);
            form.webView->disconnect();
            return;
        }
    }

    showError("SERVICE unavailable.");
}

void GoogleChat::hideElements()
{
    evalJS("var e = document.getElementsByClassName('footer-footer')[0]; e.parentElement.removeChild(e)");
    evalJS("var e = document.getElementsByClassName('title-bar-bg title-bar')[0]; e.parentElement.removeChild(e)");
    QTimer::singleShot(2000, this, SLOT(hideElements()));
}

void GoogleChat::loginPage(bool ok) {
    QString location = form.webView->url().toString();
    if (!ok) {
        if (location.indexOf("CheckCookie"))
            return;
        showError("Service unavailable");
    } else {
        // check for any error message
        QString c = evalJS("document.getElementsByClassName('errormsg').length");
        if (c == "0") {
            form.stackedWidget->setCurrentIndex(2);
            QTimer::singleShot(500, this, SLOT(hideElements()));
            return;
        }

        QString err = "Unknown login failure.";
        if (c == "1") {
            err = evalJS("document.getElementsByClassName('errormsg')[0].textContent");
            err = err.simplified();
        }
        showError(err);
    }
}
