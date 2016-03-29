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
    bool ok;
    QGradient gradient = QtGradientDialog::getGradient(&ok, QRadialGradient(), this);
    if (ok) {
        // the user clicked OK and gradient is set to the gradient the user selected
    } else {
        // the user canceled the dialog; gradient is set to the initial
        // value, in this case radial gradient
    }
//! [0]


//! [1]
    bool detailsVisible;
    QColor::Spec spec;
    QGradient gradient;

    QtGradientDialog dlg(this);
    dlg.setDetailsVisible(detailsVisible);
    dlg.setSpec(spec);
    dlg.setGradient(gradient);
    if (dlg.exec() != QDialog::Accepted)
        return;

    gradient = dlg.gradient();
    // detailsVisible and spec properties can be changed interactively by the user too,
    // we store the values of these properties for the next time QtGradientDialog is executed.
    detailsVisible = dlg.detailsVisible();
    spec = dlg.spec();
//! [1]


//! [2]
        QtGradientDialog dlg;
        dlg.setBackgroundCheckered(true);
//! [2]


//! [3]
        QtGradientDialog dlg;
        dlg.setBackgroundCheckered(false);
//! [3]


