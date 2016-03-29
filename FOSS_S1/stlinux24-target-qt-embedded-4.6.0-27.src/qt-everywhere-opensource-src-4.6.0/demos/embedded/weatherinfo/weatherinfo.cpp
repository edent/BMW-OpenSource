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
#include <QtSvg>

#if defined (Q_OS_SYMBIAN)
#include "sym_iap_util.h"
#endif

class WeatherInfo: public QMainWindow
{
    Q_OBJECT

private:

    QGraphicsView *m_view;
    QGraphicsScene m_scene;
    QString city;
    QGraphicsRectItem *m_statusItem;
    QGraphicsTextItem *m_temperatureItem;
    QGraphicsTextItem *m_conditionItem;
    QGraphicsSvgItem *m_iconItem;
    QList<QGraphicsRectItem*> m_forecastItems;
    QList<QGraphicsTextItem*> m_dayItems;
    QList<QGraphicsSvgItem*> m_conditionItems;
    QList<QGraphicsTextItem*> m_rangeItems;
    QTimeLine m_timeLine;
    QHash<QString, QString> m_icons;

public:
    WeatherInfo(QWidget *parent = 0): QMainWindow(parent) {

        m_view = new QGraphicsView(this);
        setCentralWidget(m_view);

        setupScene();
        m_view->setScene(&m_scene);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_view->setFrameShape(QFrame::NoFrame);
        setWindowTitle("Weather Info");

        QStringList cities;
        cities << "Oslo";
        cities << "Berlin";
        cities << "Brisbane";
        cities << "Helsinki";
        cities << "San Diego";
        for (int i = 0; i < cities.count(); ++i) {
            QAction *action = new QAction(cities[i], this);
            connect(action, SIGNAL(triggered()), SLOT(chooseCity()));
            addAction(action);
#if defined(Q_OS_SYMBIAN)
            menuBar()->addAction(action);
#endif
        }
        setContextMenuPolicy(Qt::ActionsContextMenu);

        QTimer::singleShot(0, this, SLOT(delayedInit()));
    }

private slots:
    void delayedInit() {
#if defined(Q_OS_SYMBIAN)
        qt_SetDefaultIap();
#else
        request("Oslo");
#endif
    }

private slots:

    void chooseCity() {
        QAction *action = qobject_cast<QAction*>(sender());
        if (action)
            request(action->text());
    }

    void handleNetworkData(QNetworkReply *networkReply) {
        QUrl url = networkReply->url();
        if (!networkReply->error())
            digest(QString::fromUtf8(networkReply->readAll()));
        networkReply->deleteLater();
        networkReply->manager()->deleteLater();
    }

    void animate(int frame) {
        qreal progress = static_cast<qreal>(frame) / 100;
#if QT_VERSION >= 0x040500
        m_iconItem->setOpacity(progress);
#endif
        qreal hw = width() / 2.0;
        m_statusItem->setPos(-hw + hw * progress, 0);
        for (int i = 0; i < m_forecastItems.count(); ++i) {
            qreal ofs = i * 0.5 / m_forecastItems.count();
            qreal alpha = qBound(qreal(0), 2 * (progress - ofs), qreal(1));
#if QT_VERSION >= 0x040500
            m_conditionItems[i]->setOpacity(alpha);
#endif
            QPointF pos = m_forecastItems[i]->pos();
            if (width() > height()) {
                qreal fx = width() - width() * 0.4 * alpha;
                m_forecastItems[i]->setPos(fx, pos.y());
            } else {
                qreal fx = height() - height() * 0.5 * alpha;
                m_forecastItems[i]->setPos(pos.x(), fx);
            }
        }
    }

private:

