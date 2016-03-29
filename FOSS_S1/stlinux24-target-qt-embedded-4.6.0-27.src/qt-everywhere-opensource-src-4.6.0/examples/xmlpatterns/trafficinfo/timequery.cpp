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

#include "timequery.h"

#include <QtCore/QStringList>
#include <QtXmlPatterns/QXmlQuery>

TimeInformation::TimeInformation(const QString &time, const QString &direction)
    : m_time(time), m_direction(direction)
{
}

QString TimeInformation::time() const
{
    return m_time;
}

QString TimeInformation::direction() const
{
    return m_direction;
}

TimeInformation::List TimeQuery::query(const QString &stationId, const QStringList &lineNumbers, const QDateTime &dateTime)
{
    const TimeInformation::List information = queryInternal(stationId, dateTime);

    TimeInformation::List filteredInformation;

    if (!lineNumbers.isEmpty()) {
        for (int i = 0; i < information.count(); ++i) {
            const TimeInformation info = information.at(i);
            for (int j = 0; j < lineNumbers.count(); ++j) {
                if (info.direction().startsWith(QString("%1 ").arg(lineNumbers.at(j))))
                    filteredInformation.append(info);
            }
        }
    } else {
        filteredInformation = information;
    }

    return filteredInformation;
}

//! [1]
TimeInformation::List TimeQuery::queryInternal(const QString &stationId, const QDateTime &dateTime)
{
    const QString timesQueryUrl = QString("doc('http://wap.trafikanten.no/F.asp?f=%1&amp;t=%2&amp;m=%3&amp;d=%4&amp;start=1')/wml/card/p/small/a[fn:starts-with(@href, 'Rute')]/string()")
                                         .arg(stationId)
                                         .arg(dateTime.time().hour())
                                         .arg(dateTime.time().minute())
                                         .arg(dateTime.toString("dd.MM.yyyy"));
    const QString directionsQueryUrl = QString("doc('http://wap.trafikanten.no/F.asp?f=%1&amp;t=%2&amp;m=%3&amp;d=%4&amp;start=1')/wml/card/p/small/text()[matches(., '[0-9].*')]/string()")
                                              .arg(stationId)
                                              .arg(dateTime.time().hour())
                                              .arg(dateTime.time().minute())
                                              .arg(dateTime.toString("dd.MM.yyyy"));

    QStringList times;
    QStringList directions;

    QXmlQuery query;
    query.setQuery(timesQueryUrl);
    query.evaluateTo(&times);

    query.setQuery(directionsQueryUrl);
    query.evaluateTo(&directions);

    if (times.count() != directions.count()) // something went wrong...
        return TimeInformation::List();

    TimeInformation::List information;
    for (int i = 0; i < times.count(); ++i)
        information.append(TimeInformation(times.at(i).simplified(), directions.at(i).simplified()));

    return information;
}
//! [1]
