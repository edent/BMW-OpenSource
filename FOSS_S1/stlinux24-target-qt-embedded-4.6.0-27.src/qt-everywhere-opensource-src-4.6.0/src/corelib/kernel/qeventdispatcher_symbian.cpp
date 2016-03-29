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

#include "qeventdispatcher_symbian_p.h"
#include <private/qthread_p.h>
#include <qcoreapplication.h>
#include <private/qcoreapplication_p.h>
#include <qdatetime.h>

#include <unistd.h>
#include <errno.h>

QT_BEGIN_NAMESPACE

#define WAKE_UP_PRIORITY CActive::EPriorityStandard
#define TIMER_PRIORITY CActive::EPriorityHigh
#define NULLTIMER_PRIORITY CActive::EPriorityLow
#define COMPLETE_DEFERRED_ACTIVE_OBJECTS_PRIORITY CActive::EPriorityIdle

static inline int qt_pipe_write(int socket, const char *data, qint64 len)
{
	return ::write(socket, data, len);
}
#if defined(write)
# undef write
#endif

static inline int qt_pipe_close(int socket)
{
	return ::close(socket);
}
#if defined(close)
# undef close
#endif

static inline int qt_pipe_fcntl(int socket, int command)
{
	return ::fcntl(socket, command);
}
static inline int qt_pipe2_fcntl(int socket, int command, int option)
{
	return ::fcntl(socket, command, option);
}
#if defined(fcntl)
# undef fcntl
#endif

static inline int qt_socket_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return ::select(nfds, readfds, writefds, exceptfds, timeout);
}

// This simply interrupts the select and locks the mutex until destroyed.
class QSelectMutexGrabber
{
public:
    QSelectMutexGrabber(int fd, QMutex *mutex)
        : m_mutex(mutex)
    {
        if (m_mutex->tryLock())
            return;

        char dummy = 0;
        qt_pipe_write(fd, &dummy, 1);

        m_mutex->lock();
    }

    ~QSelectMutexGrabber()
    {
        m_mutex->unlock();
    }

private:
    QMutex *m_mutex;
};

/*
 * This class is designed to aid in implementing event handling in a more round robin fashion. We
 * cannot change active objects that we do not own, but the active objects that Qt owns will use
 * this as a base class with convenience functions.
 *
 * Here is how it works: On every RunL, the deriving class should call okToRun(). This will allow
 * exactly one run of the active object, and mark it as such. If it is called again, it will return
 * false, and add the object to a queue so it can be run later.
 *
 * The QCompleteDeferredAOs class is a special object that runs after all others, which will
 * reactivate the objects that were previously not run.
 */
inline QActiveObject::QActiveObject(TInt priority, QEventDispatcherSymbian *dispatcher)
    : CActive(priority),
      m_dispatcher(dispatcher),
      m_hasAlreadyRun(false),
      m_hasRunAgain(false),
      m_iterationCount(1)
{
}

QActiveObject::~QActiveObject()
{
    if (m_hasRunAgain)
        m_dispatcher->removeDeferredActiveObject(this);
}

bool QActiveObject::okToRun()
{
    Q_ASSERT(!m_hasRunAgain);

    if (!m_hasAlreadyRun || m_dispatcher->iterationCount() != m_iterationCount) {
        // First occurrence of this event in this iteration.
        m_hasAlreadyRun = true;
        m_iterationCount = m_dispatcher->iterationCount();
        return true;
    } else {
        // The event has already occurred.
        m_dispatcher->addDeferredActiveObject(this);
        m_hasRunAgain = true;
        return false;
    }
}

void QActiveObject::reactivateAndComplete()
{
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus *status = &iStatus;
    QEventDispatcherSymbian::RequestComplete(status, KErrNone);

    m_hasRunAgain = false;
    m_hasAlreadyRun = false;
}

QWakeUpActiveObject::QWakeUpActiveObject(QEventDispatcherSymbian *dispatcher)
    : CActive(WAKE_UP_PRIORITY),
      m_dispatcher(dispatcher)
{
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
}

QWakeUpActiveObject::~QWakeUpActiveObject()
{
    Cancel();
}

