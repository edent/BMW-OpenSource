/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptdate_p.h"

#include <QtCore/qnumeric.h>
#include <QtCore/qstringlist.h>

#include <math.h>

#ifndef Q_WS_WIN
#   include <time.h>
#   include <sys/time.h>
#else
#   include <windows.h>
#endif

QT_BEGIN_NAMESPACE

namespace QScript {

qsreal ToInteger(qsreal n);

static const qsreal HoursPerDay = 24.0;
static const qsreal MinutesPerHour = 60.0;
static const qsreal SecondsPerMinute = 60.0;
static const qsreal msPerSecond = 1000.0;
static const qsreal msPerMinute = 60000.0;
static const qsreal msPerHour = 3600000.0;
static const qsreal msPerDay = 86400000.0;

static qsreal LocalTZA = 0.0; // initialized at startup

static inline qsreal TimeWithinDay(qsreal t)
{
    qsreal r = ::fmod(t, msPerDay);
    return (r >= 0) ? r : r + msPerDay;
}

static inline int HourFromTime(qsreal t)
{
    int r = int(::fmod(::floor(t / msPerHour), HoursPerDay));
    return (r >= 0) ? r : r + int(HoursPerDay);
}

static inline int MinFromTime(qsreal t)
{
    int r = int(::fmod(::floor(t / msPerMinute), MinutesPerHour));
    return (r >= 0) ? r : r + int(MinutesPerHour);
}

static inline int SecFromTime(qsreal t)
{
    int r = int(::fmod(::floor(t / msPerSecond), SecondsPerMinute));
    return (r >= 0) ? r : r + int(SecondsPerMinute);
}

static inline int msFromTime(qsreal t)
{
    int r = int(::fmod(t, msPerSecond));
    return (r >= 0) ? r : r + int(msPerSecond);
}

static inline qsreal Day(qsreal t)
{
    return ::floor(t / msPerDay);
}

static inline qsreal DaysInYear(qsreal y)
{
    if (::fmod(y, 4))
        return 365;

    else if (::fmod(y, 100))
        return 366;

    else if (::fmod(y, 400))
        return 365;

    return 366;
}

static inline qsreal DayFromYear(qsreal y)
{
    return 365 * (y - 1970)
        + ::floor((y - 1969) / 4)
        - ::floor((y - 1901) / 100)
        + ::floor((y - 1601) / 400);
}

static inline qsreal TimeFromYear(qsreal y)
{
    return msPerDay * DayFromYear(y);
}

static inline qsreal YearFromTime(qsreal t)
{
    int y = 1970;
    y += (int) ::floor(t / (msPerDay * 365.2425));

    qsreal t2 = TimeFromYear(y);
    return (t2 > t) ? y - 1 : ((t2 + msPerDay * DaysInYear(y)) <= t) ? y + 1 : y;
}

static inline bool InLeapYear(qsreal t)
{
    qsreal x = DaysInYear(YearFromTime(t));
    if (x == 365)
        return 0;

    Q_ASSERT (x == 366);
    return 1;
}

static inline qsreal DayWithinYear(qsreal t)
{
    return Day(t) - DayFromYear(YearFromTime(t));
}

static inline qsreal MonthFromTime(qsreal t)
{
    qsreal d = DayWithinYear(t);
    qsreal l = InLeapYear(t);

    if (d < 31.0)
        return 0;

    else if (d < 59.0 + l)
        return 1;

    else if (d < 90.0 + l)
        return 2;

    else if (d < 120.0 + l)
        return 3;

    else if (d < 151.0 + l)
        return 4;

    else if (d < 181.0 + l)
        return 5;

    else if (d < 212.0 + l)
        return 6;

    else if (d < 243.0 + l)
        return 7;

    else if (d < 273.0 + l)
        return 8;

    else if (d < 304.0 + l)
        return 9;

    else if (d < 334.0 + l)
        return 10;

    else if (d < 365.0 + l)
        return 11;

    return qSNaN(); // ### assert?
}

static inline qsreal DateFromTime(qsreal t)
{
    int m = (int) ToInteger(MonthFromTime(t));
    qsreal d = DayWithinYear(t);
    qsreal l = InLeapYear(t);

    switch (m) {
    case 0: return d + 1.0;
    case 1: return d - 30.0;
    case 2: return d - 58.0 - l;
    case 3: return d - 89.0 - l;
    case 4: return d - 119.0 - l;
    case 5: return d - 150.0 - l;
    case 6: return d - 180.0 - l;
    case 7: return d - 211.0 - l;
    case 8: return d - 242.0 - l;
    case 9: return d - 272.0 - l;
    case 10: return d - 303.0 - l;
    case 11: return d - 333.0 - l;
    }

    return qSNaN(); // ### assert
}

static inline qsreal WeekDay(qsreal t)
{
    qsreal r = ::fmod (Day(t) + 4.0, 7.0);
    return (r >= 0) ? r : r + 7.0;
}


static inline qsreal MakeTime(qsreal hour, qsreal min, qsreal sec, qsreal ms)
{
    return ((hour * MinutesPerHour + min) * SecondsPerMinute + sec) * msPerSecond + ms;
}

static inline qsreal DayFromMonth(qsreal month, qsreal leap)
{
    switch ((int) month) {
    case 0: return 0;
    case 1: return 31.0;
    case 2: return 59.0 + leap;
    case 3: return 90.0 + leap;
    case 4: return 120.0 + leap;
    case 5: return 151.0 + leap;
    case 6: return 181.0 + leap;
    case 7: return 212.0 + leap;
    case 8: return 243.0 + leap;
    case 9: return 273.0 + leap;
    case 10: return 304.0 + leap;
    case 11: return 334.0 + leap;
    }

    return qSNaN(); // ### assert?
}

static qsreal MakeDay(qsreal year, qsreal month, qsreal day)
{
    year += ::floor(month / 12.0);

    month = ::fmod(month, 12.0);
    if (month < 0)
        month += 12.0;

    qsreal t = TimeFromYear(year);
    qsreal leap = InLeapYear(t);

    day += ::floor(t / msPerDay);
    day += DayFromMonth(month, leap);

    return day - 1;
}

static inline qsreal MakeDate(qsreal day, qsreal time)
{
    return day * msPerDay + time;
}

static inline qsreal DaylightSavingTA(double t)
{
#ifndef Q_WS_WIN
    long int tt = (long int)(t / msPerSecond);
    struct tm *tmtm = localtime((const time_t*)&tt);
    if (! tmtm)
        return 0;
    return (tmtm->tm_isdst > 0) ? msPerHour : 0;
#else
    Q_UNUSED(t);
    /// ### implement me
    return 0;
#endif
}

static inline qsreal LocalTime(qsreal t)
{
    return t + LocalTZA + DaylightSavingTA(t);
}

static inline qsreal UTC(qsreal t)
{
    return t - LocalTZA - DaylightSavingTA(t - LocalTZA);
}

static inline qsreal TimeClip(qsreal t)
{
    if (! qIsFinite(t) || fabs(t) > 8.64e15)
        return qSNaN();
    return ToInteger(t);
}

static qsreal getLocalTZA()
{
#ifndef Q_WS_WIN
    struct tm* t;
    time_t curr;
    time(&curr);
    t = localtime(&curr);
    time_t locl = mktime(t);
    t = gmtime(&curr);
    time_t globl = mktime(t);
    return double(locl - globl) * 1000.0;
#else
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);
    return -tzInfo.Bias * 60.0 * 1000.0;
#endif
}

