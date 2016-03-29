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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"

#include <QtCore/qhash.h>
#include <QtCore/qset.h>
#include "qscriptvalue_p.h"
#include "qscriptstring_p.h"

#include "Debugger.h"
#include "Lexer.h"
#include "RefPtr.h"
#include "SourceProvider.h"
#include "Structure.h"
#include "JSGlobalObject.h"
#include "JSValue.h"

namespace JSC
{
    class EvalExecutable;
    class ExecState;
    typedef ExecState CallFrame;
    class JSCell;
    class JSGlobalObject;
    class UString;
}


QT_BEGIN_NAMESPACE

class QString;
class QStringList;
class QScriptContext;
class QScriptValue;
class QScriptTypeInfo;
class QScriptEngineAgent;
class QScriptEnginePrivate;
class QScriptSyntaxCheckResult;
class QScriptEngine;

namespace QScript
{
    class QObjectPrototype;
    class QMetaObjectPrototype;
    class QVariantPrototype;
#ifndef QT_NO_QOBJECT
    class QObjectData;
#endif
    class TimeoutCheckerProxy;

    //some conversion helper functions
    inline QScriptEnginePrivate *scriptEngineFromExec(const JSC::ExecState *exec);
    bool isFunction(JSC::JSValue value);

    class UStringSourceProviderWithFeedback;

struct GlobalClientData : public JSC::JSGlobalData::ClientData
{
    GlobalClientData(QScriptEnginePrivate *e)
        : engine(e) {}
    virtual ~GlobalClientData() {}
    virtual void mark(JSC::MarkStack& markStack);

    QScriptEnginePrivate *engine;
};

} // namespace QScript

class QScriptEnginePrivate
#ifndef QT_NO_QOBJECT
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QScriptEngine)
public:
    QScriptEnginePrivate();
    virtual ~QScriptEnginePrivate();

    static QScriptEnginePrivate *get(QScriptEngine *q) { return q ? q->d_func() : 0; }
    static QScriptEngine *get(QScriptEnginePrivate *d) { return d ? d->q_func() : 0; }

    static bool convert(const QScriptValue &value,
                        int type, void *ptr,
                        QScriptEnginePrivate *eng);
    QScriptValue create(int type, const void *ptr);
    bool hasDemarshalFunction(int type) const;

    inline QScriptValue scriptValueFromJSCValue(JSC::JSValue value);
    inline JSC::JSValue scriptValueToJSCValue(const QScriptValue &value);

    QScriptValue scriptValueFromVariant(const QVariant &value);
    QVariant scriptValueToVariant(const QScriptValue &value, int targetType);

    JSC::JSValue jscValueFromVariant(const QVariant &value);
    QVariant jscValueToVariant(JSC::JSValue value, int targetType);

    QScriptValue arrayFromStringList(const QStringList &lst);
    static QStringList stringListFromArray(const QScriptValue &arr);

    QScriptValue arrayFromVariantList(const QVariantList &lst);
    static QVariantList variantListFromArray(const QScriptValue &arr);

    QScriptValue objectFromVariantMap(const QVariantMap &vmap);
    static QVariantMap variantMapFromObject(const QScriptValue &obj);

    JSC::JSValue defaultPrototype(int metaTypeId) const;
    void setDefaultPrototype(int metaTypeId, JSC::JSValue prototype);

    static inline QScriptContext *contextForFrame(JSC::ExecState *frame);
    static inline JSC::ExecState *frameForContext(QScriptContext *context);
    static inline const JSC::ExecState *frameForContext(const QScriptContext *context);

    JSC::JSGlobalObject *originalGlobalObject() const;
    JSC::JSObject *getOriginalGlobalObjectProxy();
    JSC::JSObject *customGlobalObject() const;
    JSC::JSObject *globalObject() const;
    void setGlobalObject(JSC::JSObject *object);
    inline JSC::ExecState *globalExec() const;
    JSC::JSValue toUsableValue(JSC::JSValue value);
    static JSC::JSValue thisForContext(JSC::ExecState *frame);
    static JSC::Register *thisRegisterForFrame(JSC::ExecState *frame);

    JSC::CallFrame *pushContext(JSC::CallFrame *exec, JSC::JSValue thisObject, const JSC::ArgList& args,
                                JSC::JSObject *callee, bool calledAsConstructor = false, bool clearScopeChain = false);
    void popContext();

    void mark(JSC::MarkStack& markStack);
    bool isCollecting() const;
    void collectGarbage();

    //flags that we set on the return value register for native function. (ie when codeBlock is 0)
    enum ContextFlags {
        NativeContext = 1,
        CalledAsConstructorContext = 2,
        HasScopeContext = 4, // Specifies that the is a QScriptActivationObject
        ShouldRestoreCallFrame = 8
    };
    static uint contextFlags(JSC::ExecState *);
    static void setContextFlags(JSC::ExecState *, uint);

    QScript::TimeoutCheckerProxy *timeoutChecker() const;

    void agentDeleted(QScriptEngineAgent *agent);

    void setCurrentException(QScriptValue exception) { m_currentException = exception; }
    QScriptValue currentException() const { return m_currentException; }
    void clearCurrentException() { m_currentException.d_ptr.reset(); }

