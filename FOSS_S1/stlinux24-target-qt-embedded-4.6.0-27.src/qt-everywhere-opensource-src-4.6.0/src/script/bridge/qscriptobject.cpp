/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "config.h"
#include "qscriptobject_p.h"
#include "private/qobject_p.h"

namespace JSC
{
//QT_USE_NAMESPACE
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScriptObject));
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScriptObjectPrototype));
}

QT_BEGIN_NAMESPACE

// masquerading as JSC::JSObject
const JSC::ClassInfo QScriptObject::info = { "Object", 0, 0, 0 };

QScriptObject::Data::~Data()
{
    delete delegate;
}

QScriptObject::QScriptObject(WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), d(0)
{
}

QScriptObject::~QScriptObject()
{
    delete d;
}

bool QScriptObject::getOwnPropertySlot(JSC::ExecState* exec,
                                       const JSC::Identifier& propertyName,
                                       JSC::PropertySlot& slot)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
    return d->delegate->getOwnPropertySlot(this, exec, propertyName, slot);
}

void QScriptObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                        JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (!d || !d->delegate) {
        JSC::JSObject::put(exec, propertyName, value, slot);
        return;
    }
    d->delegate->put(this, exec, propertyName, value, slot);
}

bool QScriptObject::deleteProperty(JSC::ExecState* exec,
                                   const JSC::Identifier& propertyName,
                                   bool checkDontDelete)
{
    if (!d || !d->delegate)
        return JSC::JSObject::deleteProperty(exec, propertyName, checkDontDelete);
    return d->delegate->deleteProperty(this, exec, propertyName, checkDontDelete);
}

bool QScriptObject::getPropertyAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                          unsigned& attributes) const
{
    if (!d || !d->delegate)
        return JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
    return d->delegate->getPropertyAttributes(this, exec, propertyName, attributes);
}

void QScriptObject::getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames,
                                        bool includeNonEnumerable)
{
    if (!d || !d->delegate) {
        JSC::JSObject::getOwnPropertyNames(exec, propertyNames, includeNonEnumerable);
        return;
    }
    d->delegate->getOwnPropertyNames(this, exec, propertyNames, includeNonEnumerable);
}

bool QScriptObject::compareToObject(JSC::ExecState* exec, JSC::JSObject *other)
{
    if (!d || !d->delegate) {
        return JSC::JSObject::compareToObject(exec, other);
    }
    return d->delegate->compareToObject(this, exec, other);
}

void QScriptObject::markChildren(JSC::MarkStack& markStack)
{
    if (!d)
        d = new Data();
    if (d->isMarking)
        return;
    QBoolBlocker markBlocker(d->isMarking, true);
    if (d && d->data)
        markStack.append(d->data);
    if (!d || !d->delegate) {
        JSC::JSObject::markChildren(markStack);
        return;
    }
    d->delegate->markChildren(this, markStack);
}

JSC::CallType QScriptObject::getCallData(JSC::CallData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getCallData(data);
    return d->delegate->getCallData(this, data);
}

JSC::ConstructType QScriptObject::getConstructData(JSC::ConstructData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getConstructData(data);
    return d->delegate->getConstructData(this, data);
}

bool QScriptObject::hasInstance(JSC::ExecState* exec, JSC::JSValue value, JSC::JSValue proto)
{
    if (!d || !d->delegate)
        return JSC::JSObject::hasInstance(exec, value, proto);
    return d->delegate->hasInstance(this, exec, value, proto);
}

QScriptObjectPrototype::QScriptObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure> structure,
                                               JSC::Structure* /*prototypeFunctionStructure*/)
    : QScriptObject(structure)
{
}

QScriptObjectDelegate::QScriptObjectDelegate()
{
}

QScriptObjectDelegate::~QScriptObjectDelegate()
{
}

bool QScriptObjectDelegate::getOwnPropertySlot(QScriptObject* object, JSC::ExecState* exec,
                                               const JSC::Identifier& propertyName,
                                               JSC::PropertySlot& slot)
{
    return object->JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

void QScriptObjectDelegate::put(QScriptObject* object, JSC::ExecState* exec,
                                const JSC::Identifier& propertyName,
                                JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    object->JSC::JSObject::put(exec, propertyName, value, slot);
}

bool QScriptObjectDelegate::deleteProperty(QScriptObject* object, JSC::ExecState* exec,
                                           const JSC::Identifier& propertyName,
                                           bool checkDontDelete)
{
    return object->JSC::JSObject::deleteProperty(exec, propertyName, checkDontDelete);
}

bool QScriptObjectDelegate::getPropertyAttributes(const QScriptObject* object,
                                                  JSC::ExecState* exec,
                                                  const JSC::Identifier& propertyName,
                                                  unsigned& attributes) const
{
    return object->JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
}

void QScriptObjectDelegate::getOwnPropertyNames(QScriptObject* object, JSC::ExecState* exec,
                                                JSC::PropertyNameArray& propertyNames,
                                                bool includeNonEnumerable)
{
    object->JSC::JSObject::getOwnPropertyNames(exec, propertyNames, includeNonEnumerable);
}

void QScriptObjectDelegate::markChildren(QScriptObject* object, JSC::MarkStack& markStack)
{
    // ### should this call the virtual function instead??
    object->JSC::JSObject::markChildren(markStack);
}

JSC::CallType QScriptObjectDelegate::getCallData(QScriptObject* object, JSC::CallData& data)
{
    return object->JSC::JSObject::getCallData(data);
}

JSC::ConstructType QScriptObjectDelegate::getConstructData(QScriptObject* object, JSC::ConstructData& data)
{
    return object->JSC::JSObject::getConstructData(data);
}

bool QScriptObjectDelegate::hasInstance(QScriptObject* object, JSC::ExecState* exec,
                                        JSC::JSValue value, JSC::JSValue proto)
{
    return object->JSC::JSObject::hasInstance(exec, value, proto);
}

bool QScriptObjectDelegate::compareToObject(QScriptObject* object, JSC::ExecState* exec, JSC::JSObject* o)
{
    return object->JSC::JSObject::compareToObject(exec, o);
}

QT_END_NAMESPACE
