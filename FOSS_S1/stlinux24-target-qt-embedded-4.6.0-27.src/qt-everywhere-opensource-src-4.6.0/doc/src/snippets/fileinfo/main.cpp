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

#include <QApplication>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//! [0]
    QFileInfo fileInfo1("~/examples/191697/.");
    QFileInfo fileInfo2("~/examples/191697/..");
    QFileInfo fileInfo3("~/examples/191697/main.cpp");
//! [0]    
//! [1]    
    QFileInfo fileInfo4(".");
    QFileInfo fileInfo5("..");
    QFileInfo fileInfo6("main.cpp");
//! [1]    

    qDebug() << fileInfo1.fileName();
    qDebug() << fileInfo2.fileName();
    qDebug() << fileInfo3.fileName();
    qDebug() << fileInfo4.fileName();
    qDebug() << fileInfo5.fileName();
    qDebug() << fileInfo6.fileName();

    QPushButton* button1 = new QPushButton(fileInfo1.dir().path());
    QPushButton* button2 = new QPushButton(fileInfo2.dir().path());
    QPushButton* button3 = new QPushButton(fileInfo3.dir().path());
    QPushButton* button4 = new QPushButton(fileInfo4.dir().path());
    QPushButton* button5 = new QPushButton(fileInfo5.dir().path());
    QPushButton* button6 = new QPushButton(fileInfo6.dir().path());

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(button1);
    vbox->addWidget(button2);
    vbox->addWidget(button3);
    vbox->addWidget(button4);
    vbox->addWidget(button5);
    vbox->addWidget(button6);
    vbox->addStretch(1);

    QGroupBox *groupBox = new QGroupBox("QFileInfo::dir() test");
    groupBox->setLayout(vbox);
    groupBox->show();

    return app.exec();
}
