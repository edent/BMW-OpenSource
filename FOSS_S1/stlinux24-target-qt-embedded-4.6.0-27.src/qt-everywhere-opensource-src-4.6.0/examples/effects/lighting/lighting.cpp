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

#include "lighting.h"

#include <QtGui>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Lighting::Lighting(QWidget *parent): QGraphicsView(parent), angle(0.0)
{
    setScene(&m_scene);

    setupScene();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(animate()));
    timer->setInterval(30);
    timer->start();

    setRenderHint(QPainter::Antialiasing, true);
    setFrameStyle(QFrame::NoFrame);
}

void Lighting::setupScene()
{
    m_scene.setSceneRect(-300, -200, 600, 460);

    QLinearGradient linearGrad(QPointF(-100, -100), QPointF(100, 100));
    linearGrad.setColorAt(0, QColor(255, 255, 255));
    linearGrad.setColorAt(1, QColor(192, 192, 255));
    setBackgroundBrush(linearGrad);

    QRadialGradient radialGrad(30, 30, 30);
    radialGrad.setColorAt(0, Qt::yellow);
    radialGrad.setColorAt(0.2, Qt::yellow);
    radialGrad.setColorAt(1, Qt::transparent);
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.setBrush(radialGrad);
    painter.drawEllipse(0, 0, 60, 60);
    painter.end();

    m_lightSource = m_scene.addPixmap(pixmap);
    m_lightSource->setZValue(2);

    for (int i = -2; i < 3; ++i)
        for (int j = -2; j < 3; ++j) {
            QAbstractGraphicsShapeItem *item;
            if ((i + j) & 1)
                item = new QGraphicsEllipseItem(0, 0, 50, 50);
            else
                item = new QGraphicsRectItem(0, 0, 50, 50);

            item->setPen(QPen(Qt::black, 1));
            item->setBrush(QBrush(Qt::white));
            QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
            effect->setBlurRadius(8);
            item->setGraphicsEffect(effect);
            item->setZValue(1);
            item->setPos(i * 80, j * 80);
            m_scene.addItem(item);
            m_items << item;
        }


}

void Lighting::animate()
{
    angle += (M_PI / 30);
    qreal xs = 200 * sin(angle) - 40 + 25;
    qreal ys = 200 * cos(angle) - 40 + 25;
    m_lightSource->setPos(xs, ys);

    for (int i = 0; i < m_items.size(); ++i) {
        QGraphicsItem *item = m_items.at(i);
        Q_ASSERT(item);
        QGraphicsDropShadowEffect *effect = static_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect());
        Q_ASSERT(effect);

        QPointF delta(item->x() - xs, item->y() - ys);
        effect->setOffset(delta.toPoint() / 30);

        qreal dx = delta.x();
        qreal dy = delta.y();
        qreal dd = sqrt(dx * dx + dy * dy);
        QColor color = effect->color();
        color.setAlphaF(qBound(0.4, 1 - dd / 200.0, 0.7));
        effect->setColor(color);
    }

    m_scene.update();
}

