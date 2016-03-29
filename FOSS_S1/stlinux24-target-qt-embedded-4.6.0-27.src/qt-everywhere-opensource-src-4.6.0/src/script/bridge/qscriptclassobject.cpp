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
#include "qscriptclassobject_p.h"

#include "../api/qscriptengine.h"
#include "../api/qscriptengine_p.h"
#include "../api/qscriptcontext.h"
#include "../api/qscriptcontext_p.h"
#include "../api/qscriptclass.h"
#include "../api/qscriptclasspropertyiterator.h"

#include "Error.h"
#include "PropertyNameArray.h"

Q_DECLARE_METATYPE(QScriptContext*)
Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QScriptValueList)

QT_BEGIN_NAMESPACE

namespace QScript
{

ClassObjectDelegate::ClassObjectDelegate(QScriptClass *scriptClass)
    : m_scriptClass(scriptClass)
{
}

ClassObjectDelegate::~ClassObjectDelegate()
{
}

QScriptObjectDelegate::Type ClassObjectDelegate::type() const
{
    return ClassObject;
}

bool ClassObjectDelegate::getOwnPropertySlot(QScriptObject* object,
                                             JSC::ExecState *exec,
                                             const JSC::Identifier &propertyName,
                                             JSC::PropertySlot &slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(engine, exec);
    // for compatibility with the old back-end, normal JS properties
    // are queried first.
    if (QScriptObjectDelegate::getOwnPropertySlot(object, exec, propertyName, slot))
        return true;

    QScriptValue scriptObject = engine->scriptValueFromJSCValue(object);
    QScriptString scriptName;
    QScriptStringPrivate scriptName_d(engine, propertyName, QScriptStringPrivate::StackAllocated);
    QScriptStringPrivate::init(scriptName, &scriptName_d);
    uint id = 0;
    QScriptClass::QueryFlags flags = m_scriptClass->queryProperty(
        scriptObject, scriptName, QScriptClass::HandlesReadAccess, &id);
    if (flags & QScriptClass::HandlesReadAccess) {
        QScriptValue value = m_scriptClass->property(scriptObject, scriptName, id);
        slot.setValue(engine->scriptValueToJSCValue(value));
        return true;
    }
    return false;
}

void ClassObjectDelegate::put(QScriptObject* object, JSC::ExecState *exec,
                              const JSC::Identifier &propertyName,
                              JSC::JSValue value, JSC::PutPropertySlot &slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(engine, exec);
    QScriptValue scriptObject = engine->scriptValueFromJSCValue(object);
    QScriptString scriptName;
    QScriptStringPrivate scriptName_d(engine, propertyName, QScriptStringPrivate::StackAllocated);
    QScriptStringPrivate::init(scriptName, &scriptName_d);
    uint id = 0;
    QScriptClass::QueryFlags flags = m_scriptClass->queryProperty(
        scriptObject, scriptName, QScriptClass::HandlesWriteAccess, &id);
    if (flags & QScriptClass::HandlesWriteAccess) {
        m_scriptClass->setProperty(scriptObject, scriptName, id, engine->scriptValueFromJSCValue(value));
        return;
    }
    QScriptObjectDelegate::put(object, exec, propertyName, value, slot);
}

bool ClassObjectDelegate::deleteProperty(QScriptObject* object, JSC::ExecState *exec,
                                         const JSC::Identifier &propertyName,
                                         bool checkDontDelete)
{
    // ### avoid duplication of put()
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(engine, exec);
    QScriptValue scriptObject = engine->scriptValueFromJSCValue(object);
    QScriptString scriptName;
    QScriptStringPrivate scriptName_d(engine, propertyName, QScriptStringPrivate::StackAllocated);
    QScriptStringPrivate::init(scriptName, &scriptName_d);
    uint id = 0;
    QScriptClass::QueryFlags flags = m_scriptClass->queryProperty(
        scriptObject, scriptName, QScriptClass::HandlesWriteAccess, &id);
    if (flags & QScriptClass::HandlesWriteAccess) {
        if (m_scriptClass->propertyFlags(scriptObject, scriptName, id) & QScriptValue::Undeletable)
            return false;
        m_scriptClass->setProperty(scriptObject, scriptName, id, QScriptValue());
        return true;
    }
    return QScriptObjectDelegate::deleteProperty(object, exec, propertyName, checkDontDelete);
}

bool ClassObjectDelegate::getPropertyAttributes(const QScriptObject* object, JSC::ExecState *exec,
                                                const JSC::Identifier &propertyName,
                                                unsigned &attribs) const
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(engine, exec);
    QScriptValue scriptObject = engine->scriptValueFromJSCValue(object);
    QScriptString scriptName;
    QScriptStringPrivate scriptName_d(engine, propertyName, QScriptStringPrivate::StackAllocated);
    QScriptStringPrivate::init(scriptName, &scriptName_d);
    uint id = 0;
    QScriptClass::QueryFlags flags = m_scriptClass->queryProperty(
        scriptObject, scriptName, QScriptClass::HandlesReadAccess, &id);
    if (flags & QScriptClass::HandlesReadAccess) {
        QScriptValue::PropertyFlags flags = m_scriptClass->propertyFlags(scriptObject, scriptName, id);
        attribs = 0;
        if (flags & QScriptValue::ReadOnly)
            attribs |= JSC::ReadOnly;
        if (flags & QScriptValue::SkipInEnumeration)
            attribs |= JSC::DontEnum;
        if (flags & QScriptValue::Undeletable)
            attribs |= JSC::DontDelete;
        if (flags & QScriptValue::PropertyGetter)
            attribs |= JSC::Getter;
        if (flags & QScriptValue::PropertySetter)
            attribs |= JSC::Setter;
        attribs |= flags & QScriptValue::UserRange;
        return true;
    }
    return QScriptObjectDelegate::getPropertyAttributes(object, exec, propertyName, attribs);
}

