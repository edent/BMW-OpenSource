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

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include <QtCore/qdebug.h>
#include "qegl_p.h"

QT_BEGIN_NAMESPACE

// Current GL and VG contexts.  These are used to determine if
// we can avoid an eglMakeCurrent() after a call to lazyDoneCurrent().
// If a background thread modifies the value, the worst that will
// happen is a redundant eglMakeCurrent() in the foreground thread.
static QEglContext * volatile currentGLContext = 0;
static QEglContext * volatile currentVGContext = 0;

QEglContext::QEglContext()
    : apiType(QEgl::OpenGL)
    , dpy(EGL_NO_DISPLAY)
    , ctx(EGL_NO_CONTEXT)
    , cfg(0)
    , currentSurface(EGL_NO_SURFACE)
    , current(false)
    , ownsContext(true)
    , sharing(false)
{
}

QEglContext::~QEglContext()
{
    destroy();

    if (currentGLContext == this)
        currentGLContext = 0;
    if (currentVGContext == this)
        currentVGContext = 0;
}

bool QEglContext::isValid() const
{
    return (ctx != EGL_NO_CONTEXT);
}

bool QEglContext::isCurrent() const
{
    return current;
}

// Open the EGL display associated with "device".
bool QEglContext::openDisplay(QPaintDevice *device)
{
    if (dpy == EGL_NO_DISPLAY)
        dpy = defaultDisplay(device);
    return (dpy != EGL_NO_DISPLAY);
}

// Choose a configuration that matches "properties".
bool QEglContext::chooseConfig
        (const QEglProperties& properties, QEgl::PixelFormatMatch match)
{
    QEglProperties props(properties);
    do {
        // Get the number of matching configurations for this set of properties.
        EGLint matching = 0;
        if (!eglChooseConfig(dpy, props.properties(), 0, 0, &matching) || !matching)
            continue;

        // If we want the best pixel format, then return the first
        // matching configuration.
        if (match == QEgl::BestPixelFormat) {
            eglChooseConfig(dpy, props.properties(), &cfg, 1, &matching);
            if (matching < 1)
                continue;
            return true;
        }

        // Fetch all of the matching configurations and find the
        // first that matches the pixel format we wanted.
        EGLint size = matching;
        EGLConfig *configs = new EGLConfig [size];
        eglChooseConfig(dpy, props.properties(), configs, size, &matching);
        for (EGLint index = 0; index < size; ++index) {
            EGLint red, green, blue, alpha;
            eglGetConfigAttrib(dpy, configs[index], EGL_RED_SIZE, &red);
            eglGetConfigAttrib(dpy, configs[index], EGL_GREEN_SIZE, &green);
            eglGetConfigAttrib(dpy, configs[index], EGL_BLUE_SIZE, &blue);
            eglGetConfigAttrib(dpy, configs[index], EGL_ALPHA_SIZE, &alpha);
            if (red == props.value(EGL_RED_SIZE) &&
                    green == props.value(EGL_GREEN_SIZE) &&
                    blue == props.value(EGL_BLUE_SIZE) &&
                    (props.value(EGL_ALPHA_SIZE) == 0 ||
                     alpha == props.value(EGL_ALPHA_SIZE))) {
                cfg = configs[index];
                delete [] configs;
                return true;
            }
        }
        delete [] configs;
    } while (props.reduceConfiguration());

#ifdef EGL_BIND_TO_TEXTURE_RGBA
    // Don't report an error just yet if we failed to get a pbuffer
    // configuration with texture rendering.  Only report failure if
    // we cannot get any pbuffer configurations at all.
    if (props.value(EGL_BIND_TO_TEXTURE_RGBA) == EGL_DONT_CARE &&
        props.value(EGL_BIND_TO_TEXTURE_RGB) == EGL_DONT_CARE)
#endif
    {
        qWarning() << "QEglContext::chooseConfig(): Could not find a suitable EGL configuration";
        qWarning() << "Requested:" << props.toString();
        qWarning() << "Available:";
        dumpAllConfigs();
    }
    return false;
}