void QWakeUpActiveObject::DoCancel()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QWakeUpActiveObject::RunL()
{
    iStatus = KRequestPending;
    SetActive();
    QT_TRYCATCH_LEAVING(m_dispatcher->wakeUpWasCalled());
}

QTimerActiveObject::QTimerActiveObject(QEventDispatcherSymbian *dispatcher, SymbianTimerInfo *timerInfo)
    : QActiveObject((timerInfo->interval) ? TIMER_PRIORITY : NULLTIMER_PRIORITY , dispatcher),
      m_timerInfo(timerInfo)
{
}

QTimerActiveObject::~QTimerActiveObject()
{
    Cancel();
}

void QTimerActiveObject::DoCancel()
{
    if (m_timerInfo->interval > 0) {
        m_rTimer.Cancel();
        m_rTimer.Close();
    } else {
        if (iStatus.Int() == KRequestPending) {
            TRequestStatus *status = &iStatus;
            QEventDispatcherSymbian::RequestComplete(status, KErrNone);
        }
    }
}

void QTimerActiveObject::RunL()
{
    int error;
    QT_TRYCATCH_ERROR(error, Run());
    // All Symbian error codes are negative.
    if (error < 0) {
        CActiveScheduler::Current()->Error(error);  // stop and report here, as this timer will be deleted on scope exit
    }
}

#define MAX_SYMBIAN_TIMEOUT_MS 2000000
void QTimerActiveObject::StartTimer()
{
    if (m_timerInfo->msLeft > MAX_SYMBIAN_TIMEOUT_MS) {
        //There is loss of accuracy anyway due to needing to restart the timer every 33 minutes,
        //so the 1/64s res of After() is acceptable for these very long timers.
        m_rTimer.After(iStatus, MAX_SYMBIAN_TIMEOUT_MS * 1000);
        m_timerInfo->msLeft -= MAX_SYMBIAN_TIMEOUT_MS;
    } else {
        //HighRes gives the 1ms accuracy expected by Qt, the +1 is to ensure that
        //"Timers will never time out earlier than the specified timeout value"
        //condition is always met.
        m_rTimer.HighRes(iStatus, (m_timerInfo->msLeft + 1) * 1000);
        m_timerInfo->msLeft = 0;
    }
    SetActive();
}

