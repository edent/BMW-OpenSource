/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qdesktopwidget.h"
#include "qapplication_p.h"
#include "qwidget_p.h"
#include "qt_s60_p.h"
#include <w32std.h>

#include "hal.h"
#include "hal_data.h"

QT_BEGIN_NAMESPACE

class QDesktopWidgetPrivate : public QWidgetPrivate
{

public:
    QDesktopWidgetPrivate();
    ~QDesktopWidgetPrivate();

    static void init(QDesktopWidget *that);
    static void cleanup();

    static int screenCount;
    static int primaryScreen;

    static QVector<QRect> *rects;
    static QVector<QRect> *workrects;

    static int refcount;
};

int QDesktopWidgetPrivate::screenCount = 1;
int QDesktopWidgetPrivate::primaryScreen = 0;
QVector<QRect> *QDesktopWidgetPrivate::rects = 0;
QVector<QRect> *QDesktopWidgetPrivate::workrects = 0;
int QDesktopWidgetPrivate::refcount = 0;

QDesktopWidgetPrivate::QDesktopWidgetPrivate()
{
    ++refcount;
}

QDesktopWidgetPrivate::~QDesktopWidgetPrivate()
{
    if (!--refcount)
        cleanup();
}

void QDesktopWidgetPrivate::init(QDesktopWidget *that)
{
    int screenCount=0;

    if (HAL::Get(0, HALData::EDisplayNumberOfScreens, screenCount) == KErrNone)
        QDesktopWidgetPrivate::screenCount = screenCount;
    else
        QDesktopWidgetPrivate::screenCount = 0;

    rects = new QVector<QRect>();
    workrects = new QVector<QRect>();

    rects->resize(QDesktopWidgetPrivate::screenCount);
    workrects->resize(QDesktopWidgetPrivate::screenCount);

    // ### TODO: Implement proper multi-display support
    rects->resize(1);
    rects->replace(0, that->rect());
    workrects->resize(1);
    workrects->replace(0, that->rect());
}

void QDesktopWidgetPrivate::cleanup()
{
    delete rects;
    rects = 0;
    delete workrects;
    workrects = 0;
}


QDesktopWidget::QDesktopWidget()
    : QWidget(*new QDesktopWidgetPrivate, 0, Qt::Desktop)
{
    setObjectName(QLatin1String("desktop"));
    QDesktopWidgetPrivate::init(this);
}

QDesktopWidget::~QDesktopWidget()
{
}

bool QDesktopWidget::isVirtualDesktop() const
{
    return true;
}

int QDesktopWidget::primaryScreen() const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

int QDesktopWidget::numScreens() const
{
    Q_D(const QDesktopWidget);
    return QDesktopWidgetPrivate::screenCount;
}

QWidget *QDesktopWidget::screen(int /* screen */)
{
    return this;
}

const QRect QDesktopWidget::availableGeometry(int /* screen */) const
{
    TRect clientRect = static_cast<CEikAppUi*>(S60->appUi())->ClientRect();
    return qt_TRect2QRect(clientRect);
}

const QRect QDesktopWidget::screenGeometry(int /* screen */) const
{
    Q_D(const QDesktopWidget);
    return QRect(0, 0, S60->screenWidthInPixels, S60->screenHeightInPixels);
    }

int QDesktopWidget::screenNumber(const QWidget * /* widget */) const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

int QDesktopWidget::screenNumber(const QPoint & /* point */) const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

void QDesktopWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QDesktopWidget);
    QVector<QRect> oldrects;
    oldrects = *d->rects;
    QVector<QRect> oldworkrects;
    oldworkrects = *d->workrects;
    int oldscreencount = d->screenCount;

    QDesktopWidgetPrivate::cleanup();
    QDesktopWidgetPrivate::init(this);

    for (int i = 0; i < qMin(oldscreencount, d->screenCount); ++i) {
        QRect oldrect = oldrects[i];
        QRect newrect = d->rects->at(i);
        if (oldrect != newrect)
            emit resized(i);
    }

    for (int j = 0; j < qMin(oldscreencount, d->screenCount); ++j) {
        QRect oldrect = oldworkrects[j];
        QRect newrect = d->workrects->at(j);
        if (oldrect != newrect)
            emit workAreaResized(j);
    }
}

QT_END_NAMESPACE
