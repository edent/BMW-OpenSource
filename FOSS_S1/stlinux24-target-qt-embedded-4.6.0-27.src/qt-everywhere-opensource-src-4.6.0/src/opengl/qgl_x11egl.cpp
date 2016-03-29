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

#include "qgl.h"
#include <private/qt_x11_p.h>
#include <private/qpixmap_x11_p.h>
#include <private/qimagepixmapcleanuphooks_p.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include "qgl_egl_p.h"
#include "qcolormap.h"
#include <QDebug>


QT_BEGIN_NAMESPACE

bool QGLFormat::hasOpenGLOverlays()
{
    return false;
}

void qt_egl_add_platform_config(QEglProperties& props, QPaintDevice *device)
{
    if (device->devType() == QInternal::Image)
        props.setPixelFormat(static_cast<QImage *>(device)->format());
}

bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);

    if (!device())
        return false;

    int devType = device()->devType();

    // Get the display and initialize it.
    d->eglContext = new QEglContext();
    d->eglContext->setApi(QEgl::OpenGL);
    if (!d->eglContext->openDisplay(device())) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

    // Construct the configuration we need for this surface.
    QEglProperties configProps;
    qt_egl_set_format(configProps, devType, d->glFormat);
    qt_egl_add_platform_config(configProps, device());
    configProps.setRenderableType(QEgl::OpenGL);

    QEgl::PixelFormatMatch matchType = QEgl::BestPixelFormat;
    if (device()->depth() == 16) {
        configProps.setValue(EGL_RED_SIZE, 5);
        configProps.setValue(EGL_GREEN_SIZE, 6);
        configProps.setValue(EGL_BLUE_SIZE, 5);
        configProps.setValue(EGL_ALPHA_SIZE, 0);
        matchType = QEgl::ExactPixelFormat;
    }
    configProps.setRenderableType(QEgl::OpenGL);

    // Search for a matching configuration, reducing the complexity
    // each time until we get something that matches.
    if (!d->eglContext->chooseConfig(configProps, matchType)) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

    // Inform the higher layers about the actual format properties.
    qt_egl_update_format(*(d->eglContext), d->glFormat);

    // Create a new context for the configuration.
    if (!d->eglContext->createContext
            (shareContext ? shareContext->d_func()->eglContext : 0)) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }
    d->sharing = d->eglContext->isSharing();
    if (d->sharing && shareContext)
        const_cast<QGLContext *>(shareContext)->d_func()->sharing = true;

#if defined(EGL_VERSION_1_1)
    if (d->glFormat.swapInterval() != -1 && devType == QInternal::Widget)
        eglSwapInterval(d->eglContext->display(), d->glFormat.swapInterval());
#endif

    return true;
}

void QGLWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QGLWidget);
    if (!isValid())
        return;
    makeCurrent();
    if (!d->glcx->initialized())
        glInit();
    resizeGL(width(), height());
    //handle overlay
}

const QGLContext* QGLWidget::overlayContext() const
{
    return 0;
}

void QGLWidget::makeOverlayCurrent()
{
    //handle overlay
}

void QGLWidget::updateOverlayGL()
{
    //handle overlay
}

bool qt_egl_setup_x11_visual(XVisualInfo &vi, EGLDisplay display, EGLConfig config, const QX11Info &x11Info, bool useArgbVisual)
{
    bool foundVisualIsArgb = useArgbVisual;

    memset(&vi, 0, sizeof(XVisualInfo));

    // Check to see if EGL is suggesting an appropriate visual id:
    EGLint nativeVisualId;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &nativeVisualId);
    vi.visualid = nativeVisualId;

    if (vi.visualid) {
        // EGL has suggested a visual id, so get the rest of the visual info for that id:
        XVisualInfo *chosenVisualInfo;
        int matchingCount = 0;
        chosenVisualInfo = XGetVisualInfo(x11Info.display(), VisualIDMask, &vi, &matchingCount);
        if (chosenVisualInfo) {
#if !defined(QT_NO_XRENDER)
            if (useArgbVisual) {
                // Check to make sure the visual provided by EGL is ARGB
                XRenderPictFormat *format;
                format = XRenderFindVisualFormat(x11Info.display(), chosenVisualInfo->visual);
                if (format->type == PictTypeDirect && format->direct.alphaMask) {
//                    qDebug("Using ARGB X Visual ID (%d) provided by EGL", (int)vi.visualid);
                    foundVisualIsArgb = true;
                    vi = *chosenVisualInfo;
                }
                else {
                    qWarning("Warning: EGL suggested using X visual ID %d for config %d, but this is not ARGB",
                             nativeVisualId, (int)config);
                    vi.visualid = 0;
                }
            } else
#endif
            {
//                qDebug("Using opaque X Visual ID (%d) provided by EGL", (int)vi.visualid);
                vi = *chosenVisualInfo;
            }
            XFree(chosenVisualInfo);
        }
        else {
            qWarning("Warning: EGL suggested using X visual ID %d for config %d, but this seems to be invalid!",
                     nativeVisualId, (int)config);
            vi.visualid = 0;
        }
    }

    // If EGL does not know the visual ID, so try to select an appropriate one ourselves, first
    // using XRender if we're supposed to have an alpha, then falling back to XGetVisualInfo
          
