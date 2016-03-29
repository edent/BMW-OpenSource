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

#include "informationwindow.h"
#include "imageitem.h"
#include "view.h"

//! [0]
View::View(const QString &offices, const QString &images, QWidget *parent)
    : QGraphicsView(parent)
{
    officeTable = new QSqlRelationalTableModel(this);
    officeTable->setTable(offices);
    officeTable->setRelation(1, QSqlRelation(images, "locationid", "file"));
    officeTable->select();
//! [0]

//! [1]
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 465, 615);
    setScene(scene);

    addItems();

    QGraphicsPixmapItem *logo = scene->addPixmap(QPixmap(":/logo.png"));
    logo->setPos(30, 515);

#ifndef Q_OS_SYMBIAN
    setMinimumSize(470, 620);
    setMaximumSize(470, 620);
#else
    setDragMode(QGraphicsView::ScrollHandDrag);
#endif

   setWindowTitle(tr("Offices World Wide"));
}
//! [1]

//! [3]
void View::addItems()
{
    int officeCount = officeTable->rowCount();

    int imageOffset = 150;
    int leftMargin = 70;
    int topMargin = 40;

    for (int i = 0; i < officeCount; i++) {
        ImageItem *image;
        QGraphicsTextItem *label;
        QSqlRecord record = officeTable->record(i);

        int id = record.value("id").toInt();
        QString file = record.value("file").toString();
        QString location = record.value("location").toString();

        int columnOffset = ((i / 3) * 37);
        int x = ((i / 3) * imageOffset) + leftMargin + columnOffset;
        int y = ((i % 3) * imageOffset) + topMargin;

        image = new ImageItem(id, QPixmap(":/" + file));
        image->setData(0, i);
        image->setPos(x, y);
        scene->addItem(image);

        label = scene->addText(location);
        QPointF labelOffset((150 - label->boundingRect().width()) / 2, 120.0);
        label->setPos(QPointF(x, y) + labelOffset);
    }
}
//! [3]

//! [5]
void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (QGraphicsItem *item = itemAt(event->pos())) {
        if (ImageItem *image = qgraphicsitem_cast<ImageItem *>(item))
            showInformation(image);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
//! [5]

//! [6]
void View::showInformation(ImageItem *image)
{
    int id = image->id();
    if (id < 0 || id >= officeTable->rowCount())
        return;

    InformationWindow *window = findWindow(id);
    if (window && window->isVisible()) {
        window->raise();
        window->activateWindow();
    } else if (window && !window->isVisible()) {
#ifndef Q_OS_SYMBIAN
        window->show();
#else
        window->showFullScreen();
#endif
    } else {
        InformationWindow *window;
        window = new InformationWindow(id, officeTable, this);

        connect(window, SIGNAL(imageChanged(int, QString)),
                this, SLOT(updateImage(int, QString)));

#ifndef Q_OS_SYMBIAN
        window->move(pos() + QPoint(20, 40));
        window->show();
#else
        window->showFullScreen();
#endif
        informationWindows.append(window);
    }
}
//! [6]

//! [7]
void View::updateImage(int id, const QString &fileName)
{
    QList<QGraphicsItem *> items = scene->items();

    while(!items.empty()) {
        QGraphicsItem *item = items.takeFirst();

        if (ImageItem *image = qgraphicsitem_cast<ImageItem *>(item)) {
            if (image->id() == id){
                image->setPixmap(QPixmap(":/" +fileName));
                image->adjust();
                break;
            }
        }
    }
}
//! [7]

//! [8]
InformationWindow* View::findWindow(int id)
{
    QList<InformationWindow*>::iterator i, beginning, end;

    beginning = informationWindows.begin();
    end = informationWindows.end();

    for (i = beginning; i != end; ++i) {
        InformationWindow *window = (*i);
        if (window && (window->id() == id))
            return window;
    }
    return 0;
}
//! [8]

