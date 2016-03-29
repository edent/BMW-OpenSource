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

int main(int argv, char **args)
{
    QApplication app(argv, args);
    QWidget *button;

  {
//![0]
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *s1 = new QState();
    s1->assignProperty(object, "fooBar", 1.0);
    machine.addState(s1);
    machine.setInitialState(s1);

    QState *s2 = new QState();
    machine.addState(s2);
//![0]
  }

  {

//![2]
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *s1 = new QState();
    s1->assignProperty(object, "fooBar", 1.0);
    machine.addState(s1);
    machine.setInitialState(s1);

    QState *s2 = new QState(s1);
    s2->assignProperty(object, "fooBar", 2.0);
    s1->setInitialState(s2);

    QState *s3 = new QState(s1);
//![2]

  }

  {
//![3]
    QState *s1 = new QState();
    QState *s2 = new QState();

    s1->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    s2->assignProperty(button, "geometry", QRectF(0, 0, 100, 100));

    s1->addTransition(button, SIGNAL(clicked()), s2);
//![3]

  }

  {
//![4]
    QState *s1 = new QState();
    QState *s2 = new QState();

    s1->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    s2->assignProperty(button, "geometry", QRectF(0, 0, 100, 100));

    QSignalTransition *transition = s1->addTransition(button, SIGNAL(clicked()), s2);
    transition->addAnimation(new QPropertyAnimation(button, "geometry"));
//![4]

  }

  {
    QMainWindow *mainWindow = 0;

//![5]
    QMessageBox *messageBox = new QMessageBox(mainWindow);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->setText("Button geometry has been set!");
    messageBox->setIcon(QMessageBox::Information);

    QState *s1 = new QState();

    QState *s2 = new QState();
    s2->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    connect(s2, SIGNAL(entered()), messageBox, SLOT(exec()));

    s1->addTransition(button, SIGNAL(clicked()), s2);
//![5]
  }

  {
    QMainWindow *mainWindow = 0;

//![6]
    QMessageBox *messageBox = new QMessageBox(mainWindow);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->setText("Button geometry has been set!");
    messageBox->setIcon(QMessageBox::Information);

    QState *s1 = new QState();

    QState *s2 = new QState();
    s2->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));

    QState *s3 = new QState();
    connect(s3, SIGNAL(entered()), messageBox, SLOT(exec()));

    s1->addTransition(button, SIGNAL(clicked()), s2);
    s2->addTransition(s2, SIGNAL(propertiesAssigned()), s3);
//![6]

  }

  {

//![7]
    QState *s1 = new QState();
    QState *s2 = new QState();

    s2->assignProperty(object, "fooBar", 2.0);
    s1->addTransition(s2);

    QStateMachine machine;
    machine.setInitialState(s1);
    machine.addDefaultAnimation(new QPropertyAnimation(object, "fooBar"));
//![7]

  }



    return app.exec();
}

