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
(void) new Q3ListViewItem(listView, "Column 1", "Column 2");
(void) new Q3ListViewItem(listView->firstChild(), "A", "B", "C");
//! [0]


//! [1]
Q3ListViewItem * myChild = myItem->firstChild();
while(myChild) {
    doSomething(myChild);
    myChild = myChild->nextSibling();
}
//! [1]


//! [2]
Q3ListViewItemIterator it(listview);
while (it.current()) {
    Q3ListViewItem *item = it.current();
    doSomething(item);
    ++it;
}
//! [2]


//! [3]
int MyListViewItem::compare(Q3ListViewItem *i, int col,
                             bool ascending) const
{
    return key(col, ascending).compare(i->key(col, ascending));
}
//! [3]


//! [4]
Q3ListViewItem *i = itemAt(p);
if (i) {
    if (p.x() > header()->sectionPos(header()->mapToIndex(0)) +
            treeStepSize() * (i->depth() + (rootIsDecorated() ? 1 : 0)) + itemMargin() ||
            p.x() < header()->sectionPos(header()->mapToIndex(0))) {
        ; // p is not on root decoration
    else
        ; // p is on the root decoration
}
//! [4]


//! [5]
QRect r(listView->itemRect(item));
r.setHeight(qMin(item->totalHeight(),
                 listView->viewport->height() - r.y()))
//! [5]


//! [6]
QList<Q3ListViewItem *> lst;
Q3ListViewItemIterator it(myListView);
while (it.current()) {
    if (it.current()->isSelected())
        lst.append(it.current());
    ++it;
}
//! [6]


//! [7]
QList<Q3ListViewItem *> lst;
Q3ListViewItemIterator it(myListView, Selected);
while (it.current()) {
    lst.append(it.current());
    ++it;
}
//! [7]
