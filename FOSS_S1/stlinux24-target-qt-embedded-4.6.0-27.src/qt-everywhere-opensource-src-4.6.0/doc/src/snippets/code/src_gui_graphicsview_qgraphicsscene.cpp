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

//! [0]
QGraphicsScene scene;
scene.addText("Hello, world!");

QGraphicsView view(&scene);
view.show();
//! [0]


//! [1]
QGraphicsScene scene;
scene.addItem(...
...
QPrinter printer(QPrinter::HighResolution);
printer.setPaperSize(QPrinter::A4);

QPainter painter(&printer);
scene.render(&painter);
//! [1]


//! [2]
QSizeF segmentSize = sceneRect().size() / pow(2, depth - 1);
//! [2]


//! [3]
QGraphicsScene scene;
QGraphicsView view(&scene);
view.show();

// a blue background
scene.setBackgroundBrush(Qt::blue);

// a gradient background
QRadialGradient gradient(0, 0, 10);
gradient.setSpread(QGradient::RepeatSpread);
scene.setBackgroundBrush(gradient);
//! [3]


//! [4]
QGraphicsScene scene;
QGraphicsView view(&scene);
view.show();

// a white semi-transparent foreground
scene.setForegroundBrush(QColor(255, 255, 255, 127));

// a grid foreground
scene.setForegroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));
//! [4]


//! [5]
QRectF TileScene::rectForTile(int x, int y) const
{
    // Return the rectangle for the tile at position (x, y).
    return QRectF(x * tileWidth, y * tileHeight, tileWidth, tileHeight);
}

void TileScene::setTile(int x, int y, const QPixmap &pixmap)
{
    // Sets or replaces the tile at position (x, y) with pixmap.
    if (x >= 0 && x < numTilesH && y >= 0 && y < numTilesV) {
        tiles[y][x] = pixmap;
        invalidate(rectForTile(x, y), BackgroundLayer);
    }
}

void TileScene::drawBackground(QPainter *painter, const QRectF &exposed)
{
    // Draws all tiles that intersect the exposed area.
    for (int y = 0; y < numTilesV; ++y) {
        for (int x = 0; x < numTilesH; ++x) {
            QRectF rect = rectForTile(x, y);
            if (exposed.intersects(rect))
                painter->drawPixmap(rect.topLeft(), tiles[y][x]);
        }
    }
}
//! [5]
