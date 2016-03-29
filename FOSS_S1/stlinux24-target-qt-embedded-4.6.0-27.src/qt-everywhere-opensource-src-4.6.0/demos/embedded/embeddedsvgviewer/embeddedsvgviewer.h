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

#ifndef EMBEDDED_SVG_VIEWER_H
#define EMBEDDED_SVG_VIEWER_H

#include <QWidget>
#include <QString>
#include <QSvgRenderer>
#include <QSize>
#include <QMouseEvent>
#include <QSlider>
#include <QPushButton>

class EmbeddedSvgViewer : public QWidget
{
    Q_OBJECT
public:
    EmbeddedSvgViewer(const QString& filePath);
    virtual void paintEvent(QPaintEvent *event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void resizeEvent ( QResizeEvent * event );

public slots:
    void setZoom(int); // 100 <= newZoom < 0

private:
    QSvgRenderer* m_renderer;
    QSlider* m_zoomSlider;
    QPushButton* m_quitButton;
    QSize m_imageSize;
    qreal m_zoomLevel;
    qreal m_imageScale; // How many Image coords 1 widget pixel is worth

    QRectF m_viewBox;
    QRectF m_viewBoxBounds;
    QSizeF m_viewBoxSize;
    QPointF m_viewBoxCenter;
    QPointF m_viewBoxCenterOnMousePress;
    QPoint m_mousePress;

    void updateImageScale();
    QRectF getViewBox(QPointF viewBoxCenter);
};



#endif
