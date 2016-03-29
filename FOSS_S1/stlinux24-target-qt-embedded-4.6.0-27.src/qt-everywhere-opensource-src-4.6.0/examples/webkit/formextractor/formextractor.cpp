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

#include "formextractor.h"

#include <QWebElement>

FormExtractor::FormExtractor(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);
    ui.webView->setUrl(QUrl("qrc:/form.html"));
    connect(ui.webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    resize(300, 300);
}

FormExtractor::~FormExtractor()
{
}

void FormExtractor::submit()
{
    QWebFrame *frame = ui.webView->page()->mainFrame();

    QWebElement firstName = frame->findFirstElement("#firstname");
    QWebElement lastName = frame->findFirstElement("#lastname");
    QWebElement maleGender = frame->findFirstElement("#genderMale");
    QWebElement femaleGender = frame->findFirstElement("#genderFemale");
    QWebElement updates = frame->findFirstElement("#updates");

    ui.firstNameEdit->setText(firstName.evaluateJavaScript("this.value").toString());
    ui.lastNameEdit->setText(lastName.evaluateJavaScript("this.value").toString());

    if (maleGender.evaluateJavaScript("this.checked").toBool())
        ui.genderEdit->setText(maleGender.evaluateJavaScript("this.value").toString());
    else if (femaleGender.evaluateJavaScript("this.checked").toBool())
        ui.genderEdit->setText(femaleGender.evaluateJavaScript("this.value").toString());

    if (updates.evaluateJavaScript("this.checked").toBool())
        ui.updatesEdit->setText("Yes");
    else
        ui.updatesEdit->setText("No");
}

void FormExtractor::populateJavaScriptWindowObject()
{
    ui.webView->page()->mainFrame()->addToJavaScriptWindowObject("formExtractor", this);
}
