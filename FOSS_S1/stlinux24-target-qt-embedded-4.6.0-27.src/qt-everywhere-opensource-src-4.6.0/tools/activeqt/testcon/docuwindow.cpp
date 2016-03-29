/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "docuwindow.h"
#include <QTextBrowser>
#include <QTextDocument>
#include <QToolBar>
#include <QToolButton>
#include <QFileDialog>
#include <QFile>
#include <QStatusBar>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QTextStream>

QT_BEGIN_NAMESPACE

static const char *filesave[] = {
"    14    14        4            1",
". c #040404",
"# c #808304",
"a c #bfc2bf",
"b c None",
"..............",
".#.aaaaaaaa.a.",
".#.aaaaaaaa...",
".#.aaaaaaaa.#.",
".#.aaaaaaaa.#.",
".#.aaaaaaaa.#.",
".#.aaaaaaaa.#.",
".##........##.",
".############.",
".##.........#.",
".##......aa.#.",
".##......aa.#.",
".##......aa.#.",
"b............."
};

static const char *fileprint[] = {
"    16    14        6            1",
". c #000000",
"# c #848284",
"a c #c6c3c6",
"b c #ffff00",
"c c #ffffff",
"d c None",
"ddddd.........dd",
"dddd.cccccccc.dd",
"dddd.c.....c.ddd",
"ddd.cccccccc.ddd",
"ddd.c.....c....d",
"dd.cccccccc.a.a.",
"d..........a.a..",
".aaaaaaaaaa.a.a.",
".............aa.",
".aaaaaa###aa.a.d",
".aaaaaabbbaa...d",
".............a.d",
"d.aaaaaaaaa.a.dd",
"dd...........ddd"
};


DocuWindow::DocuWindow(const QString& docu, QWidget *parent, QWidget *source)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("%1 - Documentation").arg(source->windowTitle()));

    browser = new QTextBrowser(this);
    browser->setHtml(docu);

    setCentralWidget(browser);

    QToolBar *fileTools = new QToolBar(tr("File Operations"), this);
    fileTools->addAction(QPixmap(filesave), tr("Save File"), this, SLOT(save()));
    fileTools->addAction(QPixmap(fileprint), tr("Print"), this, SLOT(print()));

    addToolBar(fileTools);
    statusBar();
}

void DocuWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this);

    if (filename.isEmpty())
	return;

    QString text = browser->document()->toHtml();
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly)) {
	statusBar()->showMessage(tr("Could not write to %1").arg(filename), 2000);
	return;
    }

    QTextStream t(&f);
    t << text;
    f.close();

    statusBar()->showMessage(tr("File %1 saved").arg(filename), 2000);
}

void DocuWindow::print()
{
    QPrinter printer;
    if (printer.printerName().isEmpty()) {
	statusBar()->showMessage(tr("No printer installed"), 2000);
	return;
    }

    QPrintDialog printDialog(&printer, this);
    if (!printDialog.exec()) {
	statusBar()->showMessage(tr("Printing aborted"), 2000);
	return;
    }

    browser->document()->print(&printer);
}

QT_END_NAMESPACE
