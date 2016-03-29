/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <private/qgl_p.h>
#include <private/qegl_p.h>
#include <private/qeglproperties_p.h>
#include <private/qpaintengineex_opengl2_p.h>

#include "qpixmapdata_x11gl_p.h"

QT_BEGIN_NAMESPACE

extern EGLConfig qt_chooseEGLConfigForPixmap(bool hasAlpha, bool readOnly); // in qgl_x11egl.cpp
extern bool qt_createEGLSurfaceForPixmap(QPixmapData* pmd, bool readOnly); // in qgl_x11egl.cpp

// On 16bpp systems, RGB & ARGB pixmaps are different bit-depths and therefore need
// different contexts:
static EGLContext qPixmapARGBSharedEglContext = EGL_NO_CONTEXT;
static EGLContext qPixmapRGBSharedEglContext = EGL_NO_CONTEXT;

bool QX11GLPixmapData::hasX11GLPixmaps()
{
    static bool checkedForX11Pixmaps = false;
    static bool haveX11Pixmaps = false;

    if (checkedForX11Pixmaps)
        return haveX11Pixmaps;

    checkedForX11Pixmaps = true;

    QX11PixmapData *argbPixmapData = 0;
    QX11PixmapData *rgbPixmapData = 0;
    do {
        if (qgetenv("QT_USE_X11GL_PIXMAPS").isEmpty())
            break;

        // Check we actually have EGL configs which support pixmaps
        EGLConfig argbConfig = qt_chooseEGLConfigForPixmap(true, false);
        EGLConfig rgbConfig = qt_chooseEGLConfigForPixmap(false, false);

        if (argbConfig == 0 || rgbConfig == 0)
            break;

        // Create the shared contexts:
        eglBindAPI(EGL_OPENGL_ES_API);
        EGLint contextAttribs[] = {
#if defined(QT_OPENGL_ES_2)
            EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
            EGL_NONE
        };
        qPixmapARGBSharedEglContext = eglCreateContext(QEglContext::defaultDisplay(0),
                                                       argbConfig, 0, contextAttribs);

        if (argbConfig == rgbConfig) {
            // If the configs are the same, we can re-use the same context.
            qPixmapRGBSharedEglContext = qPixmapARGBSharedEglContext;
        } else {
            qPixmapRGBSharedEglContext = eglCreateContext(QEglContext::defaultDisplay(0),
                                                           rgbConfig, 0, contextAttribs);
        }

        argbPixmapData = new QX11PixmapData(QPixmapData::PixmapType);
        argbPixmapData->resize(100, 100);
        argbPixmapData->fill(Qt::transparent); // Force ARGB

        if (!qt_createEGLSurfaceForPixmap(argbPixmapData, false))
            break;

        haveX11Pixmaps = eglMakeCurrent(QEglContext::defaultDisplay(0),
                                        (EGLSurface)argbPixmapData->gl_surface,
                                        (EGLSurface)argbPixmapData->gl_surface,
                                        qPixmapARGBSharedEglContext);
        if (!haveX11Pixmaps) {
            EGLint err = eglGetError();
            qWarning() << "Unable to make pixmap config current:" << err << QEglContext::errorString(err);
            break;
        }

        // If the ARGB & RGB configs are the same, we don't need to check RGB too
        if (haveX11Pixmaps && (argbConfig != rgbConfig)) {
            rgbPixmapData = new QX11PixmapData(QPixmapData::PixmapType);
            rgbPixmapData->resize(100, 100);
            rgbPixmapData->fill(Qt::red);

            // Try to actually create an EGL pixmap surface
            if (!qt_createEGLSurfaceForPixmap(rgbPixmapData, false))
                break;

            haveX11Pixmaps = eglMakeCurrent(QEglContext::defaultDisplay(0),
                                            (EGLSurface)rgbPixmapData->gl_surface,
                                            (EGLSurface)rgbPixmapData->gl_surface,
                                            qPixmapRGBSharedEglContext);
            if (!haveX11Pixmaps) {
                EGLint err = eglGetError();
                qWarning() << "Unable to make pixmap config current:" << err << QEglContext::errorString(err);
                break;
            }
        }
    } while (0);

    if (qPixmapARGBSharedEglContext || qPixmapRGBSharedEglContext) {
        eglMakeCurrent(QEglContext::defaultDisplay(0),
                       EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    if (argbPixmapData) {
        if (argbPixmapData->gl_surface)
            QGLContextPrivate::destroyGlSurfaceForPixmap(argbPixmapData);
        delete argbPixmapData;
        argbPixmapData = 0;
    }
    if (rgbPixmapData) {
        if (rgbPixmapData->gl_surface)
            QGLContextPrivate::destroyGlSurfaceForPixmap(rgbPixmapData);
        delete rgbPixmapData;
        rgbPixmapData = 0;
    }

    if (!haveX11Pixmaps) {
        // Clean up the context(s) if we can't use X11GL pixmaps
        if (qPixmapARGBSharedEglContext != EGL_NO_CONTEXT)
            eglDestroyContext(QEglContext::defaultDisplay(0), qPixmapARGBSharedEglContext);

        if (qPixmapRGBSharedEglContext != qPixmapARGBSharedEglContext &&
            qPixmapRGBSharedEglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(QEglContext::defaultDisplay(0), qPixmapRGBSharedEglContext);
        }
        qPixmapRGBSharedEglContext = EGL_NO_CONTEXT;
        qPixmapARGBSharedEglContext = EGL_NO_CONTEXT;
    }

    if (haveX11Pixmaps)
        qDebug("QX11GLPixmapData is supported");
    else
        qDebug("QX11GLPixmapData is *NOT* being used");

    return haveX11Pixmaps;
}

QX11GLPixmapData::QX11GLPixmapData()
    : QX11PixmapData(QPixmapData::PixmapType),
      ctx(0)
{
}

QX11GLPixmapData::~QX11GLPixmapData()
{
}

static QGL2PaintEngineEx* qt_gl2_engine_for_pixmaps = 0;

QPaintEngine* QX11GLPixmapData::paintEngine() const
{
    // We need to create the context before beginPaint - do it here:
    if (!ctx) {
        ctx = new QGLContext(glFormat());
        if (ctx->d_func()->eglContext == 0)
            ctx->d_func()->eglContext = new QEglContext();
        ctx->d_func()->eglContext->openDisplay(0); // ;-)
        ctx->d_func()->eglContext->setApi(QEgl::OpenGL);
        ctx->d_func()->eglContext->setContext(hasAlphaChannel() ? qPixmapARGBSharedEglContext
                                                                : qPixmapRGBSharedEglContext);
    }

    if (!qt_gl2_engine_for_pixmaps)
        qt_gl2_engine_for_pixmaps = new QGL2PaintEngineEx();

    // Support multiple painters on multiple pixmaps simultaniously
    if (qt_gl2_engine_for_pixmaps->isActive()) {
        qWarning("Pixmap paint engine already active");
        QPaintEngine* engine = new QGL2PaintEngineEx();
        engine->setAutoDestruct(true);
        return engine;
    }

    return qt_gl2_engine_for_pixmaps;
}

void QX11GLPixmapData::beginPaint()
{
//    qDebug("QX11GLPixmapData::beginPaint()");
    if ((EGLSurface)gl_surface == EGL_NO_SURFACE) {
        qt_createEGLSurfaceForPixmap(this, false);
        ctx->d_func()->eglSurface = (EGLSurface)gl_surface;
        ctx->d_func()->valid = true; // ;-)
    }
    QGLPaintDevice::beginPaint();
}

void QX11GLPixmapData::endPaint()
{
    glFinish();
    QGLPaintDevice::endPaint();
}

QGLContext* QX11GLPixmapData::context() const
{
    return ctx;
}

QSize QX11GLPixmapData::size() const
{
    return QSize(w, h);
}


QGLFormat QX11GLPixmapData::glFormat()
{
    return QGLFormat::defaultFormat(); //###
}

QT_END_NAMESPACE
