/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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
#include <QtNetwork>

#if defined (Q_OS_SYMBIAN)
#include "sym_iap_util.h"
#endif

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// how long (milliseconds) the user need to hold (after a tap on the screen)
// before triggering the magnifying glass feature
// 701, a prime number, is the sum of 229, 233, 239
// (all three are also prime numbers, consecutive!)
#define HOLD_TIME 701

// maximum size of the magnifier
// Hint: see above to find why I picked this one :)
#define MAX_MAGNIFIER 229

uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}

// tile size in pixels
const int tdim = 256;

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) +
                          1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0;
    return QPointF(tx * zn, ty * zn);
}

qreal longitudeFromTile(qreal tx, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal lat = tx / zn * 360.0 - 180.0;
    return lat;
}

qreal latitudeFromTile(qreal ty, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal n = M_PI - 2 * M_PI * ty / zn;
    qreal lng = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    return lng;
}

class SlippyMap: public QObject
{
    Q_OBJECT

public:
    int width;
    int height;
    int zoom;
    qreal latitude;
    qreal longitude;

    SlippyMap(QObject *parent = 0)
            : QObject(parent)
            , width(400)
            , height(300)
            , zoom(15)
            , latitude(59.9138204)
            , longitude(10.7387413) {
        m_emptyTile = QPixmap(tdim, tdim);
        m_emptyTile.fill(Qt::lightGray);

        QNetworkDiskCache *cache = new QNetworkDiskCache;
        cache->setCacheDirectory(QDesktopServices::storageLocation
                                 (QDesktopServices::CacheLocation));
        m_manager.setCache(cache);
        connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleNetworkData(QNetworkReply*)));
    }

    void invalidate() {
        if (width <= 0 || height <= 0)
            return;

        QPointF ct = tileForCoordinate(latitude, longitude, zoom);
        qreal tx = ct.x();
        qreal ty = ct.y();

        // top-left corner of the center tile
        int xp = width / 2 - (tx - floor(tx)) * tdim;
        int yp = height / 2 - (ty - floor(ty)) * tdim;

        // first tile vertical and horizontal
        int xa = (xp + tdim - 1) / tdim;
        int ya = (yp + tdim - 1) / tdim;
        int xs = static_cast<int>(tx) - xa;
        int ys = static_cast<int>(ty) - ya;

        // offset for top-left tile
        m_offset = QPoint(xp - xa * tdim, yp - ya * tdim);

        // last tile vertical and horizontal
        int xe = static_cast<int>(tx) + (width - xp - 1) / tdim;
        int ye = static_cast<int>(ty) + (height - yp - 1) / tdim;

        // build a rect
        m_tilesRect = QRect(xs, ys, xe - xs + 1, ye - ys + 1);

        if (m_url.isEmpty())
            download();

        emit updated(QRect(0, 0, width, height));
    }

    void render(QPainter *p, const QRect &rect) {
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp(x + m_tilesRect.left(), y + m_tilesRect.top());
                QRect box = tileRect(tp);
                if (rect.intersects(box)) {
                    if (m_tilePixmaps.contains(tp))
                        p->drawPixmap(box, m_tilePixmaps.value(tp));
                    else
                        p->drawPixmap(box, m_emptyTile);
                }
            }
    }

    void pan(const QPoint &delta) {
        QPointF dx = QPointF(delta) / qreal(tdim);
        QPointF center = tileForCoordinate(latitude, longitude, zoom) - dx;
        latitude = latitudeFromTile(center.y(), zoom);
        longitude = longitudeFromTile(center.x(), zoom);
        invalidate();
    }

