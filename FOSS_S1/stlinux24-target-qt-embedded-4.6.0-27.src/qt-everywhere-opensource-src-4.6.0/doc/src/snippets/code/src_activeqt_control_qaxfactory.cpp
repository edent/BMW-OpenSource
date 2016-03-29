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
QStringList ActiveQtFactory::featureList() const
{
    QStringList list;
    list << "ActiveX1";
    list << "ActiveX2";
    return list;
}

QObject *ActiveQtFactory::createObject(const QString &key)
{
    if (key == "ActiveX1")
        return new ActiveX1(parent);
    if (key == "ActiveX2")
        return new ActiveX2(parent);
    return 0;
}

const QMetaObject *ActiveQtFactory::metaObject(const QString &key) const
{
    if (key == "ActiveX1")
        return &ActiveX1::staticMetaObject;
    if (key == "ActiveX2")
        return &ActiveX2::staticMetaObject;
}

QUuid ActiveQtFactory::classID(const QString &key) const
{
    if (key == "ActiveX1")
        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
    ...
    return QUuid();
}

QUuid ActiveQtFactory::interfaceID(const QString &key) const
{
    if (key == "ActiveX1")
        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
    ...
    return QUuid();
}

QUuid ActiveQtFactory::eventsID(const QString &key) const
{
    if (key == "ActiveX1")
        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
    ...
    return QUuid();
}

QAXFACTORY_EXPORT(
    ActiveQtFactory,			      // factory class
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
//! [0]


//! [1]
QAXFACTORY_BEGIN(
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
    QAXCLASS(Class1)
    QAXCLASS(Class2)
QAXFACTORY_END()
//! [1]


//! [2]
#include <qapplication.h>
#include <qaxfactory.h>

#include "theactivex.h"

QAXFACTORY_DEFAULT(
    TheActiveX,				  // widget class
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // class ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // interface ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // event interface ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
//! [2]


//! [3]
settings->setValue("/CLSID/" + classID(key)
                   + "/Implemented Categories/"
                   + "/{00000000-0000-0000-000000000000}/.",
                   QString());
//! [3]


//! [4]
settings->remove("/CLSID/" + classID(key)
                 + "/Implemented Categories"
                 + "/{00000000-0000-0000-000000000000}/.");
//! [4]


//! [5]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!QAxFactory::isServer()) {
        // initialize for stand-alone execution
    }
    return app.exec();
}
//! [5]


//! [6]
if (QAxFactory::isServer()) {
    QAxFactory::stopServer();
    QAxFactory::startServer(QAxFactory::SingleInstance);
}
//! [6]


//! [7]
#include <qaxfactory.h>

#include "theactivex.h"

QAXFACTORY_DEFAULT(
    TheActiveX,				  // widget class
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // class ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // interface ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // event interface ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
//! [7]


//! [8]
QAXFACTORY_EXPORT(
    MyFactory,			              // factory class
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
//! [8]


//! [9]
QAXFACTORY_BEGIN(
    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
)
    QAXCLASS(Class1)
    QAXCLASS(Class2)
QAXFACTORY_END()
//! [9]