#ifndef QT_NO_QOBJECT
    JSC::JSValue newQObject(QObject *object,
        QScriptEngine::ValueOwnership ownership = QScriptEngine::QtOwnership,
        const QScriptEngine:: QObjectWrapOptions &options = 0);
    JSC::JSValue newQMetaObject(const QMetaObject *metaObject,
                                JSC::JSValue ctor);

    static QScriptSyntaxCheckResult checkSyntax(const QString &program);
    static bool canEvaluate(const QString &program);
    static bool convertToNativeQObject(const QScriptValue &value,
                                       const QByteArray &targetType,
                                       void **result);

    JSC::JSValue evaluateHelper(JSC::ExecState *exec, intptr_t sourceId,
                                JSC::EvalExecutable *executable,
                                bool &compile);

    QScript::QObjectData *qobjectData(QObject *object);
    void disposeQObject(QObject *object);
    void emitSignalHandlerException();

    bool scriptConnect(QObject *sender, const char *signal,
                       JSC::JSValue receiver, JSC::JSValue function,
                       Qt::ConnectionType type);
    bool scriptDisconnect(QObject *sender, const char *signal,
                          JSC::JSValue receiver, JSC::JSValue function);

    bool scriptConnect(QObject *sender, int index,
                       JSC::JSValue receiver, JSC::JSValue function,
                       JSC::JSValue senderWrapper,
                       Qt::ConnectionType type);
    bool scriptDisconnect(QObject *sender, int index,
                          JSC::JSValue receiver, JSC::JSValue function);

    bool scriptConnect(JSC::JSValue signal, JSC::JSValue receiver,
                       JSC::JSValue function, Qt::ConnectionType type);
    bool scriptDisconnect(JSC::JSValue signal, JSC::JSValue receiver,
                          JSC::JSValue function);

    inline QScriptValuePrivate *allocateScriptValuePrivate(size_t);
    inline void freeScriptValuePrivate(QScriptValuePrivate *p);

    inline void registerScriptValue(QScriptValuePrivate *value);
    inline void unregisterScriptValue(QScriptValuePrivate *value);
    void detachAllRegisteredScriptValues();

    inline void registerScriptString(QScriptStringPrivate *value);
    inline void unregisterScriptString(QScriptStringPrivate *value);
    void detachAllRegisteredScriptStrings();

    // private slots
    void _q_objectDestroyed(QObject *);
