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

#include "qfilesystemwatcher.h"
#include "qfilesystemwatcher_symbian_p.h"
#include "qfileinfo.h"
#include "qdebug.h"
#include "private/qcore_symbian_p.h"
#include <QDir>

#ifndef QT_NO_FILESYSTEMWATCHER


QT_BEGIN_NAMESPACE

QNotifyChangeEvent::QNotifyChangeEvent(RFs &fs, const TDesC &file,
                                       QSymbianFileSystemWatcherEngine *e, bool aIsDir,
									   TInt aPriority) :
        CActive(aPriority),
        isDir(aIsDir),
        fsSession(fs),
        watchedPath(file),
        engine(e),
        failureCount(0)
{
    if (isDir) {
        fsSession.NotifyChange(ENotifyEntry, iStatus, file);
    } else {
        fsSession.NotifyChange(ENotifyAll, iStatus, file);
    }
    CActiveScheduler::Add(this);
    SetActive();
}

QNotifyChangeEvent::~QNotifyChangeEvent()
{
    Cancel();
}

void QNotifyChangeEvent::RunL()
{
    if(iStatus.Int() == KErrNone) {
        failureCount = 0;
    } else {
        qWarning("QNotifyChangeEvent::RunL() - Failed to order change notifications: %d", iStatus.Int());
        failureCount++;
    }

    // Re-request failed notification once, but if it won't start working,
    // we can't do much besides just not request any more notifications.
    if (failureCount < 2) {
        if (isDir) {
            fsSession.NotifyChange(ENotifyEntry, iStatus, watchedPath);
        } else {
            fsSession.NotifyChange(ENotifyAll, iStatus, watchedPath);
        }
        SetActive();

        if (!failureCount) {
            QT_TRYCATCH_LEAVING(engine->emitPathChanged(this));
        }
    }
}

void QNotifyChangeEvent::DoCancel()
{
    fsSession.NotifyChangeCancel(iStatus);
}

QSymbianFileSystemWatcherEngine::QSymbianFileSystemWatcherEngine() :
        errorCode(KErrNone), watcherStarted(false)
{
    moveToThread(this);
}

QSymbianFileSystemWatcherEngine::~QSymbianFileSystemWatcherEngine()
{
    stop();
}

QStringList QSymbianFileSystemWatcherEngine::addPaths(const QStringList &paths, QStringList *files,
        QStringList *directories)
{
    QMutexLocker locker(&mutex);
    QStringList p = paths;

    if (!startWatcher()) {
        qWarning("Could not start QSymbianFileSystemWatcherEngine thread");

        return p;
    }

    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo fi(path);
        if (!fi.exists())
            continue;

        bool isDir = fi.isDir();
        if (isDir) {
            if (directories->contains(path))
                continue;
        } else {
            if (files->contains(path))
                continue;
        }

        // Use absolute filepath as relative paths seem to have some issues.
        QString filePath = fi.absoluteFilePath();
        if (isDir && filePath.at(filePath.size() - 1) != QChar(L'/')) {
            filePath += QChar(L'/');
        }

        currentEvent = NULL;
        QMetaObject::invokeMethod(this,
                                  "addNativeListener",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, filePath));

        syncCondition.wait(&mutex);

        if (currentEvent) {
            currentEvent->isDir = isDir;

            activeObjectToPath.insert(currentEvent, path);
            it.remove();

            if (isDir)
                directories->append(path);
            else
                files->append(path);
        }
    }

    return p;
}

QStringList QSymbianFileSystemWatcherEngine::removePaths(const QStringList &paths,
                                                         QStringList *files,
                                                         QStringList *directories)
{
    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();

        currentEvent = activeObjectToPath.key(path);
        if (!currentEvent)
            continue;
        activeObjectToPath.remove(currentEvent);

        QMetaObject::invokeMethod(this,
                                  "removeNativeListener",
                                  Qt::QueuedConnection);

        syncCondition.wait(&mutex);

        it.remove();

        files->removeAll(path);
        directories->removeAll(path);
    }

    if (activeObjectToPath.size() == 0)
        stop();

    return p;
}

void QSymbianFileSystemWatcherEngine::emitPathChanged(QNotifyChangeEvent *e)
{
    QMutexLocker locker(&mutex);

    QString path = activeObjectToPath.value(e);
    QFileInfo fi(path);

    if (e->isDir)
        emit directoryChanged(path, !fi.exists());
    else
        emit fileChanged(path, !fi.exists());
}

void QSymbianFileSystemWatcherEngine::stop()
{
    QMetaObject::invokeMethod(this, "quit");
    wait();
}

// This method must be called inside mutex
bool QSymbianFileSystemWatcherEngine::startWatcher()
{
    bool retval = true;

    if (!watcherStarted) {
        setStackSize(0x5000);
        start();
        syncCondition.wait(&mutex);

        if (errorCode != KErrNone) {
            retval = false;
        } else {
            watcherStarted = true;
        }
    }
    return retval;
}


void QSymbianFileSystemWatcherEngine::run()
{
    // Initialize file session

    mutex.lock();
    syncCondition.wakeOne();
    mutex.unlock();

    if (errorCode == KErrNone) {
        exec();

        foreach(QNotifyChangeEvent *e, activeObjectToPath.keys()) {
            e->Cancel();
            delete e;
        }

        activeObjectToPath.clear();
        watcherStarted = false;
    }
}

void QSymbianFileSystemWatcherEngine::addNativeListener(const QString &directoryPath)
{
    QMutexLocker locker(&mutex);
    QString nativeDir(QDir::toNativeSeparators(directoryPath));
    TPtrC ptr(qt_QString2TPtrC(nativeDir));
    currentEvent = new QNotifyChangeEvent(qt_s60GetRFs(), ptr, this, directoryPath.endsWith(QChar(L'/'), Qt::CaseSensitive));
    syncCondition.wakeOne();
}

void QSymbianFileSystemWatcherEngine::removeNativeListener()
{
    QMutexLocker locker(&mutex);
    currentEvent->Cancel();
    delete currentEvent;
    currentEvent = NULL;
    syncCondition.wakeOne();
}


QT_END_NAMESPACE
#endif // QT_NO_FILESYSTEMWATCHER
