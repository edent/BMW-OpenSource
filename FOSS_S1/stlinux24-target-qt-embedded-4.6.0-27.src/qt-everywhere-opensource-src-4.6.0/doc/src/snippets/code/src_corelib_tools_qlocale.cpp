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
QLocale egyptian(QLocale::Arabic, QLocale::Egypt);
QString s1 = egyptian.toString(1.571429E+07, 'e');
QString s2 = egyptian.toString(10);

double d = egyptian.toDouble(s1);
int i = egyptian.toInt(s2);
//! [0]


//! [1]
QLocale::setDefault(QLocale(QLocale::Hebrew, QLocale::Israel));
QLocale hebrew; // Constructs a default QLocale
QString s1 = hebrew.toString(15714.3, 'e');

bool ok;
double d;

QLocale::setDefault(QLocale::C);
d = QString("1234,56").toDouble(&ok);   // ok == false
d = QString("1234.56").toDouble(&ok);   // ok == true, d == 1234.56

QLocale::setDefault(QLocale::German);
d = QString("1234,56").toDouble(&ok);   // ok == true, d == 1234.56
d = QString("1234.56").toDouble(&ok);   // ok == true, d == 1234.56

QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
str = QString("%1 %L2 %L3")
      .arg(12345).arg(12345).arg(12345, 0, 16);
// str == "12345 12,345 3039"
//! [1]


//! [2]
QLocale korean("ko");
QLocale swiss("de_CH");
//! [2]


//! [3]
bool ok;
double d;

QLocale c(QLocale::C);
d = c.toDouble( "1234.56", &ok );  // ok == true, d == 1234.56
d = c.toDouble( "1,234.56", &ok ); // ok == true, d == 1234.56
d = c.toDouble( "1234,56", &ok );  // ok == false

QLocale german(QLocale::German);
d = german.toDouble( "1234,56", &ok );  // ok == true, d == 1234.56
d = german.toDouble( "1.234,56", &ok ); // ok == true, d == 1234.56
d = german.toDouble( "1234.56", &ok );  // ok == false

d = german.toDouble( "1.234", &ok );    // ok == true, d == 1234.0
//! [3]
