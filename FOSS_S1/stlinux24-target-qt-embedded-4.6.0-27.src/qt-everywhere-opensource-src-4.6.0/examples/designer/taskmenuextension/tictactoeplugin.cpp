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
#include <QtPlugin>

#include "tictactoe.h"
#include "tictactoeplugin.h"
#include "tictactoetaskmenu.h"

//! [0]
TicTacToePlugin::TicTacToePlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

QString TicTacToePlugin::name() const
{
    return "TicTacToe";
}

QString TicTacToePlugin::group() const
{
    return "Display Widgets [Examples]";
}

QString TicTacToePlugin::toolTip() const
{
    return "";
}

QString TicTacToePlugin::whatsThis() const
{
    return "";
}

QString TicTacToePlugin::includeFile() const
{
    return "tictactoe.h";
}

QIcon TicTacToePlugin::icon() const
{
    return QIcon();
}

bool TicTacToePlugin::isContainer() const
{
    return false;
}

QWidget *TicTacToePlugin::createWidget(QWidget *parent)
{
    TicTacToe *ticTacToe = new TicTacToe(parent);
    ticTacToe->setState("-X-XO----");
    return ticTacToe;
}

bool TicTacToePlugin::isInitialized() const
{
    return initialized;
}

//! [0] //! [1]
void TicTacToePlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
//! [1] //! [2]
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != 0);
//! [2]

//! [3]
    manager->registerExtensions(new TicTacToeTaskMenuFactory(manager),
                                Q_TYPEID(QDesignerTaskMenuExtension));

    initialized = true;
}

QString TicTacToePlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"TicTacToe\" name=\"ticTacToe\"/>\
    <customwidgets>\
        <customwidget>\
            <class>TicTacToe</class>\
            <propertyspecifications>\
            <stringpropertyspecification name=\"state\" notr=\"true\" type=\"singleline\"/>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}

//! [3]

//! [4]
Q_EXPORT_PLUGIN2(taskmenuextension, TicTacToePlugin)
//! [4]
