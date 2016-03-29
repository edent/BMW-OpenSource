/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qcore_unix_p.h"

#ifndef Q_OS_VXWORKS
#  include <sys/select.h>
#  include <sys/time.h>
#else
#  include <selectLib.h>
#endif

#include <stdlib.h>

#ifdef Q_OS_MAC
#include <mach/mach_time.h>
#endif

#if !defined(QT_NO_CLOCK_MONOTONIC)
# if defined(QT_BOOTSTRAPPED)
#  define QT_NO_CLOCK_MONOTONIC
# endif
#endif

QT_BEGIN_NAMESPACE

bool qt_gettime_is_monotonic()
{
#if (_POSIX_MONOTONIC_CLOCK-0 > 0) || defined(Q_OS_MAC)
    return true;
#else
    static int returnValue = 0;

    if (returnValue == 0) {
#  if (_POSIX_MONOTONIC_CLOCK-0 < 0)
        returnValue = -1;
#  elif (_POSIX_MONOTONIC_CLOCK == 0)
        // detect if the system support monotonic timers
        long x = sysconf(_SC_MONOTONIC_CLOCK);
        returnValue = (x >= 200112L) ? 1 : -1;
#  endif
    }

    return returnValue != -1;
#endif
}

timeval qt_gettime()
{
    timeval tv;
#if defined(Q_OS_MAC)
    static mach_timebase_info_data_t info = {0,0};
    if (info.denom == 0)
        mach_timebase_info(&info);

    uint64_t cpu_time = mach_absolute_time();
    uint64_t nsecs = cpu_time * (info.numer / info.denom);
    tv.tv_sec = nsecs / 1000000000ull;
    tv.tv_usec = (nsecs / 1000) - (tv.tv_sec * 1000000);
    return tv;
#elif (_POSIX_MONOTONIC_CLOCK-0 > 0)
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
    return tv;
#else
#  if !defined(QT_NO_CLOCK_MONOTONIC) && !defined(QT_BOOTSTRAPPED)
    if (qt_gettime_is_monotonic()) {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec / 1000;
        return tv;
    }
#  endif
    // use gettimeofday
    ::gettimeofday(&tv, 0);
    return tv;
#endif
}

static inline bool time_update(struct timeval *tv, const struct timeval &start,
                               const struct timeval &timeout)
{
    if (!qt_gettime_is_monotonic()) {
        // we cannot recalculate the timeout without a monotonic clock as the time may have changed
        return false;
    }

    // clock source is monotonic, so we can recalculate how much timeout is left
    struct timeval now = qt_gettime();
    *tv = timeout + start - now;
    return true;
}

int qt_safe_select(int nfds, fd_set *fdread, fd_set *fdwrite, fd_set *fdexcept,
                   const struct timeval *orig_timeout)
{
    if (!orig_timeout) {
        // no timeout -> block forever
        register int ret;
        EINTR_LOOP(ret, select(nfds, fdread, fdwrite, fdexcept, 0));
        return ret;
    }

    timeval start = qt_gettime();
    timeval timeout = *orig_timeout;

    // loop and recalculate the timeout as needed
    int ret;
    forever {
        ret = ::select(nfds, fdread, fdwrite, fdexcept, &timeout);
        if (ret != -1 || errno != EINTR)
            return ret;

        // recalculate the timeout
        if (!time_update(&timeout, start, *orig_timeout)) {
            // clock reset, fake timeout error
            return 0;
        }
    }
}

QT_END_NAMESPACE
