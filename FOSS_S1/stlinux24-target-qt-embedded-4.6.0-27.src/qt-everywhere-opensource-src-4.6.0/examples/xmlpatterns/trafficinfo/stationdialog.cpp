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

#include "stationdialog.h"
#include "ui_stationdialog.h"

#include <QtCore/QAbstractListModel>

class StationModel : public QAbstractListModel
{
    public:
        enum Role
        {
            StationIdRole = Qt::UserRole + 1,
            StationNameRole
        };

        StationModel(QObject *parent = 0)
            : QAbstractListModel(parent)
        {
        }

        void setStations(const StationInformation::List &list)
        {
            m_stations = list;
            layoutChanged();
        }

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
        {
            if (!parent.isValid())
                return m_stations.count();
            else
                return 0;
        }

        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
        {
            if (!parent.isValid())
                return 1;
            else
                return 0;
        }

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid())
                return QVariant();

            if (index.column() > 1 || index.row() >= m_stations.count())
                return QVariant();

            const StationInformation info = m_stations.at(index.row());
            if (role == Qt::DisplayRole || role == StationNameRole)
                return info.name();
            else if (role == StationIdRole)
                return info.id();

            return QVariant();
        }

    private:
        StationInformation::List m_stations;
};

StationDialog::StationDialog(const QString &name, const QStringList &lineNumbers, QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.m_searchButton, SIGNAL(clicked()), this, SLOT(searchStations()));

    m_ui.m_searchButton->setDefault(true);
    m_ui.m_input->setText(name);

    m_model = new StationModel(this);
    m_ui.m_view->setModel(m_model);

    for (int i = 0; i < lineNumbers.count(); ++i) {
        if (i == 0)
            m_ui.m_line1->setText(lineNumbers.at(i));
        else if (i == 1)
            m_ui.m_line2->setText(lineNumbers.at(i));
        else if (i == 2)
            m_ui.m_line3->setText(lineNumbers.at(i));
        else if (i == 3)
            m_ui.m_line4->setText(lineNumbers.at(i));
    }

    QMetaObject::invokeMethod(this, SLOT(searchStations()), Qt::QueuedConnection);
}

StationInformation StationDialog::selectedStation() const
{
    const QModelIndex index = m_ui.m_view->currentIndex();

    if (!index.isValid())
        return StationInformation();

    return StationInformation(index.data(StationModel::StationIdRole).toString(),
                              index.data(StationModel::StationNameRole).toString());
}

QStringList StationDialog::lineNumbers() const
{
    QStringList lines;

    if (!m_ui.m_line1->text().simplified().isEmpty())
        lines.append(m_ui.m_line1->text().simplified());
    if (!m_ui.m_line2->text().simplified().isEmpty())
        lines.append(m_ui.m_line2->text().simplified());
    if (!m_ui.m_line3->text().simplified().isEmpty())
        lines.append(m_ui.m_line3->text().simplified());
    if (!m_ui.m_line4->text().simplified().isEmpty())
        lines.append(m_ui.m_line4->text().simplified());

    return lines;
}

void StationDialog::searchStations()
{
    m_model->setStations(StationQuery::query(m_ui.m_input->text()));
    m_ui.m_view->keyboardSearch(m_ui.m_input->text());
}
