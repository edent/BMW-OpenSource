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

//! [0]
w->setWindowState(w->windowState() ^ Qt::WindowFullScreen);
//! [0]


//! [1]
w->setWindowState(w->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
//! [1]


//! [2]
width = baseSize().width() + i * sizeIncrement().width();
height = baseSize().height() + j * sizeIncrement().height();
//! [2]


//! [3]
aWidget->window()->setWindowTitle("New Window Title");
//! [3]


//! [4]
QFont font("Helvetica", 12, QFont::Bold);
setFont(font);
//! [4]


//! [5]
QFont font;
font.setBold(false);
setFont(font);
//! [5]


//! [6]
setCursor(Qt::IBeamCursor);
//! [6]


//! [7]
QPixmap pixmap(widget->size());
widget->render(&pixmap);
//! [7]


//! [8]
QPainter painter(this);
...
painter.end();
myWidget->render(this);
//! [8]


//! [9]
setTabOrder(a, b); // a to b
setTabOrder(b, c); // a to b to c
setTabOrder(c, d); // a to b to c to d
//! [9]


//! [10]
// WRONG
setTabOrder(c, d); // c to d
setTabOrder(a, b); // a to b AND c to d
setTabOrder(b, c); // a to b to c, but not c to d
//! [10]


//! [11]
void MyWidget::closeEvent(QCloseEvent *event)
{
    QSettings settings("MyCompany", "MyApp");
    settings.setValue("geometry", saveGeometry());
    QWidget::closeEvent(event);
}
//! [11]


//! [12]
QSettings settings("MyCompany", "MyApp");
myWidget->restoreGeometry(settings.value("myWidget/geometry").toByteArray());
//! [12]


//! [13]
setUpdatesEnabled(false);
bigVisualChanges();
setUpdatesEnabled(true);
//! [13]


//! [14]
...
extern void qt_x11_set_global_double_buffer(bool);
qt_x11_set_global_double_buffer(false);
...
//! [14]