private slots:

    void handleNetworkData(QNetworkReply *reply) {
        QImage img;
        QPoint tp = reply->request().attribute(QNetworkRequest::User).toPoint();
        QUrl url = reply->url();
        if (!reply->error())
            if (!img.load(reply, 0))
                img = QImage();
        reply->deleteLater();
        m_tilePixmaps[tp] = QPixmap::fromImage(img);
        if (img.isNull())
            m_tilePixmaps[tp] = m_emptyTile;
        emit updated(tileRect(tp));

        // purge unused spaces
        QRect bound = m_tilesRect.adjusted(-2, -2, 2, 2);
        foreach(QPoint tp, m_tilePixmaps.keys())
        if (!bound.contains(tp))
            m_tilePixmaps.remove(tp);

        download();
    }

    void download() {
        QPoint grab(0, 0);
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp = m_tilesRect.topLeft() + QPoint(x, y);
                if (!m_tilePixmaps.contains(tp)) {
                    grab = tp;
                    break;
                }
            }
        if (grab == QPoint(0, 0)) {
            m_url = QUrl();
            return;
        }

        QString path = "http://tile.openstreetmap.org/%1/%2/%3.png";
        m_url = QUrl(path.arg(zoom).arg(grab.x()).arg(grab.y()));
        QNetworkRequest request;
        request.setUrl(m_url);
        request.setRawHeader("User-Agent", "Nokia (Qt) Graphics Dojo 1.0");
        request.setAttribute(QNetworkRequest::User, QVariant(grab));
        m_manager.get(request);
    }

signals:
    void updated(const QRect &rect);

protected:
    QRect tileRect(const QPoint &tp) {
        QPoint t = tp - m_tilesRect.topLeft();
        int x = t.x() * tdim + m_offset.x();
        int y = t.y() * tdim + m_offset.y();
        return QRect(x, y, tdim, tdim);
    }

private:
    QPoint m_offset;
    QRect m_tilesRect;
    QPixmap m_emptyTile;
    QHash<QPoint, QPixmap> m_tilePixmaps;
    QNetworkAccessManager m_manager;
    QUrl m_url;
};

class LightMaps: public QWidget
{
    Q_OBJECT

public:
    LightMaps(QWidget *parent = 0)
            : QWidget(parent)
            , pressed(false)
            , snapped(false)
            , zoomed(false)
            , invert(false) {
        m_normalMap = new SlippyMap(this);
        m_largeMap = new SlippyMap(this);
        connect(m_normalMap, SIGNAL(updated(QRect)), SLOT(updateMap(QRect)));
        connect(m_largeMap, SIGNAL(updated(QRect)), SLOT(update()));
    }

    void setCenter(qreal lat, qreal lng) {
        m_normalMap->latitude = lat;
        m_normalMap->longitude = lng;
        m_normalMap->invalidate();
        m_largeMap->invalidate();
    }

public slots:
    void toggleNightMode() {
        invert = !invert;
        update();
    }

private slots:
    void updateMap(const QRect &r) {
        update(r);
    }

protected:

    void activateZoom() {
        zoomed = true;
        tapTimer.stop();
        m_largeMap->zoom = m_normalMap->zoom + 1;
        m_largeMap->width = m_normalMap->width * 2;
        m_largeMap->height = m_normalMap->height * 2;
        m_largeMap->latitude = m_normalMap->latitude;
        m_largeMap->longitude = m_normalMap->longitude;
        m_largeMap->invalidate();
        update();
    }

    void resizeEvent(QResizeEvent *) {
        m_normalMap->width = width();
        m_normalMap->height = height();
        m_normalMap->invalidate();
        m_largeMap->width = m_normalMap->width * 2;
        m_largeMap->height = m_normalMap->height * 2;
        m_largeMap->invalidate();
    }

