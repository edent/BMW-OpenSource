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

#ifndef QEGLPROPERTIES_P_H
#define QEGLPROPERTIES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QtOpenGL and QtOpenVG modules.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qvarlengtharray.h>
#include <QtGui/qimage.h>

QT_BEGIN_INCLUDE_NAMESPACE

#if defined(QT_GLES_EGL)
#include <GLES/egl.h>
#else
#include <EGL/egl.h>
#endif

#if defined(Q_WS_X11)
// If <EGL/egl.h> included <X11/Xlib.h>, then the global namespace
// may have been polluted with X #define's.  The following makes sure
// the X11 headers were included properly and then cleans things up.
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Bool
#undef Status
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef Type
#undef FontChange
#undef CursorShape
#endif

QT_END_INCLUDE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace QEgl {
    enum API
    {
        OpenGL,
        OpenVG
    };

    enum PixelFormatMatch
    {
        ExactPixelFormat,
        BestPixelFormat
    };
};

class QX11Info;
class QPaintDevice;

class Q_GUI_EXPORT QEglProperties
{
public:
    QEglProperties();
    QEglProperties(EGLConfig);
    QEglProperties(const QEglProperties& other) : props(other.props) {}
    ~QEglProperties() {}

    int value(int name) const;
    void setValue(int name, int value);
    bool removeValue(int name);
    bool isEmpty() const { return props[0] == EGL_NONE; }

    const int *properties() const { return props.constData(); }

    void setPixelFormat(QImage::Format pixelFormat);
#ifdef Q_WS_X11
    void setVisualFormat(const QX11Info *xinfo);
#endif
    void setRenderableType(QEgl::API api);

    void setPaintDeviceFormat(QPaintDevice *dev);

    bool reduceConfiguration();

    QString toString() const;

    static void dumpAllConfigs();

private:
    QVarLengthArray<int> props;
};

QT_END_NAMESPACE

#endif // QEGLPROPERTIES_P_H
