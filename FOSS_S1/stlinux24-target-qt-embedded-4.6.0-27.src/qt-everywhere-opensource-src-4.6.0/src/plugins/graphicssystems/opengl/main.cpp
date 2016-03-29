/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <private/qgraphicssystemplugin_p.h>
#include <private/qgraphicssystem_gl_p.h>
#include <qgl.h>

QT_BEGIN_NAMESPACE

class QGLGraphicsSystemPlugin : public QGraphicsSystemPlugin
{
public:
    QStringList keys() const;
    QGraphicsSystem *create(const QString&);
};

QStringList QGLGraphicsSystemPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("OpenGL") << QLatin1String("OpenGL1");
#if !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
    list << QLatin1String("OpenGL2");
#endif
    return list;
}

QGraphicsSystem* QGLGraphicsSystemPlugin::create(const QString& system)
{
    if (system.toLower() == QLatin1String("opengl1")) {
        QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
        return new QGLGraphicsSystem;
    }

#if !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
    if (system.toLower() == QLatin1String("opengl2")) {
        QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);
        return new QGLGraphicsSystem;
    }
#endif

    if (system.toLower() == QLatin1String("opengl"))
        return new QGLGraphicsSystem;

    return 0;
}

Q_EXPORT_PLUGIN2(opengl, QGLGraphicsSystemPlugin)

QT_END_NAMESPACE
