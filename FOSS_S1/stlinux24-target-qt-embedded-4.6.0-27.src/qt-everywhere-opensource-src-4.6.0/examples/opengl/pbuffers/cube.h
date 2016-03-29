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

#ifndef CUBE_H
#define CUBE_H

#include <QtOpenGL/qgl.h>
#include <QtCore/qvector.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>

QT_BEGIN_NAMESPACE
class QPropertyAnimation;
QT_END_NAMESPACE

class Geometry
{
public:
    void loadArrays() const;
    void addQuad(const QVector3D &a, const QVector3D &b,
                 const QVector3D &c, const QVector3D &d,
                 const QVector<QVector2D> &tex);
    void setColors(int start, GLfloat colors[4][4]);
    const GLushort *indices() const { return faces.constData(); }
    int count() const { return faces.count(); }
private:
    QVector<GLushort> faces;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> texCoords;
    QVector<QVector4D> colors;
    int append(const QVector3D &a, const QVector3D &n, const QVector2D &t);
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n);
    friend class Tile;
};

class Tile
{
public:
    void draw() const;
    void setColors(GLfloat[4][4]);
protected:
    Tile(const QVector3D &loc = QVector3D());
    QVector3D location;
    QQuaternion orientation;
private:
    int start;
    int count;
    bool useFlatColor;
    GLfloat faceColor[4];
    Geometry *geom;
    friend class TileBuilder;
};

class TileBuilder
{
public:
    enum { bl, br, tr, tl };
    TileBuilder(Geometry *, qreal depth = 0.0f, qreal size = 1.0f);
    Tile *newTile(const QVector3D &loc = QVector3D()) const;
    void setColor(QColor c) { color = c; }
protected:
    void initialize(Tile *) const;
    QVector<QVector3D> verts;
    QVector<QVector2D> tex;
    int start;
    int count;
    Geometry *geom;
    QColor color;
};

class Cube : public QObject, public Tile
{
    Q_OBJECT
    Q_PROPERTY(qreal range READ range WRITE setRange)
    Q_PROPERTY(qreal altitude READ altitude WRITE setAltitude)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
public:
    Cube(const QVector3D &loc = QVector3D());
    ~Cube();
    qreal range() { return location.x(); }
    void setRange(qreal r);
    qreal altitude() { return location.y(); }
    void setAltitude(qreal a);
    qreal rotation() { return rot; }
    void setRotation(qreal r);
    void removeBounce();
    void startAnimation();
    void setAnimationPaused(bool paused);
signals:
    void changed();
private:
    qreal rot;
    QPropertyAnimation *r;
    QPropertyAnimation *a;
    QPropertyAnimation *rtn;
    qreal startx;
    friend class CubeBuilder;
};

class CubeBuilder : public TileBuilder
{
public:
    CubeBuilder(Geometry *, qreal depth = 0.0f, qreal size = 1.0f);
    Cube *newCube(const QVector3D &loc = QVector3D()) const;
private:
    mutable int ix;
};

#endif // CUBE_H
