/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <qapplication.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <QtGui>
#include <QtCore>

class MyClass : public QWidget
{
public:
     MyClass(QWidget *parent = 0) : QWidget(parent) { }

protected:
     void paintEvent(QPaintEvent *e)
     {
        /*QRadialGradient rg(50, 50, 50, 50, 50);
         rg.setColorAt(0, QColor::fromRgbF(1, 0, 0, 1));
         rg.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
         QPainter pmp(&pm);
         pmp.fillRect(0, 0, 100, 100, rg);
         pmp.end();*/

	createImage();

	QPainter p(this);
	p.fillRect(rect(), Qt::white);

	p.drawPixmap(0, 0, pixmap);

	p.drawPixmap(100, 0, channelImage);
     }

    void createImage()
    {
//! [0]
	pixmap = QPixmap(100, 100);
	pixmap.fill(Qt::transparent);

	QRadialGradient gradient(50, 50, 50, 50, 50);
	gradient.setColorAt(0, QColor::fromRgbF(1, 0, 0, 1));
	gradient.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
	QPainter painter(&pixmap);
	painter.fillRect(0, 0, 100, 100, gradient);

	channelImage = pixmap.alphaChannel();
	update();
//! [0]
    }

    QPixmap channelImage, pixmap;
    QSize sizeHint() const { return QSize(500, 500); }
};

int main(int argc, char **argv)
{
     QApplication app(argc, argv);

     MyClass cl;
     cl.show();
     QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

     int ret = app.exec();
     return ret;
}
