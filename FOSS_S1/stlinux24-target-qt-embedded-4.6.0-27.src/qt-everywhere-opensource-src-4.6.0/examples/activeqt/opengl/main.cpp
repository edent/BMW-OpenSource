/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
//
// Qt OpenGL example: Box
//
// A small example showing how a GLWidget can be used just as any Qt widget
//
// File: main.cpp
//
// The main() function
//

#include "globjwin.h"
#include "glbox.h"
#include <QApplication>
#include <QtOpenGL>
//! [0]
#include <QAxFactory>

QAXFACTORY_DEFAULT( GLBox,
		    "{5fd9c22e-ed45-43fa-ba13-1530bb6b03e0}",
		    "{33b051af-bb25-47cf-a390-5cfd2987d26a}",
		    "{8c996c29-eafa-46ac-a6f9-901951e765b5}",
		    "{2c3c183a-eeda-41a4-896e-3d9c12c3577d}",
		    "{83e16271-6480-45d5-aaf1-3f40b7661ae4}"
		  )

//! [0] //! [1]
/*
  The main program is here.
*/

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a(argc,argv);

    if ( !QGLFormat::hasOpenGL() ) {
	qWarning( "This system has no OpenGL support. Exiting." );
	return -1;
    }

    if ( !QAxFactory::isServer() ) {
	GLObjectWindow w;
	w.resize( 400, 350 );
	w.show();
	return a.exec();
//! [1] //! [2]
    }
    return a.exec();
//! [2] //! [3]
}
//! [3]
