/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#include "qwindowsurface_vgegl_p.h"
#include "qpaintengine_vg_p.h"
#include "qpixmapdata_vg_p.h"
#include "qvg_p.h"

#if !defined(QT_NO_EGL)

QT_BEGIN_NAMESPACE

// Turn off "direct to window" rendering if EGL cannot support it.
#if !defined(EGL_RENDER_BUFFER) || !defined(EGL_SINGLE_BUFFER)
#if defined(QVG_DIRECT_TO_WINDOW)
#undef QVG_DIRECT_TO_WINDOW
#endif
#endif

// Determine if preserved window contents should be used.
#if !defined(EGL_SWAP_BEHAVIOR) || !defined(EGL_BUFFER_PRESERVED)
#if !defined(QVG_NO_PRESERVED_SWAP)
#define QVG_NO_PRESERVED_SWAP 1
#endif
#endif

VGImageFormat qt_vg_config_to_vg_format(QEglContext *context)
{
    return qt_vg_image_to_vg_format
        (qt_vg_config_to_image_format(context));
}

QImage::Format qt_vg_config_to_image_format(QEglContext *context)
{
    EGLint red = 0;
    EGLint green = 0;
    EGLint blue = 0;
    EGLint alpha = 0;
    context->configAttrib(EGL_RED_SIZE, &red);
    context->configAttrib(EGL_GREEN_SIZE, &green);
    context->configAttrib(EGL_BLUE_SIZE, &blue);
    context->configAttrib(EGL_ALPHA_SIZE, &alpha);
    QImage::Format argbFormat;
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
    EGLint type = 0;
    context->configAttrib(EGL_SURFACE_TYPE, &type);
    if ((type & EGL_VG_ALPHA_FORMAT_PRE_BIT) != 0)
        argbFormat = QImage::Format_ARGB32_Premultiplied;
    else
        argbFormat = QImage::Format_ARGB32;
#else
    argbFormat = QImage::Format_ARGB32;
#endif
    if (red == 8 && green == 8 && blue == 8 && alpha == 8)
        return argbFormat;
    else if (red == 8 && green == 8 && blue == 8 && alpha == 0)
        return QImage::Format_RGB32;
    else if (red == 5 && green == 6 && blue == 5 && alpha == 0)
        return QImage::Format_RGB16;
    else if (red == 4 && green == 4 && blue == 4 && alpha == 4)
        return QImage::Format_ARGB4444_Premultiplied;
    else
        return argbFormat;       // XXX
}

#if !defined(QVG_NO_SINGLE_CONTEXT)

class QVGSharedContext
{
public:
    QVGSharedContext();
    ~QVGSharedContext();

    QEglContext *context;
    int refCount;
    QVGPaintEngine *engine;
    EGLSurface surface;
};

QVGSharedContext::QVGSharedContext()
    : context(0)
    , refCount(0)
    , engine(0)
    , surface(EGL_NO_SURFACE)
{
}

QVGSharedContext::~QVGSharedContext()
{
    // Don't accidentally destroy the QEglContext if the reference
    // count falls to zero while deleting the paint engine.
    ++refCount;

    if (context)
        context->makeCurrent(qt_vg_shared_surface());
    delete engine;
    if (context)
        context->doneCurrent();
    if (context && surface != EGL_NO_SURFACE)
        context->destroySurface(surface);
    delete context;
}

Q_GLOBAL_STATIC(QVGSharedContext, sharedContext);

QVGPaintEngine *qt_vg_create_paint_engine(void)
{
    QVGSharedContext *shared = sharedContext();
    if (!shared->engine)
        shared->engine = new QVGPaintEngine();
    return shared->engine;
}

void qt_vg_destroy_paint_engine(QVGPaintEngine *engine)
{
    Q_UNUSED(engine);
}

#else

QVGPaintEngine *qt_vg_create_paint_engine(void)
{
    return new QVGPaintEngine();
}

void qt_vg_destroy_paint_engine(QVGPaintEngine *engine)
{
    delete engine;
}

#endif

#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT

static bool isPremultipliedContext(const QEglContext *context)
{
    EGLint value = 0;
    if (context->configAttrib(EGL_SURFACE_TYPE, &value))
        return (value & EGL_VG_ALPHA_FORMAT_PRE_BIT) != 0;
    else
        return false;
}

#endif

