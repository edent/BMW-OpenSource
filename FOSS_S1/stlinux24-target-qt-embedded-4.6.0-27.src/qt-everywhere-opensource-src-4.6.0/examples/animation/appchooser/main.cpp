/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <QtCore>
#include <QtGui>


class Pixmap : public QGraphicsWidget
{
    Q_OBJECT

public:
    Pixmap(const QPixmap &pix, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), orig(pix), p(pix)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * )
    {
        emit clicked();
    }

    virtual void setGeometry(const QRectF &rect)
    {
        QGraphicsWidget::setGeometry(rect);

        if (rect.size().width() > orig.size().width())
            p = orig.scaled(rect.size().toSize());
        else
            p = orig;
    }

Q_SIGNALS:
    void clicked();

private:
    QPixmap orig;
    QPixmap p;
};

void createStates(const QObjectList &objects,
                  const QRect &selectedRect, QState *parent)
{
    for (int i = 0; i < objects.size(); ++i) {
        QState *state = new QState(parent);
        state->assignProperty(objects.at(i), "geometry", selectedRect);
        parent->addTransition(objects.at(i), SIGNAL(clicked()), state);
    }
}

void createAnimations(const QObjectList &objects, QStateMachine *machine)
{
    for (int i=0; i<objects.size(); ++i)
        machine->addDefaultAnimation(new QPropertyAnimation(objects.at(i), "geometry"));    
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(appchooser);

    QApplication app(argc, argv);

    Pixmap *p1 = new Pixmap(QPixmap(":/digikam.png"));
    Pixmap *p2 = new Pixmap(QPixmap(":/akregator.png"));
    Pixmap *p3 = new Pixmap(QPixmap(":/accessories-dictionary.png"));
    Pixmap *p4 = new Pixmap(QPixmap(":/k3b.png"));

    p1->setObjectName("p1");
    p2->setObjectName("p2");
    p3->setObjectName("p3");
    p4->setObjectName("p4");

    p1->setGeometry(QRectF(0.0, 0.0, 64.0, 64.0));
    p2->setGeometry(QRectF(236.0, 0.0, 64.0, 64.0));
    p3->setGeometry(QRectF(236.0, 236.0, 64.0, 64.0));
    p4->setGeometry(QRectF(0.0, 236.0, 64.0, 64.0));

    QGraphicsScene scene(0, 0, 300, 300);
    scene.setBackgroundBrush(Qt::white);
    scene.addItem(p1);
    scene.addItem(p2);
    scene.addItem(p3);
    scene.addItem(p4);

    QGraphicsView window(&scene);
    window.setFrameStyle(0);
    window.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    window.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    window.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QStateMachine machine;
    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *group = new QState(&machine);
    group->setObjectName("group");
    QRect selectedRect(86, 86, 128, 128);

    QState *idleState = new QState(group);
    group->setInitialState(idleState);

    QObjectList objects; 
    objects << p1 << p2 << p3 << p4;
    createStates(objects, selectedRect, group);
    createAnimations(objects, &machine);

    machine.setInitialState(group);
    machine.start();

    window.resize(300, 300);
    window.show();

    return app.exec();
}

#include "main.moc"
