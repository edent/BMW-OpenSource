/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
QGLFormat fmt;
fmt.setAlpha(true);
fmt.setStereo(true);
QGLFormat::setDefaultFormat(fmt);
//! [0]


//! [1]
QGLFormat fmt;
fmt.setDoubleBuffer(false);                 // single buffer
fmt.setDirectRendering(false);              // software rendering
MyGLWidget* myWidget = new MyGLWidget(fmt, ...);
//! [1]


//! [2]
QGLFormat fmt;
fmt.setOverlay(true);
fmt.setStereo(true);
MyGLWidget* myWidget = new MyGLWidget(fmt, ...);
if (!myWidget->format().stereo()) {
    // ok, goggles off
    if (!myWidget->format().hasOverlay()) {
        qFatal("Cool hardware required");
    }
}
//! [2]


//! [3]
// The rendering in MyGLWidget depends on using
// stencil buffer and alpha channel
MyGLWidget::MyGLWidget(QWidget* parent)
    : QGLWidget(QGLFormat(QGL::StencilBuffer | QGL::AlphaChannel), parent)
{
    if (!format().stencil())
        qWarning("Could not get stencil buffer; results will be suboptimal");
    if (!format().alpha())
        qWarning("Could not get alpha channel; results will be suboptimal");
    ...
}
//! [3]


//! [4]
QApplication a(argc, argv);
QGLFormat f;
f.setDoubleBuffer(false);
QGLFormat::setDefaultFormat(f);
//! [4]


//! [5]
QGLFormat f = QGLFormat::defaultOverlayFormat();
f.setDoubleBuffer(true);
QGLFormat::setDefaultOverlayFormat(f);
//! [5]


//! [6]
// ...continued from above
MyGLWidget* myWidget = new MyGLWidget(QGLFormat(QGL::HasOverlay), ...);
if (myWidget->format().hasOverlay()) {
    // Yes, we got an overlay, let's check _its_ format:
    QGLContext* olContext = myWidget->overlayContext();
    if (olContext->format().doubleBuffer())
        ; // yes, we got a double buffered overlay
    else
        ; // no, only single buffered overlays are available
}
//! [6]


//! [7]
QGLContext *cx;
//  ...
QGLFormat f;
f.setStereo(true);
cx->setFormat(f);
if (!cx->create())
    exit(); // no OpenGL support, or cannot render on the specified paintdevice
if (!cx->format().stereo())
    exit(); // could not create stereo context
//! [7]


//! [8]
class MyGLDrawer : public QGLWidget
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    MyGLDrawer(QWidget *parent)
        : QGLWidget(parent) {}

protected:

    void initializeGL()
    {
        // Set up the rendering context, define display lists etc.:
        ...
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_DEPTH_TEST);
        ...
    }

    void resizeGL(int w, int h)
    {
        // setup viewport, projection etc.:
        glViewport(0, 0, (GLint)w, (GLint)h);
        ...
        glFrustum(...);
        ...
    }

    void paintGL()
    {
        // draw the scene:
        ...
        glRotatef(...);
        glMaterialfv(...);
        glBegin(GL_QUADS);
        glVertex3f(...);
        glVertex3f(...);
        ...
        glEnd();
        ...
    }

};
//! [8]
