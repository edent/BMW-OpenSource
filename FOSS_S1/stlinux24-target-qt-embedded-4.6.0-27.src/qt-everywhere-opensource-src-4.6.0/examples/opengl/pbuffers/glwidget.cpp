/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "glwidget.h"
#include <math.h>

#include "cube.h"

#include <QGLPixelBuffer>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static GLfloat colorArray[][4] = {
    {0.243f, 0.423f, 0.125f, 1.0f},
    {0.176f, 0.31f, 0.09f, 1.0f},
    {0.4f, 0.69f, 0.212f, 1.0f},
    {0.317f, 0.553f, 0.161f, 1.0f}
};

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , geom(0)
    , cube(0)
{
    // create the pbuffer
    pbuffer = new QGLPixelBuffer(QSize(512, 512), format(), this);
    setWindowTitle(tr("OpenGL pbuffers"));
    initializeGeometry();
}

GLWidget::~GLWidget()
{
    pbuffer->releaseFromDynamicTexture();
    glDeleteTextures(1, &dynamicTexture);
    delete pbuffer;

    qDeleteAll(cubes);
    qDeleteAll(tiles);
    delete cube;
}

void GLWidget::initializeGL()
{
    initCommon();
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    initPbuffer();
    cube->startAnimation();
    connect(cube, SIGNAL(changed()), this, SLOT(update()));
    for (int i = 0; i < 3; ++i)
    {
        cubes[i]->startAnimation();
        connect(cubes[i], SIGNAL(changed()), this, SLOT(update()));
    }
}

void GLWidget::paintGL()
{
    pbuffer->makeCurrent();
    drawPbuffer();
    // On direct render platforms, drawing onto the pbuffer context above
    // automatically updates the dynamic texture.  For cases where rendering
    // directly to a texture is not supported, explicitly copy.
    if (!hasDynamicTextureUpdate)
        pbuffer->updateDynamicTexture(dynamicTexture);
    makeCurrent();

    // Use the pbuffer as a texture to render the scene
    glBindTexture(GL_TEXTURE_2D, dynamicTexture);

    // set up to render the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);

    // draw the background
    glPushMatrix();
    glScalef(aspect, 1.0f, 1.0f);
    for (int i = 0; i < tiles.count(); ++i)
        tiles[i]->draw();
    glPopMatrix();

    // draw the bouncing cubes
    for (int i = 0; i < cubes.count(); ++i)
        cubes[i]->draw();
}

void GLWidget::initializeGeometry()
{
    geom = new Geometry();
    CubeBuilder cBuilder(geom, 0.5);
    cBuilder.setColor(QColor(255, 255, 255, 212));
    // build the 3 bouncing, spinning cubes
    for (int i = 0; i < 3; ++i)
        cubes.append(cBuilder.newCube(QVector3D((float)(i-1), -1.5f, 5 - i)));

    // build the spinning cube which goes in the dynamic texture
    cube = cBuilder.newCube();
    cube->removeBounce();

    // build the background tiles
    TileBuilder tBuilder(geom);
    tBuilder.setColor(QColor(Qt::white));
    for (int c = -2; c <= +2; ++c)
        for (int r = -2; r <= +2; ++r)
            tiles.append(tBuilder.newTile(QVector3D(c, r, 0)));

    // graded backdrop for the pbuffer scene
    TileBuilder bBuilder(geom, 0.0f, 2.0f);
    bBuilder.setColor(QColor(102, 176, 54, 210));
    backdrop = bBuilder.newTile(QVector3D(0.0f, 0.0f, -1.5f));
    backdrop->setColors(colorArray);
}

void GLWidget::initCommon()
{
    qglClearColor(QColor(Qt::darkGray));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);

    geom->loadArrays();
}

void GLWidget::perspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glFrustumf(-aspect, +aspect, -1.0, +1.0, 4.0, 15.0);
#else
    glFrustum(-aspect, +aspect, -1.0, +1.0, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::orthographicProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glOrthof(-1.0, +1.0, -1.0, +1.0, -90.0, +90.0);
#else
    glOrtho(-1.0, +1.0, -1.0, +1.0, -90.0, +90.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (qreal)width / (qreal)(height ? height : 1);
    perspectiveProjection();
}

void GLWidget::drawPbuffer()
{
    orthographicProjection();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    backdrop->draw();
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glDisable(GL_CULL_FACE);
    cube->draw();
    glEnable(GL_CULL_FACE);

    glFlush();
}

void GLWidget::initPbuffer()
{
    pbuffer->makeCurrent();

    cubeTexture = bindTexture(QImage(":res/cubelogo.png"));

    initCommon();

    // generate a texture that has the same size/format as the pbuffer
    dynamicTexture = pbuffer->generateDynamicTexture();

    // bind the dynamic texture to the pbuffer - this is a no-op under X11
    hasDynamicTextureUpdate = pbuffer->bindToDynamicTexture(dynamicTexture);
    makeCurrent();
}

void GLWidget::setAnimationPaused(bool enable)
{
    cube->setAnimationPaused(enable);
    for (int i = 0; i < 3; ++i)
        cubes[i]->setAnimationPaused(enable);
}
