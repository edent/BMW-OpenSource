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

#include "domimage.h"

#include <QVariant>

#include <qscriptcontext.h>

QScriptValue DomImage::s_self;

DomImage::DomImage()
{
}


int DomImage::width() const
{
    return m_image.width();
}


int DomImage::height() const
{
    return m_image.height();
}


QString DomImage::src() const
{
    return m_src;
}

void DomImage::setSrc(const QString &src)
{
    m_src = src;
    m_image = QImage(m_src);
}


QString DomImage::name() const
{
    return m_src;
}

static QScriptValue Image(QScriptContext *context, QScriptEngine *env)
{
    QScriptValue val = context->thisObject();
    DomImage *image = new DomImage();
    QScriptValue klass = env->newVariant(qVariantFromValue(image));
    klass.setPrototype(DomImage::s_self);
    return klass;
}


static QScriptValue width(QScriptContext *context, QScriptEngine *)
{
    QScriptValue val = context->thisObject();

    DomImage *image = qvariant_cast<DomImage*> (val.toVariant());
    if (image)
        return image->width();

    return 0;
}


static QScriptValue height(QScriptContext *context, QScriptEngine *)
{
    QScriptValue val = context->thisObject();

    DomImage *image = qvariant_cast<DomImage*> (val.toVariant());
    if (image)
        return image->height();

    return 0;
}


static QScriptValue setSrc(QScriptContext *context, QScriptEngine *env)
{
    QScriptValue val = context->thisObject();
    QString src  = context->argument(0).toString();

    DomImage *image = qvariant_cast<DomImage*> (val.toVariant());
    if (image)
        image->setSrc(src);

    return env->undefinedValue();
}


static QScriptValue name(QScriptContext *context, QScriptEngine *)
{
    QScriptValue val = context->thisObject();

    DomImage *image = qvariant_cast<DomImage*> (val.toVariant());
    if (image)
        return image->name();

    return QString();
}


void DomImage::setup(QScriptEngine *e)
{
    qRegisterMetaType<DomImage>();

    e->globalObject().setProperty("Image",
                                  e->newFunction(::Image, 0));

    s_self = e->newObject();
    s_self.setProperty("setSrc", e->newFunction(&::setSrc, 1));
    s_self.setProperty("width", e->newFunction(&::width));
    s_self.setProperty("height", e->newFunction(&::height));
    s_self.setProperty("name", e->newFunction(&::name));

    e->setDefaultPrototype(qMetaTypeId<DomImage>(), s_self);
}
