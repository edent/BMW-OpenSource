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

#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>
#include <QtGui>

static QGraphicsProxyWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0),
                                   const QString &name = "0")
{
    QGraphicsProxyWidget *w = new QGraphicsProxyWidget;
    w->setWidget(new QPushButton(name));
    w->setData(0, name);
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);

    w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    return w;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 480);

    QSizeF minSize(30, 100);
    QSizeF prefSize(210, 100);
    QSizeF maxSize(300, 100);

    QGraphicsProxyWidget *a = createItem(minSize, prefSize, maxSize, "A");
    QGraphicsProxyWidget *b = createItem(minSize, prefSize, maxSize, "B");
    QGraphicsProxyWidget *c = createItem(minSize, prefSize, maxSize, "C");
    QGraphicsProxyWidget *d = createItem(minSize, prefSize, maxSize, "D");
    QGraphicsProxyWidget *e = createItem(minSize, prefSize, maxSize, "E");
    QGraphicsProxyWidget *f = createItem(QSizeF(30, 50), QSizeF(150, 50), maxSize, "F");
    QGraphicsProxyWidget *g = createItem(QSizeF(30, 50), QSizeF(30, 100), maxSize, "G");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setSpacing(0);

    QGraphicsWidget *w = new QGraphicsWidget(0, Qt::Window);
    w->setPos(20, 20);
    w->setLayout(l);

    // vertical
    QGraphicsAnchor *anchor = l->addAnchor(a, Qt::AnchorTop, l, Qt::AnchorTop);
    anchor = l->addAnchor(b, Qt::AnchorTop, l, Qt::AnchorTop);

    anchor = l->addAnchor(c, Qt::AnchorTop, a, Qt::AnchorBottom);
    anchor = l->addAnchor(c, Qt::AnchorTop, b, Qt::AnchorBottom);
    anchor = l->addAnchor(c, Qt::AnchorBottom, d, Qt::AnchorTop);
    anchor = l->addAnchor(c, Qt::AnchorBottom, e, Qt::AnchorTop);

    anchor = l->addAnchor(d, Qt::AnchorBottom, l, Qt::AnchorBottom);
    anchor = l->addAnchor(e, Qt::AnchorBottom, l, Qt::AnchorBottom);

    anchor = l->addAnchor(c, Qt::AnchorTop, f, Qt::AnchorTop);
    anchor = l->addAnchor(c, Qt::AnchorVerticalCenter, f, Qt::AnchorBottom);
    anchor = l->addAnchor(f, Qt::AnchorBottom, g, Qt::AnchorTop);
    anchor = l->addAnchor(c, Qt::AnchorBottom, g, Qt::AnchorBottom);

    // horizontal
    anchor = l->addAnchor(l, Qt::AnchorLeft, a, Qt::AnchorLeft);
    anchor = l->addAnchor(l, Qt::AnchorLeft, d, Qt::AnchorLeft);
    anchor = l->addAnchor(a, Qt::AnchorRight, b, Qt::AnchorLeft);

    anchor = l->addAnchor(a, Qt::AnchorRight, c, Qt::AnchorLeft);
    anchor = l->addAnchor(c, Qt::AnchorRight, e, Qt::AnchorLeft);

    anchor = l->addAnchor(b, Qt::AnchorRight, l, Qt::AnchorRight);
    anchor = l->addAnchor(e, Qt::AnchorRight, l, Qt::AnchorRight);
    anchor = l->addAnchor(d, Qt::AnchorRight, e, Qt::AnchorLeft);

    anchor = l->addAnchor(l, Qt::AnchorLeft, f, Qt::AnchorLeft);
    anchor = l->addAnchor(l, Qt::AnchorLeft, g, Qt::AnchorLeft);
    anchor = l->addAnchor(f, Qt::AnchorRight, g, Qt::AnchorRight);


    scene.addItem(w);
    scene.setBackgroundBrush(Qt::darkGreen);
    QGraphicsView *view = new QGraphicsView(&scene);
    view->show();

    return app.exec();
}