#if !defined(QT_NO_XRENDER)
    if (vi.visualid == 0 && useArgbVisual) {
        // Try to use XRender to find an ARGB visual we can use
        vi.screen  = x11Info.screen();
        vi.depth   = 32; //### We might at some point (soon) get ARGB4444
        vi.c_class = TrueColor;
        XVisualInfo *matchingVisuals;
        int matchingCount = 0;
        matchingVisuals = XGetVisualInfo(x11Info.display(),
                                         VisualScreenMask|VisualDepthMask|VisualClassMask,
                                         &vi, &matchingCount);

        for (int i = 0; i < matchingCount; ++i) {
            XRenderPictFormat *format;
            format = XRenderFindVisualFormat(x11Info.display(), matchingVisuals[i].visual);
            if (format->type == PictTypeDirect && format->direct.alphaMask) {
                vi = matchingVisuals[i];
                foundVisualIsArgb = true;
//                qDebug("Using X Visual ID (%d) for ARGB visual as provided by XRender", (int)vi.visualid);
                break;
            }
        }
        XFree(matchingVisuals);
    }
#endif

    if (vi.visualid == 0) {
        EGLint depth;
        eglGetConfigAttrib(display, config, EGL_BUFFER_SIZE, &depth);
        int err;
        err = XMatchVisualInfo(x11Info.display(), x11Info.screen(), depth, TrueColor, &vi);
        if (err == 0) {
            qWarning("Warning: Can't find an X visual which matches the EGL config(%d)'s depth (%d)!",
                     (int)config, depth);
            depth = x11Info.depth();
            err = XMatchVisualInfo(x11Info.display(), x11Info.screen(), depth, TrueColor, &vi);
            if (err == 0) {
                qWarning("Error: Couldn't get any matching X visual!");
                return false;
            } else
                qWarning("         - Falling back to X11 suggested depth (%d)", depth);
        }
//        else
//            qDebug("Using X Visual ID (%d) for EGL provided depth (%d)", (int)vi.visualid, depth);

        // Don't try to use ARGB now unless the visual is 32-bit - even then it might stil fail :-(
        if (useArgbVisual)
            foundVisualIsArgb = vi.depth == 32; //### We might at some point (soon) get ARGB4444
    }

//    qDebug("Visual Info:");
//    qDebug("   bits_per_rgb=%d", vi.bits_per_rgb);
//    qDebug("   red_mask=0x%x", vi.red_mask);
//    qDebug("   green_mask=0x%x", vi.green_mask);
//    qDebug("   blue_mask=0x%x", vi.blue_mask);
//    qDebug("   colormap_size=%d", vi.colormap_size);
//    qDebug("   c_class=%d", vi.c_class);
//    qDebug("   depth=%d", vi.depth);
//    qDebug("   screen=%d", vi.screen);
//    qDebug("   visualid=%d", vi.visualid);
    return foundVisualIsArgb;
}

