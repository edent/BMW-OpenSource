/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE

class InvokeMethod;
class ChangeProperties;
class AmbientProperties;
class QAxScriptManager;

class QWorkspace;

QT_END_NAMESPACE

QT_USE_NAMESPACE


class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void on_actionFileNew_triggered();
    void on_actionFileLoad_triggered();
    void on_actionFileSave_triggered();

    void on_actionContainerSet_triggered();
    void on_actionContainerClear_triggered();
    void on_actionContainerProperties_triggered();

    void on_actionControlInfo_triggered();
    void on_actionControlDocumentation_triggered();
    void on_actionControlPixmap_triggered();
    void on_actionControlProperties_triggered();
    void on_actionControlMethods_triggered();
    void on_VerbMenu_aboutToShow();

    void on_actionScriptingLoad_triggered();
    void on_actionScriptingRun_triggered();

private:
    InvokeMethod *dlgInvoke;
    ChangeProperties *dlgProperties;
    AmbientProperties *dlgAmbient;
    QAxScriptManager *scripts;
    QWorkspace *workspace;

    QtMsgHandler oldDebugHandler;

private slots:
    void updateGUI();
    void logPropertyChanged(const QString &prop);
    void logSignal(const QString &signal, int argc, void *argv);
    void logException(int code, const QString&source, const QString&desc, const QString&help);
    void logMacro(int code, const QString &description, int sourcePosition, const QString &sourceText);

    void on_VerbMenu_triggered(QAction *action);
};

#endif // MAINWINDOW_H