void QTimerActiveObject::Run()
{
    //restart timer immediately, if the timeout has been split because it overflows max for platform.
    if (m_timerInfo->msLeft > 0) {
        StartTimer();
        return;
    }

    if (!okToRun())
        return;

    if (m_timerInfo->interval > 0) {
        // Start a new timer immediately so that we don't lose time.
        m_timerInfo->msLeft = m_timerInfo->interval;
        StartTimer();

        m_timerInfo->dispatcher->timerFired(m_timerInfo->timerId);
    } else {
        // However, we only complete zero timers after the event has finished,
        // in order to prevent busy looping when doing nested loops.

        // Keep the refpointer around in order to avoid deletion until the end of this function.
        SymbianTimerInfoPtr timerInfoPtr(m_timerInfo);

        m_timerInfo->dispatcher->timerFired(m_timerInfo->timerId);

        iStatus = KRequestPending;
        SetActive();
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QTimerActiveObject::Start()
{
    CActiveScheduler::Add(this);
    m_timerInfo->msLeft = m_timerInfo->interval;
    if (m_timerInfo->interval > 0) {
        m_rTimer.CreateLocal();
        StartTimer();
    } else {
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

SymbianTimerInfo::SymbianTimerInfo()
    : timerAO(0)
{
}

SymbianTimerInfo::~SymbianTimerInfo()
{
    delete timerAO;
}

QCompleteDeferredAOs::QCompleteDeferredAOs(QEventDispatcherSymbian *dispatcher)
    : CActive(COMPLETE_DEFERRED_ACTIVE_OBJECTS_PRIORITY),
      m_dispatcher(dispatcher)
{
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
}

QCompleteDeferredAOs::~QCompleteDeferredAOs()
{
    Cancel();
}

void QCompleteDeferredAOs::complete()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QCompleteDeferredAOs::DoCancel()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QCompleteDeferredAOs::RunL()
{
    iStatus = KRequestPending;
    SetActive();

    QT_TRYCATCH_LEAVING(m_dispatcher->reactivateDeferredActiveObjects());
}

QSelectThread::QSelectThread()
    : m_quit(false)
{
    if (::pipe(m_pipeEnds) != 0) {
        qWarning("Select thread was unable to open a pipe, errno: %i", errno);
    } else {
        int flags0 = qt_pipe_fcntl(m_pipeEnds[0], F_GETFL);
        int flags1 = qt_pipe_fcntl(m_pipeEnds[1], F_GETFL);
        // We should check the error code here, but Open C has a bug that returns
        // failure even though the operation was successful.
        qt_pipe2_fcntl(m_pipeEnds[0], F_SETFL, flags0 | O_NONBLOCK);
        qt_pipe2_fcntl(m_pipeEnds[1], F_SETFL, flags1 | O_NONBLOCK);
    }
}

QSelectThread::~QSelectThread()
{
    qt_pipe_close(m_pipeEnds[1]);
    qt_pipe_close(m_pipeEnds[0]);
}

void QSelectThread::run()
{
    Q_D(QThread);

    m_mutex.lock();

    while (!m_quit) {
        fd_set readfds;
        fd_set writefds;
        fd_set exceptionfds;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptionfds);

        int maxfd = 0;
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Read, &readfds));
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Write, &writefds));
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Exception, &exceptionfds));
        maxfd = qMax(maxfd, m_pipeEnds[0]);
        maxfd++;

        FD_SET(m_pipeEnds[0], &readfds);

        int ret;
        int savedSelectErrno;
        ret = qt_socket_select(maxfd, &readfds, &writefds, &exceptionfds, 0);
        savedSelectErrno = errno;

        char buffer;

        while (::read(m_pipeEnds[0], &buffer, 1) > 0) {}

        if(ret == 0) {
         // do nothing
        } else if (ret < 0) {
            switch (savedSelectErrno) {
            case EBADF:
            case EINVAL:
            case ENOMEM:
            case EFAULT:
                qWarning("::select() returned an error: %i", savedSelectErrno);
                break;
            case ECONNREFUSED:
            case EPIPE:
                qWarning("::select() returned an error: %i (go through sockets)", savedSelectErrno);
                // prepare to go through all sockets
                // mark in fd sets both:
                //     good ones
                //     ones that return -1 in select
                // after loop update notifiers for all of them

                // as we dont have "exception" notifier type
                // we should force monitoring fd_set of this
                // type as well

                // clean @ start
                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                FD_ZERO(&exceptionfds);
                for (QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
                        i != m_AOStatuses.end(); ++i) {

                    fd_set onefds;
                    FD_ZERO(&onefds);
                    FD_SET(i.key()->socket(), &onefds);

                    fd_set excfds;
                    FD_ZERO(&excfds);
                    FD_SET(i.key()->socket(), &excfds);

                    maxfd = i.key()->socket() + 1;

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;

                    ret = 0;

                    if(i.key()->type() == QSocketNotifier::Read) {
                        ret = ::select(maxfd, &onefds, 0, &excfds, &timeout);
                        if(ret != 0) FD_SET(i.key()->socket(), &readfds);
                    } else if(i.key()->type() == QSocketNotifier::Write) {
                        ret = ::select(maxfd, 0, &onefds, &excfds, &timeout);
                        if(ret != 0) FD_SET(i.key()->socket(), &writefds);
                    }

                } // end for

                // traversed all, so update
                updateActivatedNotifiers(QSocketNotifier::Read, &readfds);
                updateActivatedNotifiers(QSocketNotifier::Write, &writefds);
                updateActivatedNotifiers(QSocketNotifier::Exception, &exceptionfds);

                break;
            case EINTR: // Should never occur on Symbian, but this is future proof!
            default:
                qWarning("::select() returned an unknown error: %i", savedSelectErrno);

                break;
            }
        } else {
            updateActivatedNotifiers(QSocketNotifier::Read, &readfds);
            updateActivatedNotifiers(QSocketNotifier::Write, &writefds);
            updateActivatedNotifiers(QSocketNotifier::Exception, &exceptionfds);
        }

        m_waitCond.wait(&m_mutex);
    }

    m_mutex.unlock();
}

