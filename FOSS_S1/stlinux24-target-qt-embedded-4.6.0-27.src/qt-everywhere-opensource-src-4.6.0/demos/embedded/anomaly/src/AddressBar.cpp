/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
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

#include "AddressBar.h"

#include <QtCore>
#include <QtGui>

class LineEdit: public QLineEdit
{
public:
    LineEdit(QWidget *parent = 0): QLineEdit(parent) {}

    void paintEvent(QPaintEvent *event) {
        QLineEdit::paintEvent(event);
        if (text().isEmpty()) {
            QPainter p(this);
            int flags = Qt::AlignLeft | Qt::AlignVCenter;
            p.setPen(palette().color(QPalette::Disabled, QPalette::Text));
            p.drawText(rect().adjusted(10, 0, 0, 0), flags, "Enter address or search terms");
            p.end();
        }
    }
};

AddressBar::AddressBar(QWidget *parent)
    : QWidget(parent)
{
    m_lineEdit = new LineEdit(parent);
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(processAddress()));
    m_toolButton = new QToolButton(parent);
    m_toolButton->setText("Go");
    connect(m_toolButton, SIGNAL(clicked()), SLOT(processAddress()));
}

QSize AddressBar::sizeHint() const
{
    return m_lineEdit->sizeHint();
}

void AddressBar::processAddress()
{
    if (!m_lineEdit->text().isEmpty())
        emit addressEntered(m_lineEdit->text());
}

void AddressBar::resizeEvent(QResizeEvent *event)
{
    int x, y, w, h;

    m_toolButton->adjustSize();
    x = width() - m_toolButton->width();
    y = 0;
    w = m_toolButton->width();
    h = height() - 1;
    m_toolButton->setGeometry(x, y, w, h);
    m_toolButton->show();

    x = 0;
    y = 0;
    w = width() - m_toolButton->width();
    h = height() - 1;
    m_lineEdit->setGeometry(x, y, w, h);
    m_lineEdit->show();
}

void AddressBar::focusInEvent(QFocusEvent *event)
{
    m_lineEdit->setFocus();
    QWidget::focusInEvent(event);
}
