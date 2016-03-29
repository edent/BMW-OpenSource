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
Q3CString str("helloworld", 6); // assigns "hello" to str
//! [0]


//! [1]
Q3CString a;                // a.data() == 0,  a.size() == 0, a.length() == 0
Q3CString b == "";        // b.data() == "", b.size() == 1, b.length() == 0
a.isNull();                // true  because a.data() == 0
a.isEmpty();        // true  because a.length() == 0
b.isNull();                // false because b.data() == ""
b.isEmpty();        // true  because b.length() == 0
//! [1]


//! [2]
Q3CString s = "truncate this string";
s.truncate(5);                      // s == "trunc"
//! [2]


//! [3]
Q3CString s;
s.sprintf("%d - %s", 1, "first");                // result < 256 chars

Q3CString big(25000);                        // very long string
big.sprintf("%d - %s", 2, longString);        // result < 25000 chars
//! [3]


//! [4]
Q3CString s("apple");
Q3CString t = s.leftJustify(8, '.');  // t == "apple..."
//! [4]


//! [5]
Q3CString s("pie");
Q3CString t = s.rightJustify(8, '.');  // t == ".....pie"
//! [5]
