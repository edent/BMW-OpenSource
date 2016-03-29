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

#include <QtGui>

class Pixmap : public QGraphicsObject
{
    Q_OBJECT
public:
    Pixmap(const QPixmap &pix) : QGraphicsObject(), p(pix)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

    QRectF boundingRect() const
    {
        return QRectF( QPointF(0, 0), p.size());
    }

private:
    QPixmap p;
};

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(states);

    QApplication app(argc, argv);

    // Text edit and button
    QTextEdit *edit = new QTextEdit;
    edit->setText("asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy!");

    QPushButton *button = new QPushButton;
    QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget;
    buttonProxy->setWidget(button);
    QGraphicsProxyWidget *editProxy = new QGraphicsProxyWidget;
    editProxy->setWidget(edit);

    QGroupBox *box = new QGroupBox;
    box->setFlat(true);
    box->setTitle("Options");

    QVBoxLayout *layout2 = new QVBoxLayout;
    box->setLayout(layout2);
    layout2->addWidget(new QRadioButton("Herring"));
    layout2->addWidget(new QRadioButton("Blue Parrot"));
    layout2->addWidget(new QRadioButton("Petunias"));
    layout2->addStretch();

    QGraphicsProxyWidget *boxProxy = new QGraphicsProxyWidget;
    boxProxy->setWidget(box);

    // Parent widget
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, widget);
    layout->addItem(editProxy);
    layout->addItem(buttonProxy);
    widget->setLayout(layout);

    Pixmap *p1 = new Pixmap(QPixmap(":/digikam.png"));
    Pixmap *p2 = new Pixmap(QPixmap(":/akregator.png"));
    Pixmap *p3 = new Pixmap(QPixmap(":/accessories-dictionary.png"));
    Pixmap *p4 = new Pixmap(QPixmap(":/k3b.png"));
    Pixmap *p5 = new Pixmap(QPixmap(":/help-browser.png"));
    Pixmap *p6 = new Pixmap(QPixmap(":/kchart.png"));

    QGraphicsScene scene(0, 0, 400, 300);
    scene.setBackgroundBrush(scene.palette().window());
    scene.addItem(widget);
    scene.addItem(boxProxy);
    scene.addItem(p1);
    scene.addItem(p2);
    scene.addItem(p3);
    scene.addItem(p4);
    scene.addItem(p5);
    scene.addItem(p6);

    QStateMachine machine;
    QState *state1 = new QState(&machine);
    QState *state2 = new QState(&machine);
    QState *state3 = new QState(&machine);
    machine.setInitialState(state1);

    // State 1
    state1->assignProperty(button, "text", "Switch to state 2");
    state1->assignProperty(widget, "geometry", QRectF(0, 0, 400, 150));
    state1->assignProperty(box, "geometry", QRect(-200, 150, 200, 150));
    state1->assignProperty(p1, "pos", QPointF(68, 185));
    state1->assignProperty(p2, "pos", QPointF(168, 185));
    state1->assignProperty(p3, "pos", QPointF(268, 185));
    state1->assignProperty(p4, "pos", QPointF(68-150, 48-150));
    state1->assignProperty(p5, "pos", QPointF(168, 48-150));
    state1->assignProperty(p6, "pos", QPointF(268+150, 48-150));
    state1->assignProperty(p1, "rotation", qreal(0));
    state1->assignProperty(p2, "rotation", qreal(0));
    state1->assignProperty(p3, "rotation", qreal(0));
    state1->assignProperty(p4, "rotation", qreal(-270));
    state1->assignProperty(p5, "rotation", qreal(-90));
    state1->assignProperty(p6, "rotation", qreal(270));
    state1->assignProperty(boxProxy, "opacity", qreal(0));
    state1->assignProperty(p1, "opacity", qreal(1));
    state1->assignProperty(p2, "opacity", qreal(1));
    state1->assignProperty(p3, "opacity", qreal(1));
    state1->assignProperty(p4, "opacity", qreal(0));
    state1->assignProperty(p5, "opacity", qreal(0));
    state1->assignProperty(p6, "opacity", qreal(0));

    // State 2
    state2->assignProperty(button, "text", "Switch to state 3");
    state2->assignProperty(widget, "geometry", QRectF(200, 150, 200, 150));
    state2->assignProperty(box, "geometry", QRect(9, 150, 190, 150));
    state2->assignProperty(p1, "pos", QPointF(68-150, 185+150));
    state2->assignProperty(p2, "pos", QPointF(168, 185+150));
    state2->assignProperty(p3, "pos", QPointF(268+150, 185+150));
    state2->assignProperty(p4, "pos", QPointF(64, 48));
    state2->assignProperty(p5, "pos", QPointF(168, 48));
    state2->assignProperty(p6, "pos", QPointF(268, 48));
    state2->assignProperty(p1, "rotation", qreal(-270));
    state2->assignProperty(p2, "rotation", qreal(90));
    state2->assignProperty(p3, "rotation", qreal(270));
    state2->assignProperty(p4, "rotation", qreal(0));
    state2->assignProperty(p5, "rotation", qreal(0));
    state2->assignProperty(p6, "rotation", qreal(0));
    state2->assignProperty(boxProxy, "opacity", qreal(1));
    state2->assignProperty(p1, "opacity", qreal(0));
    state2->assignProperty(p2, "opacity", qreal(0));
    state2->assignProperty(p3, "opacity", qreal(0));
    state2->assignProperty(p4, "opacity", qreal(1));
    state2->assignProperty(p5, "opacity", qreal(1));
    state2->assignProperty(p6, "opacity", qreal(1));

    // State 3
    state3->assignProperty(button, "text", "Switch to state 1");
    state3->assignProperty(p1, "pos", QPointF(0, 5));
    state3->assignProperty(p2, "pos", QPointF(0, 5 + 64 + 5));
    state3->assignProperty(p3, "pos", QPointF(5, 5 + (64 + 5) + 64));
    state3->assignProperty(p4, "pos", QPointF(5 + 64 + 5, 5));
    state3->assignProperty(p5, "pos", QPointF(5 + 64 + 5, 5 + 64 + 5));
    state3->assignProperty(p6, "pos", QPointF(5 + 64 + 5, 5 + (64 + 5) + 64));
    state3->assignProperty(widget, "geometry", QRectF(138, 5, 400 - 138, 200));
    state3->assignProperty(box, "geometry", QRect(5, 205, 400, 90));
    state3->assignProperty(p1, "opacity", qreal(1));
    state3->assignProperty(p2, "opacity", qreal(1));
    state3->assignProperty(p3, "opacity", qreal(1));
    state3->assignProperty(p4, "opacity", qreal(1));
    state3->assignProperty(p5, "opacity", qreal(1));
    state3->assignProperty(p6, "opacity", qreal(1));

    QAbstractTransition *t1 = state1->addTransition(button, SIGNAL(clicked()), state2);
    QSequentialAnimationGroup *animation1SubGroup = new QSequentialAnimationGroup;
    animation1SubGroup->addPause(250);
    animation1SubGroup->addAnimation(new QPropertyAnimation(box, "geometry"));
    t1->addAnimation(animation1SubGroup);
    t1->addAnimation(new QPropertyAnimation(widget, "geometry"));
    t1->addAnimation(new QPropertyAnimation(p1, "pos"));
    t1->addAnimation(new QPropertyAnimation(p2, "pos"));
    t1->addAnimation(new QPropertyAnimation(p3, "pos"));
    t1->addAnimation(new QPropertyAnimation(p4, "pos"));
    t1->addAnimation(new QPropertyAnimation(p5, "pos"));
    t1->addAnimation(new QPropertyAnimation(p6, "pos"));
    t1->addAnimation(new QPropertyAnimation(p1, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p2, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p3, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p4, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p5, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p6, "rotation"));
    t1->addAnimation(new QPropertyAnimation(p1, "opacity"));
    t1->addAnimation(new QPropertyAnimation(p2, "opacity"));
    t1->addAnimation(new QPropertyAnimation(p3, "opacity"));
    t1->addAnimation(new QPropertyAnimation(p4, "opacity"));
    t1->addAnimation(new QPropertyAnimation(p5, "opacity"));
    t1->addAnimation(new QPropertyAnimation(p6, "opacity"));

    QAbstractTransition *t2 = state2->addTransition(button, SIGNAL(clicked()), state3);
    t2->addAnimation(new QPropertyAnimation(box, "geometry"));
    t2->addAnimation(new QPropertyAnimation(widget, "geometry"));
    t2->addAnimation(new QPropertyAnimation(p1, "pos"));
    t2->addAnimation(new QPropertyAnimation(p2, "pos"));
    t2->addAnimation(new QPropertyAnimation(p3, "pos"));
    t2->addAnimation(new QPropertyAnimation(p4, "pos"));
    t2->addAnimation(new QPropertyAnimation(p5, "pos"));
    t2->addAnimation(new QPropertyAnimation(p6, "pos"));
    t2->addAnimation(new QPropertyAnimation(p1, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p2, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p3, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p4, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p5, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p6, "rotation"));
    t2->addAnimation(new QPropertyAnimation(p1, "opacity"));
    t2->addAnimation(new QPropertyAnimation(p2, "opacity"));
    t2->addAnimation(new QPropertyAnimation(p3, "opacity"));
    t2->addAnimation(new QPropertyAnimation(p4, "opacity"));
    t2->addAnimation(new QPropertyAnimation(p5, "opacity"));
    t2->addAnimation(new QPropertyAnimation(p6, "opacity"));

    QAbstractTransition *t3 = state3->addTransition(button, SIGNAL(clicked()), state1);
    t3->addAnimation(new QPropertyAnimation(box, "geometry"));
    t3->addAnimation(new QPropertyAnimation(widget, "geometry"));
    t3->addAnimation(new QPropertyAnimation(p1, "pos"));
    t3->addAnimation(new QPropertyAnimation(p2, "pos"));
    t3->addAnimation(new QPropertyAnimation(p3, "pos"));
    t3->addAnimation(new QPropertyAnimation(p4, "pos"));
    t3->addAnimation(new QPropertyAnimation(p5, "pos"));
    t3->addAnimation(new QPropertyAnimation(p6, "pos"));
    t3->addAnimation(new QPropertyAnimation(p1, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p2, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p3, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p4, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p5, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p6, "rotation"));
    t3->addAnimation(new QPropertyAnimation(p1, "opacity"));
    t3->addAnimation(new QPropertyAnimation(p2, "opacity"));
    t3->addAnimation(new QPropertyAnimation(p3, "opacity"));
    t3->addAnimation(new QPropertyAnimation(p4, "opacity"));
    t3->addAnimation(new QPropertyAnimation(p5, "opacity"));
    t3->addAnimation(new QPropertyAnimation(p6, "opacity"));

    machine.start();

    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}

#include "main.moc"
