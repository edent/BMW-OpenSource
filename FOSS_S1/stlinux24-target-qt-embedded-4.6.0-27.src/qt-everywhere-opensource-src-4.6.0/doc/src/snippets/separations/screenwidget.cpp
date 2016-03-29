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

/*
screenwidget.cpp

A widget to display colour components from an image using independently
selected colors. Controls are provided to allow the image to be inverted, and
the color to be selection via a standard dialog. The image is displayed in a
label widget.
*/

#include <QApplication>
#include <QColorDialog>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

#include "screenwidget.h"

/*!
Initializes the paint color, the mask color (cyan, magenta,
or yellow), connects the color selector and invert checkbox to functions,
and creates a two-by-two grid layout.
*/

ScreenWidget::ScreenWidget(QWidget *parent, QColor initialColor,
                           const QString &name, Separation mask,
                           const QSize &labelSize)
    : QFrame(parent)
{
    paintColor = initialColor;
    maskColor = mask;
    inverted = false;

    imageLabel = new QLabel;
    imageLabel->setFrameShadow(QFrame::Sunken);
    imageLabel->setFrameShape(QFrame::StyledPanel);
    imageLabel->setMinimumSize(labelSize);

    nameLabel = new QLabel(name);
    colorButton = new QPushButton(tr("Modify..."));
    colorButton->setBackgroundRole(QPalette::Button);
    colorButton->setMinimumSize(32, 32);

    QPalette palette(colorButton->palette());
    palette.setColor(QPalette::Button, initialColor);
    colorButton->setPalette(palette);

    invertButton = new QPushButton(tr("Invert"));
    //invertButton->setToggleButton(true);
    //invertButton->setOn(inverted);
    invertButton->setEnabled(false);

    connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
    connect(invertButton, SIGNAL(clicked()), this, SLOT(invertImage()));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(imageLabel, 0, 0, 1, 2);
    gridLayout->addWidget(nameLabel, 1, 0);
    gridLayout->addWidget(colorButton, 1, 1);
    gridLayout->addWidget(invertButton, 2, 1, 1, 1);
    setLayout(gridLayout);
}

/*!
    Creates a new image by separating out the cyan, magenta, or yellow
    component, depending on the mask color specified in the constructor.

    The amount of the component found in each pixel of the image is used
    to determine how much of a user-selected ink is used for each pixel
    in the new image for the label widget.
*/

void ScreenWidget::createImage()
{
    newImage = originalImage.copy();

    // Create CMY components for the ink being used.
    float cyanInk = (255 - paintColor.red())/255.0;
    float magentaInk = (255 - paintColor.green())/255.0;
    float yellowInk = (255 - paintColor.blue())/255.0;

    int (*convert)(QRgb);

    switch (maskColor) {
        case Cyan:
            convert = qRed;
            break;
        case Magenta:
            convert = qGreen;
            break;
        case Yellow:
            convert = qBlue;
            break;
    }

    for (int y = 0; y < newImage.height(); ++y) {
        for (int x = 0; x < newImage.width(); ++x) {
            QRgb p(originalImage.pixel(x, y));

            // Separate the source pixel into its cyan component.
            int amount;

            if (inverted)
                amount = convert(p);
            else
                amount = 255 - convert(p);

            QColor newColor(
                255 - qMin(int(amount * cyanInk), 255),
                255 - qMin(int(amount * magentaInk), 255),
                255 - qMin(int(amount * yellowInk), 255));

            newImage.setPixel(x, y, newColor.rgb());
        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(newImage));
}

/*!
    Returns a pointer to the modified image.
*/

QImage* ScreenWidget::image()
{
    return &newImage;
}

/*!
    Sets whether the amount of ink applied to the canvas is to be inverted
    (subtracted from the maximum value) before the ink is applied.
*/

void ScreenWidget::invertImage()
{
    //inverted = invertButton->isOn();
    inverted = !inverted;
    createImage();
    emit imageChanged();
}

/*!
    Separate the current image into cyan, magenta, and yellow components.
    Create a representation of how each component might appear when applied
    to a blank white piece of paper.
*/

void ScreenWidget::setColor()
{
    QColor newColor = QColorDialog::getColor(paintColor);

    if (newColor.isValid()) {
        paintColor = newColor;
        QPalette palette(colorButton->palette());
        palette.setColor(QPalette::Button, paintColor);
        colorButton->setPalette(palette);
        createImage();
        emit imageChanged();
    }
}

/*!
    Records the original image selected by the user, creates a color
    separation, and enables the invert image checkbox.
*/

void ScreenWidget::setImage(QImage &image)
{
    originalImage = image;
    createImage();
    invertButton->setEnabled(true);
}