#endif

    JSC::JSGlobalData *globalData;
    JSC::JSObject *originalGlobalObjectProxy;
    JSC::ExecState *currentFrame;

    WTF::RefPtr<JSC::Structure> scriptObjectStructure;

    QScript::QObjectPrototype *qobjectPrototype;
    WTF::RefPtr<JSC::Structure> qobjectWrapperObjectStructure;

    QScript::QMetaObjectPrototype *qmetaobjectPrototype;
    WTF::RefPtr<JSC::Structure> qmetaobjectWrapperObjectStructure;

    QScript::QVariantPrototype *variantPrototype;
    WTF::RefPtr<JSC::Structure> variantWrapperObjectStructure;

    QList<QScriptEngineAgent*> ownedAgents;
    QScriptEngineAgent *activeAgent;
    int agentLineNumber;
    QScriptValuePrivate *registeredScriptValues;
    QScriptValuePrivate *freeScriptValues;
    QScriptStringPrivate *registeredScriptStrings;
    QHash<int, QScriptTypeInfo*> m_typeInfos;
    int processEventsInterval;
    QScriptValue abortResult;
    bool inEval;

    QSet<QString> importedExtensions;
    QSet<QString> extensionsBeingImported;
    
    QHash<intptr_t, QScript::UStringSourceProviderWithFeedback*> loadedScripts;
    QScriptValue m_currentException;

#ifndef QT_NO_QOBJECT
    QHash<QObject*, QScript::QObjectData*> m_qobjectData;
#endif

#ifdef QT_NO_QOBJECT
    QScriptEngine *q_ptr;
#endif
};

namespace QScript
{

/*Helper class. Main purpose is to give debugger feedback about unloading and loading scripts.
  It keeps pointer to JSGlobalObject assuming that it is always the same - there is no way to update
  this data. Class is internal and used as an implementation detail in and only in QScriptEngine::evaluate.*/
class UStringSourceProviderWithFeedback: public JSC::UStringSourceProvider
{
public:
    static PassRefPtr<UStringSourceProviderWithFeedback> create(
        const JSC::UString& source, const JSC::UString& url,
        int lineNumber, QScriptEnginePrivate* engine)
    {
        return adoptRef(new UStringSourceProviderWithFeedback(source, url, lineNumber, engine));
    }

    /* Destruction means that there is no more copies of script so create scriptUnload event
       and unregister script in QScriptEnginePrivate::loadedScripts */
    virtual ~UStringSourceProviderWithFeedback()
    {
        if (m_ptr) {
            if (JSC::Debugger* debugger = this->debugger())
                debugger->scriptUnload(asID());
            m_ptr->loadedScripts.remove(asID());
        }
    }

    /* set internal QScriptEnginePrivate pointer to null and create unloadScript event, should be called
       only if QScriptEnginePrivate is about to be  destroyed.*/
    void disconnectFromEngine()
    {
        if (JSC::Debugger* debugger = this->debugger())
            debugger->scriptUnload(asID());
        m_ptr = 0;
    }

    int columnNumberFromOffset(int offset) const
    {
        for (const UChar *c = m_source.data() + offset; c >= m_source.data(); --c) {
            if (JSC::Lexer::isLineTerminator(*c))
                return offset - static_cast<int>(c - data());
        }
        return offset + 1;
    }

protected:
    UStringSourceProviderWithFeedback(const JSC::UString& source, const JSC::UString& url,
                                      int lineNumber, QScriptEnginePrivate* engine)
        : UStringSourceProvider(source, url),
          m_ptr(engine)
    {
        if (JSC::Debugger* debugger = this->debugger())
            debugger->scriptLoad(asID(), source, url, lineNumber);
        if (m_ptr)
            m_ptr->loadedScripts.insert(asID(), this);
    }

    JSC::Debugger* debugger()
    {
        //if m_ptr is null it mean that QScriptEnginePrivate was destroyed and scriptUnload was called
        //else m_ptr is stable and we can use it as normal pointer without hesitation
        if(!m_ptr)
            return 0; //we are in ~QScriptEnginePrivate
        else
            return m_ptr->originalGlobalObject()->debugger(); //QScriptEnginePrivate is still alive
    }