// Create the EGLContext.
bool QEglContext::createContext(QEglContext *shareContext, const QEglProperties *properties)
{
    // We need to select the correct API before calling eglCreateContext().
#ifdef EGL_OPENGL_ES_API
    if (apiType == QEgl::OpenGL)
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
#ifdef EGL_OPENVG_API
    if (apiType == QEgl::OpenVG)
        eglBindAPI(EGL_OPENVG_API);
#endif

    // Create a new context for the configuration.
    QEglProperties contextProps;
    if (properties)
        contextProps = *properties;
#if defined(QT_OPENGL_ES_2)
    if (apiType == QEgl::OpenGL)
        contextProps.setValue(EGL_CONTEXT_CLIENT_VERSION, 2);
#endif
    sharing = false;
    if (shareContext && shareContext->ctx == EGL_NO_CONTEXT)
        shareContext = 0;
    if (shareContext) {
        ctx = eglCreateContext(dpy, cfg, shareContext->ctx, contextProps.properties());
        if (ctx == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Could not share context:" << errorString(eglGetError());
            shareContext = 0;
        } else {
            sharing = true;
        }
    }
    if (ctx == EGL_NO_CONTEXT) {
        ctx = eglCreateContext(dpy, cfg, 0, contextProps.properties());
        if (ctx == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Unable to create EGL context:" << errorString(eglGetError());
            return false;
        }
    }
    return true;
}

// Destroy an EGL surface object.  If it was current on this context
// then call doneCurrent() for it first.
void QEglContext::destroySurface(EGLSurface surface)
{
    if (surface != EGL_NO_SURFACE) {
        if (surface == currentSurface)
            doneCurrent();
        eglDestroySurface(dpy, surface);
    }
}

// Destroy the context.  Note: this does not destroy the surface.
void QEglContext::destroy()
{
    if (ctx != EGL_NO_CONTEXT && ownsContext)
        eglDestroyContext(dpy, ctx);
    dpy = EGL_NO_DISPLAY;
    ctx = EGL_NO_CONTEXT;
    cfg = 0;
}

bool QEglContext::makeCurrent(EGLSurface surface)
{
    if (ctx == EGL_NO_CONTEXT) {
        qWarning() << "QEglContext::makeCurrent(): Cannot make invalid context current";
        return false;
    }

    // If lazyDoneCurrent() was called on the surface, then we may be able
    // to assume that it is still current within the thread.
    if (surface == currentSurface && currentContext(apiType) == this) {
        current = true;
        return true;
    }

    current = true;
    currentSurface = surface;
    setCurrentContext(apiType, this);

    bool ok = eglMakeCurrent(dpy, surface, surface, ctx);
    if (!ok)
        qWarning() << "QEglContext::makeCurrent():" << errorString(eglGetError());
    return ok;
}

bool QEglContext::doneCurrent()
{
    // If the context is invalid, we assume that an error was reported
    // when makeCurrent() was called.
    if (ctx == EGL_NO_CONTEXT)
        return false;

    current = false;
    currentSurface = EGL_NO_SURFACE;
    setCurrentContext(apiType, 0);

    // We need to select the correct API before calling eglMakeCurrent()
    // with EGL_NO_CONTEXT because threads can have both OpenGL and OpenVG
    // contexts active at the same time.
#ifdef EGL_OPENGL_ES_API
    if (apiType == QEgl::OpenGL)
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
#ifdef EGL_OPENVG_API
    if (apiType == QEgl::OpenVG)
        eglBindAPI(EGL_OPENVG_API);
#endif

    bool ok = eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (!ok)
        qWarning() << "QEglContext::doneCurrent():" << errorString(eglGetError());
    return ok;
}

// Act as though doneCurrent() was called, but keep the context
// and the surface active for the moment.  This allows makeCurrent()
// to skip a call to eglMakeCurrent() if we are using the same
// surface as the last set of painting operations.  We leave the
// currentContext() pointer as-is for now.
bool QEglContext::lazyDoneCurrent()
{
    current = false;
    return true;
}

bool QEglContext::swapBuffers(EGLSurface surface)
{
    if(ctx == EGL_NO_CONTEXT)
        return false;

    bool ok = eglSwapBuffers(dpy, surface);
    if (!ok)
        qWarning() << "QEglContext::swapBuffers():" << errorString(eglGetError());
    return ok;
}

// Wait for native rendering operations to complete before starting
// to use OpenGL/OpenVG operations.
void QEglContext::waitNative()
{
#ifdef EGL_CORE_NATIVE_ENGINE
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
#endif
}

