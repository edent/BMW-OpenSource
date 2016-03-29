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
#include "qscriptglobalobject_p.h"

#include "../api/qscriptengine.h"
#include "../api/qscriptengine_p.h"

namespace JSC
{
QT_USE_NAMESPACE

ASSERT_CLASS_FITS_IN_CELL(QScript::GlobalObject);
ASSERT_CLASS_FITS_IN_CELL(QScript::OriginalGlobalObjectProxy);

} // namespace JSC

QT_BEGIN_NAMESPACE

namespace QScript
{

GlobalObject::GlobalObject()
    : JSC::JSGlobalObject(), customGlobalObject(0)
{
}

GlobalObject::~GlobalObject()
{
}

void GlobalObject::markChildren(JSC::MarkStack& markStack)
{
    JSC::JSGlobalObject::markChildren(markStack);
    if (customGlobalObject)
        markStack.append(customGlobalObject);
}

bool GlobalObject::getOwnPropertySlot(JSC::ExecState* exec,
                                      const JSC::Identifier& propertyName,
                                      JSC::PropertySlot& slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    if (propertyName == exec->propertyNames().arguments && engine->currentFrame->argumentCount() > 0) {
        JSC::JSValue args = engine->scriptValueToJSCValue(engine->contextForFrame(engine->currentFrame)->argumentsObject());
        slot.setValue(args);
        return true;
    }
    if (customGlobalObject)
        return customGlobalObject->getOwnPropertySlot(exec, propertyName, slot);
    return JSC::JSGlobalObject::getOwnPropertySlot(exec, propertyName, slot);
}

void GlobalObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                       JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (customGlobalObject)
        customGlobalObject->put(exec, propertyName, value, slot);
    else
        JSC::JSGlobalObject::put(exec, propertyName, value, slot);
}

void GlobalObject::putWithAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                     JSC::JSValue value, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->putWithAttributes(exec, propertyName, value, attributes);
    else
        JSC::JSGlobalObject::putWithAttributes(exec, propertyName, value, attributes);
}

bool GlobalObject::deleteProperty(JSC::ExecState* exec,
                                  const JSC::Identifier& propertyName, bool checkDontDelete)
{
    if (customGlobalObject)
        return customGlobalObject->deleteProperty(exec, propertyName, checkDontDelete);
    return JSC::JSGlobalObject::deleteProperty(exec, propertyName, checkDontDelete);
}

bool GlobalObject::getPropertyAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                         unsigned& attributes) const
{
    if (customGlobalObject)
        return customGlobalObject->getPropertyAttributes(exec, propertyName, attributes);
    return JSC::JSGlobalObject::getPropertyAttributes(exec, propertyName, attributes);
}

void GlobalObject::getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames,
                                       bool includeNonEnumerable)
{
    if (customGlobalObject)
        customGlobalObject->getOwnPropertyNames(exec, propertyNames, includeNonEnumerable);
    else
        JSC::JSGlobalObject::getOwnPropertyNames(exec, propertyNames, includeNonEnumerable);
}

void GlobalObject::defineGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->defineGetter(exec, propertyName, getterFunction, attributes);
    else
        JSC::JSGlobalObject::defineGetter(exec, propertyName, getterFunction, attributes);
}

void GlobalObject::defineSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->defineSetter(exec, propertyName, setterFunction, attributes);
    else
        JSC::JSGlobalObject::defineSetter(exec, propertyName, setterFunction, attributes);
}

JSC::JSValue GlobalObject::lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (customGlobalObject)
        return customGlobalObject->lookupGetter(exec, propertyName);
    return JSC::JSGlobalObject::lookupGetter(exec, propertyName);
}

JSC::JSValue GlobalObject::lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (customGlobalObject)
        return customGlobalObject->lookupSetter(exec, propertyName);
    return JSC::JSGlobalObject::lookupSetter(exec, propertyName);
}

} // namespace QScript

QT_END_NAMESPACE
