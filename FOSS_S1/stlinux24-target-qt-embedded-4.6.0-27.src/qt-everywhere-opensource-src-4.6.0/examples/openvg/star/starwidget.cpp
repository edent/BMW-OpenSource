/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include "starwidget.h"

StarWidget::StarWidget(QWidget *parent)
    : QWidget(parent)
    , path(VG_INVALID_HANDLE)
    , pen(Qt::red, 4.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , brush(Qt::yellow)
{
    setMinimumSize(220, 250);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

static VGubyte const starSegments[] = {
    VG_MOVE_TO_ABS,
    VG_LINE_TO_REL,
    VG_LINE_TO_REL,
    VG_LINE_TO_REL,
    VG_LINE_TO_REL,
    VG_CLOSE_PATH
};
static VGfloat const starCoords[] = {
    110, 35,
    50, 160,
    -130, -100,
    160, 0,
    -130, 100
};

void StarWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);

    // Make sure that we are using the OpenVG paint engine.
    if (painter.paintEngine()->type() != QPaintEngine::OpenVG) {
#ifdef Q_WS_QWS
        qWarning("Not using OpenVG: use the '-display' option to specify an OpenVG driver");
#else
        qWarning("Not using OpenVG: specify '-graphicssystem OpenVG'");
#endif
        return;
    }

    // Select a pen and a brush for drawing the star.
    painter.setPen(pen);
    painter.setBrush(brush);

    // We want the star border to be anti-aliased.
    painter.setRenderHints(QPainter::Antialiasing);

    // Flush the state changes to the OpenVG implementation
    // and prepare to perform raw OpenVG calls.
    painter.beginNativePainting();

    // Cache the path if we haven't already.
    if (path == VG_INVALID_HANDLE) {
        path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                            VG_PATH_DATATYPE_F,
                            1.0f, // scale
                            0.0f, // bias
                            6,    // segmentCapacityHint
                            10,   // coordCapacityHint
                            VG_PATH_CAPABILITY_ALL);
        vgAppendPathData(path, sizeof(starSegments), starSegments, starCoords);
    }

    // Draw the star directly using the OpenVG API.
    vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);

    // Restore normal QPainter operations.
    painter.endNativePainting();

    painter.end();
}
