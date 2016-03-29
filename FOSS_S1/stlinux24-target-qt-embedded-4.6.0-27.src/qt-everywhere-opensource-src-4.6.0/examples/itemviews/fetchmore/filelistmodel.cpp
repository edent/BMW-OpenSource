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

#include "filelistmodel.h"
#include <QApplication>
#include <QPalette>
#include <QBrush>
#include <QDir>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

//![4]
int FileListModel::rowCount(const QModelIndex & /* parent */) const
{
    return fileCount;
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= fileList.size() || index.row() < 0)
        return QVariant();
    
    if (role == Qt::DisplayRole)
        return fileList.at(index.row());
    else if (role == Qt::BackgroundRole) {
        int batch = (index.row() / 100) % 2;
        if (batch == 0)
            return qApp->palette().base();
        else
            return qApp->palette().alternateBase();
    }
    return QVariant();
}
//![4]

//![1]
bool FileListModel::canFetchMore(const QModelIndex & /* index */) const
{
    if (fileCount < fileList.size())
        return true;
    else
        return false;
}
//![1]

//![2]
void FileListModel::fetchMore(const QModelIndex & /* index */)
{
    int remainder = fileList.size() - fileCount;
    int itemsToFetch = qMin(100, remainder);

    beginInsertRows(QModelIndex(), fileCount, fileCount+itemsToFetch);
    
    fileCount += itemsToFetch;

    endInsertRows();

    emit numberPopulated(itemsToFetch);
}
//![2]

//![0]
void FileListModel::setDirPath(const QString &path)
{
    QDir dir(path);

    fileList = dir.entryList();
    fileCount = 0;
    reset();
}
//![0]