/*!
  \internal

  Converts the QDateTime \a dt to an ECMA Date value (in UTC form).
*/
qsreal FromDateTime(const QDateTime &dt)
{
    if (!dt.isValid())
        return qSNaN();
    if (!LocalTZA) // ### move
        LocalTZA = getLocalTZA();
    QDate date = dt.date();
    QTime taim = dt.time();
    int year = date.year();
    int month = date.month() - 1;
    int day = date.day();
    int hours = taim.hour();
    int mins = taim.minute();
    int secs = taim.second();
    int ms = taim.msec();
    double t = MakeDate(MakeDay(year, month, day),
                        MakeTime(hours, mins, secs, ms));
    if (dt.timeSpec() == Qt::LocalTime)
        t = UTC(t);
    return TimeClip(t);
}

/*!
  \internal

  Converts the ECMA Date value \tt (in UTC form) to QDateTime
  according to \a spec.
*/
QDateTime ToDateTime(qsreal t, Qt::TimeSpec spec)
{
    if (qIsNaN(t))
        return QDateTime();
    if (!LocalTZA) // ### move
        LocalTZA = getLocalTZA();
    if (spec == Qt::LocalTime)
        t = LocalTime(t);
    int year = int(YearFromTime(t));
    int month = int(MonthFromTime(t) + 1);
    int day = int(DateFromTime(t));
    int hours = HourFromTime(t);
    int mins = MinFromTime(t);
    int secs = SecFromTime(t);
    int ms = msFromTime(t);
    return QDateTime(QDate(year, month, day), QTime(hours, mins, secs, ms), spec);
}

} // namespace QScript

QT_END_NAMESPACE
