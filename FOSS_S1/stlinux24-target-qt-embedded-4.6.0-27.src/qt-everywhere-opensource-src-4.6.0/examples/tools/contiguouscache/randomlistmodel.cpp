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
#include "randomlistmodel.h"
#include <stdlib.h>

static const int bufferSize(500);
static const int lookAhead(100);
static const int halfLookAhead(lookAhead/2);

RandomListModel::RandomListModel(QObject *parent)
: QAbstractListModel(parent), m_rows(bufferSize), m_count(10000)
{
}

RandomListModel::~RandomListModel()
{
}

int RandomListModel::rowCount(const QModelIndex &) const
{
    return m_count;
}

//! [0]
QVariant RandomListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();

    if (row > m_rows.lastIndex()) {
        if (row - m_rows.lastIndex() > lookAhead)
            cacheRows(row-halfLookAhead, qMin(m_count, row+halfLookAhead));
        else while (row > m_rows.lastIndex())
            m_rows.append(fetchRow(m_rows.lastIndex()+1));
    } else if (row < m_rows.firstIndex()) {
        if (m_rows.firstIndex() - row > lookAhead)
            cacheRows(qMax(0, row-halfLookAhead), row+halfLookAhead);
        else while (row < m_rows.firstIndex())
            m_rows.prepend(fetchRow(m_rows.firstIndex()-1));
    }

    return m_rows.at(row);
}

void RandomListModel::cacheRows(int from, int to) const
{
    for (int i = from; i <= to; ++i)
        m_rows.insert(i, fetchRow(i));
}
//![0]

//![1]
QString RandomListModel::fetchRow(int position) const
{
    return QString::number(rand() % ++position);
}
//![1]