// Wait for client OpenGL/OpenVG operations to complete before
// using native rendering operations.
void QEglContext::waitClient()
{
#ifdef EGL_OPENGL_ES_API
    if (apiType == QEgl::OpenGL) {
        eglBindAPI(EGL_OPENGL_ES_API);
        eglWaitClient();
    }
#else
    if (apiType == QEgl::OpenGL)
        eglWaitGL();
#endif
#ifdef EGL_OPENVG_API
    if (apiType == QEgl::OpenVG) {
        eglBindAPI(EGL_OPENVG_API);
        eglWaitClient();
    }
#endif
}

// Query the value of a configuration attribute.
bool QEglContext::configAttrib(int name, EGLint *value) const
{
    return eglGetConfigAttrib(dpy, cfg, name, value);
}

// Retrieve all of the properties on "cfg".  If zero, return
// the context's configuration.
QEglProperties QEglContext::configProperties(EGLConfig cfg) const
{
    if (!cfg)
        cfg = config();
    QEglProperties props;
    for (int name = 0x3020; name <= 0x304F; ++name) {
        EGLint value;
        if (name != EGL_NONE && eglGetConfigAttrib(dpy, cfg, name, &value))
            props.setValue(name, value);
    }
    eglGetError();  // Clear the error state.
    return props;
}

// Initialize and return the default display.
EGLDisplay QEglContext::defaultDisplay(QPaintDevice *device)
{
    static EGLDisplay dpy = EGL_NO_DISPLAY;
    if (dpy == EGL_NO_DISPLAY) {
        dpy = getDisplay(device);
        if (dpy == EGL_NO_DISPLAY) {
            qWarning() << "QEglContext::defaultDisplay(): Cannot open EGL display";
            return EGL_NO_DISPLAY;
        }
        if (!eglInitialize(dpy, NULL, NULL)) {
            qWarning() << "QEglContext::defaultDisplay(): Cannot initialize EGL display:" << errorString(eglGetError());
            return EGL_NO_DISPLAY;
        }
#ifdef EGL_OPENGL_ES_API
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
    }
    return dpy;
}

// Return the error string associated with a specific code.
QString QEglContext::errorString(EGLint code)
{
    static const char * const errors[] = {
        "Success (0x3000)",                 // No tr
        "Not initialized (0x3001)",         // No tr
        "Bad access (0x3002)",              // No tr
        "Bad alloc (0x3003)",               // No tr
        "Bad attribute (0x3004)",           // No tr
        "Bad config (0x3005)",              // No tr
        "Bad context (0x3006)",             // No tr
        "Bad current surface (0x3007)",     // No tr
        "Bad display (0x3008)",             // No tr
        "Bad match (0x3009)",               // No tr
        "Bad native pixmap (0x300A)",       // No tr
        "Bad native window (0x300B)",       // No tr
        "Bad parameter (0x300C)",           // No tr
        "Bad surface (0x300D)",             // No tr
        "Context lost (0x300E)"             // No tr
    };
    if (code >= 0x3000 && code <= 0x300E) {
        return QString::fromLatin1(errors[code - 0x3000]);
    } else {
        return QLatin1String("0x") + QString::number(int(code), 16);
    }
}

// Dump all of the EGL configurations supported by the system.
void QEglContext::dumpAllConfigs()
{
    QEglProperties props;
    EGLint count = 0;
    if (!eglGetConfigs(dpy, 0, 0, &count) || count < 1)
        return;
    EGLConfig *configs = new EGLConfig [count];
    eglGetConfigs(dpy, configs, count, &count);
    for (EGLint index = 0; index < count; ++index) {
        props = configProperties(configs[index]);
        qWarning() << props.toString();
    }
    delete [] configs;
}

QString QEglContext::extensions()
{
    const char* exts = eglQueryString(QEglContext::defaultDisplay(0), EGL_EXTENSIONS);
    return QString(QLatin1String(exts));
}

bool QEglContext::hasExtension(const char* extensionName)
{
    return extensions().contains(QLatin1String(extensionName));
}

QEglContext *QEglContext::currentContext(QEgl::API api)
{
    if (api == QEgl::OpenGL)
        return currentGLContext;
    else
        return currentVGContext;
}

void QEglContext::setCurrentContext(QEgl::API api, QEglContext *context)
{
    if (api == QEgl::OpenGL)
        currentGLContext = context;
    else
        currentVGContext = context;
}

QT_END_NAMESPACE