void QGLWidget::setContext(QGLContext *context, const QGLContext* shareContext, bool deleteOldContext)
{
    Q_D(QGLWidget);
    if (context == 0) {
        qWarning("QGLWidget::setContext: Cannot set null context");
        return;
    }
    if (!context->deviceIsPixmap() && context->device() != this) {
        qWarning("QGLWidget::setContext: Context must refer to this widget");
        return;
    }

    if (d->glcx)
        d->glcx->doneCurrent();
    QGLContext* oldcx = d->glcx;
    d->glcx = context;

    if (parentWidget()) {
        // force creation of delay-created widgets
        parentWidget()->winId();
        if (parentWidget()->x11Info().screen() != x11Info().screen())
            d_func()->xinfo = parentWidget()->d_func()->xinfo;
    }

    // If the application has set WA_TranslucentBackground and not explicitly set
    // the alpha buffer size to zero, modify the format so it have an alpha channel
    QGLFormat& fmt = d->glcx->d_func()->glFormat;
    const bool tryArgbVisual = testAttribute(Qt::WA_TranslucentBackground);
    if (tryArgbVisual && fmt.alphaBufferSize() == -1)
        fmt.setAlphaBufferSize(1);

    bool createFailed = false;
    if (!d->glcx->isValid()) {
        if (!d->glcx->create(shareContext ? shareContext : oldcx))
            createFailed = true;
    }
    if (createFailed) {
        if (deleteOldContext)
            delete oldcx;
        return;
    }

    if (d->glcx->windowCreated() || d->glcx->deviceIsPixmap()) {
        if (deleteOldContext)
            delete oldcx;
        return;
    }

    bool visible = isVisible();
    if (visible)
        hide();

    XVisualInfo vi;
    QEglContext *eglContext = d->glcx->d_func()->eglContext;
    bool usingArgbVisual = qt_egl_setup_x11_visual(vi, eglContext->display(), eglContext->config(),
                                                   x11Info(), tryArgbVisual);

    XSetWindowAttributes a;

    Window p = RootWindow(x11Info().display(), x11Info().screen());
    if (parentWidget())
        p = parentWidget()->winId();

    QColormap colmap = QColormap::instance(vi.screen);
    a.background_pixel = colmap.pixel(palette().color(backgroundRole()));
    a.border_pixel = colmap.pixel(Qt::black);

    unsigned int valueMask = CWBackPixel|CWBorderPixel;
    if (usingArgbVisual) {
        a.colormap = XCreateColormap(x11Info().display(), p, vi.visual, AllocNone);
        valueMask |= CWColormap;
    }

    Window w = XCreateWindow(x11Info().display(), p, x(), y(), width(), height(),
                             0, vi.depth, InputOutput, vi.visual, valueMask, &a);

    if (deleteOldContext)
        delete oldcx;
    oldcx = 0;

    create(w); // Create with the ID of the window we've just created


    // Create the EGL surface to draw into.
    QGLContextPrivate *ctxpriv = d->glcx->d_func();
    ctxpriv->eglSurface = ctxpriv->eglContext->createSurface(this);
    if (ctxpriv->eglSurface == EGL_NO_SURFACE) {
        delete ctxpriv->eglContext;
        ctxpriv->eglContext = 0;
        return;
    }

    d->eglSurfaceWindowId = w; // Remember the window id we created the surface for

    if (visible)
        show();

    XFlush(X11->display);
    d->glcx->setWindowCreated(true);
}

void QGLWidgetPrivate::init(QGLContext *context, const QGLWidget* shareWidget)
{
    Q_Q(QGLWidget);

    initContext(context, shareWidget);

    if(q->isValid() && glcx->format().hasOverlay()) {
        //no overlay
        qWarning("QtOpenGL ES doesn't currently support overlays");
    }
}

void QGLWidgetPrivate::cleanupColormaps()
{
}

const QGLColormap & QGLWidget::colormap() const
{
    return d_func()->cmap;
}

void QGLWidget::setColormap(const QGLColormap &)
{
}

void QGLExtensions::init()
{
    static bool init_done = false;

    if (init_done)
        return;
    init_done = true;

    // We need a context current to initialize the extensions.
    QGLWidget tmpWidget;
    tmpWidget.makeCurrent();

    init_extensions();

    tmpWidget.doneCurrent();
}

