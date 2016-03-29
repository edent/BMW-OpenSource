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

#ifndef STICKMAN_H
#define STICKMAN_H

#include <QGraphicsObject>

static const int NodeCount = 16;
static const int BoneCount = 24;

class Node;
QT_BEGIN_NAMESPACE
QT_END_NAMESPACE
class StickMan: public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor WRITE setPenColor READ penColor)
    Q_PROPERTY(QColor fillColor WRITE setFillColor READ fillColor)
    Q_PROPERTY(bool isDead WRITE setIsDead READ isDead)
public:
    StickMan();
    ~StickMan();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int nodeCount() const; 
    Node *node(int idx) const;

    void setDrawSticks(bool on);
    bool drawSticks() const { return m_sticks; }

    QColor penColor() const { return m_penColor; }
    void setPenColor(const QColor &color) { m_penColor = color; }

    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor &color) { m_fillColor = color; }

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = isDead; }

public slots:
    void stabilize();
    void childPositionChanged();

protected:
    void timerEvent(QTimerEvent *e);

private:

    QPointF posFor(int idx) const;

    Node *m_nodes[NodeCount];
    qreal m_perfectBoneLengths[BoneCount];
    
    uint m_sticks : 1;
    uint m_isDead : 1;
    uint m_reserved : 30;

    QPixmap m_pixmap;
    QColor m_penColor;
    QColor m_fillColor;
};

#endif // STICKMAN_H
