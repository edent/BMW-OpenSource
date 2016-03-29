/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QGUARD_P_H
#define QGUARD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qglobal.h"

QT_BEGIN_NAMESPACE

class QObject;
template<class T>
class QGuard
{
    QObject *o;
    QGuard<QObject> *next;
    QGuard<QObject> **prev;
    friend void q_guard_addGuard(QGuard<QObject> *);
    friend void q_guard_removeGuard(QGuard<QObject> *);
    friend class QObjectPrivate;
public:
    inline QGuard();
    inline QGuard(T *);
    inline QGuard(const QGuard<T> &);
    inline virtual ~QGuard();

    inline QGuard<T> &operator=(const QGuard<T> &o);
    inline QGuard<T> &operator=(T *);
    
    inline bool isNull() const
        { return !o; }

    inline T* operator->() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }
    inline T& operator*() const
        { return *static_cast<T*>(const_cast<QObject*>(o)); }
    inline operator T*() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }
    inline T* data() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }

protected:
    virtual void objectDestroyed(T *) {}
};

QT_END_NAMESPACE

#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

inline void q_guard_addGuard(QGuard<QObject> *);
inline void q_guard_removeGuard(QGuard<QObject> *);

template<class T>
QGuard<T>::QGuard()
: o(0), next(0), prev(0)
{
}

template<class T>
QGuard<T>::QGuard(T *g)
: o(g), next(0), prev(0)
{
    if (o) q_guard_addGuard(reinterpret_cast<QGuard<QObject> *>(this));
}

template<class T>
QGuard<T>::QGuard(const QGuard<T> &g)
: o(g.o), next(0), prev(0)
{
    if (o) q_guard_addGuard(reinterpret_cast<QGuard<QObject> *>(this));
}

template<class T>
QGuard<T>::~QGuard()
{
    if (prev) q_guard_removeGuard(reinterpret_cast<QGuard<QObject> *>(this));
    o = 0;
}

template<class T>
QGuard<T> &QGuard<T>::operator=(const QGuard<T> &g)
{
    if (g.o != o) {
        if (prev) 
            q_guard_removeGuard(reinterpret_cast<QGuard<QObject> *>(this));
        o = g.o;
        if (o) q_guard_addGuard(reinterpret_cast<QGuard<QObject> *>(this));
    }
    return *this;
}

template<class T>
inline QGuard<T> &QGuard<T>::operator=(T *g)
{
    if (g != o) {
        if (prev) 
            q_guard_removeGuard(reinterpret_cast<QGuard<QObject> *>(this));
        o = g;
        if (o) q_guard_addGuard(reinterpret_cast<QGuard<QObject> *>(this));
    }
    return *this;
}

QT_END_NAMESPACE

#endif // QGUARD_P_H
