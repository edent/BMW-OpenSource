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

    QStateMachine machine;

//![0]
    QState *s1 = new QState();
    QState *s11 = new QState(s1);
    QState *s12 = new QState(s1);
    QState *s13 = new QState(s1);
    s1->setInitialState(s11);
    machine.addState(s1);
//![0]

//![2]
    s12->addTransition(quitButton, SIGNAL(clicked()), s12);
//![2]

//![1]
    QFinalState *s2 = new QFinalState();
    s1->addTransition(quitButton, SIGNAL(clicked()), s2);
    machine.addState(s2);

    QObject::connect(&machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
//![1]

  QButton *interruptButton = new QPushButton("Interrupt Button");
  QWidget *mainWindow = new QWidget();

//![3]
    QHistoryState *s1h = new QHistoryState(s1);

    QState *s3 = new QState();
    s3->assignProperty(label, "text", "In s3");
    QMessageBox *mbox = new QMessageBox(mainWindow);
    mbox->addButton(QMessageBox::Ok);
    mbox->setText("Interrupted!");
    mbox->setIcon(QMessageBox::Information);
    QObject::connect(s3, SIGNAL(entered()), mbox, SLOT(exec()));
    s3->addTransition(s1h);
    machine.addState(s3);

    s1->addTransition(interruptButton, SIGNAL(clicked()), s3);
//![3]

  return app.exec();
}

