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

#include "menucontent.h"
#include "colors.h"
#include "menumanager.h"
#include "demotextitem.h"
#include "headingitem.h"

MenuContentItem::MenuContentItem(const QDomElement &el, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->name = el.attribute("name");
    this->heading = 0;
    this->description1 = 0;
    this->description2 = 0;

    if (el.tagName() == "demos")
        this->readmePath = QLibraryInfo::location(QLibraryInfo::DemosPath) + "/README";
    else
        this->readmePath = QLibraryInfo::location(QLibraryInfo::ExamplesPath) + "/" + el.attribute("dirname") + "/README";

}

void MenuContentItem::prepare()
{
    if (!this->prepared){
        this->prepared= true;
        this->createContent();
    }
}

void MenuContentItem::animationStopped(int id)
{
    if (this->name == Colors::rootMenuName)
        return; // Optimization hack.

    if (id == DemoItemAnimation::ANIM_OUT){
        // Free up some memory:
        delete this->heading;
        delete this->description1;
        delete this->description2;
        this->heading = 0;
        this->description1 = 0;
        this->description2 = 0;
        this->prepared = false;
    }
}

QString MenuContentItem::loadDescription(int startPara, int nrPara)
{
    QString result;
    QFile readme(this->readmePath);
    if (!readme.open(QFile::ReadOnly)){
        if (Colors::verbose)
            qDebug() << "- MenuContentItem::loadDescription: Could not load:" << this->readmePath;
        return "";
    }

    QTextStream in(&readme);
    // Skip a certain number of paragraphs:
    while (startPara)
        if (in.readLine().isEmpty()) --startPara;

    // Read in the number of wanted paragraphs:
    QString line = in.readLine();
    do {
        result += line + " ";
        line = in.readLine();
        if (line.isEmpty()){
            --nrPara;
            line = "<br><br>" + in.readLine();
        }
    } while (nrPara && !in.atEnd());

    return Colors::contentColor + result;
}

void MenuContentItem::createContent()
{
    // Create the items:
    this->heading = new HeadingItem(this->name, this->scene(), this);
    QString para1 = this->loadDescription(0, 1);
    if (para1.isEmpty())
        para1 = Colors::contentColor + QLatin1String("Could not load description. Ensure that the documentation for Qt is built.");
    QColor bgcolor = Colors::sceneBg1.darker(200);
    bgcolor.setAlpha(100);
    this->description1 = new DemoTextItem(para1, Colors::contentFont(), Colors::heading, 500, this->scene(), this, DemoTextItem::STATIC_TEXT);
    this->description2 = new DemoTextItem(this->loadDescription(1, 2), Colors::contentFont(), Colors::heading, 250, this->scene(), this, DemoTextItem::STATIC_TEXT);

    // Place the items on screen:
    this->heading->setPos(0, 3);
    this->description1->setPos(0, this->heading->pos().y() + this->heading->boundingRect().height() + 10);
    this->description2->setPos(0, this->description1->pos().y() + this->description1->boundingRect().height() + 15);
}

QRectF MenuContentItem::boundingRect() const
{
    return QRectF(0, 0, 500, 350);
}


