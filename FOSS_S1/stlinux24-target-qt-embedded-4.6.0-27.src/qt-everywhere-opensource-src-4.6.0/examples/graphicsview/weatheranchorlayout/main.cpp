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

#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QApplication>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>
#include <QGraphicsSceneResizeEvent>


class PixmapWidget : public QGraphicsLayoutItem
{

public:
    PixmapWidget(const QPixmap &pix) : QGraphicsLayoutItem()
    {
        original = new QGraphicsPixmapItem(pix);
        setGraphicsItem(original);
        original->show();
        r = QRectF(QPointF(0, 0), pix.size());
    }

    ~PixmapWidget()
    {
        setGraphicsItem(0);
        delete original;
    }

    void setZValue(qreal z)
    {
        original->setZValue(z);
    }

    void setGeometry (const QRectF &rect)
    {
        original->scale(rect.width() / r.width(), rect.height() / r.height());
        original->setPos(rect.x(), rect.y());
        r = rect;
    }

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
    {
        Q_UNUSED(constraint);
        QSizeF sh;
        switch (which) {
            case Qt::MinimumSize:
                sh = QSizeF(0, 0);
                break;
            case Qt::PreferredSize:
                sh = QSizeF(50, 50);
                break;
            case Qt::MaximumSize:
                sh = QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                break;
        }
         return sh;
    }

private:
    QGraphicsPixmapItem *original;
    QRectF r;
};


class PlaceWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    PlaceWidget(const QPixmap &pix) : QGraphicsWidget(), original(pix), scaled(pix)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        QPointF reflection = QPointF();
        reflection.setY(scaled.height() + 2);

        painter->drawPixmap(QPointF(), scaled);

        QPixmap tmp(scaled.size());
        tmp.fill(Qt::transparent);
        QPainter p(&tmp);

        // create gradient
        QPoint p1(scaled.width() / 2, 0);
        QPoint p2(scaled.width() / 2, scaled.height());
        QLinearGradient linearGrad(p1, p2);
        linearGrad.setColorAt(0, QColor(0, 0, 0, 0));
        linearGrad.setColorAt(0.65, QColor(0, 0, 0, 127));
        linearGrad.setColorAt(1, QColor(0, 0, 0, 255));

        // apply 'mask'
        p.setBrush(linearGrad);
        p.fillRect(0, 0, tmp.width(), tmp.height(), QBrush(linearGrad));
        p.fillRect(0, 0, tmp.width(), tmp.height(), QBrush(linearGrad));

        // paint the image flipped
        p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        p.drawPixmap(0, 0, QPixmap::fromImage(scaled.toImage().mirrored(false, true)));
        p.end();

        painter->drawPixmap(reflection, tmp);
    }

    void resizeEvent(QGraphicsSceneResizeEvent *event)
    {
        QSize newSize = event->newSize().toSize();
        newSize.setHeight(newSize.height() / 2);
        scaled = original.scaled(newSize);
    }

    QRectF boundingRect() const
    {
        QSize size(scaled.width(), scaled.height() * 2 + 2);
        return QRectF(QPointF(0, 0), size);
    }

private:
    QPixmap original;
    QPixmap scaled;
};


static QGraphicsProxyWidget *createItem(const QString &name = "Unnamed")
{
    QGraphicsProxyWidget *w = new QGraphicsProxyWidget;
    w->setWidget(new QPushButton(name));
    w->setData(0, name);
    w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    return w;
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(weatheranchorlayout);

    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 480);

#ifdef DEBUG_MODE
        QGraphicsProxyWidget *title = createItem("Title");
        QGraphicsProxyWidget *place = createItem("Place");
        QGraphicsProxyWidget *sunnyWeather = createItem("Sun");
        QGraphicsProxyWidget *details = createItem("Details");
        QGraphicsProxyWidget *tabbar = createItem("Tabbar");
#else
        // pixmaps widgets
        PixmapWidget *title = new PixmapWidget(QPixmap(":/images/title.jpg"));
        PlaceWidget *place = new PlaceWidget(QPixmap(":/images/place.jpg"));
        PixmapWidget *details = new PixmapWidget(QPixmap(":/images/5days.jpg"));
        PixmapWidget *sunnyWeather = new PixmapWidget(QPixmap(":/images/weather-few-clouds.png"));
        PixmapWidget *tabbar = new PixmapWidget(QPixmap(":/images/tabbar.jpg"));
#endif


    // setup sizes
    title->setPreferredSize(QSizeF(348, 45));
    title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    place->setPreferredSize(QSizeF(96, 72));
    place->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    details->setMinimumSize(QSizeF(200, 112));
    details->setPreferredSize(QSizeF(200, 112));
    details->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    tabbar->setPreferredSize(QSizeF(70, 24));
    tabbar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    sunnyWeather->setPreferredSize(QSizeF(128, 97));
    sunnyWeather->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sunnyWeather->setZValue(9999);

    // start anchor layout
    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setSpacing(0);

    // setup the main widget
    QGraphicsWidget *w = new QGraphicsWidget(0, Qt::Window);
    QPalette p;
    p.setColor(QPalette::Window, Qt::black);
    w->setPalette(p);
    w->setPos(20, 20);
    w->setLayout(l);

    // vertical anchors
    QGraphicsAnchor *anchor = l->addAnchor(title, Qt::AnchorTop, l, Qt::AnchorTop);
    anchor = l->addAnchor(place, Qt::AnchorTop, title, Qt::AnchorBottom);
    anchor->setSpacing(12);
    anchor = l->addAnchor(place, Qt::AnchorBottom, l, Qt::AnchorBottom);
    anchor->setSpacing(12);

    anchor = l->addAnchor(sunnyWeather, Qt::AnchorTop, title, Qt::AnchorTop);
    anchor = l->addAnchor(sunnyWeather, Qt::AnchorBottom, l, Qt::AnchorVerticalCenter);

    anchor = l->addAnchor(tabbar, Qt::AnchorTop, title, Qt::AnchorBottom);
    anchor->setSpacing(5);
    anchor = l->addAnchor(details, Qt::AnchorTop, tabbar, Qt::AnchorBottom);
    anchor->setSpacing(2);
    anchor = l->addAnchor(details, Qt::AnchorBottom, l, Qt::AnchorBottom);
    anchor->setSpacing(12);

    // horizontal anchors
    anchor = l->addAnchor(l, Qt::AnchorLeft, title, Qt::AnchorLeft);
    anchor = l->addAnchor(title, Qt::AnchorRight, l, Qt::AnchorRight);

    anchor = l->addAnchor(place, Qt::AnchorLeft, l, Qt::AnchorLeft);
    anchor->setSpacing(15);
    anchor = l->addAnchor(place, Qt::AnchorRight, details, Qt::AnchorLeft);
    anchor->setSpacing(35);

    anchor = l->addAnchor(sunnyWeather, Qt::AnchorLeft, place, Qt::AnchorHorizontalCenter);
    anchor = l->addAnchor(sunnyWeather, Qt::AnchorRight, l, Qt::AnchorHorizontalCenter);

    anchor = l->addAnchor(tabbar, Qt::AnchorHorizontalCenter, details, Qt::AnchorHorizontalCenter);
    anchor = l->addAnchor(details, Qt::AnchorRight, l, Qt::AnchorRight);

    // QGV setup
    scene.addItem(w);
    scene.setBackgroundBrush(Qt::white);
    QGraphicsView *view = new QGraphicsView(&scene);
    view->show();

    return app.exec();
}

#include "main.moc"
