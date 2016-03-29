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
QPushButton p("&Exit", parent); // automatic shortcut Alt+E
Q3PopupMenu *fileMenu = new fileMenu(parent);
fileMenu->insertItem("Undo", parent, SLOT(undo()),
                     Qt::CTRL + Qt::Key_Z);
//! [0]


//! [1]
accelerator->insertItem(QKeySequence("M"));
//! [1]


//! [2]
Q3Accel *a = new Q3Accel(myWindow);
a->connectItem(a->insertItem(Qt::CTRL + Qt::Key_P),
               myWindow, SLOT(printDoc()));
//! [2]


//! [3]
Q3Accel *a = new Q3Accel(myWindow);	   // create accels for myWindow
a->insertItem(CTRL + Key_P, 200);	   // Ctrl+P, e.g. to print document
a->insertItem(ALT + Key_X, 201);	   // Alt+X, e.g. to quit
a->insertItem(UNICODE_ACCEL + 'q', 202);   // Unicode 'q', e.g. to quit
a->insertItem(Key_D);			   // gets a unique negative id < -1
a->insertItem(CTRL + SHIFT + Key_P);	   // gets a unique negative id < -1
//! [3]


//! [4]
a->connectItem(201, mainView, SLOT(quit()));
//! [4]


//! [5]
Q3PopupMenu *file = new Q3PopupMenu(this);
file->insertItem(p1, tr("&Open..."), this, SLOT(open()),
                  Q3Accel::stringToKey(tr("Ctrl+O", "File|Open")));
//! [5]
