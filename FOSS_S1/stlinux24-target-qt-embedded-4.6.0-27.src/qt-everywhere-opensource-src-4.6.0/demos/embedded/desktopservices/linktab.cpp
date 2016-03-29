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
#include <QUrl>
#include <QMessageBox>
#include <QListWidgetItem>

// INTERNAL INCLUDES

// CLASS HEADER
#include "linktab.h"

LinkTab::LinkTab(QWidget *parent) :
        ContentTab(parent)
{
}

LinkTab::~LinkTab()
{
}

void LinkTab::populateListWidget()
{
    m_WebItem = new QListWidgetItem(QIcon(":/resources/browser.png"), tr("Launch Browser"), this);
    m_MailToItem = new QListWidgetItem(QIcon(":/resources/message.png"), tr("New e-mail"), this);
}

QUrl LinkTab::itemUrl(QListWidgetItem *item)
{
    if (m_WebItem == item) {
        return QUrl(tr("http://qt.nokia.com"));
    } else if (m_MailToItem == item) {
        return QUrl(tr("mailto:qts60-feedback@trolltech.com?subject=QtS60 feedback&body=Hello"));
    } else {
        // We should never endup here
        Q_ASSERT(false);
        return QUrl();
    }
}
void LinkTab::handleErrorInOpen(QListWidgetItem *item)
{
    if (m_MailToItem == item) {
        QMessageBox::warning(this, tr("Operation Failed"), tr("Please check that you have\ne-mail account defined."), QMessageBox::Close);
    } else {
        ContentTab::handleErrorInOpen(item);
    }
}

// End of file
