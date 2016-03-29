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
(void) new Q3IconViewItem(
                iconView,        // A pointer to a Q3IconView
                "This is the text of the item",
                aPixmap);
//! [0]


//! [1]
Q3IconViewItem *item;
for (item = iconView->firstItem(); item; item = item->nextItem())
    do_something_with(item);
//! [1]


//! [2]
Q3IconViewItem *item;
for (item = iconView->firstItem(); item; item = item->nextItem())
    do_something_with(item);
//! [2]


//! [3]
int MyIconViewItem::compare(Q3IconViewItem *i) const
{
    return key().compare(i->key());
}
//! [3]


//! [4]
Q3IconView *iv = new Q3IconView(this);
QDir dir(path, "*.xpm");
for (uint i = 0; i < dir.count(); i++) {
    (void) new Q3IconViewItem(iv, dir[i], QPixmap(path + dir[i]));
}
iv->resize(600, 400);
iv->show();
//! [4]


//! [5]
for (Q3IconViewItem *item = iv->firstItem(); item; item = item->nextItem())
    do_something(item);
//! [5]


//! [6]
Q3DragObject *MyIconView::dragObject()
{
    return new Q3TextDrag(currentItem()->text(), this);
}
//! [6]


//! [7]
bool MyIconViewItem::acceptDrop(const QMimeSource *mime) const
{
    if (mime->provides("text/plain"))
        return true;
    return false;
}

void MyIconViewItem::dropped(QDropEvent *evt, const Q3ValueList<Q3IconDragItem>&)
{
    QString label;
    if (Q3TextDrag::decode(evt, label))
        setText(label);
}
//! [7]


//! [8]
(void) new Q3IconViewItem(myIconview, "The text of the item", aPixmap);
//! [8]
