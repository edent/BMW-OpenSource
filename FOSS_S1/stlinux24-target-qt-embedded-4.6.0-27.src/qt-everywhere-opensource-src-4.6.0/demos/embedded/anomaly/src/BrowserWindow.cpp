/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
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

#include "BrowserWindow.h"

#include <QtCore>
#include <QtGui>

#include "BrowserView.h"
#include "HomeView.h"

BrowserWindow::BrowserWindow()
    : QWidget()
    , m_homeView(0)
    , m_browserView(0)
{
    m_timeLine = new QTimeLine(300, this);
    m_timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
    QTimer::singleShot(0, this, SLOT(initialize()));
}

void BrowserWindow::initialize()
{
    m_homeView = new HomeView(this);
    m_browserView = new BrowserView(this);

    m_homeView->hide();
    m_homeView->resize(size());
    m_homeView->move(0, 0);

    m_browserView->hide();
    m_browserView->resize(size());
    m_browserView->move(0, 0);

    connect(m_homeView, SIGNAL(addressEntered(QString)), SLOT(gotoAddress(QString)));
    connect(m_homeView, SIGNAL(urlActivated(QUrl)), SLOT(navigate(QUrl)));

    connect(m_browserView, SIGNAL(menuButtonClicked()), SLOT(showHomeView()));

    m_homeView->setVisible(false);
    m_browserView->setVisible(false);
    slide(0);

    connect(m_timeLine, SIGNAL(frameChanged(int)), SLOT(slide(int)));
}


// from Demo Browser
QUrl guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp test(QLatin1String("^[a-zA-Z]+\\:.*"));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = test.exactMatch(urlStr);
    if (hasSchema) {
        QUrl url = QUrl::fromEncoded(urlStr.toUtf8(), QUrl::TolerantMode);
        if (url.isValid())
            return url;
    }

    // Might be a file.
    if (QFile::exists(urlStr)) {
        QFileInfo info(urlStr);
        return QUrl::fromLocalFile(info.absoluteFilePath());
    }

    // Might be a shorturl - try to detect the schema.
    if (!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if (dotIndex != -1) {
            QString prefix = urlStr.left(dotIndex).toLower();
            QString schema = (prefix == QString("ftp")) ? prefix.toLatin1() : QString("http");
            QString location = schema + "://" + urlStr;
            QUrl url = QUrl::fromEncoded(location.toUtf8(), QUrl::TolerantMode);
            if (url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    QUrl url = QUrl::fromEncoded(string.toUtf8(), QUrl::TolerantMode);

    // finally for cases where the user just types in a hostname add http
    if (url.scheme().isEmpty())
        url = QUrl::fromEncoded("http://" + string.toUtf8(), QUrl::TolerantMode);
    return url;
}

void BrowserWindow::gotoAddress(const QString &address)
{
    m_browserView->navigate(guessUrlFromString(address));
    showBrowserView();
}

void BrowserWindow::navigate(const QUrl &url)
{
    m_browserView->navigate(url);
    showBrowserView();
}

void BrowserWindow::slide(int pos)
{
    m_browserView->move(pos, 0);
    m_homeView->move(pos - width(), 0);
    m_browserView->show();
    m_homeView->show();
}

void BrowserWindow::showHomeView()
{
    if (m_timeLine->state() != QTimeLine::NotRunning)
        return;

    m_timeLine->setFrameRange(0, width());
    m_timeLine->start();
    m_homeView->setFocus();
}

void BrowserWindow::showBrowserView()
{
    if (m_timeLine->state() != QTimeLine::NotRunning)
        return;

    m_timeLine->setFrameRange(width(), 0);
    m_timeLine->start();
    m_browserView->setFocus();
}

void BrowserWindow::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);

    if (event->key() == Qt::Key_F3) {
        if (m_homeView->isVisible())
            showBrowserView();
        else
            showHomeView();
    }
}

void BrowserWindow::resizeEvent(QResizeEvent *event)
{
    if (m_homeView)
        m_homeView->resize(size());

    if (m_browserView)
        m_browserView->resize(size());
}
