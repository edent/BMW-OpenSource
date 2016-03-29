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
QBitArray ba(200);
//! [0]


//! [1]
QBitArray ba;
ba.resize(3);
ba[0] = true;
ba[1] = false;
ba[2] = true;
//! [1]


//! [2]
QBitArray ba(3);
ba.setBit(0, true);
ba.setBit(1, false);
ba.setBit(2, true);
//! [2]


//! [3]
QBitArray x(5);
x.setBit(3, true);
// x: [ 0, 0, 0, 1, 0 ]

QBitArray y(5);
y.setBit(4, true);
// y: [ 0, 0, 0, 0, 1 ]

x |= y;
// x: [ 0, 0, 0, 1, 1 ]
//! [3]


//! [4]
QBitArray().isNull();           // returns true
QBitArray().isEmpty();          // returns true

QBitArray(0).isNull();          // returns false
QBitArray(0).isEmpty();         // returns true

QBitArray(3).isNull();          // returns false
QBitArray(3).isEmpty();         // returns false
//! [4]


//! [5]
QBitArray().isNull();           // returns true
QBitArray(0).isNull();          // returns false
QBitArray(3).isNull();          // returns false
//! [5]


//! [6]
QBitArray ba(8);
ba.fill(true);
// ba: [ 1, 1, 1, 1, 1, 1, 1, 1 ]

ba.fill(false, 2);
// ba: [ 0, 0 ]
//! [6]


//! [7]
QBitArray a(3);
a[0] = false;
a[1] = true;
a[2] = a[0] ^ a[1];
//! [7]


//! [8]
QBitArray a(3);
QBitArray b(2);
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
a &= b;                         // a: [ 1, 0, 0 ]
//! [8]


//! [9]
QBitArray a(3);
QBitArray b(2);
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
a |= b;                         // a: [ 1, 1, 1 ]
//! [9]


//! [10]
QBitArray a(3);
QBitArray b(2);
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
a ^= b;                         // a: [ 0, 1, 1 ]
//! [10]


//! [11]
QBitArray a(3);
QBitArray b;
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b = ~a;                         // b: [ 0, 1, 0 ]
//! [11]


//! [12]
QBitArray a(3);
QBitArray b(2);
QBitArray c;
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
c = a & b;                      // c: [ 1, 0, 0 ]
//! [12]


//! [13]
QBitArray a(3);
QBitArray b(2);
QBitArray c;
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
c = a | b;                      // c: [ 1, 1, 1 ]
//! [13]


//! [14]
QBitArray a(3);
QBitArray b(2);
QBitArray c;
a[0] = 1; a[1] = 0; a[2] = 1;   // a: [ 1, 0, 1 ]
b[0] = 1; b[1] = 0;             // b: [ 1, 1 ]
c = a ^ b;                      // c: [ 0, 1, 1 ]
//! [14]
