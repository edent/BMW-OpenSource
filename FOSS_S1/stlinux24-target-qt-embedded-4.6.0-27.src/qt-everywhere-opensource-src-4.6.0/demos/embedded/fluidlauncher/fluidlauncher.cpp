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

#include <QXmlStreamReader>

#include "fluidlauncher.h"


#define DEFAULT_INPUT_TIMEOUT 10000
#define SIZING_FACTOR_HEIGHT 6/10
#define SIZING_FACTOR_WIDTH 6/10

FluidLauncher::FluidLauncher(QStringList* args)
{
    pictureFlowWidget = new PictureFlow();
    slideShowWidget = new SlideShow();
    inputTimer = new QTimer();

    addWidget(pictureFlowWidget);
    addWidget(slideShowWidget);
    setCurrentWidget(pictureFlowWidget);
    pictureFlowWidget->setFocus();

    QRect screen_size = QApplication::desktop()->screenGeometry();

    QObject::connect(pictureFlowWidget, SIGNAL(itemActivated(int)), this, SLOT(launchApplication(int)));
    QObject::connect(pictureFlowWidget, SIGNAL(inputReceived()),    this, SLOT(resetInputTimeout()));
    QObject::connect(slideShowWidget,   SIGNAL(inputReceived()),    this, SLOT(switchToLauncher()));
    QObject::connect(inputTimer,        SIGNAL(timeout()),          this, SLOT(inputTimedout()));

    inputTimer->setSingleShot(true);
    inputTimer->setInterval(DEFAULT_INPUT_TIMEOUT);

    const int h = screen_size.height() * SIZING_FACTOR_HEIGHT;
    const int w = screen_size.width() * SIZING_FACTOR_WIDTH;
    const int hh = qMin(h, w);
    const int ww = hh / 3 * 2;
    pictureFlowWidget->setSlideSize(QSize(ww, hh));

    bool success;
    int configIndex = args->indexOf("-config");
    if ( (configIndex != -1) && (configIndex != args->count()-1) )
        success = loadConfig(args->at(configIndex+1));
    else
        success = loadConfig("config.xml");

    if (success) {
      populatePictureFlow();

      showFullScreen();
      inputTimer->start();
    } else {
        pictureFlowWidget->setAttribute(Qt::WA_DeleteOnClose, true);
        pictureFlowWidget->close();
    }

}

FluidLauncher::~FluidLauncher()
{
    delete pictureFlowWidget;
    delete slideShowWidget;
}

bool FluidLauncher::loadConfig(QString configPath)
{
    QFile xmlFile(configPath);

    if (!xmlFile.exists() || (xmlFile.error() != QFile::NoError)) {
        qDebug() << "ERROR: Unable to open config file " << configPath;
        return false;
    }

    slideShowWidget->clearImages();

    xmlFile.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&xmlFile);
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            if (reader.name() == "demos")
                parseDemos(reader);
            else if(reader.name() == "slideshow")
                parseSlideshow(reader);
        }
    }

    if (reader.hasError()) {
       qDebug() << QString("Error parsing %1 on line %2 column %3: \n%4")
                .arg(configPath)
                .arg(reader.lineNumber())
                .arg(reader.columnNumber())
                .arg(reader.errorString());
    }

    // Append an exit Item
    DemoApplication* exitItem = new DemoApplication(QString(), QLatin1String("Exit Embedded Demo"), QString(), QStringList());
    demoList.append(exitItem);

    return true;
}


void FluidLauncher::parseDemos(QXmlStreamReader& reader)
{
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == "example") {
            QXmlStreamAttributes attrs = reader.attributes();
            QStringRef filename = attrs.value("filename");
            if (!filename.isEmpty()) {
                QStringRef name = attrs.value("name");
                QStringRef image = attrs.value("image");
                QStringRef args = attrs.value("args");

                DemoApplication* newDemo = new DemoApplication(
                        filename.toString(),
                        name.isEmpty() ? "Unamed Demo" : name.toString(),
                        image.toString(),
                        args.toString().split(" "));
                demoList.append(newDemo);
            }
        } else if(reader.isEndElement() && reader.name() == "demos") {
            return;
        }
    }
}

void FluidLauncher::parseSlideshow(QXmlStreamReader& reader)
{
    QXmlStreamAttributes attrs = reader.attributes();

    QStringRef timeout = attrs.value("timeout");
    bool valid;
    if (!timeout.isEmpty()) {
        int t = timeout.toString().toInt(&valid);
        if (valid)
            inputTimer->setInterval(t);
    }

    QStringRef interval = attrs.value("interval");
    if (!interval.isEmpty()) {
        int i = interval.toString().toInt(&valid);
        if (valid)
            slideShowWidget->setSlideInterval(i);
    }

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QXmlStreamAttributes attrs = reader.attributes();
            if (reader.name() == "imagedir") {
                QStringRef dir = attrs.value("dir");
                slideShowWidget->addImageDir(dir.toString());
            } else if(reader.name() == "image") {
                QStringRef image = attrs.value("image");
                slideShowWidget->addImage(image.toString());
            }
        } else if(reader.isEndElement() && reader.name() == "slideshow") {
            return;
        }
    }

}

void FluidLauncher::populatePictureFlow()
{
    pictureFlowWidget->setSlideCount(demoList.count());

    for (int i=demoList.count()-1; i>=0; --i) {
        pictureFlowWidget->setSlide(i, *(demoList[i]->getImage()));
        pictureFlowWidget->setSlideCaption(i, demoList[i]->getCaption());
    }

    pictureFlowWidget->setCurrentSlide(demoList.count()/2);
}


void FluidLauncher::launchApplication(int index)
{
    // NOTE: Clearing the caches will free up more memory for the demo but will cause
    // a delay upon returning, as items are reloaded.
    //pictureFlowWidget->clearCaches();

    if (index == demoList.size() -1) {
        qApp->quit();
        return;
    }

    inputTimer->stop();

    QObject::connect(demoList[index], SIGNAL(demoFinished()), this, SLOT(demoFinished()));

    demoList[index]->launch();
}


void FluidLauncher::switchToLauncher()
{
    slideShowWidget->stopShow();
    inputTimer->start();
    setCurrentWidget(pictureFlowWidget);
}


void FluidLauncher::resetInputTimeout()
{
    if (inputTimer->isActive())
        inputTimer->start();
}

void FluidLauncher::inputTimedout()
{
    switchToSlideshow();
}


void FluidLauncher::switchToSlideshow()
{
    inputTimer->stop();
    slideShowWidget->startShow();
    setCurrentWidget(slideShowWidget);
}

void FluidLauncher::demoFinished()
{
    setCurrentWidget(pictureFlowWidget);
    inputTimer->start();

    // Bring the Fluidlauncher to the foreground to allow selecting another demo
    raise();
    activateWindow();
}

void FluidLauncher::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            if(currentWidget() == pictureFlowWidget) {
                resetInputTimeout();
            } else {
                slideShowWidget->startShow();
            }
        } else {
            inputTimer->stop();
            slideShowWidget->stopShow();
        }
    }
    QStackedWidget::changeEvent(event);
}
