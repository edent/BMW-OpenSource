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
QFtp *ftp = new QFtp(parent);
ftp->connectToHost("ftp.qt.nokia.com");
ftp->login();
//! [0]


//! [1]
ftp->connectToHost("ftp.qt.nokia.com");   // id == 1
ftp->login();                             // id == 2
ftp->cd("qt");                            // id == 3
ftp->get("INSTALL");                      // id == 4
ftp->close();                             // id == 5
//! [1]


//! [2]
start(1)
stateChanged(HostLookup)
stateChanged(Connecting)
stateChanged(Connected)
finished(1, false)

start(2)
stateChanged(LoggedIn)
finished(2, false)

start(3)
finished(3, false)

start(4)
dataTransferProgress(0, 3798)
dataTransferProgress(2896, 3798)
readyRead()
dataTransferProgress(3798, 3798)
readyRead()
finished(4, false)

start(5)
stateChanged(Closing)
stateChanged(Unconnected)
finished(5, false)

done(false)
//! [2]


//! [3]
start(1)
stateChanged(HostLookup)
stateChanged(Connecting)
stateChanged(Connected)
finished(1, false)

start(2)
finished(2, true)

done(true)
//! [3]
