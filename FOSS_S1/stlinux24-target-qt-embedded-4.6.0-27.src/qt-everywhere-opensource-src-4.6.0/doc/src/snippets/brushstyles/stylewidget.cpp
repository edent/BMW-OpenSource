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

#include <QtGui>

#include "stylewidget.h"

StyleWidget::StyleWidget(QWidget *parent)
        : QWidget(parent)
{
    solid = new RenderArea(new QBrush(Qt::SolidPattern));
    dense1 = new RenderArea(new QBrush(Qt::Dense1Pattern));
    dense2 = new RenderArea(new QBrush(Qt::Dense2Pattern));
    dense3 = new RenderArea(new QBrush(Qt::Dense3Pattern));
    dense4 = new RenderArea(new QBrush(Qt::Dense4Pattern));
    dense5 = new RenderArea(new QBrush(Qt::Dense5Pattern));
    dense6 = new RenderArea(new QBrush(Qt::Dense6Pattern));
    dense7 = new RenderArea(new QBrush(Qt::Dense7Pattern));
    no = new RenderArea(new QBrush(Qt::NoBrush));
    hor = new RenderArea(new QBrush(Qt::HorPattern));
    ver = new RenderArea(new QBrush(Qt::VerPattern));
    cross = new RenderArea(new QBrush(Qt::CrossPattern));
    bdiag = new RenderArea(new QBrush(Qt::BDiagPattern));
    fdiag = new RenderArea(new QBrush(Qt::FDiagPattern));
    diagCross = new RenderArea(new QBrush(Qt::DiagCrossPattern));
    linear = new RenderArea(new QBrush(QLinearGradient()));
    radial = new RenderArea(new QBrush(QRadialGradient()));
    conical = new RenderArea(new QBrush(QConicalGradient()));
    texture = new RenderArea(new QBrush(QPixmap("qt-logo.png")));

    solidLabel = new QLabel("Qt::SolidPattern");
    dense1Label = new QLabel("Qt::Dense1Pattern");
    dense2Label = new QLabel("Qt::Dense2Pattern");
    dense3Label = new QLabel("Qt::Dense3Pattern");
    dense4Label = new QLabel("Qt::Dense4Pattern");
    dense5Label = new QLabel("Qt::Dense5Pattern");
    dense6Label = new QLabel("Qt::Dense6Pattern");
    dense7Label = new QLabel("Qt::Dense7Pattern");
    noLabel = new QLabel("Qt::NoPattern");
    horLabel = new QLabel("Qt::HorPattern");
    verLabel = new QLabel("Qt::VerPattern");
    crossLabel = new QLabel("Qt::CrossPattern");
    bdiagLabel = new QLabel("Qt::BDiagPattern");
    fdiagLabel = new QLabel("Qt::FDiagPattern");
    diagCrossLabel = new QLabel("Qt::DiagCrossPattern");
    linearLabel = new QLabel("Qt::LinearGradientPattern");
    radialLabel = new QLabel("Qt::RadialGradientPattern");
    conicalLabel = new QLabel("Qt::ConicalGradientPattern");
    textureLabel = new QLabel("Qt::TexturePattern");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(solid, 0, 0);
    layout->addWidget(dense1, 0, 1);
    layout->addWidget(dense2, 0, 2);
    layout->addWidget(solidLabel, 1, 0);
    layout->addWidget(dense1Label, 1, 1);
    layout->addWidget(dense2Label, 1, 2);

    layout->addWidget(dense3, 2, 0 );
    layout->addWidget(dense4, 2, 1);
    layout->addWidget(dense5, 2, 2);
    layout->addWidget(dense3Label, 3, 0);
    layout->addWidget(dense4Label, 3, 1);
    layout->addWidget(dense5Label, 3, 2);

    layout->addWidget(dense6, 4, 0);
    layout->addWidget(dense7, 4, 1);
    layout->addWidget(no, 4, 2);
    layout->addWidget(dense6Label, 5, 0);
    layout->addWidget(dense7Label, 5, 1);
    layout->addWidget(noLabel, 5, 2);

    layout->addWidget(hor, 6, 0);
    layout->addWidget(ver, 6, 1);
    layout->addWidget(cross, 6, 2);
    layout->addWidget(horLabel, 7, 0);
    layout->addWidget(verLabel, 7, 1);
    layout->addWidget(crossLabel, 7, 2);

    layout->addWidget(bdiag, 8, 0);
    layout->addWidget(fdiag, 8, 1);
    layout->addWidget(diagCross, 8, 2);
    layout->addWidget(bdiagLabel, 9, 0);
    layout->addWidget(fdiagLabel, 9, 1);
    layout->addWidget(diagCrossLabel, 9, 2);

    layout->addWidget(linear, 10, 0);
    layout->addWidget(radial, 10, 1);
    layout->addWidget(conical, 10, 2);
    layout->addWidget(linearLabel, 11, 0);
    layout->addWidget(radialLabel, 11, 1);
    layout->addWidget(conicalLabel, 11, 2);

    layout->addWidget(texture, 12, 0, 1, 3);
    layout->addWidget(textureLabel, 13, 0, 1, 3);

    setLayout(layout);

    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::white);
    setPalette(newPalette);

    setWindowTitle(tr("Brush Styles"));
    resize(430, 605);
}