    void paintEvent(QPaintEvent *event) {
        QPainter p;
        p.begin(this);
        m_normalMap->render(&p, event->rect());
        p.setPen(Qt::black);
#if defined(Q_OS_SYMBIAN)
        QFont font = p.font();
        font.setPixelSize(13);
        p.setFont(font);
#endif
        p.drawText(rect(),  Qt::AlignBottom | Qt::TextWordWrap,
                   "Map data CCBYSA 2009 OpenStreetMap.org contributors");
        p.end();

        if (zoomed) {
            int dim = qMin(width(), height());
            int magnifierSize = qMin(MAX_MAGNIFIER, dim * 2 / 3);
            int radius = magnifierSize / 2;
            int ring = radius - 15;
            QSize box = QSize(magnifierSize, magnifierSize);

            // reupdate our mask
            if (maskPixmap.size() != box) {
                maskPixmap = QPixmap(box);
                maskPixmap.fill(Qt::transparent);

                QRadialGradient g;
                g.setCenter(radius, radius);
                g.setFocalPoint(radius, radius);
                g.setRadius(radius);
                g.setColorAt(1.0, QColor(255, 255, 255, 0));
                g.setColorAt(0.5, QColor(128, 128, 128, 255));

                QPainter mask(&maskPixmap);
                mask.setRenderHint(QPainter::Antialiasing);
                mask.setCompositionMode(QPainter::CompositionMode_Source);
                mask.setBrush(g);
                mask.setPen(Qt::NoPen);
                mask.drawRect(maskPixmap.rect());
                mask.setBrush(QColor(Qt::transparent));
                mask.drawEllipse(g.center(), ring, ring);
                mask.end();
            }

            QPoint center = dragPos - QPoint(0, radius);
            center = center + QPoint(0, radius / 2);
            QPoint corner = center - QPoint(radius, radius);

            QPoint xy = center * 2 - QPoint(radius, radius);

            // only set the dimension to the magnified portion
            if (zoomPixmap.size() != box) {
                zoomPixmap = QPixmap(box);
                zoomPixmap.fill(Qt::lightGray);
            }
            if (true) {
                QPainter p(&zoomPixmap);
                p.translate(-xy);
                m_largeMap->render(&p, QRect(xy, box));
                p.end();
            }

            QPainterPath clipPath;
            clipPath.addEllipse(center, ring, ring);

            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            p.setClipPath(clipPath);
            p.drawPixmap(corner, zoomPixmap);
            p.setClipping(false);
            p.drawPixmap(corner, maskPixmap);
            p.setPen(Qt::gray);
            p.drawPath(clipPath);
        }
        if (invert) {
            QPainter p(this);
            p.setCompositionMode(QPainter::CompositionMode_Difference);
            p.fillRect(event->rect(), Qt::white);
            p.end();
        }
    }

    void timerEvent(QTimerEvent *) {
        if (!zoomed)
            activateZoom();
        update();
    }

    void mousePressEvent(QMouseEvent *event) {
        if (event->buttons() != Qt::LeftButton)
            return;
        pressed = snapped = true;
        pressPos = dragPos = event->pos();
        tapTimer.stop();
        tapTimer.start(HOLD_TIME, this);
    }

    void mouseMoveEvent(QMouseEvent *event) {
        if (!event->buttons())
            return;
        if (!zoomed) {
            if (!pressed || !snapped) {
                QPoint delta = event->pos() - pressPos;
                pressPos = event->pos();
                m_normalMap->pan(delta);
                return;
            } else {
                const int threshold = 10;
                QPoint delta = event->pos() - pressPos;
                if (snapped) {
                    snapped &= delta.x() < threshold;
                    snapped &= delta.y() < threshold;
                    snapped &= delta.x() > -threshold;
                    snapped &= delta.y() > -threshold;
                }
                if (!snapped)
                    tapTimer.stop();
            }
        } else {
            dragPos = event->pos();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *) {
        zoomed = false;
        update();
    }

    void keyPressEvent(QKeyEvent *event) {
        if (!zoomed) {
            if (event->key() == Qt::Key_Left)
                m_normalMap->pan(QPoint(20, 0));
            if (event->key() == Qt::Key_Right)
                m_normalMap->pan(QPoint(-20, 0));
            if (event->key() == Qt::Key_Up)
                m_normalMap->pan(QPoint(0, 20));
            if (event->key() == Qt::Key_Down)
                m_normalMap->pan(QPoint(0, -20));
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                dragPos = QPoint(width() / 2, height() / 2);
                activateZoom();
            }
        } else {
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                zoomed = false;
                update();
            }
            QPoint delta(0, 0);
            if (event->key() == Qt::Key_Left)
                delta = QPoint(-15, 0);
            if (event->key() == Qt::Key_Right)
                delta = QPoint(15, 0);
            if (event->key() == Qt::Key_Up)
                delta = QPoint(0, -15);
            if (event->key() == Qt::Key_Down)
                delta = QPoint(0, 15);
            if (delta != QPoint(0, 0)) {
                dragPos += delta;
                update();
            }
        }
    }

