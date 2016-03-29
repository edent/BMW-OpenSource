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

#include <QCheckBox>
#include <QMouseEvent>

class MyCheckBox : public QCheckBox
{
public:
    void mousePressEvent(QMouseEvent *event);
};

//! [0]
void MyCheckBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // handle left mouse button here
    } else {
        // pass on other buttons to base class
        QCheckBox::mousePressEvent(event);
    }
}
//! [0]

class MyWidget : public QWidget
{
public:
    bool event(QEvent *event);
};

static const int MyCustomEventType = 1099;

class MyCustomEvent : public QEvent
{
public:
    MyCustomEvent() : QEvent((QEvent::Type)MyCustomEventType) {}
};

//! [1]
bool MyWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	if (ke->key() == Qt::Key_Tab) {
	    // special tab handling here
	    return true;
	}
    } else if (event->type() == MyCustomEventType) {
	MyCustomEvent *myEvent = static_cast<MyCustomEvent *>(event);
	// custom event handling here
	return true;
    }

    return QWidget::event(event);
}
//! [1]

int main()
{
}
