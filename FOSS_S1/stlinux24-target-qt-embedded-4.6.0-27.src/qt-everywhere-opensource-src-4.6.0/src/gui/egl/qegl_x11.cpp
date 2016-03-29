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

#include <QtCore/qdebug.h>

#include <private/qt_x11_p.h>
#include <QtGui/qx11info_x11.h>
#include <private/qpixmapdata_p.h>
#include <private/qpixmap_x11_p.h>

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include "qegl_p.h"


QT_BEGIN_NAMESPACE

EGLSurface QEglContext::createSurface(QPaintDevice *device, const QEglProperties *properties)
{
    // Create the native drawable for the paint device.
    int devType = device->devType();
    EGLNativePixmapType pixmapDrawable = 0;
    EGLNativeWindowType windowDrawable = 0;
    bool ok;
    if (devType == QInternal::Pixmap) {
        pixmapDrawable = (EGLNativePixmapType)(static_cast<QPixmap *>(device))->handle();
        ok = (pixmapDrawable != 0);
    } else if (devType == QInternal::Widget) {
        windowDrawable = (EGLNativeWindowType)(static_cast<QWidget *>(device))->winId();
        ok = (windowDrawable != 0);
    } else {
        ok = false;
    }
    if (!ok) {
        qWarning("QEglContext::createSurface(): Cannot create the native EGL drawable");
        return EGL_NO_SURFACE;
    }

    // Create the EGL surface to draw into, based on the native drawable.
    const int *props;
    if (properties)
        props = properties->properties();
    else
        props = 0;
    EGLSurface surf;
    if (devType == QInternal::Widget)
        surf = eglCreateWindowSurface(dpy, cfg, windowDrawable, props);
    else
        surf = eglCreatePixmapSurface(dpy, cfg, pixmapDrawable, props);
    if (surf == EGL_NO_SURFACE) {
        qWarning() << "QEglContext::createSurface(): Unable to create EGL surface:"
                   << errorString(eglGetError());
    }
    return surf;
}

EGLDisplay QEglContext::getDisplay(QPaintDevice *device)
{
    Q_UNUSED(device);
    Display *xdpy = QX11Info::display();
    if (!xdpy) {
        qWarning("QEglContext::getDisplay(): X11 display is not open");
        return EGL_NO_DISPLAY;
    }
    return eglGetDisplay(EGLNativeDisplayType(xdpy));
}

static int countBits(unsigned long mask)
{
    int count = 0;
    while (mask != 0) {
        if (mask & 1)
            ++count;
        mask >>= 1;
    }
    return count;
}

// Set the pixel format parameters from the visual in "xinfo".
void QEglProperties::setVisualFormat(const QX11Info *xinfo)
{
    if (!xinfo)
        return;
    Visual *visual = (Visual*)xinfo->visual();
    if (!visual)
        return;
    if (visual->c_class != TrueColor && visual->c_class != DirectColor)
        return;
    setValue(EGL_RED_SIZE, countBits(visual->red_mask));
    setValue(EGL_GREEN_SIZE, countBits(visual->green_mask));
    setValue(EGL_BLUE_SIZE, countBits(visual->blue_mask));

    EGLint alphaBits = 0;
#if !defined(QT_NO_XRENDER)
    XRenderPictFormat *format;
    format = XRenderFindVisualFormat(xinfo->display(), visual);
    if (format && (format->type == PictTypeDirect) && format->direct.alphaMask) {
        alphaBits = countBits(format->direct.alphaMask);
        qDebug("QEglProperties::setVisualFormat() - visual's alphaMask is %d", alphaBits);
    }
#endif
    setValue(EGL_ALPHA_SIZE, alphaBits);
}

extern const QX11Info *qt_x11Info(const QPaintDevice *pd);

// Set pixel format and other properties based on a paint device.
void QEglProperties::setPaintDeviceFormat(QPaintDevice *dev)
{
    if (!dev)
        return;
    if (dev->devType() == QInternal::Image)
        setPixelFormat(static_cast<QImage *>(dev)->format());
    else
        setVisualFormat(qt_x11Info(dev));
}

QT_END_NAMESPACE