void ClassObjectDelegate::getOwnPropertyNames(QScriptObject* object, JSC::ExecState *exec,
                                              JSC::PropertyNameArray &propertyNames,
                                              bool includeNonEnumerable)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(engine, exec);
    QScriptValue scriptObject = engine->scriptValueFromJSCValue(object);
    QScriptClassPropertyIterator *it = m_scriptClass->newIterator(scriptObject);
    if (it != 0) {
        while (it->hasNext()) {
            it->next();
            QString name = it->name().toString();
            propertyNames.add(JSC::Identifier(exec, name));
        }
        delete it;
    }
    QScriptObjectDelegate::getOwnPropertyNames(object, exec, propertyNames, includeNonEnumerable);
}

JSC::CallType ClassObjectDelegate::getCallData(QScriptObject*, JSC::CallData &callData)
{
    if (!m_scriptClass->supportsExtension(QScriptClass::Callable))
        return JSC::CallTypeNone;
    callData.native.function = call;
    return JSC::CallTypeHost;
}

JSC::JSValue JSC_HOST_CALL ClassObjectDelegate::call(JSC::ExecState *exec, JSC::JSObject *callee,
                                             JSC::JSValue thisValue, const JSC::ArgList &args)
{
    if (!callee->inherits(&QScriptObject::info))
        return JSC::throwError(exec, JSC::TypeError, "callee is not a ClassObject object");
    QScriptObject *obj = static_cast<QScriptObject*>(callee);
    QScriptObjectDelegate *delegate = obj->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::ClassObject))
        return JSC::throwError(exec, JSC::TypeError, "callee is not a ClassObject object");

    QScriptClass *scriptClass = static_cast<ClassObjectDelegate*>(delegate)->scriptClass();
    QScriptEnginePrivate *eng_p = scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, thisValue, args, callee);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);
    QScriptValue scriptObject = eng_p->scriptValueFromJSCValue(obj);
    QVariant result = scriptClass->extension(QScriptClass::Callable, qVariantFromValue(ctx));
    eng_p->popContext();
    eng_p->currentFrame = oldFrame;
    return eng_p->jscValueFromVariant(result);
}

JSC::ConstructType ClassObjectDelegate::getConstructData(QScriptObject*, JSC::ConstructData &constructData)
{
    if (!m_scriptClass->supportsExtension(QScriptClass::Callable))
        return JSC::ConstructTypeNone;
    constructData.native.function = construct;
    return JSC::ConstructTypeHost;
}

JSC::JSObject* ClassObjectDelegate::construct(JSC::ExecState *exec, JSC::JSObject *callee,
                                              const JSC::ArgList &args)
{
    Q_ASSERT(callee->inherits(&QScriptObject::info));
    QScriptObject *obj = static_cast<QScriptObject*>(callee);
    QScriptObjectDelegate *delegate = obj->delegate();
    QScriptClass *scriptClass = static_cast<ClassObjectDelegate*>(delegate)->scriptClass();

    QScriptEnginePrivate *eng_p = scriptEngineFromExec(exec);
    //JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, JSC::JSValue(), args, callee, true);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue defaultObject = ctx->thisObject();
    QScriptValue result = qvariant_cast<QScriptValue>(scriptClass->extension(QScriptClass::Callable, qVariantFromValue(ctx)));
    if (!result.isObject())
        result = defaultObject;
    return JSC::asObject(eng_p->scriptValueToJSCValue(result));
}

bool ClassObjectDelegate::hasInstance(QScriptObject* object, JSC::ExecState *exec,
                                      JSC::JSValue value, JSC::JSValue proto)
{
    if (!scriptClass()->supportsExtension(QScriptClass::HasInstance))
        return QScriptObjectDelegate::hasInstance(object, exec, value, proto);
    QScriptValueList args;
    QScriptEnginePrivate *eng_p = scriptEngineFromExec(exec);
    QScript::SaveFrameHelper saveFrame(eng_p, exec);
    args << eng_p->scriptValueFromJSCValue(object) << eng_p->scriptValueFromJSCValue(value);
    QVariant result = scriptClass()->extension(QScriptClass::HasInstance, qVariantFromValue(args));
    return result.toBool();
}

} // namespace QScript

QT_END_NAMESPACE
