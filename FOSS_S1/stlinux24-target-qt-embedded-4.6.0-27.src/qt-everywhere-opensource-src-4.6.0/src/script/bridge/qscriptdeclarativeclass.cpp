/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qscriptdeclarativeclass_p.h"
#include "qscriptdeclarativeobject_p.h"
#include "qscriptobject_p.h"
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptengineagent.h>
#include <private/qscriptengine_p.h>
#include <private/qscriptvalue_p.h>
#include <private/qscriptqobject_p.h>
#include <private/qscriptactivationobject_p.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier()
{
    new (&d) JSC::Identifier();
}

QScriptDeclarativeClass::PersistentIdentifier::~PersistentIdentifier()
{
    ((JSC::Identifier &)d).JSC::Identifier::~Identifier();
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier(const PersistentIdentifier &other)
{
    identifier = other.identifier;
    new (&d) JSC::Identifier((JSC::Identifier &)(other.d));
}

QScriptDeclarativeClass::PersistentIdentifier &
QScriptDeclarativeClass::PersistentIdentifier::operator=(const PersistentIdentifier &other)
{
    identifier = other.identifier;
    ((JSC::Identifier &)d) = (JSC::Identifier &)(other.d);
    return *this;
}

QScriptDeclarativeClass::QScriptDeclarativeClass(QScriptEngine *engine)
: d_ptr(new QScriptDeclarativeClassPrivate)
{
    Q_ASSERT(sizeof(void*) == sizeof(JSC::Identifier));
    d_ptr->q_ptr = this;
    d_ptr->engine = engine;
}

QScriptValue QScriptDeclarativeClass::newObject(QScriptEngine *engine, 
                                                QScriptDeclarativeClass *scriptClass, 
                                                Object *object)
{
    Q_ASSERT(engine);
    Q_ASSERT(scriptClass);

    QScriptEnginePrivate *p = static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(engine)); 

    JSC::ExecState* exec = p->currentFrame;
    QScriptObject *result = new (exec) QScriptObject(p->scriptObjectStructure);
    result->setDelegate(new QScript::DeclarativeObjectDelegate(scriptClass, object));
    return p->scriptValueFromJSCValue(result);
}

QScriptDeclarativeClass *QScriptDeclarativeClass::scriptClass(const QScriptValue &v)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);
    if (!d || !d->isJSC() || !d->jscValue.inherits(&QScriptObject::info))
        return 0;
    QScriptObject *scriptObject = static_cast<QScriptObject*>(JSC::asObject(d->jscValue));
    QScriptObjectDelegate *delegate = scriptObject->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::DeclarativeClassObject))
        return 0;
    return static_cast<QScript::DeclarativeObjectDelegate*>(delegate)->scriptClass();
}

QScriptDeclarativeClass::Object *QScriptDeclarativeClass::object(const QScriptValue &v)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);
    if (!d || !d->isJSC() || !d->jscValue.inherits(&QScriptObject::info))
        return 0;
    QScriptObject *scriptObject = static_cast<QScriptObject*>(JSC::asObject(d->jscValue));
    QScriptObjectDelegate *delegate = scriptObject->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::DeclarativeClassObject))
        return 0;
    return static_cast<QScript::DeclarativeObjectDelegate*>(delegate)->object();
}

QScriptValue QScriptDeclarativeClass::function(const QScriptValue &v, const Identifier &name)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);

    if (!d->isObject())
        return QScriptValue();

    JSC::ExecState *exec = d->engine->currentFrame;
    JSC::JSObject *object = d->jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;

    JSC::Identifier id(exec, (JSC::UString::Rep *)name);

    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
        if (QScript::isFunction(result))
            return d->engine->scriptValueFromJSCValue(result);
    }

    return QScriptValue();
}

QScriptValue QScriptDeclarativeClass::property(const QScriptValue &v, const Identifier &name)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);

    if (!d->isObject())
        return QScriptValue();

    JSC::ExecState *exec = d->engine->currentFrame;
    JSC::JSObject *object = d->jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;

    JSC::Identifier id(exec, (JSC::UString::Rep *)name);

    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
        return d->engine->scriptValueFromJSCValue(result);
    }

    return QScriptValue();
}