// Re-creates the EGL surface if the window ID has changed or if force is true
void QGLWidgetPrivate::recreateEglSurface(bool force)
{
    Q_Q(QGLWidget);

    Window currentId = q->winId();

    if ( force || (currentId != eglSurfaceWindowId) ) {
        // The window id has changed so we need to re-create the EGL surface
        QEglContext *ctx = glcx->d_func()->eglContext;
        EGLSurface surface = glcx->d_func()->eglSurface;
        if (surface != EGL_NO_SURFACE)
            ctx->destroySurface(surface); // Will force doneCurrent() if nec.
        surface = ctx->createSurface(q);
        if (surface == EGL_NO_SURFACE)
            qWarning("Error creating EGL window surface: 0x%x", eglGetError());
        glcx->d_func()->eglSurface = surface;

        eglSurfaceWindowId = currentId;
    }
}

// Selects which configs should be used
EGLConfig Q_OPENGL_EXPORT qt_chooseEGLConfigForPixmap(bool hasAlpha, bool readOnly)
{
    // Cache the configs we select as they wont change:
    static EGLConfig roPixmapRGBConfig = 0;
    static EGLConfig roPixmapRGBAConfig = 0;
    static EGLConfig rwPixmapRGBConfig = 0;
    static EGLConfig rwPixmapRGBAConfig = 0;

    EGLConfig* targetConfig;

    if (hasAlpha) {
        if (readOnly)
            targetConfig = &roPixmapRGBAConfig;
        else
            targetConfig = &rwPixmapRGBAConfig;
    }
    else {
        if (readOnly)
            targetConfig = &roPixmapRGBConfig;
        else
            targetConfig = &rwPixmapRGBConfig;
    }

    if (*targetConfig == 0) {
        QEglProperties configAttribs;
        configAttribs.setValue(EGL_SURFACE_TYPE, EGL_PIXMAP_BIT);
        configAttribs.setRenderableType(QEgl::OpenGL);
        if (hasAlpha)
            configAttribs.setValue(EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE);
        else
            configAttribs.setValue(EGL_BIND_TO_TEXTURE_RGB, EGL_TRUE);

        // If this is going to be a render target, it needs to have a depth, stencil & sample buffer
        if (!readOnly) {
            configAttribs.setValue(EGL_DEPTH_SIZE, 1);
            configAttribs.setValue(EGL_STENCIL_SIZE, 1);
            configAttribs.setValue(EGL_SAMPLE_BUFFERS, 1);
        }

        EGLint configCount = 0;
        do {
            eglChooseConfig(QEglContext::defaultDisplay(0), configAttribs.properties(), targetConfig, 1, &configCount);
            if (configCount > 0) {
                // Got one
                qDebug() << "Found an" << (hasAlpha ? "ARGB" : "RGB") << (readOnly ? "readonly" : "target" )
                         << "config (" << int(*targetConfig) << ") to create a pixmap surface:";

//                QEglProperties configProps(*targetConfig);
//                qDebug() << configProps.toString();
                break;
            }
            qWarning("choosePixmapConfig() - No suitible config found, reducing requirements");
        } while (configAttribs.reduceConfiguration());
    }

    if (*targetConfig == 0)
        qWarning("choosePixmapConfig() - Couldn't find a suitable config");

    return *targetConfig;
}

bool Q_OPENGL_EXPORT qt_createEGLSurfaceForPixmap(QPixmapData* pmd, bool readOnly)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData* pixmapData = static_cast<QX11PixmapData*>(pmd);

    bool hasAlpha = pixmapData->hasAlphaChannel();

    EGLConfig pixmapConfig = qt_chooseEGLConfigForPixmap(hasAlpha, readOnly);

    QEglProperties pixmapAttribs;

    // If the pixmap can't be bound to a texture, it's pretty useless
    pixmapAttribs.setValue(EGL_TEXTURE_TARGET, EGL_TEXTURE_2D);
    if (hasAlpha)
        pixmapAttribs.setValue(EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA);
    else
        pixmapAttribs.setValue(EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB);

    EGLSurface pixmapSurface;
    pixmapSurface = eglCreatePixmapSurface(QEglContext::defaultDisplay(0),
                                           pixmapConfig,
                                           (EGLNativePixmapType) pixmapData->handle(),
                                           pixmapAttribs.properties());