    //trace global object and debugger instance
    QScriptEnginePrivate* m_ptr;
};

class SaveFrameHelper
{
public:
    SaveFrameHelper(QScriptEnginePrivate *eng,
                    JSC::ExecState *newFrame)
        : engine(eng), oldFrame(eng->currentFrame)
    {
        eng->currentFrame = newFrame;
    }
    ~SaveFrameHelper()
    {
        engine->currentFrame = oldFrame;
    }
private:
    QScriptEnginePrivate *engine;
    JSC::ExecState *oldFrame;
};

inline QScriptEnginePrivate *scriptEngineFromExec(const JSC::ExecState *exec)
{
    return static_cast<GlobalClientData*>(exec->globalData().clientData)->engine;
}

} // namespace QScript

inline QScriptValuePrivate *QScriptEnginePrivate::allocateScriptValuePrivate(size_t size)
{
    if (freeScriptValues) {
        QScriptValuePrivate *p = freeScriptValues;
        freeScriptValues = p->next;
        return p;
    }
    return reinterpret_cast<QScriptValuePrivate*>(qMalloc(size));
}

inline void QScriptEnginePrivate::freeScriptValuePrivate(QScriptValuePrivate *p)
{
    p->next = freeScriptValues;
    freeScriptValues = p;
}

inline void QScriptEnginePrivate::registerScriptValue(QScriptValuePrivate *value)
{
    value->prev = 0;
    value->next = registeredScriptValues;
    if (registeredScriptValues)
        registeredScriptValues->prev = value;
    registeredScriptValues = value;
}

inline void QScriptEnginePrivate::unregisterScriptValue(QScriptValuePrivate *value)
{
    if (value->prev)
        value->prev->next = value->next;
    if (value->next)
        value->next->prev = value->prev;
    if (value == registeredScriptValues)
        registeredScriptValues = value->next;
    value->prev = 0;
    value->next = 0;
}

inline QScriptValue QScriptEnginePrivate::scriptValueFromJSCValue(JSC::JSValue value)
{
    if (!value)
        return QScriptValue();

    QScriptValuePrivate *p_value = new (this)QScriptValuePrivate(this);
    p_value->initFrom(value);
    return QScriptValuePrivate::toPublic(p_value);
}

inline JSC::JSValue QScriptEnginePrivate::scriptValueToJSCValue(const QScriptValue &value)
{
    QScriptValuePrivate *vv = QScriptValuePrivate::get(value);
    if (!vv)
        return JSC::JSValue();
    if (vv->type != QScriptValuePrivate::JavaScriptCore) {
        Q_ASSERT(!vv->engine || vv->engine == this);
        vv->engine = this;
        if (vv->type == QScriptValuePrivate::Number) {
            vv->initFrom(JSC::jsNumber(currentFrame, vv->numberValue));
        } else { //QScriptValuePrivate::String
            vv->initFrom(JSC::jsString(currentFrame, vv->stringValue));
        }
    }
    return vv->jscValue;
}

inline QScriptValuePrivate::~QScriptValuePrivate()
{
    if (engine)
        engine->unregisterScriptValue(this);
}

inline void QScriptValuePrivate::initFrom(JSC::JSValue value)
{
    if (value.isCell()) {
        Q_ASSERT(engine != 0);
        value = engine->toUsableValue(value);
    }
    type = JavaScriptCore;
    jscValue = value;
    if (engine)
        engine->registerScriptValue(this);
}

inline void QScriptValuePrivate::initFrom(qsreal value)
{
    type = Number;
    numberValue = value;
    if (engine)
        engine->registerScriptValue(this);
}

inline void QScriptValuePrivate::initFrom(const QString &value)
{
    type = String;
    stringValue = value;
    if (engine)
        engine->registerScriptValue(this);
}

inline QScriptValue QScriptValuePrivate::property(const QString &name, int resolveMode) const
{
    JSC::ExecState *exec = engine->currentFrame;
    return property(JSC::Identifier(exec, name), resolveMode);
}