private:
    SlippyMap *m_normalMap;
    SlippyMap *m_largeMap;
    bool pressed;
    bool snapped;
    QPoint pressPos;
    QPoint dragPos;
    QBasicTimer tapTimer;
    bool zoomed;
    QPixmap zoomPixmap;
    QPixmap maskPixmap;
    bool invert;
};

class MapZoom : public QMainWindow
{
    Q_OBJECT

private:
    LightMaps *map;

public:
    MapZoom(): QMainWindow(0) {
        map = new LightMaps(this);
        setCentralWidget(map);
        map->setFocus();

        QAction *osloAction = new QAction("&Oslo", this);
        QAction *berlinAction = new QAction("&Berlin", this);
        QAction *jakartaAction = new QAction("&Jakarta", this);
        QAction *nightModeAction = new QAction("Night Mode", this);
        nightModeAction->setCheckable(true);
        nightModeAction->setChecked(false);
        QAction *osmAction = new QAction("About OpenStreetMap", this);
        connect(osloAction, SIGNAL(triggered()), SLOT(chooseOslo()));
        connect(berlinAction, SIGNAL(triggered()), SLOT(chooseBerlin()));
        connect(jakartaAction, SIGNAL(triggered()), SLOT(chooseJakarta()));
        connect(nightModeAction, SIGNAL(triggered()), map, SLOT(toggleNightMode()));
        connect(osmAction, SIGNAL(triggered()), SLOT(aboutOsm()));

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM)
        menuBar()->addAction(osloAction);
        menuBar()->addAction(berlinAction);
        menuBar()->addAction(jakartaAction);
        menuBar()->addAction(nightModeAction);
        menuBar()->addAction(osmAction);
#else
        QMenu *menu = menuBar()->addMenu("&Options");
        menu->addAction(osloAction);
        menu->addAction(berlinAction);
        menu->addAction(jakartaAction);
        menu->addSeparator();
        menu->addAction(nightModeAction);
        menu->addAction(osmAction);
#endif

        QTimer::singleShot(0, this, SLOT(delayedInit()));
    }

private slots:

    void delayedInit() {
#if defined(Q_OS_SYMBIAN)
        qt_SetDefaultIap();
#endif
    }

    void chooseOslo() {
        map->setCenter(59.9138204, 10.7387413);
    }

    void chooseBerlin() {
        map->setCenter(52.52958999943302, 13.383053541183472);
    }

    void chooseJakarta() {
        map->setCenter(-6.211544, 106.845172);
    }

    void aboutOsm() {
        QDesktopServices::openUrl(QUrl("http://www.openstreetmap.org"));
    }
};


#include "lightmaps.moc"

int main(int argc, char **argv)
{
#if defined(Q_WS_X11)
    QApplication::setGraphicsSystem("raster");
#endif

    QApplication app(argc, argv);
    app.setApplicationName("LightMaps");

    MapZoom w;
    w.setWindowTitle("OpenStreetMap");
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM)
    w.showMaximized();
#else
    w.resize(600, 450);
    w.show();
#endif

    return app.exec();
}
