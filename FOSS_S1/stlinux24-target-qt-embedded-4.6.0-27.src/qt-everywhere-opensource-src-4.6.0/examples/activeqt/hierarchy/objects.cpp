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

#include "objects.h"
#include <QLayout>
#include <QPainter>

/* Implementation of QParentWidget */
//! [0]
QParentWidget::QParentWidget(QWidget *parent)
: QWidget(parent)
{
    vbox = new QVBoxLayout(this);    
}

//! [0] //! [1]
void QParentWidget::createSubWidget(const QString &name)
{
    QSubWidget *sw = new QSubWidget(this, name);
    vbox->addWidget(sw);
    sw->setLabel(name);
    sw->show();
}

//! [1] //! [2]
QSubWidget *QParentWidget::subWidget(const QString &name)
{
    return qFindChild<QSubWidget*>(this, name);
}

//! [2]
QSize QParentWidget::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(100, 100));
}

/* Implementation of QSubWidget */
//! [3]
QSubWidget::QSubWidget(QWidget *parent, const QString &name)
: QWidget(parent)
{
    setObjectName(name);
}

void QSubWidget::setLabel(const QString &text)
{
    lbl = text;
    setObjectName(text);
    update();
}

QString QSubWidget::label() const
{
    return lbl;
}

QSize QSubWidget::sizeHint() const
{
    QFontMetrics fm(font());
    return QSize(fm.width(lbl), fm.height());
}

void QSubWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(palette().text().color());
    painter.drawText(rect(), Qt::AlignCenter, lbl);
//! [3] //! [4]
}
//! [4]
