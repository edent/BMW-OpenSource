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


void wrapInFunction()
{

//! [0]
for (int row = 0; row < table->numRows(); row++) {
    for (int col = 0; col < table->numCols(); col++) {
        table->setItem(row, col,
            new Q3TableItem(table, Q3TableItem::WhenCurrent, QString::number(row * col)));
    }
}
//! [0]


//! [1]
QWidget* MyTableItem::createEditor() const
{
    QHBox* hbox = new QHBox(table()->viewport());
    hbox->setFocusProxy(new QLineEdit(hbox));
    new QLineEdit(hbox);
    return hbox;
}
//! [1]


//! [2]
p->setClipRect(table()->cellRect(row, col), QPainter::ClipPainter);
//... your drawing code
p->setClipping(false);
//! [2]


//! [3]
Q3Table *table = new Q3Table(100, 250, this);
table->setPixmap(3, 2, pix);
table->setText(3, 2, "A pixmap");
//! [3]


//! [4]
p->setClipRect(cellRect(row, col), QPainter::CoordPainter);
//... your drawing code
p->setClipping(false);
//! [4]


//! [5]
Q3TableItem *i = item(row, col);
if (initFromCell || (i && !i->isReplaceable()))
    // If we had a Q3TableItem ask the base class to create the editor
    return Q3Table::createEditor(row, col, initFromCell);
else
    return ...(create your own editor)
//! [5]

}

