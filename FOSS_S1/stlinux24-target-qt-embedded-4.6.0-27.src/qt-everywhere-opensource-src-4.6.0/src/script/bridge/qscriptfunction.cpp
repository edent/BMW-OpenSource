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
#include "qscriptfunction_p.h"

#include "private/qscriptengine_p.h"
#include "qscriptcontext.h"
#include "private/qscriptcontext_p.h"
#include "private/qscriptvalue_p.h"
#include "qscriptactivationobject_p.h"
#include "qscriptobject_p.h"

#include "JSGlobalObject.h"
#include "DebuggerCallFrame.h"
#include "Debugger.h"

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScript::FunctionWrapper));
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScript::FunctionWithArgWrapper));
}

QT_BEGIN_NAMESPACE

namespace QScript
{

const JSC::ClassInfo FunctionWrapper::info = { "QtNativeFunctionWrapper", &PrototypeFunction::info, 0, 0 };
const JSC::ClassInfo FunctionWithArgWrapper::info = { "QtNativeFunctionWithArgWrapper", &PrototypeFunction::info, 0, 0 };

FunctionWrapper::FunctionWrapper(JSC::ExecState *exec, int length, const JSC::Identifier &name,
                                 QScriptEngine::FunctionSignature function)
    : JSC::PrototypeFunction(exec, length, name, proxyCall),
      data(new Data())
{
    data->function = function;
}

FunctionWrapper::~FunctionWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    consData.native.function.doNotCallDebuggerFunctionExit();
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWrapper::proxyCall(JSC::ExecState *exec, JSC::JSObject *callee,
                                        JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, thisObject, args, callee);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p));
    if (!result.isValid())
        result = QScriptValue(QScriptValue::UndefinedValue);

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWrapper::proxyConstruct(JSC::ExecState *exec, JSC::JSObject *callee,
                                               const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, JSC::JSValue(), args, callee, true);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p));

    if (JSC::Debugger* debugger = eng_p->originalGlobalObject()->debugger())
        debugger->functionExit(QScriptValuePrivate::get(result)->jscValue, -1);

    if (!result.isObject())
        result = ctx->thisObject();

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return JSC::asObject(eng_p->scriptValueToJSCValue(result));
}

FunctionWithArgWrapper::FunctionWithArgWrapper(JSC::ExecState *exec, int length, const JSC::Identifier &name,
                                               QScriptEngine::FunctionWithArgSignature function, void *arg)
    : JSC::PrototypeFunction(exec, length, name, proxyCall),
      data(new Data())
{
    data->function = function;
    data->arg = arg;
}

FunctionWithArgWrapper::~FunctionWithArgWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWithArgWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWithArgWrapper::proxyCall(JSC::ExecState *exec, JSC::JSObject *callee,
                                               JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, thisObject, args, callee);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p), self->data->arg);

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWithArgWrapper::proxyConstruct(JSC::ExecState *exec, JSC::JSObject *callee,
                                                      const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, JSC::JSValue(), args, callee, true);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p) , self->data->arg);
    if (!result.isObject())
        result = ctx->thisObject();

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return JSC::asObject(eng_p->scriptValueToJSCValue(result));
}

} // namespace QScript

QT_END_NAMESPACE
