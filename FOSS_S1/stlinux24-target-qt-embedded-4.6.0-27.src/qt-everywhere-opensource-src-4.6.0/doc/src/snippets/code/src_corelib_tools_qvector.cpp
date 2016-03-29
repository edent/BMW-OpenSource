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
QVector<int> integerVector;
QVector<QString> stringVector;
//! [0]


//! [1]
QVector<QString> vector(200);
//! [1]


//! [2]
QVector<QString> vector(200, "Pass");
//! [2]


//! [3]
if (vector[0] == "Liz")
    vector[0] = "Elizabeth";
//! [3]


//! [4]
for (int i = 0; i < vector.size(); ++i) {
    if (vector.at(i) == "Alfonso")
        cout << "Found Alfonso at position " << i << endl;
}
//! [4]


//! [5]
int i = vector.indexOf("Harumi");
if (i != -1)
    cout << "First occurrence of Harumi is at position " << i << endl;
//! [5]


//! [6]
QVector<int> vector(10);
int *data = vector.data();
for (int i = 0; i < 10; ++i)
    data[i] = 2 * i;
//! [6]


//! [7]
QVector<QString> vector(0);
vector.append("one");
vector.append("two");
vector.append("three");
// vector: ["one", "two", "three"]
//! [7]


//! [8]
QVector<QString> vector;
vector.prepend("one");
vector.prepend("two");
vector.prepend("three");
// vector: ["three", "two", "one"]
//! [8]


//! [9]
QVector<QString> vector;
vector << "alpha" << "beta" << "delta";
vector.insert(2, "gamma");
// vector: ["alpha", "beta", "gamma", "delta"]
//! [9]


//! [10]
QVector<double> vector;
vector << 2.718 << 1.442 << 0.4342;
vector.insert(1, 3, 9.9);
// vector: [2.718, 9.9, 9.9, 9.9, 1.442, 0.4342]
//! [10]


//! [11]
QVector<QString> vector(3);
vector.fill("Yes");
// vector: ["Yes", "Yes", "Yes"]

vector.fill("oh", 5);
// vector: ["oh", "oh", "oh", "oh", "oh"]
//! [11]


//! [12]
QVector<QString> vector;
vector << "A" << "B" << "C" << "B" << "A";
vector.indexOf("B");            // returns 1
vector.indexOf("B", 1);         // returns 1
vector.indexOf("B", 2);         // returns 3
vector.indexOf("X");            // returns -1
//! [12]


//! [13]
QList<QString> vector;
vector << "A" << "B" << "C" << "B" << "A";
vector.lastIndexOf("B");        // returns 3
vector.lastIndexOf("B", 3);     // returns 3
vector.lastIndexOf("B", 2);     // returns 1
vector.lastIndexOf("X");        // returns -1
//! [13]


//! [14]
QVector<double> vect;
vect << "red" << "green" << "blue" << "black";

QList<double> list = vect.toList();
// list: ["red", "green", "blue", "black"]
//! [14]


//! [15]
QStringList list;
list << "Sven" << "Kim" << "Ola";

QVector<QString> vect = QVector<QString>::fromList(list);
// vect: ["Sven", "Kim", "Ola"]
//! [15]


//! [16]
std::vector<double> stdvector;
vector.push_back(1.2);
vector.push_back(0.5);
vector.push_back(3.14);

QVector<double> vector = QVector<double>::fromStdVector(stdvector);
//! [16]


//! [17]
QVector<double> vector;
vector << 1.2 << 0.5 << 3.14;

std::vector<double> stdvector = vector.toStdVector();
//! [17]
