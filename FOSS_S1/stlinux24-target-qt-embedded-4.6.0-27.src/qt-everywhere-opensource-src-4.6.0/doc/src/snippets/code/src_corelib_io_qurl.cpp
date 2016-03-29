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
QUrl url("http://www.example.com/List of holidays.xml");
// url.toEncoded() == "http://www.example.com/List%20of%20holidays.xml"
//! [0]


//! [1]
QUrl url = QUrl::fromEncoded("http://qt.nokia.com/List%20of%20holidays.xml");
//! [1]


//! [2]
bool checkUrl(const QUrl &url) {
    if (!url.isValid()) {
        qDebug(QString("Invalid URL: %1").arg(url.toString()));
        return false;
    }

    return true;
}
//! [2]


//! [3]
QFtp ftp;
ftp.connectToHost(url.host(), url.port(21));
//! [3]


//! [4]
http://www.example.com/cgi-bin/drawgraph.cgi?type-pie/color-green
//! [4]


//! [5]
QUrl baseUrl("http://qt.nokia.com/support");
QUrl relativeUrl("../products/solutions");
qDebug(baseUrl.resolved(relativeUrl).toString());
// prints "http://qt.nokia.com/products/solutions"
//! [5]


//! [6]
QByteArray ba = QUrl::toPercentEncoding("{a fishy string?}", "{}", "s");
qDebug(ba.constData());
// prints "{a fi%73hy %73tring%3F}"
//! [6]