void QSelectThread::requestSocketEvents ( QSocketNotifier *notifier, TRequestStatus *status )
{
    Q_D(QThread);

    if (!isRunning()) {
        start();
    }

    QSelectMutexGrabber lock(m_pipeEnds[1], &m_mutex);

    Q_ASSERT(!m_AOStatuses.contains(notifier));

    m_AOStatuses.insert(notifier, status);

    m_waitCond.wakeAll();
}

void QSelectThread::cancelSocketEvents ( QSocketNotifier *notifier )
{
    QSelectMutexGrabber lock(m_pipeEnds[1], &m_mutex);

    m_AOStatuses.remove(notifier);

    m_waitCond.wakeAll();
}

void QSelectThread::restart()
{
    QSelectMutexGrabber lock(m_pipeEnds[1], &m_mutex);

    m_waitCond.wakeAll();
}

int QSelectThread::updateSocketSet(QSocketNotifier::Type type, fd_set *fds)
{
    int maxfd = 0;
    if(m_AOStatuses.isEmpty()) {
        /*
         * Wonder if should return -1
         * to signal that no descriptors
         * added to fds
        */
        return maxfd;
    }
    for ( QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
            i != m_AOStatuses.end(); ++i) {
        if (i.key()->type() == type) {
            FD_SET(i.key()->socket(), fds);
            maxfd = qMax(maxfd, i.key()->socket());
        } else if(type == QSocketNotifier::Exception) {
            /*
             * We are registering existing sockets
             * always to exception set
             *
             * Doing double FD_SET shouldn't
             * matter
             */
            FD_SET(i.key()->socket(), fds);
            maxfd = qMax(maxfd, i.key()->socket());
        }
    }

    return maxfd;
}

void QSelectThread::updateActivatedNotifiers(QSocketNotifier::Type type, fd_set *fds)
{
    Q_D(QThread);
    if(m_AOStatuses.isEmpty()) {
        return;
    }
    QList<QSocketNotifier *> toRemove;
    for (QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
            i != m_AOStatuses.end(); ++i) {
        if (i.key()->type() == type && FD_ISSET(i.key()->socket(), fds)) {
            toRemove.append(i.key());
            TRequestStatus *status = i.value();
            // Thread data is still owned by the main thread.
            QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
        } else if(type == QSocketNotifier::Exception && FD_ISSET(i.key()->socket(), fds)) {
            /*
             * check if socket is in exception set
             * then signal RequestComplete for it
             */
            qWarning("exception on %d", i.key()->socket());
            toRemove.append(i.key());
            TRequestStatus *status = i.value();
            QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
        }
    }

    for (int c = 0; c < toRemove.size(); ++c) {
        m_AOStatuses.remove(toRemove[c]);
    }
}

void QSelectThread::stop()
{
    m_quit = true;
    restart();
    wait();
}

QSocketActiveObject::QSocketActiveObject(QEventDispatcherSymbian *dispatcher, QSocketNotifier *notifier)
    : QActiveObject(CActive::EPriorityStandard, dispatcher),
      m_notifier(notifier),
      m_inSocketEvent(false),
      m_deleteLater(false)
{
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
}

QSocketActiveObject::~QSocketActiveObject()
{
    Cancel();
}

void QSocketActiveObject::DoCancel()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QSocketActiveObject::RunL()
{
    if (!okToRun())
        return;

    QT_TRYCATCH_LEAVING(m_dispatcher->socketFired(this));
}

void QSocketActiveObject::deleteLater()
{
    if (m_inSocketEvent) {
        m_deleteLater = true;
    } else {
        delete this;
    }
}

QEventDispatcherSymbian::QEventDispatcherSymbian(QObject *parent)
    : QAbstractEventDispatcher(parent),
      m_activeScheduler(0),
      m_wakeUpAO(0),
      m_completeDeferredAOs(0),
      m_interrupt(false),
      m_wakeUpDone(0),
      m_iterationCount(0),
      m_noSocketEvents(false)
{
}

QEventDispatcherSymbian::~QEventDispatcherSymbian()
{
    m_processHandle.Close();
}

