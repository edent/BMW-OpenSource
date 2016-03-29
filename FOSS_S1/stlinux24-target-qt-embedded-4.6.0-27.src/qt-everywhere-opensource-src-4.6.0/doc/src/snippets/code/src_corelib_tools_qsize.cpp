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
QSize t1(10, 12);
t1.scale(60, 60, Qt::IgnoreAspectRatio);
// t1 is (60, 60)

QSize t2(10, 12);
t2.scale(60, 60, Qt::KeepAspectRatio);
// t2 is (50, 60)

QSize t3(10, 12);
t3.scale(60, 60, Qt::KeepAspectRatioByExpanding);
// t3 is (60, 72)
//! [0]


//! [1]
QSize size(100, 10);
size.rwidth() += 20;

// size becomes (120,10)
//! [1]


//! [2]
QSize size(100, 10);
size.rheight() += 5;

// size becomes (100,15)
//! [2]


//! [3]
QSize s( 3, 7);
QSize r(-1, 4);
s += r;

// s becomes (2,11)
//! [3]


//! [4]
QSize s( 3, 7);
QSize r(-1, 4);
s -= r;

// s becomes (4,3)
//! [4]


//! [5]
QSizeF t1(10, 12);
t1.scale(60, 60, Qt::IgnoreAspectRatio);
// t1 is (60, 60)

QSizeF t2(10, 12);
t2.scale(60, 60, Qt::KeepAspectRatio);
// t2 is (50, 60)

QSizeF t3(10, 12);
t3.scale(60, 60, Qt::KeepAspectRatioByExpanding);
// t3 is (60, 72)
//! [5]


//! [6]
QSizeF size(100.3, 10);
size.rwidth() += 20.5;

 // size becomes (120.8,10)
//! [6]


//! [7]
QSizeF size(100, 10.2);
size.rheight() += 5.5;

// size becomes (100,15.7)
//! [7]


//! [8]
QSizeF s( 3, 7);
QSizeF r(-1, 4);
s += r;

// s becomes (2,11)
//! [8]


//! [9]
QSizeF s( 3, 7);
QSizeF r(-1, 4);
s -= r;

// s becomes (4,3)
//! [9]