static QEglContext *createContext(QPaintDevice *device)
{
    QEglContext *context;

    // Create the context object and open the display.
    context = new QEglContext();
    context->setApi(QEgl::OpenVG);
    if (!context->openDisplay(device)) {
        delete context;
        return 0;
    }

    // Set the swap interval for the display.
    QByteArray interval = qgetenv("QT_VG_SWAP_INTERVAL");
    if (!interval.isEmpty())
        eglSwapInterval(context->display(), interval.toInt());
    else
        eglSwapInterval(context->display(), 1);

    // Choose an appropriate configuration for rendering into the device.
    QEglProperties configProps;
    configProps.setPaintDeviceFormat(device);
    int redSize = configProps.value(EGL_RED_SIZE);
    if (redSize == EGL_DONT_CARE || redSize == 0)
        configProps.setPixelFormat(QImage::Format_ARGB32);  // XXX
#ifndef QVG_SCISSOR_CLIP
    // If we are using the mask to clip, then explicitly request a mask.
    configProps.setValue(EGL_ALPHA_MASK_SIZE, 1);
#endif
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
    configProps.setValue(EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT |
                         EGL_VG_ALPHA_FORMAT_PRE_BIT);
    configProps.setRenderableType(QEgl::OpenVG);
    if (!context->chooseConfig(configProps)) {
        // Try again without the "pre" bit.
        configProps.setValue(EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT);
        if (!context->chooseConfig(configProps)) {
            delete context;
            return 0;
        }
    }
#else
    configProps.setValue(EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT);
    configProps.setRenderableType(QEgl::OpenVG);
    if (!context->chooseConfig(configProps)) {
        delete context;
        return 0;
    }
#endif

    // Construct a new EGL context for the selected configuration.
    if (!context->createContext()) {
        delete context;
        return 0;
    }

    return context;
}

#if !defined(QVG_NO_SINGLE_CONTEXT)

QEglContext *qt_vg_create_context(QPaintDevice *device)
{
    QVGSharedContext *shared = sharedContext();
    if (shared->context) {
        ++(shared->refCount);
        return shared->context;
    } else {
        shared->context = createContext(device);
        shared->refCount = 1;
        return shared->context;
    }
}

void qt_vg_destroy_context(QEglContext *context)
{
    QVGSharedContext *shared = sharedContext();
    if (shared->context != context) {
        // This is not the shared context.  Shouldn't happen!
        delete context;
    } else if (--(shared->refCount) <= 0) {
        shared->context->makeCurrent(qt_vg_shared_surface());
        delete shared->engine;
        shared->engine = 0;
        shared->context->doneCurrent();
        if (shared->surface != EGL_NO_SURFACE) {
            eglDestroySurface(shared->context->display(), shared->surface);
            shared->surface = EGL_NO_SURFACE;
        }
        delete shared->context;
        shared->context = 0;
    }
}

EGLSurface qt_vg_shared_surface(void)
{
    QVGSharedContext *shared = sharedContext();
    if (shared->surface == EGL_NO_SURFACE) {
        EGLint attribs[7];
        attribs[0] = EGL_WIDTH;
        attribs[1] = 16;
        attribs[2] = EGL_HEIGHT;
        attribs[3] = 16;
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
        if (isPremultipliedContext(shared->context)) {
            attribs[4] = EGL_VG_ALPHA_FORMAT;
            attribs[5] = EGL_VG_ALPHA_FORMAT_PRE;
            attribs[6] = EGL_NONE;
        } else
#endif
        {
            attribs[4] = EGL_NONE;
        }
        shared->surface = eglCreatePbufferSurface
            (shared->context->display(), shared->context->config(), attribs);
    }
    return shared->surface;
}

#else

QEglContext *qt_vg_create_context(QPaintDevice *device)
{
    return createContext(device);
}

void qt_vg_destroy_context(QEglContext *context)
{
    delete context;
}

EGLSurface qt_vg_shared_surface(void)
{
    return EGL_NO_SURFACE;
}

#endif

QVGEGLWindowSurfacePrivate::QVGEGLWindowSurfacePrivate(QWindowSurface *win)
{
    winSurface = win;
    engine = 0;
}

QVGEGLWindowSurfacePrivate::~QVGEGLWindowSurfacePrivate()
{
    // Destroy the paint engine if it hasn't been destroyed already.
    destroyPaintEngine();
}

QVGPaintEngine *QVGEGLWindowSurfacePrivate::paintEngine()
{
    if (!engine)
        engine = qt_vg_create_paint_engine();
    return engine;
}

VGImage QVGEGLWindowSurfacePrivate::surfaceImage() const
{
    return VG_INVALID_HANDLE;
}

void QVGEGLWindowSurfacePrivate::destroyPaintEngine()
{
    if (engine) {
        qt_vg_destroy_paint_engine(engine);
        engine = 0;
    }
}

