/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qimagepixmapcleanuphooks_p.h"
#include "private/qpixmapdata_p.h"
#include "private/qimage_p.h"


QT_BEGIN_NAMESPACE

// Legacy, single instance hooks: ### Qt 5: remove
typedef void (*_qt_pixmap_cleanup_hook)(int);
typedef void (*_qt_pixmap_cleanup_hook_64)(qint64);
typedef void (*_qt_image_cleanup_hook)(int);
Q_GUI_EXPORT _qt_pixmap_cleanup_hook qt_pixmap_cleanup_hook = 0;
Q_GUI_EXPORT _qt_pixmap_cleanup_hook_64 qt_pixmap_cleanup_hook_64 = 0;
Q_GUI_EXPORT _qt_image_cleanup_hook qt_image_cleanup_hook = 0;
Q_GUI_EXPORT _qt_image_cleanup_hook_64 qt_image_cleanup_hook_64 = 0;


QImagePixmapCleanupHooks* qt_image_and_pixmap_cleanup_hooks = 0;


QImagePixmapCleanupHooks::QImagePixmapCleanupHooks()
{
    qt_image_and_pixmap_cleanup_hooks = this;
}

QImagePixmapCleanupHooks *QImagePixmapCleanupHooks::instance()
{
    if (!qt_image_and_pixmap_cleanup_hooks)
        qt_image_and_pixmap_cleanup_hooks = new QImagePixmapCleanupHooks;
    return qt_image_and_pixmap_cleanup_hooks;
}

void QImagePixmapCleanupHooks::addPixmapModificationHook(_qt_pixmap_cleanup_hook_pm hook)
{
    pixmapModificationHooks.append(hook);
}

void QImagePixmapCleanupHooks::addPixmapDestructionHook(_qt_pixmap_cleanup_hook_pm hook)
{
    pixmapDestructionHooks.append(hook);
}


void QImagePixmapCleanupHooks::addImageHook(_qt_image_cleanup_hook_64 hook)
{
    imageHooks.append(hook);
}

void QImagePixmapCleanupHooks::removePixmapModificationHook(_qt_pixmap_cleanup_hook_pm hook)
{
    pixmapModificationHooks.removeAll(hook);
}

void QImagePixmapCleanupHooks::removePixmapDestructionHook(_qt_pixmap_cleanup_hook_pm hook)
{
    pixmapDestructionHooks.removeAll(hook);
}

void QImagePixmapCleanupHooks::removeImageHook(_qt_image_cleanup_hook_64 hook)
{
    imageHooks.removeAll(hook);
}


void QImagePixmapCleanupHooks::executePixmapModificationHooks(QPixmap* pm)
{
    Q_ASSERT(qt_image_and_pixmap_cleanup_hooks);
    for (int i = 0; i < qt_image_and_pixmap_cleanup_hooks->pixmapModificationHooks.count(); ++i)
        qt_image_and_pixmap_cleanup_hooks->pixmapModificationHooks[i](pm);

    if (qt_pixmap_cleanup_hook_64)
        qt_pixmap_cleanup_hook_64(pm->cacheKey());
}

void QImagePixmapCleanupHooks::executePixmapDestructionHooks(QPixmap* pm)
{
    Q_ASSERT(qt_image_and_pixmap_cleanup_hooks);
    for (int i = 0; i < qt_image_and_pixmap_cleanup_hooks->pixmapDestructionHooks.count(); ++i)
        qt_image_and_pixmap_cleanup_hooks->pixmapDestructionHooks[i](pm);

    if (qt_pixmap_cleanup_hook_64)
        qt_pixmap_cleanup_hook_64(pm->cacheKey());
}

void QImagePixmapCleanupHooks::executeImageHooks(qint64 key)
{
    Q_ASSERT(qt_image_and_pixmap_cleanup_hooks);
    for (int i = 0; i < qt_image_and_pixmap_cleanup_hooks->imageHooks.count(); ++i)
        qt_image_and_pixmap_cleanup_hooks->imageHooks[i](key);

    if (qt_image_cleanup_hook_64)
        qt_image_cleanup_hook_64(key);
}

void QImagePixmapCleanupHooks::enableCleanupHooks(const QPixmap &pixmap)
{
    enableCleanupHooks(const_cast<QPixmap &>(pixmap).data_ptr().data());
}

void QImagePixmapCleanupHooks::enableCleanupHooks(QPixmapData *pixmapData)
{
    pixmapData->is_cached = true;
}

void QImagePixmapCleanupHooks::enableCleanupHooks(const QImage &image)
{
    const_cast<QImage &>(image).data_ptr()->is_cached = true;
}

QT_END_NAMESPACE
