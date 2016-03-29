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

#ifndef CONTENTTAB_H_
#define CONTENTTAB_H_

// EXTERNAL INCLUDES
#include <QDir>
#include <QUrl>
#include <QIcon>
#include <QFileInfo>
#include <QListWidget>
#include <QDesktopServices>

// INTERNAL INCLUDES

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

// CLASS DECLARATION

/**
* ContentTab class.
*
* This class implements general purpose tab for media files.
*/
class ContentTab : public QListWidget
{
    Q_OBJECT

public:         // Constructors & Destructors
    ContentTab(QWidget *parent);
    virtual ~ContentTab();

public:         // New Methods
    virtual void init(const QDesktopServices::StandardLocation &location,
                      const QString &filter,
                      const QString &icon);

protected:      // New Methods
    virtual void setContentDir(const QDesktopServices::StandardLocation &location);
    virtual void setIcon(const QString &icon);
    virtual void populateListWidget();
    virtual QString itemName(const QFileInfo &item);
    virtual QUrl itemUrl(QListWidgetItem *item);
    virtual void handleErrorInOpen(QListWidgetItem *item);
protected:
    void keyPressEvent(QKeyEvent *event);

public slots:   // New Slots
    virtual void openItem(QListWidgetItem *item);

protected:     // Owned variables
    QDir m_ContentDir;
    QIcon m_Icon;
};


#endif // CONTENTTAB_H_

// End of File