/*
Returns the scope chain entry at \a index.  If index is less than 0, returns
entries starting at the end.  For example, scopeChainValue(context, -1) will return
the value last in the scope chain.
*/
QScriptValue QScriptDeclarativeClass::scopeChainValue(QScriptContext *context, int index)
{
    context->activationObject(); //ensure the creation of the normal scope for native context
    const JSC::CallFrame *frame = QScriptEnginePrivate::frameForContext(context);
    QScriptEnginePrivate *engine = QScript::scriptEngineFromExec(frame);

    JSC::ScopeChainNode *node = frame->scopeChain();
    JSC::ScopeChainIterator it(node);

    if (index < 0) {
        int count = 0;
        for (it = node->begin(); it != node->end(); ++it) 
            ++count;

        index = qAbs(index);
        if (index > count)
            return QScriptValue();
        else
            index = count - index;
    }

    for (it = node->begin(); it != node->end(); ++it) {

        if (index == 0) {

            JSC::JSObject *object = *it;
            if (!object) return QScriptValue();

            if (object->inherits(&QScript::QScriptActivationObject::info)
                    && (static_cast<QScript::QScriptActivationObject*>(object)->delegate() != 0)) {
                // Return the object that property access is being delegated to
                object = static_cast<QScript::QScriptActivationObject*>(object)->delegate();
            }
            return engine->scriptValueFromJSCValue(object);

        } else {
            --index;
        }

    }

    return QScriptValue();
}

/*!
  Enters a new execution context and returns the associated
  QScriptContext object.

  Once you are done with the context, you should call popContext() to
  restore the old context.

  By default, the `this' object of the new context is the Global Object.
  The context's \l{QScriptContext::callee()}{callee}() will be invalid.

  Unlike pushContext(), the default scope chain is reset to include
  only the global object and the QScriptContext's activation object.

  \sa QScriptEngine::popContext()
*/
QScriptContext * QScriptDeclarativeClass::pushCleanContext(QScriptEngine *engine)
{
    if (!engine)
        return 0;

    QScriptEnginePrivate *d = QScriptEnginePrivate::get(engine);

    JSC::CallFrame* newFrame = d->pushContext(d->currentFrame, 
                                              d->currentFrame->globalData().dynamicGlobalObject,
                                              JSC::ArgList(), /*callee = */0, false, true);

    if (engine->agent())
        engine->agent()->contextPush();

    return d->contextForFrame(newFrame);
}

QScriptDeclarativeClass::~QScriptDeclarativeClass()
{
}

QScriptEngine *QScriptDeclarativeClass::engine() const
{
    return d_ptr->engine;
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const QString &str)
{
    QScriptEnginePrivate *p = 
        static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(d_ptr->engine)); 
    JSC::ExecState* exec = p->currentFrame;

    PersistentIdentifier rv(true);
    new (&rv.d) JSC::Identifier(exec, (UChar *)str.constData(), str.size());
    rv.identifier = (void *)((JSC::Identifier &)rv.d).ustring().rep();
    return rv;
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const Identifier &id)
{
    QScriptEnginePrivate *p = 
        static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(d_ptr->engine)); 
    JSC::ExecState* exec = p->currentFrame;

    PersistentIdentifier rv(true);
    new (&rv.d) JSC::Identifier(exec, (JSC::UString::Rep *)id);
    rv.identifier = (void *)((JSC::Identifier &)rv.d).ustring().rep();
    return rv;
}

QString QScriptDeclarativeClass::toString(const Identifier &identifier)
{
    JSC::UString::Rep *r = (JSC::UString::Rep *)identifier;
    return QString((QChar *)r->data(), r->size());
}

quint32 QScriptDeclarativeClass::toArrayIndex(const Identifier &identifier, bool *ok)
{
    JSC::UString::Rep *r = (JSC::UString::Rep *)identifier;
    JSC::UString s(r);
    return s.toArrayIndex(ok);
}

QScriptClass::QueryFlags 
QScriptDeclarativeClass::queryProperty(Object *object, const Identifier &name, 
                                       QScriptClass::QueryFlags flags)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    return 0;
}

QScriptValue QScriptDeclarativeClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    return QScriptValue();
}

void QScriptDeclarativeClass::setProperty(Object *object, const Identifier &name, 
                                          const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(value);
}

QScriptValue::PropertyFlags 
QScriptDeclarativeClass::propertyFlags(Object *object, const Identifier &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    return 0;
}

QStringList QScriptDeclarativeClass::propertyNames(Object *object)
{
    Q_UNUSED(object);
    return QStringList();
}

QObject *QScriptDeclarativeClass::toQObject(Object *, bool *ok)
{
    if (ok) *ok = false;
    return 0;
}

QVariant QScriptDeclarativeClass::toVariant(Object *, bool *ok)
{
    if (ok) *ok = false;
    return QVariant();
}

QScriptContext *QScriptDeclarativeClass::context() const
{
    return d_ptr->context;
}

QT_END_NAMESPACE