void QEventDispatcherSymbian::startingUp()
{
    if( !CActiveScheduler::Current() ) {
        m_activeScheduler = q_check_ptr(new CQtActiveScheduler());	// CBase derived class needs to be checked on new
        CActiveScheduler::Install(m_activeScheduler);
    }
    m_wakeUpAO = q_check_ptr(new QWakeUpActiveObject(this));
    m_completeDeferredAOs = q_check_ptr(new QCompleteDeferredAOs(this));
    // We already might have posted events, wakeup once to process them
    wakeUp();
}

void QEventDispatcherSymbian::closingDown()
{
    if (m_selectThread.isRunning()) {
        m_selectThread.stop();
    }

    delete m_completeDeferredAOs;
    delete m_wakeUpAO;
    if (m_activeScheduler) {
        delete m_activeScheduler;
    }
}

bool QEventDispatcherSymbian::processEvents ( QEventLoop::ProcessEventsFlags flags )
{
    bool handledAnyEvent = false;
    bool oldNoSocketEventsValue = m_noSocketEvents;

    QT_TRY {
        Q_D(QAbstractEventDispatcher);

        // It is safe if this counter overflows. The main importance is that each
        // iteration count is different from the last.
        m_iterationCount++;

        RThread &thread = d->threadData->symbian_thread_handle;

        bool block;
        if (flags & QEventLoop::WaitForMoreEvents) {
            block = true;
            emit aboutToBlock();
        } else {
            block = false;
        }

        if (flags & QEventLoop::ExcludeSocketNotifiers) {
            m_noSocketEvents = true;
        } else {
            m_noSocketEvents = false;
            handledAnyEvent = sendDeferredSocketEvents();
        }

        bool handledSymbianEvent = false;
        m_interrupt = false;

        /*
         * This QTime variable is used to measure the time it takes to finish
         * the event loop. If we take too long in the loop, other processes
         * may be starved and killed. After the first event has completed, we
         * take the current time, and if the remaining events take longer than
         * a preset time, we temporarily lower the priority to force a context
         * switch. For applications that do not take unecessarily long in the
         * event loop, the priority will not be altered.
         */
        QTime time;
        enum {
            FirstRun,
            SubsequentRun,
            TimeStarted
        } timeState = FirstRun;

        TProcessPriority priority;

        while (1) {
            if (block) {
                // This is where Qt will spend most of its time.
                CActiveScheduler::Current()->WaitForAnyRequest();
            } else {
                if (thread.RequestCount() == 0) {
                    break;
                }
                // This one should return without delay.
                CActiveScheduler::Current()->WaitForAnyRequest();
            }

            if (timeState == SubsequentRun) {
                time.start();
                timeState = TimeStarted;
            }

            TInt error;
            handledSymbianEvent = CActiveScheduler::RunIfReady(error, CActive::EPriorityIdle);
            if (error) {
                qWarning("CActiveScheduler::RunIfReady() returned error: %i\n", error);
                CActiveScheduler::Current()->Error(error);
            }

            if (!handledSymbianEvent) {
                qFatal("QEventDispatcherSymbian::processEvents(): Caught Symbian stray signal");
            }
            handledAnyEvent = true;
            if (m_interrupt) {
                break;
            }
            block = false;
            if (timeState == TimeStarted && time.elapsed() > 100) {
                priority = m_processHandle.Priority();
                m_processHandle.SetPriority(EPriorityBackground);
                time.start();
                // Slight chance of race condition in the next lines, but nothing fatal
                // will happen, just wrong priority.
                if (m_processHandle.Priority() == EPriorityBackground) {
                    m_processHandle.SetPriority(priority);
                }
            }
            if (timeState == FirstRun)
                timeState = SubsequentRun;
        };

        emit awake();
    } QT_CATCH (const std::exception& ex) {
#ifndef QT_NO_EXCEPTIONS
        CActiveScheduler::Current()->Error(qt_symbian_exception2Error(ex));
#endif
    }

    m_noSocketEvents = oldNoSocketEventsValue;

    return handledAnyEvent;
}

