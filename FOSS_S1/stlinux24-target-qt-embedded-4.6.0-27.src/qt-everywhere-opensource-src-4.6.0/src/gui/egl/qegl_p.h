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

#ifndef QEGL_P_H
#define QEGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience of
// the QtOpenGL and QtOpenVG modules.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qsize.h>
#include <QtGui/qimage.h>

#include <private/qeglproperties_p.h>

QT_BEGIN_INCLUDE_NAMESPACE

#if !defined(EGL_VERSION_1_3) && !defined(QEGL_NATIVE_TYPES_DEFINED)
#undef EGLNativeWindowType
#undef EGLNativePixmapType
#undef EGLNativeDisplayType
typedef NativeWindowType EGLNativeWindowType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeDisplayType EGLNativeDisplayType;
#define QEGL_NATIVE_TYPES_DEFINED 1
#endif
QT_END_INCLUDE_NAMESPACE

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QEglContext
{
public:
    QEglContext();
    ~QEglContext();

    bool isValid() const;
    bool isCurrent() const;
    bool isSharing() const { return sharing; }

    QEgl::API api() const { return apiType; }
    void setApi(QEgl::API api) { apiType = api; }

    bool openDisplay(QPaintDevice *device);
    bool chooseConfig(const QEglProperties& properties, QEgl::PixelFormatMatch match = QEgl::ExactPixelFormat);
    bool createContext(QEglContext *shareContext = 0, const QEglProperties *properties = 0);
    EGLSurface createSurface(QPaintDevice *device, const QEglProperties *properties = 0);
    void destroySurface(EGLSurface surface);

    void destroy();

    bool makeCurrent(EGLSurface surface);
    bool doneCurrent();
    bool lazyDoneCurrent();
    bool swapBuffers(EGLSurface surface);

    void waitNative();
    void waitClient();

    bool configAttrib(int name, EGLint *value) const;

    static void clearError() { eglGetError(); }
    static EGLint error() { return eglGetError(); }
    static QString errorString(EGLint code);

    EGLDisplay display() const { return dpy; }

    EGLContext context() const { return ctx; }
    void setContext(EGLContext context) { ctx = context; ownsContext = false;}

    EGLConfig config() const { return cfg; }
    void setConfig(EGLConfig config) { cfg = config; }

    QEglProperties configProperties(EGLConfig cfg = 0) const;

    static EGLDisplay defaultDisplay(QPaintDevice *device);

    void dumpAllConfigs();

    static QString extensions();
    static bool hasExtension(const char* extensionName);

private:
    QEgl::API apiType;
    EGLDisplay dpy;
    EGLContext ctx;
    EGLConfig cfg;
    EGLSurface currentSurface;
    bool current;
    bool ownsContext;
    bool sharing;

    static EGLDisplay getDisplay(QPaintDevice *device);

    static QEglContext *currentContext(QEgl::API api);
    static void setCurrentContext(QEgl::API api, QEglContext *context);
};

QT_END_NAMESPACE

#endif // QEGL_P_H