    void setupScene() {

        QColor textColor = palette().color(QPalette::WindowText);
        QFont textFont = font();
        textFont.setBold(true);
        textFont.setPointSize(textFont.pointSize() * 2);

        m_temperatureItem = m_scene.addText(QString(), textFont);
        m_temperatureItem->setDefaultTextColor(textColor);

        m_conditionItem = m_scene.addText(QString(), textFont);
        m_conditionItem->setDefaultTextColor(textColor);

        m_iconItem = new QGraphicsSvgItem;
        m_scene.addItem(m_iconItem);

        m_statusItem = m_scene.addRect(0, 0, 10, 10);
        m_statusItem->setPen(Qt::NoPen);
        m_statusItem->setBrush(Qt::NoBrush);
        m_temperatureItem->setParentItem(m_statusItem);
        m_conditionItem->setParentItem(m_statusItem);
        m_iconItem->setParentItem(m_statusItem);

        connect(&m_timeLine, SIGNAL(frameChanged(int)), SLOT(animate(int)));
        m_timeLine.setDuration(1100);
        m_timeLine.setFrameRange(0, 100);
        m_timeLine.setCurveShape(QTimeLine::EaseInCurve);
    }

    void request(const QString &location) {
        QUrl url("http://www.google.com/ig/api");
        url.addEncodedQueryItem("hl", "en");
        url.addEncodedQueryItem("weather", QUrl::toPercentEncoding(location));

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleNetworkData(QNetworkReply*)));
        manager->get(QNetworkRequest(url));

        city = QString();
        setWindowTitle("Loading...");
    }

    QString extractIcon(const QString &data) {
        if (m_icons.isEmpty()) {
            m_icons["mostly_cloudy"]    = "weather-few-clouds";
            m_icons["cloudy"]           = "weather-overcast";
            m_icons["mostly_sunny"]     = "weather-sunny-very-few-clouds";
            m_icons["partly_cloudy"]    = "weather-sunny-very-few-clouds";
            m_icons["sunny"]            = "weather-sunny";
            m_icons["flurries"]         = "weather-snow";
            m_icons["fog"]              = "weather-fog";
            m_icons["haze"]             = "weather-haze";
            m_icons["icy"]              = "weather-icy";
            m_icons["sleet"]            = "weather-sleet";
            m_icons["chance_of_sleet"]  = "weather-sleet";
            m_icons["snow"]             = "weather-snow";
            m_icons["chance_of_snow"]   = "weather-snow";
            m_icons["mist"]             = "weather-showers";
            m_icons["rain"]             = "weather-showers";
            m_icons["chance_of_rain"]   = "weather-showers";
            m_icons["storm"]            = "weather-storm";
            m_icons["chance_of_storm"]  = "weather-storm";
            m_icons["thunderstorm"]     = "weather-thundershower";
            m_icons["chance_of_tstorm"] = "weather-thundershower";
        }
        QRegExp regex("([\\w]+).gif$");
        if (regex.indexIn(data) != -1) {
            QString i = regex.cap();
            i = i.left(i.length() - 4);
            QString name = m_icons.value(i);
            if (!name.isEmpty()) {
                name.prepend(":/icons/");
                name.append(".svg");
                return name;
            }
        }
        return QString();
    }

    static QString toCelcius(QString t, QString unit) {
        bool ok = false;
        int degree = t.toInt(&ok);
        if (!ok)
            return QString();
        if (unit != "SI")
            degree = ((degree - 32) * 5 + 8)/ 9;
        return QString::number(degree) + QChar(176);
    }