inline QScriptValue QScriptValuePrivate::property(const JSC::Identifier &id, int resolveMode) const
{
    Q_ASSERT(isObject());
    JSC::ExecState *exec = engine->currentFrame;
    JSC::JSObject *object = JSC::asObject(jscValue);
    JSC::PropertySlot slot(object);
    if ((resolveMode & QScriptValue::ResolvePrototype) && object->getPropertySlot(exec, id, slot))
        return engine->scriptValueFromJSCValue(slot.getValue(exec, id));
    return propertyHelper(id, resolveMode);
}

inline QScriptValue QScriptValuePrivate::property(quint32 index, int resolveMode) const
{
    Q_ASSERT(isObject());
    JSC::ExecState *exec = engine->currentFrame;
    JSC::JSObject *object = JSC::asObject(jscValue);
    JSC::PropertySlot slot(object);
    if ((resolveMode & QScriptValue::ResolvePrototype) && object->getPropertySlot(exec, index, slot))
        return engine->scriptValueFromJSCValue(slot.getValue(exec, index));
    return propertyHelper(index, resolveMode);
}

inline void* QScriptValuePrivate::operator new(size_t size, QScriptEnginePrivate *engine)
{
    if (engine)
        return engine->allocateScriptValuePrivate(size);
    return qMalloc(size);
}

inline void QScriptValuePrivate::operator delete(void *ptr)
{
    QScriptValuePrivate *d = reinterpret_cast<QScriptValuePrivate*>(ptr);
    if (d->engine)
        d->engine->freeScriptValuePrivate(d);
    else
        qFree(d);
}

inline void QScriptValuePrivate::saveException(JSC::ExecState *exec, JSC::JSValue *val)
{
    if (exec) {
        *val = exec->exception();
        exec->clearException();
    } else {
        *val = JSC::JSValue();
    }
}

inline void QScriptValuePrivate::restoreException(JSC::ExecState *exec, JSC::JSValue val)
{
    if (exec && val)
        exec->setException(val);
}

inline void QScriptEnginePrivate::registerScriptString(QScriptStringPrivate *value)
{
    Q_ASSERT(value->type == QScriptStringPrivate::HeapAllocated);
    value->prev = 0;
    value->next = registeredScriptStrings;
    if (registeredScriptStrings)
        registeredScriptStrings->prev = value;
    registeredScriptStrings = value;
}

inline void QScriptEnginePrivate::unregisterScriptString(QScriptStringPrivate *value)
{
    Q_ASSERT(value->type == QScriptStringPrivate::HeapAllocated);
    if (value->prev)
        value->prev->next = value->next;
    if (value->next)
        value->next->prev = value->prev;
    if (value == registeredScriptStrings)
        registeredScriptStrings = value->next;
    value->prev = 0;
    value->next = 0;
}

inline QScriptContext *QScriptEnginePrivate::contextForFrame(JSC::ExecState *frame)
{
    if (frame && frame->callerFrame()->hasHostCallFrameFlag() && !frame->callee()
        && frame->callerFrame()->removeHostCallFrameFlag() == QScript::scriptEngineFromExec(frame)->globalExec()) {
        //skip the "fake" context created in Interpreter::execute.
        frame = frame->callerFrame()->removeHostCallFrameFlag();
    }
    return reinterpret_cast<QScriptContext *>(frame);
}

inline JSC::ExecState *QScriptEnginePrivate::frameForContext(QScriptContext *context)
{
    return reinterpret_cast<JSC::ExecState*>(context);
}

inline const JSC::ExecState *QScriptEnginePrivate::frameForContext(const QScriptContext *context)
{
    return reinterpret_cast<const JSC::ExecState*>(context);
}

inline JSC::ExecState *QScriptEnginePrivate::globalExec() const
{
    return originalGlobalObject()->globalExec();
}

QT_END_NAMESPACE

#endif
