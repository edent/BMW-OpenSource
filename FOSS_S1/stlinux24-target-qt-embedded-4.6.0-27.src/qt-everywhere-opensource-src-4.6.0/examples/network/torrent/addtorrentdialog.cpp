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

#include "addtorrentdialog.h"
#include "metainfo.h"

#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMetaObject>

static QString stringNumber(qint64 number)
{
    QString tmp;
    if (number > (1024 * 1024 * 1024))
        tmp.sprintf("%.2fGB", number / (1024.0 * 1024.0 * 1024.0));
    else if (number > (1024 * 1024))
        tmp.sprintf("%.2fMB", number / (1024.0 * 1024.0));
    else if (number > (1024))
        tmp.sprintf("%.2fKB", number / (1024.0));
    else
        tmp.sprintf("%d bytes", int(number));
    return tmp;
}

AddTorrentDialog::AddTorrentDialog(QWidget *parent)
  : QDialog(parent, Qt::Sheet)
{
    ui.setupUi(this);

    connect(ui.browseTorrents, SIGNAL(clicked()),
            this, SLOT(selectTorrent()));
    connect(ui.browseDestination, SIGNAL(clicked()),
            this, SLOT(selectDestination()));
    connect(ui.torrentFile, SIGNAL(textChanged(const QString &)),
            this, SLOT(setTorrent(const QString &)));

    ui.destinationFolder->setText(destinationDirectory = QDir::current().path());
    ui.torrentFile->setFocus();
}

void AddTorrentDialog::selectTorrent()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a torrent file"),
                                                    lastDirectory,
                                                    tr("Torrents (*.torrent);; All files (*.*)"));
    if (fileName.isEmpty())
        return;
    lastDirectory = QFileInfo(fileName).absolutePath();
    setTorrent(fileName);
}

void AddTorrentDialog::selectDestination()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a destination directory"),
                                                    lastDestinationDirectory);
    if (dir.isEmpty())
        return;
    lastDestinationDirectory = destinationDirectory = dir;
    ui.destinationFolder->setText(destinationDirectory);
    enableOkButton();
}

void AddTorrentDialog::enableOkButton()
{
    ui.okButton->setEnabled(!ui.destinationFolder->text().isEmpty()
                            && !ui.torrentFile->text().isEmpty());
}

void AddTorrentDialog::setTorrent(const QString &torrentFile)
{
    if (torrentFile.isEmpty()) {
        enableOkButton();
        return;
    }

    ui.torrentFile->setText(torrentFile);
    if (!torrentFile.isEmpty())
        lastDirectory = QFileInfo(torrentFile).absolutePath();

    if (lastDestinationDirectory.isEmpty())
        lastDestinationDirectory = lastDirectory;
    
    MetaInfo metaInfo;
    QFile torrent(torrentFile);
    if (!torrent.open(QFile::ReadOnly) || !metaInfo.parse(torrent.readAll())) {
        enableOkButton();
        return;
    }
    
    ui.torrentFile->setText(torrentFile);
    ui.announceUrl->setText(metaInfo.announceUrl());
    if (metaInfo.comment().isEmpty())
        ui.commentLabel->setText("<unknown>");
    else
        ui.commentLabel->setText(metaInfo.comment());
    if (metaInfo.createdBy().isEmpty())
        ui.creatorLabel->setText("<unknown>");
    else
        ui.creatorLabel->setText(metaInfo.createdBy());
    ui.sizeLabel->setText(stringNumber(metaInfo.totalSize()));
    if (metaInfo.fileForm() == MetaInfo::SingleFileForm) {
        ui.torrentContents->setHtml(metaInfo.singleFile().name);
    } else {
        QString html;
        foreach (MetaInfoMultiFile file, metaInfo.multiFiles()) {
            QString name = metaInfo.name();
            if (!name.isEmpty()) {
                html += name;
                if (!name.endsWith('/'))
                    html += '/';
            }
            html += file.path + "<br>";
        }
        ui.torrentContents->setHtml(html);
    }

    QFileInfo info(torrentFile);
    ui.destinationFolder->setText(info.absolutePath());

    enableOkButton();
}

QString AddTorrentDialog::torrentFileName() const
{
    return ui.torrentFile->text();
}

QString AddTorrentDialog::destinationFolder() const
{
    return ui.destinationFolder->text();
}
