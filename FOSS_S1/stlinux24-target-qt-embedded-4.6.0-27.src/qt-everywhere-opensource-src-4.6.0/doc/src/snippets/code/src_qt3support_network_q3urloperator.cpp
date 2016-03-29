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
Q3UrlOperator *op = new Q3UrlOperator();
op->copy( QString("ftp://ftp.qt.nokia.com/qt/source/qt-2.1.0.tar.gz"),
	 "file:///tmp" );
//! [0]


//! [1]
Q3UrlOperator op( "http://www.whatever.org/cgi-bin/search.pl?cmd=Hello" );
op.get();
//! [1]


//! [2]
Q3UrlOperator op( "ftp://ftp.whatever.org/pub" );
// do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
op.get( "a_file.txt" );
//! [2]


//! [3]
Q3UrlOperator op( "http://www.whatever.org/cgi-bin" );
op.get( "search.pl?cmd=Hello" ); // WRONG!
//! [3]


//! [4]
Q3UrlOperator op( "ftp://ftp.whatever.com/home/me/filename.dat" );
op.put( data );
//! [4]


//! [5]
Q3UrlOperator op( "ftp://ftp.whatever.com/home/me" );
// do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
op.put( data, "filename.dat" );
//! [5]
