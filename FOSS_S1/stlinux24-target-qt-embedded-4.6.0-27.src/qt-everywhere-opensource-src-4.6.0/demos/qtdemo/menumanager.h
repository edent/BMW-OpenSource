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

#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <QtGui>
#include <QtXml>
#include <QtHelp/QHelpEngineCore>

#include "score.h"
#include "textbutton.h"
#include "mainwindow.h"
#include "itemcircleanimation.h"

typedef QHash<QString, QString> StringHash;
typedef QHash<QString, StringHash> HashHash;

class TextButton;

class MenuManager : public QObject
{
    Q_OBJECT

public:
    enum BUTTON_TYPE {ROOT, MENU1, MENU2, LAUNCH, DOCUMENTATION, QUIT, FULLSCREEN, UP, DOWN, BACK};

    // singleton pattern:
    static MenuManager *instance();
    virtual ~MenuManager();

    void init(MainWindow *window);
    void itemSelected(int userCode, const QString &menuName = "");

    QByteArray getHtml(const QString &name);
    QByteArray getImage(const QString &name);
    QString resolveExeFile(const QString &name);
    QString resolveDocUrl(const QString &name);
    QString resolveImageUrl(const QString &name);
    QString resolveDataDir(const QString &name);

    HashHash info;
    ItemCircleAnimation *ticker;
    MainWindow *window;
    Score *score;
    int currentMenuCode;

private slots:
    void exampleFinished();
    void exampleError(QProcess::ProcessError error);

private:
    // singleton pattern:
    MenuManager();
    static MenuManager *pInstance;

    QByteArray getResource(const QString &name);

    void readXmlDocument();
    void initHelpEngine();
    void getDocumentationDir();
    void readInfoAboutExample(const QDomElement &example);
    void showDocInAssistant(const QString &docFile);
    void launchExample(const QString &uniqueName);

    void createMenu(const QDomElement &category, BUTTON_TYPE type);
    void createLowLeftButton(const QString &label, BUTTON_TYPE type,
        Movie *movieIn, Movie *movieOut, Movie *movieShake, const QString &menuString = QString());
    void createLowRightButton(const QString &label, BUTTON_TYPE type, Movie *movieIn, Movie *movieOut, Movie *movieShake);
    void createLowRightLeafButton(const QString &label, int pos, BUTTON_TYPE type, Movie *movieIn, Movie *movieOut, Movie * /*movieShake*/);
    void createRootMenu(const QDomElement &el);
    void createSubMenu(const QDomElement &el);
    void createLeafMenu(const QDomElement &el);
    void createInfo(DemoItem *item, const QString &name);
    void createTicker();
    void createUpnDownButtons();
    void createBackButton();

    QDomDocument *contentsDoc;
    QProcess assistantProcess;
    QString currentMenu;
    QString currentCategory;
    QString currentMenuButtons;
    QString currentInfo;
    QString helpRootUrl;
    DemoItemAnimation *tickerInAnim;
    QDir docDir;
    QDir imgDir;
    QHelpEngineCore *helpEngine;

    TextButton *upButton;
    TextButton *downButton;
};

#endif // MENU_MANAGER_H

