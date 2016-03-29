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

#include "mainwindow.h"
#include "stationdialog.h"

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QLinearGradient>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

MainWindow::MainWindow()
    : QWidget(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QAction *configAction = new QAction(tr("&Select station..."), this);
    configAction->setShortcut(tr("Ctrl+C"));
    connect(configAction, SIGNAL(triggered()), this, SLOT(configure()));

    addAction(configAction);
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    setWindowTitle(tr("Traffic Info Oslo"));

    const QSettings settings("Qt Traffic Info", "trafficinfo");
    m_station = StationInformation(settings.value("stationId", "03012130").toString(),
                                   settings.value("stationName", "Nydalen [T-bane] (OSL)").toString());
    m_lines = settings.value("lines", QStringList()).toStringList();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInformation()));
    timer->start(1000*60*5);
    QMetaObject::invokeMethod(this, SLOT(updateTimeInformation()), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    QSettings settings("Qt Traffic Info", "trafficinfo");
    settings.setValue("stationId", m_station.id());
    settings.setValue("stationName", m_station.name());
    settings.setValue("lines", m_lines);
}

QSize MainWindow::sizeHint() const
{
    return QSize(300, 200);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    const QPoint start(width()/2, 0);
    const QPoint finalStop(width()/2, height());
    QLinearGradient gradient(start, finalStop);
    const QColor qtGreen(102, 176, 54);
    gradient.setColorAt(0, qtGreen.dark());
    gradient.setColorAt(0.5, qtGreen);
    gradient.setColorAt(1, qtGreen.dark());

    QPainter p(this);
    p.fillRect(0, 0, width(), height(), gradient);

    QFont headerFont("Sans Serif", 12, QFont::Bold);
    QFont normalFont("Sans Serif", 9, QFont::Normal);

    // draw it twice for shadow effect
    p.setFont(headerFont);
    QRect headerRect(1, 1, width(), 25);
    p.setPen(Qt::black);
    p.drawText(headerRect, Qt::AlignCenter, m_station.name());

    headerRect.moveTopLeft(QPoint(0, 0));
    p.setPen(Qt::white);
    p.drawText(headerRect, Qt::AlignCenter, m_station.name());

    p.setFont(normalFont);
    int pos = 40;
    for (int i = 0; i < m_times.count() && i < 9; ++i) {
        p.setPen(Qt::black);
        p.drawText(51, pos + 1, m_times.at(i).time());
        p.drawText(101, pos + 1, m_times.at(i).direction());

        p.setPen(Qt::white);
        p.drawText(50, pos, m_times.at(i).time());
        p.drawText(100, pos, m_times.at(i).direction());

        pos += 18;
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    QBitmap maskBitmap(width(), height());
    maskBitmap.clear();

    QPainter p(&maskBitmap);
    p.setBrush(Qt::black);
    p.drawRoundRect(0, 0, width(), height(), 20, 30);
    p.end();

    setMask(maskBitmap);
}

void MainWindow::updateTimeInformation()
{
    m_times = TimeQuery::query(m_station.id(), m_lines, QDateTime::currentDateTime());

    update();
}

void MainWindow::configure()
{
    StationDialog dlg(m_station.name(), m_lines, this);
    if (dlg.exec()) {
        m_station = dlg.selectedStation();
        m_lines = dlg.lineNumbers();
        updateTimeInformation();
    }
}
