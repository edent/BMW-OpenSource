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

#include "tetrixboard.h"

#include <QtGui>

Q_DECLARE_METATYPE(QPainter*)

TetrixBoard::TetrixBoard(QWidget *parent)
    : QFrame(parent)
{
    timer = new QTimer(this);
    qMetaTypeId<QPainter*>();
}

void TetrixBoard::setNextPieceLabel(QWidget *label)
{
    nextPieceLabel = qobject_cast<QLabel*>(label);
}

QObject *TetrixBoard::getTimer()
{
    return timer;
}

QSize TetrixBoard::minimumSizeHint() const
{
    return QSize(BoardWidth * 5 + frameWidth() * 2,
                 BoardHeight * 5 + frameWidth() * 2);
}

void TetrixBoard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event->key());
}

void TetrixBoard::showNextPiece(int width, int height)
{
    if (!nextPieceLabel)
        return;

    QPixmap pixmap(width * squareWidth(), height * squareHeight());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), nextPieceLabel->palette().background());

    emit paintNextPieceRequested(&painter);

    nextPieceLabel->setPixmap(pixmap);
}

void TetrixBoard::drawPauseScreen(QPainter *painter)
{
    painter->drawText(contentsRect(), Qt::AlignCenter, tr("Pause"));
}

void TetrixBoard::drawSquare(QPainter *painter, int x, int y, int shape)
{
    static const QRgb colorTable[8] = {
        0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00
    };

    x = x*squareWidth();
    y = y*squareHeight();

    QColor color = colorTable[shape];
    painter->fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight() - 2,
                      color);

    painter->setPen(color.light());
    painter->drawLine(x, y + squareHeight() - 1, x, y);
    painter->drawLine(x, y, x + squareWidth() - 1, y);

    painter->setPen(color.dark());
    painter->drawLine(x + 1, y + squareHeight() - 1,
                      x + squareWidth() - 1, y + squareHeight() - 1);
    painter->drawLine(x + squareWidth() - 1, y + squareHeight() - 1,
                      x + squareWidth() - 1, y + 1);
}

void TetrixBoard::update()
{
    QRect rect = contentsRect();
    if (image.size() != rect.size())
        image = QImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0,0,0,0));
    QPainter painter;
    painter.begin(&image);
    int boardTop = rect.bottom() - BoardHeight*squareHeight();
    painter.translate(rect.left(), boardTop);
    emit paintRequested(&painter);
    QFrame::update();
}