QSize QVGEGLWindowSurfacePrivate::windowSurfaceSize(QWidget *widget) const
{
    Q_UNUSED(widget);

    QRect rect = winSurface->geometry();
    QSize newSize = rect.size();

#if defined(Q_WS_QWS)
    // Account for the widget mask, if any.
    if (widget && !widget->mask().isEmpty()) {
        const QRegion region = widget->mask()
                               & rect.translated(-widget->geometry().topLeft());
        newSize = region.boundingRect().size();
    }
#endif

    return newSize;
}

#if defined(QVG_VGIMAGE_BACKBUFFERS)

QVGEGLWindowSurfaceVGImage::QVGEGLWindowSurfaceVGImage(QWindowSurface *win)
    : QVGEGLWindowSurfacePrivate(win)
    , context(0)
    , backBuffer(VG_INVALID_HANDLE)
    , backBufferSurface(EGL_NO_SURFACE)
    , recreateBackBuffer(false)
    , isPaintingActive(false)
    , windowSurface(EGL_NO_SURFACE)
{
}

QVGEGLWindowSurfaceVGImage::~QVGEGLWindowSurfaceVGImage()
{
    destroyPaintEngine();
    if (context) {
        if (backBufferSurface != EGL_NO_SURFACE) {
            // We need a current context to be able to destroy the image.
            // We use the shared surface because the native window handle
            // associated with "windowSurface" may have been destroyed already.
            context->makeCurrent(qt_vg_shared_surface());
            context->destroySurface(backBufferSurface);
            vgDestroyImage(backBuffer);
            context->doneCurrent();
        }
        if (windowSurface != EGL_NO_SURFACE)
            context->destroySurface(windowSurface);
        qt_vg_destroy_context(context);
    }
}

QEglContext *QVGEGLWindowSurfaceVGImage::ensureContext(QWidget *widget)
{
    QSize newSize = windowSurfaceSize(widget);
    if (context && size != newSize) {
        // The surface size has changed, so we need to recreate
        // the back buffer.  Keep the same context and paint engine.
        size = newSize;
        if (isPaintingActive)
            context->doneCurrent();
        isPaintingActive = false;
        recreateBackBuffer = true;
    }
    if (!context) {
        // Create a new EGL context.  We create the surface in beginPaint().
        size = newSize;
        context = qt_vg_create_context(widget);
        if (!context)
            return 0;
        isPaintingActive = false;
    }
    return context;
}

void QVGEGLWindowSurfaceVGImage::beginPaint(QWidget *widget)
{
    QEglContext *context = ensureContext(widget);
    if (context) {
        if (recreateBackBuffer || backBufferSurface == EGL_NO_SURFACE) {
            // Create a VGImage object to act as the back buffer
            // for this window.  We have to create the VGImage with a
            // current context, so activate the main surface for the window.
            context->makeCurrent(mainSurface());
            recreateBackBuffer = false;
            if (backBufferSurface != EGL_NO_SURFACE) {
                eglDestroySurface(context->display(), backBufferSurface);
                backBufferSurface = EGL_NO_SURFACE;
            }
            if (backBuffer != VG_INVALID_HANDLE) {
                vgDestroyImage(backBuffer);
            }
            VGImageFormat format = qt_vg_config_to_vg_format(context);
            backBuffer = vgCreateImage
                (format, size.width(), size.height(),
                 VG_IMAGE_QUALITY_FASTER);
            if (backBuffer != VG_INVALID_HANDLE) {
                // Create an EGL surface for rendering into the VGImage.
                backBufferSurface = eglCreatePbufferFromClientBuffer
                    (context->display(), EGL_OPENVG_IMAGE,
                     (EGLClientBuffer)(backBuffer),
                     context->config(), NULL);
                if (backBufferSurface == EGL_NO_SURFACE) {
                    vgDestroyImage(backBuffer);
                    backBuffer = VG_INVALID_HANDLE;
                }
            }
        }
        if (backBufferSurface != EGL_NO_SURFACE)
            context->makeCurrent(backBufferSurface);
        else
            context->makeCurrent(mainSurface());
        isPaintingActive = true;
    }
}

void QVGEGLWindowSurfaceVGImage::endPaint
        (QWidget *widget, const QRegion& region, QImage *image)
{
    Q_UNUSED(region);
    Q_UNUSED(image);
    QEglContext *context = ensureContext(widget);
    if (context) {
        if (backBufferSurface != EGL_NO_SURFACE) {
            if (isPaintingActive)
                vgFlush();
            context->lazyDoneCurrent();
        }
        isPaintingActive = false;
    }
}

VGImage QVGEGLWindowSurfaceVGImage::surfaceImage() const
{
    return backBuffer;
}

EGLSurface QVGEGLWindowSurfaceVGImage::mainSurface() const
{
    if (windowSurface != EGL_NO_SURFACE)
        return windowSurface;
    else
        return qt_vg_shared_surface();
}

