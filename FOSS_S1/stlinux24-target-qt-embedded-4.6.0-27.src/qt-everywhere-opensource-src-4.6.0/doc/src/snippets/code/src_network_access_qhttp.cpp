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
content-type: text/html
//! [0]


//! [1]
header.setValue("content-type", "text/html");
QString contentType = header.value("content-type");
//! [1]


//! [2]
QHttpRequestHeader header("GET", QUrl::toPercentEncoding("/index.html"));
header.setValue("Host", "qt.nokia.com");
http->setHost("qt.nokia.com");
http->request(header);
//! [2]


//! [3]
http->setHost("qt.nokia.com");                // id == 1
http->get(QUrl::toPercentEncoding("/index.html")); // id == 2
//! [3]


//! [4]
requestStarted(1)
requestFinished(1, false)

requestStarted(2)
stateChanged(Connecting)
stateChanged(Sending)
dataSendProgress(77, 77)
stateChanged(Reading)
responseHeaderReceived(responseheader)
dataReadProgress(5388, 0)
readyRead(responseheader)
dataReadProgress(18300, 0)
readyRead(responseheader)
stateChanged(Connected)
requestFinished(2, false)

done(false)

stateChanged(Closing)
stateChanged(Unconnected)
//! [4]


//! [5]
http->setHost("www.foo.bar");       // id == 1
http->get("/index.html");           // id == 2
http->post("register.html", data);  // id == 3
//! [5]


//! [6]
requestStarted(1)
requestFinished(1, false)

requestStarted(2)
stateChanged(HostLookup)
requestFinished(2, true)

done(true)

stateChanged(Unconnected)
//! [6]


//! [7]
void Ticker::getTicks()
{
  http = new QHttp(this);
  connect(http, SIGNAL(done(bool)), this, SLOT(showPage()));
  http->setProxy("proxy.example.com", 3128);
  http->setHost("ticker.example.com");
  http->get("/ticks.asp");
}

void Ticker::showPage()
{
  display(http->readAll());
}
//! [7]