void QEventDispatcherSymbian::timerFired(int timerId)
{
    QHash<int, SymbianTimerInfoPtr>::iterator i = m_timerList.find(timerId);
    if (i == m_timerList.end()) {
        // The timer has been deleted. Ignore this event.
        return;
    }

    SymbianTimerInfoPtr timerInfo = *i;

    // Prevent infinite timer recursion.
    if (timerInfo->inTimerEvent) {
        return;
    }

    timerInfo->inTimerEvent = true;

    QTimerEvent event(timerInfo->timerId);
    QCoreApplication::sendEvent(timerInfo->receiver, &event);

    timerInfo->inTimerEvent = false;

    return;
}

void QEventDispatcherSymbian::socketFired(QSocketActiveObject *socketAO)
{
    if (m_noSocketEvents) {
        m_deferredSocketEvents.append(socketAO);
        return;
    }

    QEvent e(QEvent::SockAct);
    socketAO->m_inSocketEvent = true;
    QCoreApplication::sendEvent(socketAO->m_notifier, &e);
    socketAO->m_inSocketEvent = false;

    if (socketAO->m_deleteLater) {
        delete socketAO;
    } else {
        socketAO->iStatus = KRequestPending;
        socketAO->SetActive();
        reactivateSocketNotifier(socketAO->m_notifier);
    }
}

void QEventDispatcherSymbian::wakeUpWasCalled()
{
    // The reactivation should happen in RunL, right before the call to this function.
    // This is because m_wakeUpDone is the "signal" that the object can be completed
    // once more.
    // Also, by dispatching the posted events after resetting m_wakeUpDone, we guarantee
    // that no posted event notification will be lost. If we did it the other way
    // around, it would be possible for another thread to post an event right after
    // the sendPostedEvents was done, but before the object was ready to be completed
    // again. This could deadlock the application if there are no other posted events.
    m_wakeUpDone.fetchAndStoreOrdered(0);
    sendPostedEvents();
}

void QEventDispatcherSymbian::interrupt()
{
    m_interrupt = true;
    wakeUp();
}

void QEventDispatcherSymbian::wakeUp()
{
    Q_D(QAbstractEventDispatcher);

    if (m_wakeUpAO && m_wakeUpDone.testAndSetAcquire(0, 1)) {
        TRequestStatus *status = &m_wakeUpAO->iStatus;
        QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
    }
}

bool QEventDispatcherSymbian::sendPostedEvents()
{
    Q_D(QAbstractEventDispatcher);

    // moveToThread calls this and canWait == true -> Events will never get processed
    // if we check for d->threadData->canWait
    //
    // QCoreApplication::postEvent sets canWait = false, but after the object and events
    // are moved to a new thread, the canWait in new thread is true i.e. not changed to reflect
    // the flag on old thread. That's why events in a new thread will not get processed.
    // This migth be actually bug in moveToThread functionality, but because other platforms
    // do not check canWait in wakeUp (where we essentially are now) - decided to remove it from
    // here as well.

    //if (!d->threadData->canWait) {
        QCoreApplicationPrivate::sendPostedEvents(0, 0, d->threadData);
        return true;
    //}
    //return false;
}

inline void QEventDispatcherSymbian::addDeferredActiveObject(QActiveObject *object)
{
    if (m_deferredActiveObjects.isEmpty()) {
        m_completeDeferredAOs->complete();
    }
    m_deferredActiveObjects.append(object);
}

inline void QEventDispatcherSymbian::removeDeferredActiveObject(QActiveObject *object)
{
    m_deferredActiveObjects.removeAll(object);
}

void QEventDispatcherSymbian::reactivateDeferredActiveObjects()
{
    while (!m_deferredActiveObjects.isEmpty()) {
        QActiveObject *object = m_deferredActiveObjects.takeFirst();
        object->reactivateAndComplete();
    }

    // We do this because we want to return from processEvents. This is because
    // each invocation of processEvents should only run each active object once.
    // The active scheduler should run them continously, however.
    m_interrupt = true;
}

bool QEventDispatcherSymbian::sendDeferredSocketEvents()
{
    bool sentAnyEvents = false;
    while (!m_deferredSocketEvents.isEmpty()) {
        sentAnyEvents = true;
        socketFired(m_deferredSocketEvents.takeFirst());
    }

    return sentAnyEvents;
}

