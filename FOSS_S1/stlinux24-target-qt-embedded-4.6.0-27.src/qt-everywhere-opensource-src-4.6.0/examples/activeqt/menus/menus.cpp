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

#include "menus.h"
#include <QAction>
#include <QAxFactory>
#include <QMenuBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QPixmap>

#include "fileopen.xpm"
#include "filesave.xpm"

QMenus::QMenus(QWidget *parent) 
    : QMainWindow(parent, 0) // QMainWindow's default flag is WType_TopLevel
{
    QAction *action;

    QMenu *file = new QMenu(this);

    action = new QAction(QPixmap((const char**)fileopen), "&Open", this);
    action->setShortcut(tr("CTRL+O"));
    connect(action, SIGNAL(triggered()), this, SLOT(fileOpen()));
    file->addAction(action);

    action = new QAction(QPixmap((const char**)filesave),"&Save", this);
    action->setShortcut(tr("CTRL+S"));
    connect(action, SIGNAL(triggered()), this, SLOT(fileSave()));
    file->addAction(action);

    QMenu *edit = new QMenu(this);

    action = new QAction("&Normal", this);
    action->setShortcut(tr("CTRL+N"));
    action->setToolTip("Normal");
    action->setStatusTip("Toggles Normal");
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(editNormal()));
    edit->addAction(action);

    action = new QAction("&Bold", this);
    action->setShortcut(tr("CTRL+B"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(editBold()));
    edit->addAction(action);

    action = new QAction("&Underline", this);
    action->setShortcut(tr("CTRL+U"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), this, SLOT(editUnderline()));
    edit->addAction(action);

    QMenu *advanced = new QMenu(this);
    action = new QAction("&Font...", this);
    connect(action, SIGNAL(triggered()), this, SLOT(editAdvancedFont()));
    advanced->addAction(action);

    action = new QAction("&Style...", this);
    connect(action, SIGNAL(triggered()), this, SLOT(editAdvancedStyle()));
    advanced->addAction(action);

    edit->addMenu(advanced)->setText("&Advanced");

    edit->addSeparator();

    action = new QAction("Una&vailable", this);
    action->setShortcut(tr("CTRL+V"));
    action->setCheckable(true);
    action->setEnabled(false);
    connect(action, SIGNAL(triggered()), this, SLOT(editUnderline()));
    edit->addAction(action);

    QMenu *help = new QMenu(this);

    action = new QAction("&About...", this);
    action->setShortcut(tr("F1"));
    connect(action, SIGNAL(triggered()), this, SLOT(helpAbout()));
    help->addAction(action);

    action = new QAction("&About Qt...", this);
    connect(action, SIGNAL(triggered()), this, SLOT(helpAboutQt()));
    help->addAction(action);

    if (!QAxFactory::isServer())
        menuBar()->addMenu(file)->setText("&File");
    menuBar()->addMenu(edit)->setText("&Edit");
    menuBar()->addMenu(help)->setText("&Help");

    editor = new QTextEdit(this);
    setCentralWidget(editor);

    statusBar();
}

void QMenus::fileOpen()
{
    editor->append("File Open selected.");
}

void QMenus::fileSave()
{
    editor->append("File Save selected.");
}

void QMenus::editNormal()
{
    editor->append("Edit Normal selected.");
}

void QMenus::editBold()
{
    editor->append("Edit Bold selected.");
}

void QMenus::editUnderline()
{
    editor->append("Edit Underline selected.");
}

void QMenus::editAdvancedFont()
{
    editor->append("Edit Advanced Font selected.");
}

void QMenus::editAdvancedStyle()
{
    editor->append("Edit Advanced Style selected.");
}

void QMenus::helpAbout()
{
    QMessageBox::about(this, "About QMenus", 
			"This example implements an in-place ActiveX control with menus and status messages.");
}

void QMenus::helpAboutQt()
{
    QMessageBox::aboutQt(this);
}
