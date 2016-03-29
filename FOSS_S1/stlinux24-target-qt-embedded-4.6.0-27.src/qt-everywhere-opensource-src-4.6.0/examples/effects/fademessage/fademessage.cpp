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

#include "fademessage.h"

#include <QtGui>

FadeMessage::FadeMessage(QWidget *parent): QGraphicsView(parent)
{
    setScene(&m_scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setupScene();

    m_animation = new QPropertyAnimation(m_effect, "strength", this);
    m_animation->setDuration(500);
    m_animation->setEasingCurve(QEasingCurve::InOutSine);
    m_animation->setStartValue(0);
    m_animation->setEndValue(1);


    setRenderHint(QPainter::Antialiasing, true);
    setFrameStyle(QFrame::NoFrame);
}

void FadeMessage::togglePopup()
{
    if (m_message->isVisible()) {
        m_message->setVisible(false);
        m_animation->setDirection(QAbstractAnimation::Backward);
    } else {
        m_message->setVisible(true);
        m_animation->setDirection(QAbstractAnimation::Forward);
    }
    m_animation->start();
}

void FadeMessage::setupScene()
{
    QGraphicsRectItem *parent = m_scene.addRect(0, 0, 400, 600);
    parent->setPen(Qt::NoPen);
    parent->setZValue(0);

    QGraphicsPixmapItem *bg = m_scene.addPixmap(QPixmap(":/background.jpg"));
    bg->setParentItem(parent);
    bg->setZValue(-1);

    for (int i = 1; i < 5; ++i)
        for (int j = 2; j < 5; ++j) {
            QGraphicsRectItem *item = m_scene.addRect(i * 50, j * 50, 38, 38);
            item->setParentItem(parent);
            item->setZValue(1);
            int hue = 12 * (i * 5 + j);
            item->setBrush(QColor::fromHsv(hue, 128, 128));
        }

    QFont font;
    font.setPointSize(font.pointSize() * 2);
    font.setBold(true);
    QFontMetrics fontMetrics(font);
    int fh = fontMetrics.height();

    QString sceneText = "Qt Everywhere!";
    int sceneTextWidth = fontMetrics.width(sceneText);

    QGraphicsRectItem *block = m_scene.addRect(50, 300, sceneTextWidth, fh + 3);
    block->setPen(Qt::NoPen);
    block->setBrush(QColor(102, 153, 51));

    QGraphicsTextItem *text = m_scene.addText(sceneText, font);
    text->setDefaultTextColor(Qt::white);
    text->setPos(50, 300);
    block->setZValue(2);
    block->hide();

    text->setParentItem(block);
    m_message = block;

    m_effect = new QGraphicsColorizeEffect;
    m_effect->setColor(QColor(122, 193, 66));
    m_effect->setStrength(0);
    m_effect->setEnabled(true);
    parent->setGraphicsEffect(m_effect);

    QPushButton *press = new QPushButton;
    press->setText(tr("Press me"));
    connect(press, SIGNAL(clicked()), SLOT(togglePopup()));
    m_scene.addWidget(press);
    press->move(300, 500);
}

