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
QList<int> integerList;
QList<QDate> dateList;
//! [0]


//! [1]
QList<QString> list;
list << "one" << "two" << "three";
// list: ["one", "two", "three"]
//! [1]


//! [2]
if (list[0] == "Bob")
    list[0] = "Robert";
//! [2]


//! [3]
for (int i = 0; i < list.size(); ++i) {
    if (list.at(i) == "Jane")
        cout << "Found Jane at position " << i << endl;
}
//! [3]


//! [4]
QList<QWidget *> list;
...
while (!list.isEmpty())
    delete list.takeFirst();
//! [4]


//! [5]
int i = list.indexOf("Jane");
if (i != -1)
    cout << "First occurrence of Jane is at position " << i << endl;
//! [5]


//! [6]
QList<QString> list;
list.append("one");
list.append("two");
list.append("three");
// list: ["one", "two", "three"]
//! [6]


//! [7]
QList<QString> list;
list.prepend("one");
list.prepend("two");
list.prepend("three");
// list: ["three", "two", "one"]
//! [7]


//! [8]
QList<QString> list;
list << "alpha" << "beta" << "delta";
list.insert(2, "gamma");
// list: ["alpha", "beta", "gamma", "delta"]
//! [8]


//! [9]
QList<QString> list;
list << "sun" << "cloud" << "sun" << "rain";
list.removeAll("sun");
// list: ["cloud", "rain"]
//! [9]


//! [10]
QList<QString> list;
list << "sun" << "cloud" << "sun" << "rain";
list.removeOne("sun");
// list: ["cloud", ,"sun", "rain"]
//! [10]


//! [11]
QList<QString> list;
list << "A" << "B" << "C" << "D" << "E" << "F";
list.move(1, 4);
// list: ["A", "C", "D", "E", "B", "F"]
//! [11]


//! [12]
QList<QString> list;
list << "A" << "B" << "C" << "D" << "E" << "F";
list.swap(1, 4);
// list: ["A", "E", "C", "D", "B", "F"]
//! [12]


//! [13]
QList<QString> list;
list << "A" << "B" << "C" << "B" << "A";
list.indexOf("B");          // returns 1
list.indexOf("B", 1);       // returns 1
list.indexOf("B", 2);       // returns 3
list.indexOf("X");          // returns -1
//! [13]


//! [14]
QList<QString> list;
list << "A" << "B" << "C" << "B" << "A";
list.lastIndexOf("B");      // returns 3
list.lastIndexOf("B", 3);   // returns 3
list.lastIndexOf("B", 2);   // returns 1
list.lastIndexOf("X");      // returns -1
//! [14]


//! [15]
QList<QString> list;
list.append("January");
list.append("February");
...
list.append("December");

QList<QString>::iterator i;
for (i = list.begin(); i != list.end(); ++i)
    cout << *i << endl;
//! [15]


//! [16]
QList<int>::iterator i;
for (i = list.begin(); i != list.end(); ++i)
    *i += 2;
//! [16]


//! [17]
QList<QWidget *> list;
...
qDeleteAll(list.begin(), list.end());
//! [17]


//! [18]
if (*it == "Hello")
    *it = "Bonjour";
//! [18]


//! [19]
QList<QString> list;
list.append("January");
list.append("February");
...
list.append("December");

QList<QString>::const_iterator i;
for (i = list.constBegin(); i != list.constEnd(); ++i)
    cout << *i << endl;
//! [19]


//! [20]
QList<QWidget *> list;
...
qDeleteAll(list.constBegin(), list.constEnd());
//! [20]


//! [21]
QVector<double> vect;
vect << 20.0 << 30.0 << 40.0 << 50.0;

QList<double> list = QVector<T>::fromVector(vect);
// list: [20.0, 30.0, 40.0, 50.0]
//! [21]


//! [22]
QStringList list;
list << "Sven" << "Kim" << "Ola";

QVector<QString> vect = list.toVector();
// vect: ["Sven", "Kim", "Ola"]
//! [22]


//! [23]
QSet<int> set;
set << 20 << 30 << 40 << ... << 70;

QList<int> list = QList<int>::fromSet(set);
qSort(list);
//! [23]


//! [24]
QStringList list;
list << "Julia" << "Mike" << "Mike" << "Julia" << "Julia";

QSet<QString> set = list.toSet();
set.contains("Julia");  // returns true
set.contains("Mike");   // returns true
set.size();             // returns 2
//! [24]


//! [25]
std::list<double> stdlist;
list.push_back(1.2);
list.push_back(0.5);
list.push_back(3.14);

QList<double> list = QList<double>::fromStdList(stdlist);
//! [25]


//! [26]
QList<double> list;
list << 1.2 << 0.5 << 3.14;

std::list<double> stdlist = list.toStdList();
//! [26]
