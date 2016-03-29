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

#include <QtDesigner>
#include <QtGui>

#include "tictactoe.h"
#include "tictactoedialog.h"
#include "tictactoetaskmenu.h"

//! [0]
TicTacToeTaskMenu::TicTacToeTaskMenu(TicTacToe *tic, QObject *parent)
    : QObject(parent)
{
    ticTacToe = tic;

    editStateAction = new QAction(tr("Edit State..."), this);
    connect(editStateAction, SIGNAL(triggered()), this, SLOT(editState()));
}
//! [0]

//! [1]
void TicTacToeTaskMenu::editState()
{
    TicTacToeDialog dialog(ticTacToe);
    dialog.exec();
}
//! [1]

//! [2]
QAction *TicTacToeTaskMenu::preferredEditAction() const
{
    return editStateAction;
}
//! [2]

//! [3]
QList<QAction *> TicTacToeTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(editStateAction);
    return list;
}
//! [3]

//! [4]
TicTacToeTaskMenuFactory::TicTacToeTaskMenuFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}
//! [4]

//! [5]
QObject *TicTacToeTaskMenuFactory::createExtension(QObject *object,
                                                   const QString &iid,
                                                   QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (TicTacToe *tic = qobject_cast<TicTacToe*>(object))
        return new TicTacToeTaskMenu(tic, parent);

    return 0;
}
//! [5]