#define GET_DATA_ATTR xml.attributes().value("data").toString()

    void digest(const QString &data) {

        QColor textColor = palette().color(QPalette::WindowText);
        QString unitSystem;

        delete m_iconItem;
        m_iconItem = new QGraphicsSvgItem();
        m_scene.addItem(m_iconItem);
        m_iconItem->setParentItem(m_statusItem);
        qDeleteAll(m_dayItems);
        qDeleteAll(m_conditionItems);
        qDeleteAll(m_rangeItems);
        qDeleteAll(m_forecastItems);
        m_dayItems.clear();
        m_conditionItems.clear();
        m_rangeItems.clear();
        m_forecastItems.clear();

        QXmlStreamReader xml(data);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                if (xml.name() == "city") {
                    city = GET_DATA_ATTR;
                    setWindowTitle(city);
                }
                if (xml.name() == "unit_system")
                    unitSystem = xml.attributes().value("data").toString();
                // Parse current weather conditions
                if (xml.name() == "current_conditions") {
                    while (!xml.atEnd()) {
                        xml.readNext();
                        if (xml.name() == "current_conditions")
                            break;
                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                            if (xml.name() == "condition") {
                                m_conditionItem->setPlainText(GET_DATA_ATTR);
                            }
                            if (xml.name() == "icon") {
                                QString name = extractIcon(GET_DATA_ATTR);
                                if (!name.isEmpty()) {
                                    delete m_iconItem;
                                    m_iconItem = new QGraphicsSvgItem(name);
                                    m_scene.addItem(m_iconItem);
                                    m_iconItem->setParentItem(m_statusItem);
                                }
                            }
                            if (xml.name() == "temp_c") {
                                QString s = GET_DATA_ATTR + QChar(176);
                                m_temperatureItem->setPlainText(s);
                            }
                        }
                    }
                }
                // Parse and collect the forecast conditions
                if (xml.name() == "forecast_conditions") {
                    QGraphicsTextItem *dayItem  = 0;
                    QGraphicsSvgItem *statusItem = 0;
                    QString lowT, highT;
                    while (!xml.atEnd()) {
                        xml.readNext();
                        if (xml.name() == "forecast_conditions") {
                            if (dayItem && statusItem &&
                                !lowT.isEmpty() && !highT.isEmpty()) {
                                m_dayItems << dayItem;
                                m_conditionItems << statusItem;
                                QString txt = highT + '/' + lowT;
                                QGraphicsTextItem* rangeItem;
                                rangeItem = m_scene.addText(txt);
                                rangeItem->setDefaultTextColor(textColor);
                                m_rangeItems << rangeItem;
                                QGraphicsRectItem *box;
                                box = m_scene.addRect(0, 0, 10, 10);
                                box->setPen(Qt::NoPen);
                                box->setBrush(Qt::NoBrush);
                                m_forecastItems << box;
                                dayItem->setParentItem(box);
                                statusItem->setParentItem(box);
                                rangeItem->setParentItem(box);
                            } else {
                                delete dayItem;
                                delete statusItem;
                            }
                            break;
                        }
                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                            if (xml.name() == "day_of_week") {
                                QString s = GET_DATA_ATTR;
                                dayItem = m_scene.addText(s.left(3));
                                dayItem->setDefaultTextColor(textColor);
                            }
                            if (xml.name() == "icon") {
                                QString name = extractIcon(GET_DATA_ATTR);
                                if (!name.isEmpty()) {
                                    statusItem = new QGraphicsSvgItem(name);
                                    m_scene.addItem(statusItem);
                                }
                            }
                            if (xml.name() == "low")
                                lowT = toCelcius(GET_DATA_ATTR, unitSystem);
                            if (xml.name() == "high")
                                highT = toCelcius(GET_DATA_ATTR, unitSystem);
                        }
                    }
                }

            }
        }

        m_timeLine.stop();
        layoutItems();
        animate(0);
        m_timeLine.start();
    }

    void layoutItems() {
        m_scene.setSceneRect(0, 0, width() - 1, height() - 1);
        m_view->centerOn(width() / 2, height() / 2);
        if (width() > height())
            layoutItemsLandscape();
        else
            layoutItemsPortrait();
    }

    void layoutItemsLandscape() {
        m_statusItem->setRect(0, 0, width() / 2 - 1, height() - 1);

        if (!m_iconItem->boundingRect().isEmpty()) {
            qreal dim = qMin(width() * 0.6, height() * 0.8);
            qreal pad = (height()  - dim) / 2;
            qreal sw = dim / m_iconItem->boundingRect().width();
            qreal sh = dim / m_iconItem->boundingRect().height();
            m_iconItem->setTransform(QTransform().scale(sw, sh));
            m_iconItem->setPos(1, pad);
        }

        m_temperatureItem->setPos(2, 2);
        qreal h = m_conditionItem->boundingRect().height();
        m_conditionItem->setPos(10, height() - h);

        if (m_dayItems.count()) {
            qreal left = width() * 0.6;
            qreal h = height() / m_dayItems.count();
            QFont textFont = font();
            textFont.setPixelSize(static_cast<int>(h * 0.3));
            qreal statusWidth = 0;
            qreal rangeWidth = 0;
            for (int i = 0; i < m_dayItems.count(); ++i) {
                m_dayItems[i]->setFont(textFont);
                QRectF brect = m_dayItems[i]->boundingRect();
                statusWidth = qMax(statusWidth, brect.width());
                brect = m_rangeItems[i]->boundingRect();
                rangeWidth = qMax(rangeWidth, brect.width());
            }
            qreal space = width() - left - statusWidth - rangeWidth;
            qreal dim = qMin(h, space);
            qreal pad = statusWidth + (space  - dim) / 2;
            for (int i = 0; i < m_dayItems.count(); ++i) {
                qreal base = h * i;
                m_forecastItems[i]->setPos(left, base);
                m_forecastItems[i]->setRect(0, 0, width() - left, h);
                QRectF brect = m_dayItems[i]->boundingRect();
                qreal ofs = (h - brect.height()) / 2;
                m_dayItems[i]->setPos(0, ofs);
                brect = m_rangeItems[i]->boundingRect();
                ofs = (h - brect.height()) / 2;
                m_rangeItems[i]->setPos(width() - rangeWidth - left, ofs);
                brect = m_conditionItems[i]->boundingRect();
                ofs = (h - dim) / 2;
                m_conditionItems[i]->setPos(pad, ofs);
                if (brect.isEmpty())
                    continue;
                qreal sw = dim / brect.width();
                qreal sh = dim / brect.height();
                m_conditionItems[i]->setTransform(QTransform().scale(sw, sh));
            }
        }
    }

    void layoutItemsPortrait() {

        m_statusItem->setRect(0, 0, width() - 1, height() / 2 - 1);

        if (!m_iconItem->boundingRect().isEmpty()) {
            qreal dim = qMin(width() * 0.8, height() * 0.4);
            qreal ofsy = (height() / 2  - dim) / 2;
            qreal ofsx = (width() - dim) / 3;
            qreal sw = dim / m_iconItem->boundingRect().width();
            qreal sh = dim / m_iconItem->boundingRect().height();
            m_iconItem->setTransform(QTransform().scale(sw, sh));
            m_iconItem->setPos(ofsx, ofsy);
        }

        m_temperatureItem->setPos(2, 2);
        qreal ch = m_conditionItem->boundingRect().height();
        qreal cw = m_conditionItem->boundingRect().width();
        m_conditionItem->setPos(width() - cw , height() / 2 - ch - 20);

        if (m_dayItems.count()) {
            qreal top = height() * 0.5;
            qreal w = width() / m_dayItems.count();
            qreal statusHeight = 0;
            qreal rangeHeight = 0;
            for (int i = 0; i < m_dayItems.count(); ++i) {
                m_dayItems[i]->setFont(font());
                QRectF brect = m_dayItems[i]->boundingRect();
                statusHeight = qMax(statusHeight, brect.height());
                brect = m_rangeItems[i]->boundingRect();
                rangeHeight = qMax(rangeHeight, brect.height());
            }
            qreal space = height() - top - statusHeight - rangeHeight;
            qreal dim = qMin(w, space);

            qreal boxh = statusHeight + rangeHeight + dim;
            qreal pad = (height() - top - boxh) / 2;

            for (int i = 0; i < m_dayItems.count(); ++i) {
                qreal base = w * i;
                m_forecastItems[i]->setPos(base, top);
                m_forecastItems[i]->setRect(0, 0, w, boxh);
                QRectF brect = m_dayItems[i]->boundingRect();
                qreal ofs = (w - brect.width()) / 2;
                m_dayItems[i]->setPos(ofs, pad);

                brect = m_rangeItems[i]->boundingRect();
                ofs = (w - brect.width()) / 2;
                m_rangeItems[i]->setPos(ofs, pad + statusHeight + dim);

                brect = m_conditionItems[i]->boundingRect();
                ofs = (w - dim) / 2;
                m_conditionItems[i]->setPos(ofs, pad + statusHeight);
                if (brect.isEmpty())
                    continue;
                qreal sw = dim / brect.width();
                qreal sh = dim / brect.height();
                m_conditionItems[i]->setTransform(QTransform().scale(sw, sh));
            }
        }
    }


    void resizeEvent(QResizeEvent *event) {
        Q_UNUSED(event);
        layoutItems();
    }

};

#include "weatherinfo.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    WeatherInfo w;
#if defined(Q_OS_SYMBIAN)
    w.showMaximized();
#else
    w.resize(520, 288);
    w.show();
#endif

    return app.exec();
}
