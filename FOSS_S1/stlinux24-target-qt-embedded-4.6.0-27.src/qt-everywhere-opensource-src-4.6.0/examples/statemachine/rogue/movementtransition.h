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

#ifndef MOVEMENTTRANSITION_H
#define MOVEMENTTRANSITION_H

#include <QtGui>

#include "window.h"

//![0]
class MovementTransition : public QEventTransition
{
    Q_OBJECT

public:
    MovementTransition(Window *window) :
        QEventTransition(window, QEvent::KeyPress) {
        this->window = window;
    }
//![0]

//![1]
protected:
    bool eventTest(QEvent *event) {
        if (event->type() == QEvent::StateMachineWrapped &&
            static_cast<QStateMachine::WrappedEvent *>(event)->event()->type() == QEvent::KeyPress) {
            QEvent *wrappedEvent = static_cast<QStateMachine::WrappedEvent *>(event)->event();

            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(wrappedEvent);
            int key = keyEvent->key();

            return key == Qt::Key_2 || key == Qt::Key_8 || key == Qt::Key_6 ||
                   key == Qt::Key_4;
        }
        return false;
    }
//![1]

//![2]
    void onTransition(QEvent *event) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(
            static_cast<QStateMachine::WrappedEvent *>(event)->event());

        int key = keyEvent->key();
        switch (key) {
            case Qt::Key_4:
                window->movePlayer(Window::Left);
                break;
            case Qt::Key_8:
                window->movePlayer(Window::Up);
                break;
            case Qt::Key_6:
                window->movePlayer(Window::Right);
                break;
            case Qt::Key_2:
                window->movePlayer(Window::Down);
                break;
            default:
                ;
        }
    }
//![2]

private:
    Window *window;
};

#endif