#endif // QVG_VGIMAGE_BACKBUFFERS

QVGEGLWindowSurfaceDirect::QVGEGLWindowSurfaceDirect(QWindowSurface *win)
    : QVGEGLWindowSurfacePrivate(win)
    , context(0)
    , isPaintingActive(false)
    , needToSwap(false)
    , windowSurface(EGL_NO_SURFACE)
{
}

QVGEGLWindowSurfaceDirect::~QVGEGLWindowSurfaceDirect()
{
    destroyPaintEngine();
    if (context) {
        if (windowSurface != EGL_NO_SURFACE)
            context->destroySurface(windowSurface);
        qt_vg_destroy_context(context);
    }
}

QEglContext *QVGEGLWindowSurfaceDirect::ensureContext(QWidget *widget)
{
    QSize newSize = windowSurfaceSize(widget);
    QEglProperties surfaceProps;

#if defined(QVG_RECREATE_ON_SIZE_CHANGE)
#if !defined(QVG_NO_SINGLE_CONTEXT)
    if (context && size != newSize) {
        // The surface size has changed, so we need to recreate it.
        // We can keep the same context and paint engine.
        size = newSize;
        if (isPaintingActive)
            context->doneCurrent();
        context->destroySurface(windowSurface);
#if defined(EGL_VG_ALPHA_FORMAT_PRE_BIT)
        if (isPremultipliedContext(context)) {
            surfaceProps.setValue
                (EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE);
        } else {
            surfaceProps.removeValue(EGL_VG_ALPHA_FORMAT);
        }
#endif
        windowSurface = context->createSurface(widget, &surfaceProps);
        isPaintingActive = false;
    }
#else
    if (context && size != newSize) {
        // The surface size has changed, so we need to recreate
        // the EGL context for the widget.  We also need to recreate
        // the surface's paint engine if context sharing is not
        // enabled because we cannot reuse the existing paint objects
        // in the new context.
        qt_vg_destroy_paint_engine(engine);
        engine = 0;
        context->destroySurface(windowSurface);
        qt_vg_destroy_context(context);
        context = 0;
        windowSurface = EGL_NO_SURFACE;
    }
#endif
#endif
    if (!context) {
        // Create a new EGL context and bind it to the widget surface.
        size = newSize;
        context = qt_vg_create_context(widget);
        if (!context)
            return 0;
        // We want a direct to window rendering surface if possible.
#if defined(QVG_DIRECT_TO_WINDOW)
        surfaceProps.setValue(EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER);
#endif
#if defined(EGL_VG_ALPHA_FORMAT_PRE_BIT)
        if (isPremultipliedContext(context)) {
            surfaceProps.setValue
                (EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE);
        } else {
            surfaceProps.removeValue(EGL_VG_ALPHA_FORMAT);
        }
#endif
        EGLSurface surface = context->createSurface(widget, &surfaceProps);
        if (surface == EGL_NO_SURFACE) {
            qt_vg_destroy_context(context);
            context = 0;
            return 0;
        }
        needToSwap = true;
#if defined(QVG_DIRECT_TO_WINDOW)
        // Did we get a direct to window rendering surface?
        EGLint buffer = 0;
        if (eglQueryContext(context->display(), context->context(),
                            EGL_RENDER_BUFFER, &buffer) &&
                buffer == EGL_SINGLE_BUFFER) {
            needToSwap = false;
        }
#endif
#if !defined(QVG_NO_PRESERVED_SWAP)
        // Try to force the surface back buffer to preserve its contents.
        if (needToSwap) {
            eglGetError();  // Clear error state first.
            eglSurfaceAttrib(context->display(), surface,
                             EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
            if (eglGetError() != EGL_SUCCESS) {
                qWarning("QVG: could not enable preserved swap");
            }
        }
#endif
        windowSurface = surface;
        isPaintingActive = false;
    }
    return context;
}

void QVGEGLWindowSurfaceDirect::beginPaint(QWidget *widget)
{
    QEglContext *context = ensureContext(widget);
    if (context) {
        context->makeCurrent(windowSurface);
        isPaintingActive = true;
    }
}

void QVGEGLWindowSurfaceDirect::endPaint
        (QWidget *widget, const QRegion& region, QImage *image)
{
    Q_UNUSED(region);
    Q_UNUSED(image);
    QEglContext *context = ensureContext(widget);
    if (context) {
        if (needToSwap) {
            if (!isPaintingActive)
                context->makeCurrent(windowSurface);
            context->swapBuffers(windowSurface);
            context->lazyDoneCurrent();
        } else if (isPaintingActive) {
            vgFlush();
            context->lazyDoneCurrent();
        }
        isPaintingActive = false;
    }
}

QT_END_NAMESPACE

#endif