void QEventDispatcherSymbian::flush()
{
}

bool QEventDispatcherSymbian::hasPendingEvents()
{
    Q_D(QAbstractEventDispatcher);
    return (d->threadData->symbian_thread_handle.RequestCount() != 0
            || !d->threadData->canWait || !m_deferredSocketEvents.isEmpty());
}

void QEventDispatcherSymbian::registerSocketNotifier ( QSocketNotifier * notifier )
{
    QSocketActiveObject *socketAO = q_check_ptr(new QSocketActiveObject(this, notifier));
    m_notifiers.insert(notifier, socketAO);
    m_selectThread.requestSocketEvents(notifier, &socketAO->iStatus);
}

void QEventDispatcherSymbian::unregisterSocketNotifier ( QSocketNotifier * notifier )
{
    m_selectThread.cancelSocketEvents(notifier);
    if (m_notifiers.contains(notifier)) {
        QSocketActiveObject *sockObj = *m_notifiers.find(notifier);
        m_deferredSocketEvents.removeAll(sockObj);
        sockObj->deleteLater();
        m_notifiers.remove(notifier);
    }
}

void QEventDispatcherSymbian::reactivateSocketNotifier(QSocketNotifier *notifier)
{
    m_selectThread.requestSocketEvents(notifier, &m_notifiers[notifier]->iStatus);
}

void QEventDispatcherSymbian::registerTimer ( int timerId, int interval, QObject * object )
{
    if (interval < 0) {
        qWarning("Timer interval < 0");
        interval = 0;
    }

    SymbianTimerInfoPtr timer(new SymbianTimerInfo);
    timer->timerId      = timerId;
    timer->interval     = interval;
    timer->inTimerEvent = false;
    timer->receiver     = object;
    timer->dispatcher   = this;
    timer->timerAO      = q_check_ptr(new QTimerActiveObject(this, timer.data()));
    m_timerList.insert(timerId, timer);

    timer->timerAO->Start();
}

bool QEventDispatcherSymbian::unregisterTimer ( int timerId )
{
    if (!m_timerList.contains(timerId)) {
        return false;
    }

    SymbianTimerInfoPtr timerInfo = m_timerList.take(timerId);

    if (!QObjectPrivate::get(timerInfo->receiver)->inThreadChangeEvent)
        QAbstractEventDispatcherPrivate::releaseTimerId(timerId);

    return true;
}

bool QEventDispatcherSymbian::unregisterTimers ( QObject * object )
{
    if (m_timerList.isEmpty())
        return false;

    bool unregistered = false;
    for (QHash<int, SymbianTimerInfoPtr>::iterator i = m_timerList.begin(); i != m_timerList.end(); ) {
        if ((*i)->receiver == object) {
            i = m_timerList.erase(i);
            unregistered = true;
        } else {
            ++i;
        }
    }

    return unregistered;
}

QList<QEventDispatcherSymbian::TimerInfo> QEventDispatcherSymbian::registeredTimers ( QObject * object ) const
{
    QList<TimerInfo> list;
    for (QHash<int, SymbianTimerInfoPtr>::const_iterator i = m_timerList.begin(); i != m_timerList.end(); ++i) {
        if ((*i)->receiver == object) {
            list.push_back(TimerInfo((*i)->timerId, (*i)->interval));
        }
    }

    return list;
}

/*
 * This active scheduler class implements a simple report and continue policy, for Symbian OS leaves
 * or exceptions from Qt that fall back to the scheduler.
 * It will be used in cases where there is no existing active scheduler installed.
 * Apps which link to qts60main.lib will have the UI active scheduler installed in the main thread
 * instead of this one. But this would be used in other threads in the UI.
 * An app could replace this behaviour by installing an alternative active scheduler.
 */
void CQtActiveScheduler::Error(TInt aError) const
{
    QT_TRY {
        qWarning("Error from active scheduler %d", aError);
    }
    QT_CATCH (const std::bad_alloc&) {}    // ignore alloc fails, nothing more can be done
}

QT_END_NAMESPACE
