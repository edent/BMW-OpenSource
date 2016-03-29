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

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QTimer>

#include "glwidget.h"

MainWindow::MainWindow()
{
    GLWidget *glwidget = new GLWidget();
    QLabel *label = new QLabel(this);
    QTimer *timer = new QTimer(this);
    QSlider *slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    
    slider->setRange(0, 100);
    slider->setSliderPosition(50);
    timer->setInterval(10);
    label->setText("A QGlWidget with OpenGl ES");
    label->setAlignment(Qt::AlignHCenter);

    QGroupBox * groupBox = new QGroupBox(this);
    setCentralWidget(groupBox);
    groupBox->setTitle("OpenGL ES Example");

    QGridLayout *layout = new QGridLayout(groupBox);

    layout->addWidget(glwidget,1,0,8,1);
    layout->addWidget(label,9,0,1,1);
    layout->addWidget(slider, 11,0,1,1);

    groupBox->setLayout(layout);

    QMenu *fileMenu = new QMenu("File");
    QMenu *helpMenu = new QMenu("Help");
    QMenu *showMenu = new QMenu("Show");
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(showMenu);
    menuBar()->addMenu(helpMenu);
    QAction *exit = new QAction("Exit", fileMenu);
    QAction *aboutQt = new QAction("AboutQt", helpMenu);
    QAction *showLogo = new QAction("Show 3D Logo", showMenu);
    QAction *showTexture = new QAction("Show 2D Texture", showMenu);
    QAction *showBubbles = new QAction("Show bubbles", showMenu);
    showBubbles->setCheckable(true);
    showBubbles->setChecked(true);
    fileMenu->addAction(exit);
    helpMenu->addAction(aboutQt);
    showMenu->addAction(showLogo);
    showMenu->addAction(showTexture);
    showMenu->addAction(showBubbles);

    QObject::connect(timer, SIGNAL(timeout()), glwidget, SLOT(updateGL()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    QObject::connect(showLogo, SIGNAL(triggered(bool)), glwidget, SLOT(setLogo()));
    QObject::connect(showTexture, SIGNAL(triggered(bool)), glwidget, SLOT(setTexture()));
    QObject::connect(showBubbles, SIGNAL(triggered(bool)), glwidget, SLOT(showBubbles(bool)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), glwidget, SLOT(setScaling(int)));
    timer->start();
}
