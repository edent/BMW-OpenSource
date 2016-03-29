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

// EXTERNAL INCLUDES
#include <QKeyEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFileInfoList>
#include <QListWidgetItem>

// INTERNAL INCLUDES

// CLASS HEADER
#include "contenttab.h"


// CONSTRUCTORS & DESTRUCTORS
ContentTab::ContentTab(QWidget *parent) :
        QListWidget(parent)
{
    setDragEnabled(false);
    setIconSize(QSize(45, 45));
}

ContentTab::~ContentTab()
{
}

// NEW PUBLIC METHODS
void ContentTab::init(const QDesktopServices::StandardLocation &location,
                      const QString &filter,
                      const QString &icon)
{
    setContentDir(location);
    QStringList filterList;
    filterList = filter.split(";");
    m_ContentDir.setNameFilters(filterList);
    setIcon(icon);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(openItem(QListWidgetItem *)));

    populateListWidget();
}

// NEW PROTECTED METHODS
void ContentTab::setContentDir(const QDesktopServices::StandardLocation &location)
{
    m_ContentDir.setPath(QDesktopServices::storageLocation(location));
}

void ContentTab::setIcon(const QString &icon)
{
    m_Icon = QIcon(icon);
}

void ContentTab::populateListWidget()
{
    QFileInfoList fileList = m_ContentDir.entryInfoList(QDir::Files, QDir::Time);
    foreach(QFileInfo item, fileList) {
        new QListWidgetItem(m_Icon, itemName(item), this);
    }
}

QString ContentTab::itemName(const QFileInfo &item)
{
    return QString(item.baseName() + "." + item.completeSuffix());
}

QUrl ContentTab::itemUrl(QListWidgetItem *item)
{
    return QUrl("file:///" + m_ContentDir.absolutePath() + "/" + item->text());
}

void ContentTab::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        if (currentRow() == 0) {
            setCurrentRow(count() - 1);
        } else {
            setCurrentRow(currentRow() - 1);
        }
        break;
    case Qt::Key_Down:
        if (currentRow() == (count() - 1)) {
            setCurrentRow(0);
        } else {
            setCurrentRow(currentRow() + 1);
        }
        break;
    case Qt::Key_Select:
        openItem(currentItem());
    default:
        QListWidget::keyPressEvent(event);
        break;
    }
}

void ContentTab::handleErrorInOpen(QListWidgetItem *item)
{
    Q_UNUSED(item);
    QMessageBox::warning(this, tr("Operation Failed"), tr("Unkown error!"), QMessageBox::Close);
}

// NEW SLOTS
void ContentTab::openItem(QListWidgetItem *item)
{
#if defined(Q_OS_SYMBIAN) && defined(Q_CC_NOKIAX86)
    // Opening music files doesn't work in Symbian emulator and in some SDKs freezes the
    // emulator entirely, so prevent it.
    QStringList nameFilters = m_ContentDir.nameFilters();
    if (nameFilters.contains("*.mp3")) {
        QMessageBox::warning(this, tr("Operation Failed"), tr("Action not supported in emulator."),
                             QMessageBox::Close);
    } else
#endif
    {
        bool ret = QDesktopServices::openUrl(itemUrl(item));
        if (!ret)
            handleErrorInOpen(item);
    }
}


// End of File
