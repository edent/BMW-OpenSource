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

#include <qdebug.h>
#include <QApplication>
#include <QLineEdit>
#include <QValidator>

#include "ui_validators.h"

class ValidatorWidget : public QWidget, public Ui::ValidatorsForm
{
    Q_OBJECT
public:
    ValidatorWidget(QWidget *parent = 0);

private slots:
    void updateValidator();
    void updateDoubleValidator();
    void _setLocale(const QLocale &l) { setLocale(l); updateValidator(); updateDoubleValidator(); }

private:
    QIntValidator *validator;
    QDoubleValidator *doubleValidator;
};

ValidatorWidget::ValidatorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(localeSelector, SIGNAL(localeSelected(QLocale)), this, SLOT(_setLocale(QLocale)));

    connect(minVal, SIGNAL(editingFinished()), this, SLOT(updateValidator()));
    connect(maxVal, SIGNAL(editingFinished()), this, SLOT(updateValidator()));
    connect(editor, SIGNAL(editingFinished()), ledWidget, SLOT(flash()));

    connect(doubleMaxVal, SIGNAL(editingFinished()), this, SLOT(updateDoubleValidator()));
    connect(doubleMinVal, SIGNAL(editingFinished()), this, SLOT(updateDoubleValidator()));
    connect(doubleDecimals, SIGNAL(valueChanged(int)), this, SLOT(updateDoubleValidator()));
    connect(doubleFormat, SIGNAL(activated(int)), this, SLOT(updateDoubleValidator()));
    connect(doubleEditor, SIGNAL(editingFinished()), doubleLedWidget, SLOT(flash()));

    validator = 0;
    doubleValidator = 0;
    updateValidator();
    updateDoubleValidator();
};

void ValidatorWidget::updateValidator()
{
    QIntValidator *v = new QIntValidator(minVal->value(), maxVal->value(), this);
    v->setLocale(locale());
    editor->setValidator(v);
    delete validator;
    validator = v;

    QString s = editor->text();
    int i = 0;
    if (validator->validate(s, i) == QValidator::Invalid) {
        editor->clear();
    } else {
        editor->setText(s);
    }
}

void ValidatorWidget::updateDoubleValidator()
{
    QDoubleValidator *v
        = new QDoubleValidator(doubleMinVal->value(), doubleMaxVal->value(),
                                doubleDecimals->value(), this);
    v->setNotation(static_cast<QDoubleValidator::Notation>(doubleFormat->currentIndex()));
    v->setLocale(locale());
    doubleEditor->setValidator(v);
    delete doubleValidator;
    doubleValidator = v;

    QString s = doubleEditor->text();
    int i = 0;
    if (doubleValidator->validate(s, i) == QValidator::Invalid) {
        doubleEditor->clear();
    } else {
        doubleEditor->setText(s);
    }
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(validators);

    QApplication app(argc, argv);

    ValidatorWidget w;
    w.show();

    return app.exec();
}

#include "main.moc"