//    qDebug("qt_createEGLSurfaceForPixmap() created surface 0x%x for pixmap 0x%x",
//           pixmapSurface, pixmapData->handle());
    if (pixmapSurface == EGL_NO_SURFACE) {
        qWarning() << "Failed to create a pixmap surface using config" << (int)pixmapConfig
                   << ":" << QEglContext::errorString(eglGetError());
        return false;
    }

    static bool doneOnce = false;
    if (!doneOnce) {
        // Make sure QGLTextureCache is instanciated so it can install cleanup hooks
        // which cleanup the EGL surface.
        QGLTextureCache::instance();
        doneOnce = true;
    }

    Q_ASSERT(sizeof(Qt::HANDLE) >= sizeof(EGLSurface)); // Just to make totally sure!
    pixmapData->gl_surface = (Qt::HANDLE)pixmapSurface;
    QImagePixmapCleanupHooks::enableCleanupHooks(pixmapData); // Make sure the cleanup hook gets called

    return true;
}


QGLTexture *QGLContextPrivate::bindTextureFromNativePixmap(QPixmapData* pd, const qint64 key,
                                                           QGLContext::BindOptions options)
{
    Q_Q(QGLContext);

    // The EGL texture_from_pixmap has no facility to invert the y coordinate
    if (!(options & QGLContext::CanFlipNativePixmapBindOption))
        return 0;

    Q_ASSERT(pd->classId() == QPixmapData::X11Class);

    static bool checkedForTFP = false;
    static bool haveTFP = false;

    if (!checkedForTFP) {
        // Check for texture_from_pixmap egl extension
        checkedForTFP = true;
        if (eglContext->hasExtension("EGL_NOKIA_texture_from_pixmap") ||
            eglContext->hasExtension("EGL_EXT_texture_from_pixmap"))
        {
            qDebug("Found texture_from_pixmap EGL extension!");
            haveTFP = true;
        }
    }

    if (!haveTFP)
        return 0;

    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pd);

    bool hasAlpha = pixmapData->hasAlphaChannel();

    // Check to see if the surface is still valid
    if (pixmapData->gl_surface &&
        hasAlpha != (pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha))
    {
        // Surface is invalid!
        destroyGlSurfaceForPixmap(pixmapData);
    }

    if (pixmapData->gl_surface == 0) {
        bool success = qt_createEGLSurfaceForPixmap(pixmapData, true);
        if (!success) {
            haveTFP = false;
            return 0;
        }
    }

    Q_ASSERT(pixmapData->gl_surface);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // bind the egl pixmap surface to a texture
    EGLBoolean success;
    success = eglBindTexImage(eglContext->display(), (EGLSurface)pixmapData->gl_surface, EGL_BACK_BUFFER);
    if (success == EGL_FALSE) {
        qWarning() << "eglBindTexImage() failed:" << eglContext->errorString(eglGetError());
        eglDestroySurface(eglContext->display(), (EGLSurface)pixmapData->gl_surface);
        pixmapData->gl_surface = (Qt::HANDLE)EGL_NO_SURFACE;
        haveTFP = false;
        return 0;
    }

    QGLTexture *texture = new QGLTexture(q, textureId, GL_TEXTURE_2D, options);
    pixmapData->flags |= QX11PixmapData::InvertedWhenBoundToTexture;

    // We assume the cost of bound pixmaps is zero
    QGLTextureCache::instance()->insert(q, key, texture, 0);

    glBindTexture(GL_TEXTURE_2D, textureId);
    return texture;
}

void QGLContextPrivate::destroyGlSurfaceForPixmap(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        EGLBoolean success;
        success = eglDestroySurface(QEglContext::defaultDisplay(0), (EGLSurface)pixmapData->gl_surface);
        if (success == EGL_FALSE) {
            qWarning() << "destroyGlSurfaceForPixmap() - Error deleting surface: "
                       << QEglContext::errorString(eglGetError());
        }
        pixmapData->gl_surface = 0;
    }
}

void QGLContextPrivate::unbindPixmapFromTexture(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        EGLBoolean success;
        success = eglReleaseTexImage(QEglContext::defaultDisplay(0),
                                     (EGLSurface)pixmapData->gl_surface,
                                     EGL_BACK_BUFFER);
        if (success == EGL_FALSE) {
            qWarning() << "unbindPixmapFromTexture() - Unable to release bound texture: "
                       << QEglContext::errorString(eglGetError());
        }
    }
}

QT_END_NAMESPACE
