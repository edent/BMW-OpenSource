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

#include <QtGui>

#include "window.h"
#include "movementtransition.h"

//![0]
Window::Window()
{
    pX = 5;
    pY = 5;
//![0]

    QFontDatabase database;
    QFont font;
    if (database.families().contains("Monospace"))
        font = QFont("Monospace", 12);
    else {
        foreach (QString family, database.families()) {
            if (database.isFixedPitch(family)) {
                font = QFont(family, 12);
                break;
            }
        }
    }
    setFont(font);

//![1]
    setupMap();
    buildMachine();
}
//![1]

void Window::setStatus(const QString &status)
{
    myStatus = status;
    repaint();
}

QString Window::status() const
{
    return myStatus;
}

void Window::paintEvent(QPaintEvent * /* event */)
{
    QFontMetrics metrics(font());
    QPainter painter(this);
    int fontHeight = metrics.height();
    int fontWidth = metrics.width('X');
    int yPos = fontHeight;
    int xPos;

    painter.fillRect(rect(), Qt::black);
    painter.setPen(Qt::white);

    painter.drawText(QPoint(0, yPos), status());
    
    for (int y = 0; y < HEIGHT; ++y) {
        yPos += fontHeight;
        xPos = 0;

        for (int x = 0; x < WIDTH; ++x) {
            if (y == pY && x == pX) {
                xPos += fontWidth;
                continue;
            }

            painter.drawText(QPoint(xPos, yPos), map[x][y]);
            xPos += fontWidth;
        }
    }
    painter.drawText(QPoint(pX * fontWidth, (pY + 2) * fontHeight), QChar('@'));
}

QSize Window::sizeHint() const
{
    QFontMetrics metrics(font());

    return QSize(metrics.width('X') * WIDTH, metrics.height() * (HEIGHT + 1));
}

//![2]
void Window::buildMachine()
{
    machine = new QStateMachine;

    QState *inputState = new QState(machine);
    inputState->assignProperty(this, "status", "Move the rogue with 2, 4, 6, and 8");

    MovementTransition *transition = new MovementTransition(this);
    inputState->addTransition(transition);
//![2]

//![3]
    QState *quitState = new QState(machine);
    quitState->assignProperty(this, "status", "Really quit(y/n)?"); 

    QKeyEventTransition *yesTransition = new
        QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Y);
    yesTransition->setTargetState(new QFinalState(machine));
    quitState->addTransition(yesTransition);

    QKeyEventTransition *noTransition =
        new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_N);
    noTransition->setTargetState(inputState);
    quitState->addTransition(noTransition);
//![3]

//![4]
    QKeyEventTransition *quitTransition =
        new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Q);
    quitTransition->setTargetState(quitState);
    inputState->addTransition(quitTransition);
//![4]

//![5]
    machine->setInitialState(inputState);

    connect(machine, SIGNAL(finished()), qApp, SLOT(quit()));

    machine->start();
}
//![5]

void Window::movePlayer(Direction direction)
{
    switch (direction) {
        case Left:
            if (map[pX - 1][pY] != '#')
                --pX;            
            break;
        case Right:
            if (map[pX + 1][pY] != '#')
                ++pX;
            break;
        case Up:
            if (map[pX][pY - 1] != '#')
                --pY;
            break;
        case Down:
            if (map[pX][pY + 1] != '#')
                ++pY;
            break;        
    }
    repaint();
}

void Window::setupMap()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    for (int x = 0; x < WIDTH; ++x)
        for (int y = 0; y < HEIGHT; ++y) {
        if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1 || qrand() % 40 == 0)
            map[x][y] = '#';
        else
            map[x][y] = '.';
    }
}

