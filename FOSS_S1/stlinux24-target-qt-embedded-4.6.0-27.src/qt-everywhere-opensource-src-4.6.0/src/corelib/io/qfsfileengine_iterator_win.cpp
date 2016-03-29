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

#include "qfsfileengine_iterator_p.h"
#include "qfsfileengine_p.h"
#include "qplatformdefs.h"

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QFSFileEngineIteratorPlatformSpecificData
{
public:
    inline QFSFileEngineIteratorPlatformSpecificData()
        : uncShareIndex(-1), findFileHandle(INVALID_HANDLE_VALUE),
          done(false), uncFallback(false)
    {}

    QFSFileEngineIterator *it;

    QStringList uncShares;
    int uncShareIndex;

    HANDLE findFileHandle;
    WIN32_FIND_DATA findData;
    bool done;
    bool uncFallback;

    void saveCurrentFileName();
};

void QFSFileEngineIteratorPlatformSpecificData::saveCurrentFileName()
{
    if (uncFallback) {
        // Windows share / UNC path
        it->currentEntry = uncShares.at(uncShareIndex - 1);
    } else {
        // Local directory
        it->currentEntry = QString::fromWCharArray(findData.cFileName);
    }
}

void QFSFileEngineIterator::advance()
{
    platform->saveCurrentFileName();

    if (platform->done)
        return;

    if (platform->uncFallback) {
        ++platform->uncShareIndex;
    } else if (platform->findFileHandle != INVALID_HANDLE_VALUE) {
        if (!FindNextFile(platform->findFileHandle, &platform->findData)) {
            platform->done = true;
            FindClose(platform->findFileHandle);
        }
    }
}

void QFSFileEngineIterator::newPlatformSpecifics()
{
    platform = new QFSFileEngineIteratorPlatformSpecificData;
    platform->it = this;
}

void QFSFileEngineIterator::deletePlatformSpecifics()
{
    delete platform;
    platform = 0;
}

bool QFSFileEngineIterator::hasNext() const
{
    if (platform->done)
        return false;

    if (platform->uncFallback)
        return platform->uncShareIndex > 0 && platform->uncShareIndex <= platform->uncShares.size();

    if (platform->findFileHandle == INVALID_HANDLE_VALUE) {
        QString path = this->path();
        // Local directory
        if (path.endsWith(QLatin1String(".lnk")))
            path = QFileInfo(path).readLink();

        if (!path.endsWith(QLatin1Char('/')))
            path.append(QLatin1Char('/'));
        path.append(QLatin1String("*.*"));

        QString fileName = QFSFileEnginePrivate::longFileName(path);
        platform->findFileHandle = FindFirstFile((const wchar_t *)fileName.utf16(), &platform->findData);

        if (platform->findFileHandle == INVALID_HANDLE_VALUE) {
            if (path.startsWith(QLatin1String("//"))) {
                path = this->path();
                // UNC
                QStringList parts = QDir::toNativeSeparators(path).split(QLatin1Char('\\'), QString::SkipEmptyParts);

                if (parts.count() == 1 && QFSFileEnginePrivate::uncListSharesOnServer(QLatin1String("\\\\") + parts.at(0),
                                                                                      &platform->uncShares)) {
                    if (platform->uncShares.isEmpty()) {
                        platform->done = true;
                    } else {
                        platform->uncShareIndex = 1;
                    }
                    platform->uncFallback = true;
                } else {
                    platform->done = true;
                }
            } else {
                platform->done = true;
            }
        }

        if (!platform->done && (!platform->uncFallback || !platform->uncShares.isEmpty()))
            platform->saveCurrentFileName();
    }

    return !platform->done;
}

QT_END_NAMESPACE
