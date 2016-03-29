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
#include "qscriptengine.h"
#include "qscriptsyntaxchecker_p.h"
#include "qnumeric.h"

#include "qscriptengine_p.h"
#include "qscriptengineagent_p.h"
#include "qscriptcontext_p.h"
#include "qscriptstring_p.h"
#include "qscriptvalue_p.h"
#include "qscriptvalueiterator.h"
#include "qscriptclass.h"
#include "qscriptprogram.h"
#include "qscriptprogram_p.h"
#include "qdebug.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qmetaobject.h>

#include "Error.h"
#include "JSArray.h"
#include "JSLock.h"
#include "Interpreter.h"
#include "DateConstructor.h"
#include "RegExpConstructor.h"

#include "PrototypeFunction.h"
#include "InitializeThreading.h"
#include "ObjectPrototype.h"
#include "SourceCode.h"
#include "FunctionPrototype.h"
#include "TimeoutChecker.h"
#include "JSFunction.h"
#include "Parser.h"
#include "Operations.h"

#include "utils/qscriptdate_p.h"
#include "bridge/qscriptfunction_p.h"
#include "bridge/qscriptobject_p.h"
#include "bridge/qscriptclassobject_p.h"
#include "bridge/qscriptvariant_p.h"
#include "bridge/qscriptqobject_p.h"
#include "bridge/qscriptglobalobject_p.h"
#include "bridge/qscriptactivationobject_p.h"

#ifndef QT_NO_QOBJECT
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qset.h>
#include <QtCore/qtextstream.h>
#include "qscriptextensioninterface.h"
#endif

Q_DECLARE_METATYPE(QScriptValue)
#ifndef QT_NO_QOBJECT
Q_DECLARE_METATYPE(QObjectList)
#endif
Q_DECLARE_METATYPE(QList<int>)

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptEngine
  \reentrant

  \brief The QScriptEngine class provides an environment for evaluating Qt Script code.

  \ingroup script
  \mainclass

  See the \l{QtScript} documentation for information about the Qt Script language,
  and how to get started with scripting your C++ application.

  \section1 Evaluating Scripts

  Use evaluate() to evaluate script code; this is the C++ equivalent
  of the built-in script function \c{eval()}.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 0

  evaluate() returns a QScriptValue that holds the result of the
  evaluation. The QScriptValue class provides functions for converting
  the result to various C++ types (e.g. QScriptValue::toString()
  and QScriptValue::toNumber()).

  The following code snippet shows how a script function can be
  defined and then invoked from C++ using QScriptValue::call():

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 1

  As can be seen from the above snippets, a script is provided to the
  engine in the form of a string. One common way of loading scripts is
  by reading the contents of a file and passing it to evaluate():

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 2

  Here we pass the name of the file as the second argument to
  evaluate().  This does not affect evaluation in any way; the second
  argument is a general-purpose string that is used to identify the
  script for debugging purposes (for example, our filename will now
  show up in any uncaughtExceptionBacktrace() involving the script).

  \section1 Engine Configuration

  The globalObject() function returns the \bold {Global Object}
  associated with the script engine. Properties of the Global Object
  are accessible from any script code (i.e. they are global
  variables). Typically, before evaluating "user" scripts, you will
  want to configure a script engine by adding one or more properties
  to the Global Object:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 3

  Adding custom properties to the scripting environment is one of the
  standard means of providing a scripting API that is specific to your
  application. Usually these custom properties are objects created by
  the newQObject() or newObject() functions, or constructor functions
  created by newFunction().

  \section1 Script Exceptions

  evaluate() can throw a script exception (e.g. due to a syntax
  error); in that case, the return value is the value that was thrown
  (typically an \c{Error} object). You can check whether the
  evaluation caused an exception by calling hasUncaughtException(). In
  that case, you can call toString() on the error object to obtain an
  error message. The current uncaught exception is also available
  through uncaughtException(). You can obtain a human-readable
  backtrace of the exception with uncaughtExceptionBacktrace().
  Calling clearExceptions() will cause any uncaught exceptions to be
  cleared.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 4

  The checkSyntax() function can be used to determine whether code can be
  usefully passed to evaluate().

  \section1 Script Object Creation

  Use newObject() to create a standard Qt Script object; this is the
  C++ equivalent of the script statement \c{new Object()}. You can use
  the object-specific functionality in QScriptValue to manipulate the
  script object (e.g. QScriptValue::setProperty()). Similarly, use
  newArray() to create a Qt Script array object. Use newDate() to
  create a \c{Date} object, and newRegExp() to create a \c{RegExp}
  object.

  \section1 QObject Integration

  Use newQObject() to wrap a QObject (or subclass)
  pointer. newQObject() returns a proxy script object; properties,
  children, and signals and slots of the QObject are available as
  properties of the proxy object. No binding code is needed because it
  is done dynamically using the Qt meta object system.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 5

  Use qScriptConnect() to connect a C++ signal to a script function;
  this is the Qt Script equivalent of QObject::connect().  When a
  script function is invoked in response to a C++ signal, it can cause
  a script exception; you can connect to the signalHandlerException()
  signal to catch such an exception.

  Use newQMetaObject() to wrap a QMetaObject; this gives you a "script
  representation" of a QObject-based class. newQMetaObject() returns a
  proxy script object; enum values of the class are available as
  properties of the proxy object. You can also specify a function that
  will be used to construct objects of the class (e.g.  when the
  constructor is invoked from a script). For classes that have a
  "standard" Qt constructor, Qt Script can provide a default script
  constructor for you; see scriptValueFromQMetaObject().

  See the \l{QtScript} documentation for more information on
  the QObject integration.

  \section1 Support for Custom C++ Types

  Use newVariant() to wrap a QVariant. This can be used to store
  values of custom (non-QObject) C++ types that have been registered
  with the Qt meta-type system. To make such types scriptable, you
  typically associate a prototype (delegate) object with the C++ type
  by calling setDefaultPrototype(); the prototype object defines the
  scripting API for the C++ type. Unlike the QObject integration,
  there is no automatic binding possible here; i.e. you have to create
  the scripting API yourself, for example by using the QScriptable
  class.

  Use fromScriptValue() to cast from a QScriptValue to another type,
  and toScriptValue() to create a QScriptValue from another value.
  You can specify how the conversion of C++ types is to be performed
  with qScriptRegisterMetaType() and qScriptRegisterSequenceMetaType().
  By default, Qt Script will use QVariant to store values of custom
  types.

  \section1 Importing Extensions

  Use importExtension() to import plugin-based extensions into the
  engine. Call availableExtensions() to obtain a list naming all the
  available extensions, and importedExtensions() to obtain a list
  naming only those extensions that have been imported.

  Call pushContext() to open up a new variable scope, and popContext()
  to close the current scope. This is useful if you are implementing
  an extension that evaluates script code containing temporary
  variable definitions (e.g. \c{var foo = 123;}) that are safe to
  discard when evaluation has completed.

  \section1 Native Functions

  Use newFunction() to wrap native (C++) functions, including
  constructors for your own custom types, so that these can be invoked
  from script code. Such functions must have the signature
  QScriptEngine::FunctionSignature. You may then pass the function as
  argument to newFunction(). Here is an example of a function that
  returns the sum of its first two arguments:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 6

  To expose this function to script code, you can set it as a property
  of the Global Object:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 7

  Once this is done, script code can call your function in the exact
  same manner as a "normal" script function:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 8

  \section1 Long-running Scripts

  If you need to evaluate possibly long-running scripts from the main
  (GUI) thread, you should first call setProcessEventsInterval() to
  make sure that the GUI stays responsive. You can abort a currently
  running script by calling abortEvaluation(). You can determine
  whether an engine is currently running a script by calling
  isEvaluating().

  \section1 Core Debugging/Tracing Facilities

  Since Qt 4.4, you can be notified of events pertaining to script
  execution (e.g. script function calls and statement execution)
  through the QScriptEngineAgent interface; see the setAgent()
  function. This can be used to implement debugging and profiling of a
  QScriptEngine.

  \sa QScriptValue, QScriptContext, QScriptEngineAgent

*/

/*!
    \enum QScriptEngine::ValueOwnership

    This enum specifies the ownership when wrapping a C++ value, e.g. by using newQObject().

    \value QtOwnership The standard Qt ownership rules apply, i.e. the associated object will never be explicitly deleted by the script engine. This is the default. (QObject ownership is explained in \l{Object Trees and Object Ownership}.)
    \value ScriptOwnership The value is owned by the script environment. The associated data will be deleted when appropriate (i.e. after the garbage collector has discovered that there are no more live references to the value).
    \value AutoOwnership If the associated object has a parent, the Qt ownership rules apply (QtOwnership); otherwise, the object is owned by the script environment (ScriptOwnership).
*/

/*!
    \enum  QScriptEngine::QObjectWrapOption

    These flags specify options when wrapping a QObject pointer with newQObject().

    \value ExcludeChildObjects The script object will not expose child objects as properties.
    \value ExcludeSuperClassMethods The script object will not expose signals and slots inherited from the superclass.
    \value ExcludeSuperClassProperties The script object will not expose properties inherited from the superclass.
    \value ExcludeSuperClassContents Shorthand form for ExcludeSuperClassMethods | ExcludeSuperClassProperties
    \value ExcludeDeleteLater The script object will not expose the QObject::deleteLater() slot.
    \value AutoCreateDynamicProperties Properties that don't already exist in the QObject will be created as dynamic properties of that object, rather than as properties of the script object.
    \value PreferExistingWrapperObject If a wrapper object with the requested configuration already exists, return that object.
    \value SkipMethodsInEnumeration Don't include methods (signals and slots) when enumerating the object's properties.
*/

class QScriptSyntaxCheckResultPrivate
{
public:
    QScriptSyntaxCheckResultPrivate() { ref = 0; }
    ~QScriptSyntaxCheckResultPrivate() {}

    QScriptSyntaxCheckResult::State state;
    int errorColumnNumber;
    int errorLineNumber;
    QString errorMessage;
    QBasicAtomicInt ref;
};

class QScriptTypeInfo
{
public:
    QScriptTypeInfo() : signature(0, '\0'), marshal(0), demarshal(0)
    { }

    QByteArray signature;
    QScriptEngine::MarshalFunction marshal;
    QScriptEngine::DemarshalFunction demarshal;
    JSC::JSValue prototype;
};

namespace QScript
{

void GlobalClientData::mark(JSC::MarkStack& markStack)
{
    engine->mark(markStack);
}

class TimeoutCheckerProxy : public JSC::TimeoutChecker
{
public:
    TimeoutCheckerProxy(const JSC::TimeoutChecker& originalChecker)
        : JSC::TimeoutChecker(originalChecker)
        , m_shouldProcessEvents(false)
        , m_shouldAbortEvaluation(false)
    {}

    void setShouldProcessEvents(bool shouldProcess) { m_shouldProcessEvents = shouldProcess; }
    void setShouldAbort(bool shouldAbort) { m_shouldAbortEvaluation = shouldAbort; }
    bool shouldAbort() { return m_shouldAbortEvaluation; }

    virtual bool didTimeOut(JSC::ExecState* exec)
    {
        if (JSC::TimeoutChecker::didTimeOut(exec))
            return true;

        if (m_shouldProcessEvents)
            QCoreApplication::processEvents();

        return m_shouldAbortEvaluation;
    }

private:
    bool m_shouldProcessEvents;
    bool m_shouldAbortEvaluation;
};

static int toDigit(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    else if ((c >= 'a') && (c <= 'z'))
        return 10 + c - 'a';
    else if ((c >= 'A') && (c <= 'Z'))
        return 10 + c - 'A';
    return -1;
}

qsreal integerFromString(const char *buf, int size, int radix)
{
    if (size == 0)
        return qSNaN();

    qsreal sign = 1.0;
    int i = 0;
    if (buf[0] == '+') {
        ++i;
    } else if (buf[0] == '-') {
        sign = -1.0;
        ++i;
    }

    if (((size-i) >= 2) && (buf[i] == '0')) {
        if (((buf[i+1] == 'x') || (buf[i+1] == 'X'))
            && (radix < 34)) {
            if ((radix != 0) && (radix != 16))
                return 0;
            radix = 16;
            i += 2;
        } else {
            if (radix == 0) {
                radix = 8;
                ++i;
            }
        }
    } else if (radix == 0) {
        radix = 10;
    }

    int j = i;
    for ( ; i < size; ++i) {
        int d = toDigit(buf[i]);
        if ((d == -1) || (d >= radix))
            break;
    }
    qsreal result;
    if (j == i) {
        if (!qstrcmp(buf, "Infinity"))
            result = qInf();
        else
            result = qSNaN();
    } else {
        result = 0;
        qsreal multiplier = 1;
        for (--i ; i >= j; --i, multiplier *= radix)
            result += toDigit(buf[i]) * multiplier;
    }
    result *= sign;
    return result;
}

qsreal integerFromString(const QString &str, int radix)
{
    QByteArray ba = str.trimmed().toUtf8();
    return integerFromString(ba.constData(), ba.size(), radix);
}

bool isFunction(JSC::JSValue value)
{
    if (!value || !value.isObject())
        return false;
    JSC::CallData callData;
    return (JSC::asObject(value)->getCallData(callData) != JSC::CallTypeNone);
}

static JSC::JSValue JSC_HOST_CALL functionConnect(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionDisconnect(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);

JSC::JSValue JSC_HOST_CALL functionDisconnect(JSC::ExecState *exec, JSC::JSObject * /*callee*/, JSC::JSValue thisObject, const JSC::ArgList &args)
{
#ifndef QT_NO_QOBJECT
    if (args.size() == 0) {
        return JSC::throwError(exec, JSC::GeneralError, "Function.prototype.disconnect: no arguments given");
    }

    if (!JSC::asObject(thisObject)->inherits(&QScript::QtFunction::info)) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.disconnect: this object is not a signal");
    }

    QScript::QtFunction *qtSignal = static_cast<QScript::QtFunction*>(JSC::asObject(thisObject));

    const QMetaObject *meta = qtSignal->metaObject();
    if (!meta) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.discconnect: cannot disconnect from deleted QObject");
    }

    QMetaMethod sig = meta->method(qtSignal->initialIndex());
    if (sig.methodType() != QMetaMethod::Signal) {
        QString message = QString::fromLatin1("Function.prototype.disconnect: %0::%1 is not a signal")
                          .arg(QLatin1String(qtSignal->metaObject()->className()))
                          .arg(QLatin1String(sig.signature()));
        return JSC::throwError(exec, JSC::TypeError, message);
    }

    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);

    JSC::JSValue receiver;
    JSC::JSValue slot;
    JSC::JSValue arg0 = args.at(0);
    if (args.size() < 2) {
        slot = arg0;
    } else {
        receiver = arg0;
        JSC::JSValue arg1 = args.at(1);
        if (isFunction(arg1))
            slot = arg1;
        else {
            // ### don't go via QScriptValue
            QScript::SaveFrameHelper saveFrame(engine, exec);
            QScriptValue tmp = engine->scriptValueFromJSCValue(arg0);
            QString propertyName(arg1.toString(exec));
            slot = engine->scriptValueToJSCValue(tmp.property(propertyName, QScriptValue::ResolvePrototype));
        }
    }

    if (!isFunction(slot)) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.disconnect: target is not a function");
    }

    bool ok = engine->scriptDisconnect(thisObject, receiver, slot);
    if (!ok) {
        QString message = QString::fromLatin1("Function.prototype.disconnect: failed to disconnect from %0::%1")
                          .arg(QLatin1String(qtSignal->metaObject()->className()))
                          .arg(QLatin1String(sig.signature()));
        return JSC::throwError(exec, JSC::GeneralError, message);
    }
    return JSC::jsUndefined();
#else
    Q_UNUSED(eng);
    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("Function.prototype.disconnect"));
#endif // QT_NO_QOBJECT
}

JSC::JSValue JSC_HOST_CALL functionConnect(JSC::ExecState *exec, JSC::JSObject * /*callee*/, JSC::JSValue thisObject, const JSC::ArgList &args)
{
#ifndef QT_NO_QOBJECT
    if (args.size() == 0) {
        return JSC::throwError(exec, JSC::GeneralError,"Function.prototype.connect: no arguments given");
    }

    if (!JSC::asObject(thisObject)->inherits(&QScript::QtFunction::info)) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.connect: this object is not a signal");
    }

    QScript::QtFunction *qtSignal = static_cast<QScript::QtFunction*>(JSC::asObject(thisObject));

    const QMetaObject *meta = qtSignal->metaObject();
    if (!meta) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.connect: cannot connect to deleted QObject");
    }

    QMetaMethod sig = meta->method(qtSignal->initialIndex());
    if (sig.methodType() != QMetaMethod::Signal) {
        QString message = QString::fromLatin1("Function.prototype.connect: %0::%1 is not a signal")
                          .arg(QLatin1String(qtSignal->metaObject()->className()))
                          .arg(QLatin1String(sig.signature()));
        return JSC::throwError(exec, JSC::TypeError, message);
    }

    {
        QList<int> overloads = qtSignal->overloadedIndexes();
        if (!overloads.isEmpty()) {
            overloads.append(qtSignal->initialIndex());
            QByteArray signature = sig.signature();
            QString message = QString::fromLatin1("Function.prototype.connect: ambiguous connect to %0::%1(); candidates are\n")
                              .arg(QLatin1String(qtSignal->metaObject()->className()))
                              .arg(QLatin1String(signature.left(signature.indexOf('('))));
            for (int i = 0; i < overloads.size(); ++i) {
                QMetaMethod mtd = meta->method(overloads.at(i));
                message.append(QString::fromLatin1("    %0\n").arg(QString::fromLatin1(mtd.signature())));
            }
            message.append(QString::fromLatin1("Use e.g. object['%0'].connect() to connect to a particular overload")
                           .arg(QLatin1String(signature)));
            return JSC::throwError(exec, JSC::GeneralError, message);
        }
    }

    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);

    JSC::JSValue receiver;
    JSC::JSValue slot;
    JSC::JSValue arg0 = args.at(0);
    if (args.size() < 2) {
        slot = arg0;
    } else {
        receiver = arg0;
        JSC::JSValue arg1 = args.at(1);
        if (isFunction(arg1))
            slot = arg1;
        else {
            // ### don't go via QScriptValue
            QScript::SaveFrameHelper saveFrame(engine, exec);
            QScriptValue tmp = engine->scriptValueFromJSCValue(arg0);
            QString propertyName = arg1.toString(exec);
            slot = engine->scriptValueToJSCValue(tmp.property(propertyName, QScriptValue::ResolvePrototype));
        }
    }

    if (!isFunction(slot)) {
        return JSC::throwError(exec, JSC::TypeError, "Function.prototype.connect: target is not a function");
    }

    bool ok = engine->scriptConnect(thisObject, receiver, slot, Qt::AutoConnection);
    if (!ok) {
        QString message = QString::fromLatin1("Function.prototype.connect: failed to connect to %0::%1")
                          .arg(QLatin1String(qtSignal->metaObject()->className()))
                          .arg(QLatin1String(sig.signature()));
        return JSC::throwError(exec, JSC::GeneralError, message);
    }
    return JSC::jsUndefined();
#else
    Q_UNUSED(eng);
    Q_UNUSED(classInfo);
    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("Function.prototype.connect"));
#endif // QT_NO_QOBJECT
}

static JSC::JSValue JSC_HOST_CALL functionPrint(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionGC(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionVersion(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);

JSC::JSValue JSC_HOST_CALL functionPrint(JSC::ExecState* exec, JSC::JSObject*, JSC::JSValue, const JSC::ArgList& args)
{
    QString result;
    for (unsigned i = 0; i < args.size(); ++i) {
        if (i != 0)
            result.append(QLatin1Char(' '));
        QString s(args.at(i).toString(exec));
        if (exec->hadException())
            break;
        result.append(s);
    }
    if (exec->hadException())
        return exec->exception();
    qDebug("%s", qPrintable(result));
    return JSC::jsUndefined();
}

JSC::JSValue JSC_HOST_CALL functionGC(JSC::ExecState* exec, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    engine->collectGarbage();
    return JSC::jsUndefined();
}

JSC::JSValue JSC_HOST_CALL functionVersion(JSC::ExecState *exec, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&)
{
    return JSC::JSValue(exec, 1);
}

static JSC::JSValue JSC_HOST_CALL functionQsTranslate(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionQsTranslateNoOp(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionQsTr(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
static JSC::JSValue JSC_HOST_CALL functionQsTrNoOp(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);

JSC::JSValue JSC_HOST_CALL functionQsTranslate(JSC::ExecState *exec, JSC::JSObject*, JSC::JSValue, const JSC::ArgList &args)
{
    if (args.size() < 2)
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate() requires at least two arguments");
    if (!args.at(0).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): first argument (context) must be a string");
    if (!args.at(1).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): second argument (text) must be a string");
    if ((args.size() > 2) && !args.at(2).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): third argument (comment) must be a string");
    if ((args.size() > 3) && !args.at(3).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): fourth argument (encoding) must be a string");
    if ((args.size() > 4) && !args.at(4).isNumber())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): fifth argument (n) must be a number");
#ifndef QT_NO_QOBJECT
    QString context(args.at(0).toString(exec));
#endif
    QString text(args.at(1).toString(exec));
#ifndef QT_NO_QOBJECT
    QString comment;
    if (args.size() > 2)
        comment = args.at(2).toString(exec);
    QCoreApplication::Encoding encoding = QCoreApplication::CodecForTr;
    if (args.size() > 3) {
        QString encStr(args.at(3).toString(exec));
        if (encStr == QLatin1String("CodecForTr"))
            encoding = QCoreApplication::CodecForTr;
        else if (encStr == QLatin1String("UnicodeUTF8"))
            encoding = QCoreApplication::UnicodeUTF8;
        else
            return JSC::throwError(exec, JSC::GeneralError, QString::fromLatin1("qsTranslate(): invalid encoding '%s'").arg(encStr));
    }
    int n = -1;
    if (args.size() > 4)
        n = args.at(4).toInt32(exec);
#endif
    QString result;
#ifndef QT_NO_QOBJECT
    result = QCoreApplication::translate(context.toLatin1().constData(),
                                         text.toLatin1().constData(),
                                         comment.toLatin1().constData(),
                                         encoding, n);
#else
    result = text;
#endif
    return JSC::jsString(exec, result);
}

JSC::JSValue JSC_HOST_CALL functionQsTranslateNoOp(JSC::ExecState *, JSC::JSObject*, JSC::JSValue, const JSC::ArgList &args)
{
    if (args.size() < 2)
        return JSC::jsUndefined();
    return args.at(1);
}

JSC::JSValue JSC_HOST_CALL functionQsTr(JSC::ExecState *exec, JSC::JSObject*, JSC::JSValue, const JSC::ArgList &args)
{
    if (args.size() < 1)
        return JSC::throwError(exec, JSC::GeneralError, "qsTr() requires at least one argument");
    if (!args.at(0).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTr(): first argument (text) must be a string");
    if ((args.size() > 1) && !args.at(1).isString())
        return JSC::throwError(exec, JSC::GeneralError, "qsTr(): second argument (comment) must be a string");
    if ((args.size() > 2) && !args.at(2).isNumber())
        return JSC::throwError(exec, JSC::GeneralError, "qsTranslate(): third argument (n) must be a number");
#ifndef QT_NO_QOBJECT
    QString context;
// ### implement context resolution
//    if (ctx->parentContext())
//        context = QFileInfo(ctx->parentContext()->fileName()).baseName();
#endif
    QString text(args.at(0).toString(exec));
#ifndef QT_NO_QOBJECT
    QString comment;
    if (args.size() > 1)
        comment = args.at(1).toString(exec);
    int n = -1;
    if (args.size() > 2)
        n = args.at(2).toInt32(exec);
#endif
    QString result;
#ifndef QT_NO_QOBJECT
    result = QCoreApplication::translate(context.toLatin1().constData(),
                                         text.toLatin1().constData(),
                                         comment.toLatin1().constData(),
                                         QCoreApplication::CodecForTr, n);
#else
    result = text;
#endif
    return JSC::jsString(exec, result);
}

JSC::JSValue JSC_HOST_CALL functionQsTrNoOp(JSC::ExecState *, JSC::JSObject*, JSC::JSValue, const JSC::ArgList &args)
{
    if (args.size() < 1)
        return JSC::jsUndefined();
    return args.at(0);
}

static JSC::JSValue JSC_HOST_CALL stringProtoFuncArg(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);

JSC::JSValue JSC_HOST_CALL stringProtoFuncArg(JSC::ExecState *exec, JSC::JSObject*, JSC::JSValue thisObject, const JSC::ArgList &args)
{
    QString value(thisObject.toString(exec));
    JSC::JSValue arg = (args.size() != 0) ? args.at(0) : JSC::jsUndefined();
    QString result;
    if (arg.isString())
        result = value.arg(arg.toString(exec));
    else if (arg.isNumber())
        result = value.arg(arg.toNumber(exec));
    return JSC::jsString(exec, result);
}


#if !defined(QT_NO_QOBJECT) && !defined(QT_NO_LIBRARY)
static QScriptValue __setupPackage__(QScriptContext *ctx, QScriptEngine *eng)
{
    QString path = ctx->argument(0).toString();
    QStringList components = path.split(QLatin1Char('.'));
    QScriptValue o = eng->globalObject();
    for (int i = 0; i < components.count(); ++i) {
        QString name = components.at(i);
        QScriptValue oo = o.property(name);
        if (!oo.isValid()) {
            oo = eng->newObject();
            o.setProperty(name, oo);
        }
        o = oo;
    }
    return o;
}
#endif

} // namespace QScript

QScriptEnginePrivate::QScriptEnginePrivate()
    : registeredScriptValues(0), freeScriptValues(0),
      registeredScriptStrings(0), inEval(false)
{
    qMetaTypeId<QScriptValue>();
    qMetaTypeId<QList<int> >();
#ifndef QT_NO_QOBJECT
    qMetaTypeId<QObjectList>();
#endif

    if (!QCoreApplication::instance()) {
        qFatal("QScriptEngine: Must construct a Q(Core)Application before a QScriptEngine");
        return;
    }
    JSC::initializeThreading();

    globalData = JSC::JSGlobalData::create().releaseRef();
    globalData->clientData = new QScript::GlobalClientData(this);
    JSC::JSGlobalObject *globalObject = new (globalData)QScript::GlobalObject();

    JSC::ExecState* exec = globalObject->globalExec();

    scriptObjectStructure = QScriptObject::createStructure(globalObject->objectPrototype());

    qobjectPrototype = new (exec) QScript::QObjectPrototype(exec, QScript::QObjectPrototype::createStructure(globalObject->objectPrototype()), globalObject->prototypeFunctionStructure());
    qobjectWrapperObjectStructure = QScriptObject::createStructure(qobjectPrototype);

    qmetaobjectPrototype = new (exec) QScript::QMetaObjectPrototype(exec, QScript::QMetaObjectPrototype::createStructure(globalObject->objectPrototype()), globalObject->prototypeFunctionStructure());
    qmetaobjectWrapperObjectStructure = QScript::QMetaObjectWrapperObject::createStructure(qmetaobjectPrototype);

    variantPrototype = new (exec) QScript::QVariantPrototype(exec, QScript::QVariantPrototype::createStructure(globalObject->objectPrototype()), globalObject->prototypeFunctionStructure());
    variantWrapperObjectStructure = QScriptObject::createStructure(variantPrototype);

    globalObject->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, globalObject->prototypeFunctionStructure(), 1, JSC::Identifier(exec, "print"), QScript::functionPrint));
    globalObject->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, globalObject->prototypeFunctionStructure(), 0, JSC::Identifier(exec, "gc"), QScript::functionGC));
    globalObject->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, globalObject->prototypeFunctionStructure(), 0, JSC::Identifier(exec, "version"), QScript::functionVersion));

    // ### rather than extending Function.prototype, consider creating a QtSignal.prototype
    globalObject->functionPrototype()->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, globalObject->prototypeFunctionStructure(), 1, JSC::Identifier(exec, "disconnect"), QScript::functionDisconnect));
    globalObject->functionPrototype()->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, globalObject->prototypeFunctionStructure(), 1, JSC::Identifier(exec, "connect"), QScript::functionConnect));

    JSC::TimeoutChecker* originalChecker = globalData->timeoutChecker;
    globalData->timeoutChecker = new QScript::TimeoutCheckerProxy(*originalChecker);
    delete originalChecker;

    currentFrame = exec;

    originalGlobalObjectProxy = 0;
    activeAgent = 0;
    agentLineNumber = -1;
    processEventsInterval = -1;
}

QScriptEnginePrivate::~QScriptEnginePrivate()
{
    //disconnect all loadedScripts and generate all jsc::debugger::scriptUnload events
    QHash<intptr_t,QScript::UStringSourceProviderWithFeedback*>::const_iterator it;
    for (it = loadedScripts.constBegin(); it != loadedScripts.constEnd(); ++it)
        it.value()->disconnectFromEngine();

    while (!ownedAgents.isEmpty())
        delete ownedAgents.takeFirst();

    detachAllRegisteredScriptValues();
    detachAllRegisteredScriptStrings();
    qDeleteAll(m_qobjectData);
    qDeleteAll(m_typeInfos);
    JSC::JSLock lock(false);
    globalData->heap.destroy();
    globalData->deref();
    while (freeScriptValues) {
        QScriptValuePrivate *p = freeScriptValues;
        freeScriptValues = p->next;
        qFree(p);
    }
}

QScriptValue QScriptEnginePrivate::scriptValueFromVariant(const QVariant &v)
{
    Q_Q(QScriptEngine);
    QScriptValue result = q->create(v.userType(), v.data());
    Q_ASSERT(result.isValid());
    return result;
}

QVariant QScriptEnginePrivate::scriptValueToVariant(const QScriptValue &value, int targetType)
{
    QVariant v(targetType, (void *)0);
    if (QScriptEnginePrivate::convert(value, targetType, v.data(), this))
        return v;
    if (uint(targetType) == QVariant::LastType)
        return value.toVariant();
    if (value.isVariant()) {
        v = value.toVariant();
        if (v.canConvert(QVariant::Type(targetType))) {
            v.convert(QVariant::Type(targetType));
            return v;
        }
        QByteArray typeName = v.typeName();
        if (typeName.endsWith('*')
            && (QMetaType::type(typeName.left(typeName.size()-1)) == targetType)) {
            return QVariant(targetType, *reinterpret_cast<void* *>(v.data()));
        }
    }

    return QVariant();
}

JSC::JSValue QScriptEnginePrivate::jscValueFromVariant(const QVariant &v)
{
    // ### it's inefficient to convert to QScriptValue and then to JSValue
    QScriptValue vv = scriptValueFromVariant(v);
    QScriptValuePrivate *p = QScriptValuePrivate::get(vv);
    switch (p->type) {
    case QScriptValuePrivate::JavaScriptCore:
        return p->jscValue;
    case QScriptValuePrivate::Number:
        return JSC::jsNumber(currentFrame, p->numberValue);
    case QScriptValuePrivate::String: {
        JSC::UString str = p->stringValue;
        return JSC::jsString(currentFrame, str);
      }
    }
    return JSC::JSValue();
}

QVariant QScriptEnginePrivate::jscValueToVariant(JSC::JSValue value, int targetType)
{
    // ### it's inefficient to convert to QScriptValue and then to QVariant
    return scriptValueToVariant(scriptValueFromJSCValue(value), targetType);
}

QScriptValue QScriptEnginePrivate::arrayFromStringList(const QStringList &lst)
{
    Q_Q(QScriptEngine);
    QScriptValue arr = q->newArray(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        arr.setProperty(i, QScriptValue(q, lst.at(i)));
    return arr;
}

QStringList QScriptEnginePrivate::stringListFromArray(const QScriptValue &arr)
{
    QStringList lst;
    uint len = arr.property(QLatin1String("length")).toUInt32();
    for (uint i = 0; i < len; ++i)
        lst.append(arr.property(i).toString());
    return lst;
}

QScriptValue QScriptEnginePrivate::arrayFromVariantList(const QVariantList &lst)
{
    Q_Q(QScriptEngine);
    QScriptValue arr = q->newArray(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        arr.setProperty(i, scriptValueFromVariant(lst.at(i)));
    return arr;
}

QVariantList QScriptEnginePrivate::variantListFromArray(const QScriptValue &arr)
{
    QVariantList lst;
    uint len = arr.property(QLatin1String("length")).toUInt32();
    for (uint i = 0; i < len; ++i)
        lst.append(arr.property(i).toVariant());
    return lst;
}

QScriptValue QScriptEnginePrivate::objectFromVariantMap(const QVariantMap &vmap)
{
    Q_Q(QScriptEngine);
    QScriptValue obj = q->newObject();
    QVariantMap::const_iterator it;
    for (it = vmap.constBegin(); it != vmap.constEnd(); ++it)
        obj.setProperty(it.key(), scriptValueFromVariant(it.value()));
    return obj;
}

QVariantMap QScriptEnginePrivate::variantMapFromObject(const QScriptValue &obj)
{
    QVariantMap vmap;
    QScriptValueIterator it(obj);
    while (it.hasNext()) {
        it.next();
        vmap.insert(it.name(), it.value().toVariant());
    }
    return vmap;
}

JSC::JSValue QScriptEnginePrivate::defaultPrototype(int metaTypeId) const
{
    QScriptTypeInfo *info = m_typeInfos.value(metaTypeId);
    if (!info)
        return JSC::JSValue();
    return info->prototype;
}

void QScriptEnginePrivate::setDefaultPrototype(int metaTypeId, JSC::JSValue prototype)
{
    QScriptTypeInfo *info = m_typeInfos.value(metaTypeId);
    if (!info) {
        info = new QScriptTypeInfo();
        m_typeInfos.insert(metaTypeId, info);
    }
    info->prototype = prototype;
}

JSC::JSGlobalObject *QScriptEnginePrivate::originalGlobalObject() const
{
    return globalData->head;
}

JSC::JSObject *QScriptEnginePrivate::customGlobalObject() const
{
    QScript::GlobalObject *glob = static_cast<QScript::GlobalObject*>(originalGlobalObject());
    return glob->customGlobalObject;
}

JSC::JSObject *QScriptEnginePrivate::getOriginalGlobalObjectProxy()
{
    if (!originalGlobalObjectProxy) {
        JSC::ExecState* exec = currentFrame;
        originalGlobalObjectProxy = new (exec)QScript::OriginalGlobalObjectProxy(scriptObjectStructure, originalGlobalObject());
    }
    return originalGlobalObjectProxy;
}

JSC::JSObject *QScriptEnginePrivate::globalObject() const
{
    QScript::GlobalObject *glob = static_cast<QScript::GlobalObject*>(originalGlobalObject());
    if (glob->customGlobalObject)
        return glob->customGlobalObject;
    return glob;
}

void QScriptEnginePrivate::setGlobalObject(JSC::JSObject *object)
{
    if (object == globalObject())
        return;
    QScript::GlobalObject *glob = static_cast<QScript::GlobalObject*>(originalGlobalObject());
    if (object == originalGlobalObjectProxy)
        glob->customGlobalObject = 0;
    else {
        Q_ASSERT(object != originalGlobalObject());
        glob->customGlobalObject = object;
    }
}

/*!
  \internal

  If the given \a value is the original global object, returns the custom
  global object or a proxy to the original global object; otherwise returns \a
  value.
*/
JSC::JSValue QScriptEnginePrivate::toUsableValue(JSC::JSValue value)
{
    if (!value || !value.isObject() || !JSC::asObject(value)->isGlobalObject())
        return value;
    Q_ASSERT(JSC::asObject(value) == originalGlobalObject());
    if (customGlobalObject())
        return customGlobalObject();
    if (!originalGlobalObjectProxy)
        originalGlobalObjectProxy = new (currentFrame)QScript::OriginalGlobalObjectProxy(scriptObjectStructure, originalGlobalObject());
    return originalGlobalObjectProxy;
}
/*!
    \internal
    Return the 'this' value for a given context
*/
JSC::JSValue QScriptEnginePrivate::thisForContext(JSC::ExecState *frame)
{
    if (frame->codeBlock() != 0) {
        return frame->thisValue();
    } else if(frame == frame->lexicalGlobalObject()->globalExec()) {
        return frame->globalThisValue();
    } else {
        JSC::Register *thisRegister = thisRegisterForFrame(frame);
        return thisRegister->jsValue();
    }
}

JSC::Register* QScriptEnginePrivate::thisRegisterForFrame(JSC::ExecState *frame)
{
    Q_ASSERT(frame->codeBlock() == 0); // only for native calls
    return frame->registers() - JSC::RegisterFile::CallFrameHeaderSize - frame->argumentCount();
}

/*! \internal
     For native context, we use the ReturnValueRegister entry in the stackframe header to store flags.
     We can do that because this header is not used as the native function return their value thought C++

     when setting flags, NativeContext should always be set

     contextFlags returns 0 for non native context
 */
uint QScriptEnginePrivate::contextFlags(JSC::ExecState *exec)
{
    if (exec->codeBlock())
        return 0; //js function doesn't have flags

    return exec->returnValueRegister();
}

void QScriptEnginePrivate::setContextFlags(JSC::ExecState *exec, uint flags)
{
    Q_ASSERT(!exec->codeBlock());
    exec->registers()[JSC::RegisterFile::ReturnValueRegister] = JSC::Register::withInt(flags);
}


void QScriptEnginePrivate::mark(JSC::MarkStack& markStack)
{
    Q_Q(QScriptEngine);

    markStack.append(originalGlobalObject());
    markStack.append(globalObject());
    if (originalGlobalObjectProxy)
        markStack.append(originalGlobalObjectProxy);

    if (qobjectPrototype)
        markStack.append(qobjectPrototype);
    if (qmetaobjectPrototype)
        markStack.append(qmetaobjectPrototype);
    if (variantPrototype)
        markStack.append(variantPrototype);

    {
        QScriptValuePrivate *it;
        for (it = registeredScriptValues; it != 0; it = it->next) {
            if (it->isJSC())
                markStack.append(it->jscValue);
        }
    }

#ifndef QT_NO_QOBJECT
    {
        QHash<QObject*, QScript::QObjectData*>::const_iterator it;
        for (it = m_qobjectData.constBegin(); it != m_qobjectData.constEnd(); ++it) {
            QScript::QObjectData *qdata = it.value();
            qdata->mark(markStack);
        }
    }
#endif

    {
        QHash<int, QScriptTypeInfo*>::const_iterator it;
        for (it = m_typeInfos.constBegin(); it != m_typeInfos.constEnd(); ++it) {
            if ((*it)->prototype)
                markStack.append((*it)->prototype);
        }
    }

    {
        QScriptContext *context = q->currentContext();

        while (context) {
            JSC::ScopeChainNode *node = frameForContext(context)->scopeChain();
            JSC::ScopeChainIterator it(node);
            for (it = node->begin(); it != node->end(); ++it) {
                JSC::JSObject *object = *it;
                if (object)
                    markStack.append(object);
            }

            context = context->parentContext();
        }
    }
}

bool QScriptEnginePrivate::isCollecting() const
{
    return globalData->heap.isBusy();
}

void QScriptEnginePrivate::collectGarbage()
{
    JSC::JSLock lock(false);
    globalData->heap.collect();
}

QScript::TimeoutCheckerProxy *QScriptEnginePrivate::timeoutChecker() const
{
    return static_cast<QScript::TimeoutCheckerProxy*>(globalData->timeoutChecker);
}

void QScriptEnginePrivate::agentDeleted(QScriptEngineAgent *agent)
{
    ownedAgents.removeOne(agent);
    if (activeAgent == agent) {
        QScriptEngineAgentPrivate::get(agent)->detach();
        activeAgent = 0;
    }
}

JSC::JSValue QScriptEnginePrivate::evaluateHelper(JSC::ExecState *exec, intptr_t sourceId,
                                                  JSC::EvalExecutable *executable,
                                                  bool &compile)
{
    Q_Q(QScriptEngine);
    JSC::JSLock lock(false); // ### hmmm
    QBoolBlocker inEvalBlocker(inEval, true);
    q->currentContext()->activationObject(); //force the creation of a context for native function;

    JSC::Debugger* debugger = originalGlobalObject()->debugger();
    if (debugger)
        debugger->evaluateStart(sourceId);

    q->clearExceptions();
    JSC::DynamicGlobalObjectScope dynamicGlobalObjectScope(exec, exec->scopeChain()->globalObject());

    if (compile) {
        JSC::JSObject* error = executable->compile(exec, exec->scopeChain());
        if (error) {
            compile = false;
            exec->setException(error);

            if (debugger) {
                debugger->exceptionThrow(JSC::DebuggerCallFrame(exec, error), sourceId, false);
                debugger->evaluateStop(error, sourceId);
            }

            return error;
        }
    }

    JSC::JSValue thisValue = thisForContext(exec);
    JSC::JSObject* thisObject = (!thisValue || thisValue.isUndefinedOrNull())
                                ? exec->dynamicGlobalObject() : thisValue.toObject(exec);
    JSC::JSValue exceptionValue;
    timeoutChecker()->setShouldAbort(false);
    if (processEventsInterval > 0)
        timeoutChecker()->reset();

    JSC::JSValue result = exec->interpreter()->execute(executable, exec, thisObject, exec->scopeChain(), &exceptionValue);

    if (timeoutChecker()->shouldAbort()) {
        if (abortResult.isError())
            exec->setException(scriptValueToJSCValue(abortResult));

        if (debugger)
            debugger->evaluateStop(scriptValueToJSCValue(abortResult), sourceId);

        return scriptValueToJSCValue(abortResult);
    }

    if (exceptionValue) {
        exec->setException(exceptionValue);

        if (debugger)
            debugger->evaluateStop(exceptionValue, sourceId);

        return exceptionValue;
    }

    if (debugger)
        debugger->evaluateStop(result, sourceId);

    Q_ASSERT(!exec->hadException());
    return result;
}

#ifndef QT_NO_QOBJECT

JSC::JSValue QScriptEnginePrivate::newQObject(
    QObject *object, QScriptEngine::ValueOwnership ownership,
    const QScriptEngine::QObjectWrapOptions &options)
{
    if (!object)
        return JSC::jsNull();
    JSC::ExecState* exec = currentFrame;
    QScript::QObjectData *data = qobjectData(object);
    bool preferExisting = (options & QScriptEngine::PreferExistingWrapperObject) != 0;
    QScriptEngine::QObjectWrapOptions opt = options & ~QScriptEngine::PreferExistingWrapperObject;
    QScriptObject *result = 0;
    if (preferExisting) {
        result = data->findWrapper(ownership, opt);
        if (result)
            return result;
    }
    result = new (exec) QScriptObject(qobjectWrapperObjectStructure);
    if (preferExisting)
        data->registerWrapper(result, ownership, opt);
    result->setDelegate(new QScript::QObjectDelegate(object, ownership, options));
    /*if (setDefaultPrototype)*/ {
        const QMetaObject *meta = object->metaObject();
        while (meta) {
            QByteArray typeString = meta->className();
            typeString.append('*');
            int typeId = QMetaType::type(typeString);
            if (typeId != 0) {
                JSC::JSValue proto = defaultPrototype(typeId);
                if (proto) {
                    result->setPrototype(proto);
                    break;
                }
            }
            meta = meta->superClass();
        }
    }
    return result;
}

JSC::JSValue QScriptEnginePrivate::newQMetaObject(
    const QMetaObject *metaObject, JSC::JSValue ctor)
{
    if (!metaObject)
        return JSC::jsNull();
    JSC::ExecState* exec = currentFrame;
    QScript::QMetaObjectWrapperObject *result = new (exec) QScript::QMetaObjectWrapperObject(exec, metaObject, ctor, qmetaobjectWrapperObjectStructure);
    return result;
}

bool QScriptEnginePrivate::convertToNativeQObject(const QScriptValue &value,
                                                  const QByteArray &targetType,
                                                  void **result)
{
    if (!targetType.endsWith('*'))
        return false;
    if (QObject *qobject = value.toQObject()) {
        int start = targetType.startsWith("const ") ? 6 : 0;
        QByteArray className = targetType.mid(start, targetType.size()-start-1);
        if (void *instance = qobject->qt_metacast(className)) {
            *result = instance;
            return true;
        }
    }
    return false;
}

QScript::QObjectData *QScriptEnginePrivate::qobjectData(QObject *object)
{
    QHash<QObject*, QScript::QObjectData*>::const_iterator it;
    it = m_qobjectData.constFind(object);
    if (it != m_qobjectData.constEnd())
        return it.value();

    QScript::QObjectData *data = new QScript::QObjectData(this);
    m_qobjectData.insert(object, data);
    QObject::connect(object, SIGNAL(destroyed(QObject*)),
                     q_func(), SLOT(_q_objectDestroyed(QObject *)));
    return data;
}

void QScriptEnginePrivate::_q_objectDestroyed(QObject *object)
{
    QHash<QObject*, QScript::QObjectData*>::iterator it;
    it = m_qobjectData.find(object);
    Q_ASSERT(it != m_qobjectData.end());
    QScript::QObjectData *data = it.value();
    m_qobjectData.erase(it);
    delete data;
}

void QScriptEnginePrivate::disposeQObject(QObject *object)
{
    // TODO
/*    if (isCollecting()) {
        // wait until we're done with GC before deleting it
        int index = m_qobjectsToBeDeleted.indexOf(object);
        if (index == -1)
            m_qobjectsToBeDeleted.append(object);
            } else*/ {
        delete object;
    }
}

void QScriptEnginePrivate::emitSignalHandlerException()
{
    Q_Q(QScriptEngine);
    emit q->signalHandlerException(q->uncaughtException());
}

bool QScriptEnginePrivate::scriptConnect(QObject *sender, const char *signal,
                                         JSC::JSValue receiver, JSC::JSValue function,
                                         Qt::ConnectionType type)
{
    Q_ASSERT(sender);
    Q_ASSERT(signal);
    const QMetaObject *meta = sender->metaObject();
    int index = meta->indexOfSignal(QMetaObject::normalizedSignature(signal+1));
    if (index == -1)
        return false;
    return scriptConnect(sender, index, receiver, function, /*wrapper=*/JSC::JSValue(), type);
}

bool QScriptEnginePrivate::scriptDisconnect(QObject *sender, const char *signal,
                                            JSC::JSValue receiver, JSC::JSValue function)
{
    Q_ASSERT(sender);
    Q_ASSERT(signal);
    const QMetaObject *meta = sender->metaObject();
    int index = meta->indexOfSignal(QMetaObject::normalizedSignature(signal+1));
    if (index == -1)
        return false;
    return scriptDisconnect(sender, index, receiver, function);
}

bool QScriptEnginePrivate::scriptConnect(QObject *sender, int signalIndex,
                                         JSC::JSValue receiver, JSC::JSValue function,
                                         JSC::JSValue senderWrapper,
                                         Qt::ConnectionType type)
{
    QScript::QObjectData *data = qobjectData(sender);
    return data->addSignalHandler(sender, signalIndex, receiver, function, senderWrapper, type);
}

bool QScriptEnginePrivate::scriptDisconnect(QObject *sender, int signalIndex,
                                            JSC::JSValue receiver, JSC::JSValue function)
{
    QScript::QObjectData *data = qobjectData(sender);
    if (!data)
        return false;
    return data->removeSignalHandler(sender, signalIndex, receiver, function);
}

bool QScriptEnginePrivate::scriptConnect(JSC::JSValue signal, JSC::JSValue receiver,
                                         JSC::JSValue function, Qt::ConnectionType type)
{
    QScript::QtFunction *fun = static_cast<QScript::QtFunction*>(JSC::asObject(signal));
    int index = fun->mostGeneralMethod();
    return scriptConnect(fun->qobject(), index, receiver, function, fun->wrapperObject(), type);
}

bool QScriptEnginePrivate::scriptDisconnect(JSC::JSValue signal, JSC::JSValue receiver,
                                            JSC::JSValue function)
{
    QScript::QtFunction *fun = static_cast<QScript::QtFunction*>(JSC::asObject(signal));
    int index = fun->mostGeneralMethod();
    return scriptDisconnect(fun->qobject(), index, receiver, function);
}

#endif

void QScriptEnginePrivate::detachAllRegisteredScriptValues()
{
    QScriptValuePrivate *it;
    QScriptValuePrivate *next;
    for (it = registeredScriptValues; it != 0; it = next) {
        it->detachFromEngine();
        next = it->next;
        it->prev = 0;
        it->next = 0;
    }
    registeredScriptValues = 0;
}

void QScriptEnginePrivate::detachAllRegisteredScriptStrings()
{
    QScriptStringPrivate *it;
    QScriptStringPrivate *next;
    for (it = registeredScriptStrings; it != 0; it = next) {
        it->detachFromEngine();
        next = it->next;
        it->prev = 0;
        it->next = 0;
    }
    registeredScriptStrings = 0;
}

#ifdef QT_NO_QOBJECT

QScriptEngine::QScriptEngine()
    : d_ptr(new QScriptEnginePrivate)
{
    d_ptr->q_ptr = this;
}

/*! \internal
*/
QScriptEngine::QScriptEngine(QScriptEnginePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}
#else

/*!
    Constructs a QScriptEngine object.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/
QScriptEngine::QScriptEngine()
    : QObject(*new QScriptEnginePrivate, 0)
{
}

/*!
    Constructs a QScriptEngine object with the given \a parent.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/

QScriptEngine::QScriptEngine(QObject *parent)
    : QObject(*new QScriptEnginePrivate, parent)
{
}

/*! \internal
*/
QScriptEngine::QScriptEngine(QScriptEnginePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}
#endif

/*!
  Destroys this QScriptEngine.
*/
QScriptEngine::~QScriptEngine()
{
#ifdef QT_NO_QOBJECT
    delete d_ptr;
    d_ptr = 0;
#endif
}

/*!
  Returns this engine's Global Object.

  By default, the Global Object contains the built-in objects that are
  part of \l{ECMA-262}, such as Math, Date and String. Additionally,
  you can set properties of the Global Object to make your own
  extensions available to all script code. Non-local variables in
  script code will be created as properties of the Global Object, as
  well as local variables in global code.
*/
QScriptValue QScriptEngine::globalObject() const
{
    Q_D(const QScriptEngine);
    JSC::JSObject *result = d->globalObject();
    return const_cast<QScriptEnginePrivate*>(d)->scriptValueFromJSCValue(result);
}

/*!
  \since 4.5

  Sets this engine's Global Object to be the given \a object.
  If \a object is not a valid script object, this function does
  nothing.

  When setting a custom global object, you may want to use
  QScriptValueIterator to copy the properties of the standard Global
  Object; alternatively, you can set the internal prototype of your
  custom object to be the original Global Object.
*/
void QScriptEngine::setGlobalObject(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    if (!object.isObject())
        return;
    JSC::JSObject *jscObject = JSC::asObject(d->scriptValueToJSCValue(object));
    d->setGlobalObject(jscObject);
}

/*!
  Returns a QScriptValue of the primitive type Null.

  \sa undefinedValue()
*/
QScriptValue QScriptEngine::nullValue()
{
    Q_D(QScriptEngine);
    return d->scriptValueFromJSCValue(JSC::jsNull());
}

/*!
  Returns a QScriptValue of the primitive type Undefined.

  \sa nullValue()
*/
QScriptValue QScriptEngine::undefinedValue()
{
    Q_D(QScriptEngine);
    return d->scriptValueFromJSCValue(JSC::jsUndefined());
}

/*!
  Creates a constructor function from \a fun, with the given \a length.
  The \c{prototype} property of the resulting function is set to be the
  given \a prototype. The \c{constructor} property of \a prototype is
  set to be the resulting function.

  When a function is called as a constructor (e.g. \c{new Foo()}), the
  `this' object associated with the function call is the new object
  that the function is expected to initialize; the prototype of this
  default constructed object will be the function's public
  \c{prototype} property. If you always want the function to behave as
  a constructor (e.g. \c{Foo()} should also create a new object), or
  if you need to create your own object rather than using the default
  `this' object, you should make sure that the prototype of your
  object is set correctly; either by setting it manually, or, when
  wrapping a custom type, by having registered the defaultPrototype()
  of that type. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 9

  To wrap a custom type and provide a constructor for it, you'd typically
  do something like this:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 10
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun,
                                        const QScriptValue &prototype,
                                        int length)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue function = new (exec)QScript::FunctionWrapper(exec, length, JSC::Identifier(exec, ""), fun);
    QScriptValue result = d->scriptValueFromJSCValue(function);
    result.setProperty(QLatin1String("prototype"), prototype, QScriptValue::Undeletable);
    const_cast<QScriptValue&>(prototype)
        .setProperty(QLatin1String("constructor"), result,
                     QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    return result;
}

#ifndef QT_NO_REGEXP

extern QString qt_regexp_toCanonical(const QString &, QRegExp::PatternSyntax);

/*!
  Creates a QtScript object of class RegExp with the given
  \a regexp.

  \sa QScriptValue::toRegExp()
*/
QScriptValue QScriptEngine::newRegExp(const QRegExp &regexp)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue buf[2];
    JSC::ArgList args(buf, sizeof(buf));

    //convert the pattern to a ECMAScript pattern
    QString pattern = qt_regexp_toCanonical(regexp.pattern(), regexp.patternSyntax());
    if (regexp.isMinimal()) {
        QString ecmaPattern;
        int len = pattern.length();
        ecmaPattern.reserve(len);
        int i = 0;
        const QChar *wc = pattern.unicode();
        bool inBracket = false;
        while (i < len) {
            QChar c = wc[i++];
            ecmaPattern += c;
            switch (c.unicode()) {
            case '?':
            case '+':
            case '*':
            case '}':
                if (!inBracket)
                    ecmaPattern += QLatin1Char('?');
                break;
            case '\\':
                if (i < len)
                    ecmaPattern += wc[i++];
                break;
            case '[':
                inBracket = true;
                break;
            case ']':
                inBracket = false;
                break;
            default:
                break;
            }
        }
        pattern = ecmaPattern;
    }

    JSC::UString jscPattern = pattern;
    QString flags;
    if (regexp.caseSensitivity() == Qt::CaseInsensitive)
        flags.append(QLatin1Char('i'));
    JSC::UString jscFlags = flags;
    buf[0] = JSC::jsString(exec, jscPattern);
    buf[1] = JSC::jsString(exec, jscFlags);
    JSC::JSObject* result = JSC::constructRegExp(exec, args);
    return d->scriptValueFromJSCValue(result);
}

#endif // QT_NO_REGEXP

/*!
  Creates a QtScript object holding the given variant \a value.

  If a default prototype has been registered with the meta type id of
  \a value, then the prototype of the created object will be that
  prototype; otherwise, the prototype will be the Object prototype
  object.

  \sa setDefaultPrototype(), QScriptValue::toVariant()
*/
QScriptValue QScriptEngine::newVariant(const QVariant &value)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    QScriptObject *obj = new (exec) QScriptObject(d->variantWrapperObjectStructure);
    obj->setDelegate(new QScript::QVariantDelegate(value));
    QScriptValue result = d->scriptValueFromJSCValue(obj);
    QScriptValue proto = defaultPrototype(value.userType());
    if (proto.isValid())
        result.setPrototype(proto);
    return result;
}

/*!
  \since 4.4
  \overload

  Initializes the given Qt Script \a object to hold the given variant
  \a value, and returns the \a object.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a variant, or to replace
  the variant contained inside an object previously created by the
  newVariant() function.

  The prototype() of the \a object will remain unchanged.

  If \a object is not an object, this function behaves like the normal
  newVariant(), i.e. it creates a new script object and returns it.

  This function is useful when you want to provide a script
  constructor for a C++ type. If your constructor is invoked in a
  \c{new} expression (QScriptContext::isCalledAsConstructor() returns
  true), you can pass QScriptContext::thisObject() (the default
  constructed script object) to this function to initialize the new
  object.
*/
QScriptValue QScriptEngine::newVariant(const QScriptValue &object,
                                       const QVariant &value)
{
    if (!object.isObject())
        return newVariant(value);
    JSC::JSObject *jscObject = JSC::asObject(QScriptValuePrivate::get(object)->jscValue);
    if (!jscObject->inherits(&QScriptObject::info)) {
        qWarning("QScriptEngine::newVariant(): changing class of non-QScriptObject not supported");
        return QScriptValue();
    }
    QScriptObject *jscScriptObject = static_cast<QScriptObject*>(jscObject);
    if (!object.isVariant()) {
        jscScriptObject->setDelegate(new QScript::QVariantDelegate(value));
    } else {
        QScriptValuePrivate::get(object)->setVariantValue(value);
    }
    return object;
}

#ifndef QT_NO_QOBJECT
/*!
  Creates a QtScript object that wraps the given QObject \a
  object, using the given \a ownership. The given \a options control
  various aspects of the interaction with the resulting script object.

  Signals and slots, properties and children of \a object are
  available as properties of the created QScriptValue. For more
  information, see the \l{QtScript} documentation.

  If \a object is a null pointer, this function returns nullValue().

  If a default prototype has been registered for the \a object's class
  (or its superclass, recursively), the prototype of the new script
  object will be set to be that default prototype.

  If the given \a object is deleted outside of QtScript's control, any
  attempt to access the deleted QObject's members through the QtScript
  wrapper object (either by script code or C++) will result in a
  script exception.

  \sa QScriptValue::toQObject()
*/
QScriptValue QScriptEngine::newQObject(QObject *object, ValueOwnership ownership,
                                       const QObjectWrapOptions &options)
{
    Q_D(QScriptEngine);
    JSC::JSValue jscQObject = d->newQObject(object, ownership, options);
    return d->scriptValueFromJSCValue(jscQObject);
}

/*!
  \since 4.4
  \overload

  Initializes the given \a scriptObject to hold the given \a qtObject,
  and returns the \a scriptObject.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a QObject proxy, or to
  replace the QObject contained inside an object previously created by
  the newQObject() function.

  The prototype() of the \a scriptObject will remain unchanged.

  If \a scriptObject is not an object, this function behaves like the
  normal newQObject(), i.e. it creates a new script object and returns
  it.

  This function is useful when you want to provide a script
  constructor for a QObject-based class. If your constructor is
  invoked in a \c{new} expression
  (QScriptContext::isCalledAsConstructor() returns true), you can pass
  QScriptContext::thisObject() (the default constructed script object)
  to this function to initialize the new object.
*/
QScriptValue QScriptEngine::newQObject(const QScriptValue &scriptObject,
                                       QObject *qtObject,
                                       ValueOwnership ownership,
                                       const QObjectWrapOptions &options)
{
    if (!scriptObject.isObject())
        return newQObject(qtObject, ownership, options);
    JSC::JSObject *jscObject = JSC::asObject(QScriptValuePrivate::get(scriptObject)->jscValue);
    if (!jscObject->inherits(&QScriptObject::info)) {
        qWarning("QScriptEngine::newQObject(): changing class of non-QScriptObject not supported");
        return QScriptValue();
    }
    QScriptObject *jscScriptObject = static_cast<QScriptObject*>(jscObject);
    if (!scriptObject.isQObject()) {
        jscScriptObject->setDelegate(new QScript::QObjectDelegate(qtObject, ownership, options));
    } else {
        QScript::QObjectDelegate *delegate = static_cast<QScript::QObjectDelegate*>(jscScriptObject->delegate());
        delegate->setValue(qtObject);
        delegate->setOwnership(ownership);
        delegate->setOptions(options);
    }
    return scriptObject;
}

#endif // QT_NO_QOBJECT

/*!
  Creates a QtScript object of class Object.

  The prototype of the created object will be the Object
  prototype object.

  \sa newArray(), QScriptValue::setProperty()
*/
QScriptValue QScriptEngine::newObject()
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSObject *result = new (exec)QScriptObject(d->scriptObjectStructure);
    return d->scriptValueFromJSCValue(result);
}

/*!
  \since 4.4
  \overload

  Creates a QtScript Object of the given class, \a scriptClass.

  The prototype of the created object will be the Object
  prototype object.

  \a data, if specified, is set as the internal data of the
  new object (using QScriptValue::setData()).

  \sa QScriptValue::scriptClass()
*/
QScriptValue QScriptEngine::newObject(QScriptClass *scriptClass,
                                      const QScriptValue &data)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    QScriptObject *result = new (exec) QScriptObject(d->scriptObjectStructure);
    result->setDelegate(new QScript::ClassObjectDelegate(scriptClass));
    QScriptValue scriptObject = d->scriptValueFromJSCValue(result);
    scriptObject.setData(data);
    QScriptValue proto = scriptClass->prototype();
    if (proto.isValid())
        scriptObject.setPrototype(proto);
    return scriptObject;
}

/*!
  \internal
*/
QScriptValue QScriptEngine::newActivationObject()
{
    qWarning("QScriptEngine::newActivationObject() not implemented");
    // ### JSActivation or JSVariableObject?
    return QScriptValue();
}

/*!
  Creates a QScriptValue that wraps a native (C++) function. \a fun
  must be a C++ function with signature QScriptEngine::FunctionSignature.  \a
  length is the number of arguments that \a fun expects; this becomes
  the \c{length} property of the created QScriptValue.

  Note that \a length only gives an indication of the number of
  arguments that the function expects; an actual invocation of a
  function can include any number of arguments. You can check the
  \l{QScriptContext::argumentCount()}{argumentCount()} of the
  QScriptContext associated with the invocation to determine the
  actual number of arguments passed.

  A \c{prototype} property is automatically created for the resulting
  function object, to provide for the possibility that the function
  will be used as a constructor.

  By combining newFunction() and the property flags
  QScriptValue::PropertyGetter and QScriptValue::PropertySetter, you
  can create script object properties that behave like normal
  properties in script code, but are in fact accessed through
  functions (analogous to how properties work in \l{Qt's Property
  System}). Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 11

  When the property \c{foo} of the script object is subsequently
  accessed in script code, \c{getSetFoo()} will be invoked to handle
  the access.  In this particular case, we chose to store the "real"
  value of \c{foo} as a property of the accessor function itself; you
  are of course free to do whatever you like in this function.

  In the above example, a single native function was used to handle
  both reads and writes to the property; the argument count is used to
  determine if we are handling a read or write. You can also use two
  separate functions; just specify the relevant flag
  (QScriptValue::PropertyGetter or QScriptValue::PropertySetter) when
  setting the property, e.g.:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 12

  \sa QScriptValue::call()
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun, int length)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue function = new (exec)QScript::FunctionWrapper(exec, length, JSC::Identifier(exec, ""), fun);
    QScriptValue result = d->scriptValueFromJSCValue(function);
    QScriptValue proto = newObject();
    result.setProperty(QLatin1String("prototype"), proto, QScriptValue::Undeletable);
    proto.setProperty(QLatin1String("constructor"), result,
                      QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    return result;
}

/*!
  \internal
  \since 4.4
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue function = new (exec)QScript::FunctionWithArgWrapper(exec, /*length=*/0, JSC::Identifier(exec, ""), fun, arg);
    QScriptValue result = d->scriptValueFromJSCValue(function);
    QScriptValue proto = newObject();
    result.setProperty(QLatin1String("prototype"), proto, QScriptValue::Undeletable);
    proto.setProperty(QLatin1String("constructor"), result,
                      QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    return result;
}

/*!
  Creates a QtScript object of class Array with the given \a length.

  \sa newObject()
*/
QScriptValue QScriptEngine::newArray(uint length)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSArray* result = JSC::constructEmptyArray(exec, length);
    return d->scriptValueFromJSCValue(result);
}

/*!
  Creates a QtScript object of class RegExp with the given
  \a pattern and \a flags.

  The legal flags are 'g' (global), 'i' (ignore case), and 'm'
  (multiline).
*/
QScriptValue QScriptEngine::newRegExp(const QString &pattern, const QString &flags)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue buf[2];
    JSC::ArgList args(buf, sizeof(buf));
    JSC::UString jscPattern = pattern;
    QString strippedFlags;
    if (flags.contains(QLatin1Char('i')))
        strippedFlags += QLatin1Char('i');
    if (flags.contains(QLatin1Char('m')))
        strippedFlags += QLatin1Char('m');
    if (flags.contains(QLatin1Char('g')))
        strippedFlags += QLatin1Char('g');
    JSC::UString jscFlags = strippedFlags;
    buf[0] = JSC::jsString(exec, jscPattern);
    buf[1] = JSC::jsString(exec, jscFlags);
    JSC::JSObject* result = JSC::constructRegExp(exec, args);
    return d->scriptValueFromJSCValue(result);
}

/*!
  Creates a QtScript object of class Date with the given
  \a value (the number of milliseconds since 01 January 1970,
  UTC).
*/
QScriptValue QScriptEngine::newDate(qsreal value)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue val = JSC::jsNumber(exec, value);
    JSC::ArgList args(&val, 1);
    JSC::JSObject *result = JSC::constructDate(exec, args);
    return d->scriptValueFromJSCValue(result);
}

/*!
  Creates a QtScript object of class Date from the given \a value.

  \sa QScriptValue::toDateTime()
*/
QScriptValue QScriptEngine::newDate(const QDateTime &value)
{
    return newDate(QScript::FromDateTime(value));
}

#ifndef QT_NO_QOBJECT
/*!
  Creates a QtScript object that represents a QObject class, using the
  the given \a metaObject and constructor \a ctor.

  Enums of \a metaObject (declared with Q_ENUMS) are available as
  properties of the created QScriptValue. When the class is called as
  a function, \a ctor will be called to create a new instance of the
  class.

  Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 27

  \sa newQObject(), scriptValueFromQMetaObject()
*/
QScriptValue QScriptEngine::newQMetaObject(
    const QMetaObject *metaObject, const QScriptValue &ctor)
{
    Q_D(QScriptEngine);
    JSC::JSValue jscCtor = d->scriptValueToJSCValue(ctor);
    JSC::JSValue jscQMetaObject = d->newQMetaObject(metaObject, jscCtor);
    return d->scriptValueFromJSCValue(jscQMetaObject);
}

/*!
  \fn QScriptValue QScriptEngine::scriptValueFromQMetaObject()

  Creates a QScriptValue that represents the Qt class \c{T}.

  This function is used in combination with one of the
  Q_SCRIPT_DECLARE_QMETAOBJECT() macro. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 13

  \warning This function is not available with MSVC 6. Use
  qScriptValueFromQMetaObject() instead if you need to support that version
  of the compiler.

  \sa QScriptEngine::newQMetaObject()
*/

/*!
  \fn QScriptValue qScriptValueFromQMetaObject(QScriptEngine *engine)
  \since 4.3
  \relates QScriptEngine

  Uses \a engine to create a QScriptValue that represents the Qt class
  \c{T}.

  This function is equivalent to
  QScriptEngine::scriptValueFromQMetaObject(). It is provided as a
  work-around for MSVC 6, which doesn't support member template
  functions.

  \sa QScriptEngine::newQMetaObject()
*/
#endif // QT_NO_QOBJECT

/*!
  \obsolete

  Returns true if \a program can be evaluated; i.e. the code is
  sufficient to determine whether it appears to be a syntactically
  correct program, or contains a syntax error.

  This function returns false if \a program is incomplete; i.e. the
  input is syntactically correct up to the point where the input is
  terminated.

  Note that this function only does a static check of \a program;
  e.g. it does not check whether references to variables are
  valid, and so on.

  A typical usage of canEvaluate() is to implement an interactive
  interpreter for QtScript. The user is repeatedly queried for
  individual lines of code; the lines are concatened internally, and
  only when canEvaluate() returns true for the resulting program is it
  passed to evaluate().

  The following are some examples to illustrate the behavior of
  canEvaluate(). (Note that all example inputs are assumed to have an
  explicit newline as their last character, since otherwise the
  QtScript parser would automatically insert a semi-colon character at
  the end of the input, and this could cause canEvaluate() to produce
  different results.)

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 14
  canEvaluate() will return true, since the program appears to be complete.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 15
  canEvaluate() will return false, since the if-statement is not complete,
  but is syntactically correct so far.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 16
  canEvaluate() will return true, but evaluate() will throw a
  SyntaxError given the same input.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 17
  canEvaluate() will return true, even though the code is clearly not
  syntactically valid QtScript code. evaluate() will throw a
  SyntaxError when this code is evaluated.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 18
  canEvaluate() will return true, but evaluate() will throw a
  ReferenceError if \c{foo} is not defined in the script
  environment.

  \sa evaluate(), checkSyntax()
*/
bool QScriptEngine::canEvaluate(const QString &program) const
{
    return QScriptEnginePrivate::canEvaluate(program);
}


bool QScriptEnginePrivate::canEvaluate(const QString &program)
{
    QScript::SyntaxChecker checker;
    QScript::SyntaxChecker::Result result = checker.checkSyntax(program);
    return (result.state != QScript::SyntaxChecker::Intermediate);
}

/*!
  \since 4.5

  Checks the syntax of the given \a program. Returns a
  QScriptSyntaxCheckResult object that contains the result of the check.
*/
QScriptSyntaxCheckResult QScriptEngine::checkSyntax(const QString &program)
{
    return QScriptEnginePrivate::checkSyntax(program);
}

QScriptSyntaxCheckResult QScriptEnginePrivate::checkSyntax(const QString &program)
{
    QScript::SyntaxChecker checker;
    QScript::SyntaxChecker::Result result = checker.checkSyntax(program);
    QScriptSyntaxCheckResultPrivate *p = new QScriptSyntaxCheckResultPrivate();
    switch (result.state) {
    case QScript::SyntaxChecker::Error:
        p->state = QScriptSyntaxCheckResult::Error;
        break;
    case QScript::SyntaxChecker::Intermediate:
        p->state = QScriptSyntaxCheckResult::Intermediate;
        break;
    case QScript::SyntaxChecker::Valid:
        p->state = QScriptSyntaxCheckResult::Valid;
        break;
    }
    p->errorLineNumber = result.errorLineNumber;
    p->errorColumnNumber = result.errorColumnNumber;
    p->errorMessage = result.errorMessage;
    return QScriptSyntaxCheckResult(p);
}



/*!
  Evaluates \a program, using \a lineNumber as the base line number,
  and returns the result of the evaluation.

  The script code will be evaluated in the current context.

  The evaluation of \a program can cause an exception in the
  engine; in this case the return value will be the exception
  that was thrown (typically an \c{Error} object). You can call
  hasUncaughtException() to determine if an exception occurred in
  the last call to evaluate().

  \a lineNumber is used to specify a starting line number for \a
  program; line number information reported by the engine that pertain
  to this evaluation (e.g. uncaughtExceptionLineNumber()) will be
  based on this argument. For example, if \a program consists of two
  lines of code, and the statement on the second line causes a script
  exception, uncaughtExceptionLineNumber() would return the given \a
  lineNumber plus one. When no starting line number is specified, line
  numbers will be 1-based.

  \a fileName is used for error reporting. For example in error objects
  the file name is accessible through the "fileName" property if it's
  provided with this function.

  \sa canEvaluate(), hasUncaughtException(), isEvaluating(), abortEvaluation()
*/

QScriptValue QScriptEngine::evaluate(const QString &program, const QString &fileName, int lineNumber)
{
    Q_D(QScriptEngine);
    WTF::PassRefPtr<QScript::UStringSourceProviderWithFeedback> provider
            = QScript::UStringSourceProviderWithFeedback::create(program, fileName, lineNumber, d);
    intptr_t sourceId = provider->asID();
    JSC::SourceCode source(provider, lineNumber); //after construction of SourceCode provider variable will be null.

    JSC::ExecState* exec = d->currentFrame;
    JSC::EvalExecutable executable(exec, source);
    bool compile = true;
    return d->scriptValueFromJSCValue(d->evaluateHelper(exec, sourceId, &executable, compile));
}

/*!
  \internal
  \since 4.6

  Evaluates the given \a program and returns the result of the
  evaluation.
*/
QScriptValue QScriptEngine::evaluate(const QScriptProgram &program)
{
    Q_D(QScriptEngine);
    QScriptProgramPrivate *program_d = QScriptProgramPrivate::get(program);
    if (!program_d)
        return QScriptValue();

    JSC::ExecState* exec = d->currentFrame;
    JSC::EvalExecutable *executable = program_d->executable(exec, d);
    bool compile = !program_d->isCompiled;
    JSC::JSValue result = d->evaluateHelper(exec, program_d->sourceId,
                                            executable, compile);
    if (compile)
        program_d->isCompiled = true;
    return d->scriptValueFromJSCValue(result);
}

/*!
  Returns the current context.

  The current context is typically accessed to retrieve the arguments
  and `this' object in native functions; for convenience, it is
  available as the first argument in QScriptEngine::FunctionSignature.
*/
QScriptContext *QScriptEngine::currentContext() const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->contextForFrame(d->currentFrame);
}

/*!
  Enters a new execution context and returns the associated
  QScriptContext object.

  Once you are done with the context, you should call popContext() to
  restore the old context.

  By default, the `this' object of the new context is the Global Object.
  The context's \l{QScriptContext::callee()}{callee}() will be invalid.

  This function is useful when you want to evaluate script code
  as if it were the body of a function. You can use the context's
  \l{QScriptContext::activationObject()}{activationObject}() to initialize
  local variables that will be available to scripts. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 19

  In the above example, the new variable "tmp" defined in the script
  will be local to the context; in other words, the script doesn't
  have any effect on the global environment.

  Returns 0 in case of stack overflow

  \sa popContext()
*/
QScriptContext *QScriptEngine::pushContext()
{
    Q_D(QScriptEngine);

    JSC::CallFrame* newFrame = d->pushContext(d->currentFrame, d->currentFrame->globalData().dynamicGlobalObject,
                                              JSC::ArgList(), /*callee = */0);

    if (agent())
        agent()->contextPush();

    return d->contextForFrame(newFrame);
}

/*! \internal
   push a context for a native function.
   JSC native function doesn't have different stackframe or context. so we need to create one.

   use popContext right after to go back to the previous context the context if no stack overflow has hapenned

   exec is the current top frame.

   return the new top frame. (might be the same as exec if a new stackframe was not needed) or 0 if stack overflow
*/
JSC::CallFrame *QScriptEnginePrivate::pushContext(JSC::CallFrame *exec, JSC::JSValue _thisObject,
                                                  const JSC::ArgList& args, JSC::JSObject *callee, bool calledAsConstructor,
                                                  bool clearScopeChain)
{
    JSC::JSValue thisObject = _thisObject;
    if (calledAsConstructor) {
        //JSC doesn't create default created object for native functions. so we do it
        JSC::JSValue prototype = callee->get(exec, exec->propertyNames().prototype);
        JSC::Structure *structure = prototype.isObject() ? JSC::asObject(prototype)->inheritorID()
                                                         : originalGlobalObject()->emptyObjectStructure();
        thisObject = new (exec) QScriptObject(structure);
    }

    int flags = NativeContext;
    if (calledAsConstructor)
        flags |= CalledAsConstructorContext;

    //build a frame
    JSC::CallFrame *newCallFrame = exec;
    if (callee == 0 //called from  public QScriptEngine::pushContext
        || exec->returnPC() == 0 || (contextFlags(exec) & NativeContext) //called from native-native call
        || (exec->codeBlock() && exec->callee() != callee)) { //the interpreter did not build a frame for us.
        //We need to check if the Interpreter might have already created a frame for function called from JS.
        JSC::Interpreter *interp = exec->interpreter();
        JSC::Register *oldEnd = interp->registerFile().end();
        int argc = args.size() + 1; //add "this"
        JSC::Register *newEnd = oldEnd + argc + JSC::RegisterFile::CallFrameHeaderSize;
        if (!interp->registerFile().grow(newEnd))
            return 0; //### Stack overflow
        newCallFrame = JSC::CallFrame::create(oldEnd);
        newCallFrame[0] = thisObject;
        int dst = 0;
        JSC::ArgList::const_iterator it;
        for (it = args.begin(); it != args.end(); ++it)
            newCallFrame[++dst] = *it;
        newCallFrame += argc + JSC::RegisterFile::CallFrameHeaderSize;

        if (!clearScopeChain) {
            newCallFrame->init(0, /*vPC=*/0, exec->scopeChain(), exec, flags | ShouldRestoreCallFrame, argc, callee);
        } else {
            JSC::JSObject *jscObject = originalGlobalObject();
            JSC::ScopeChainNode *scn = new JSC::ScopeChainNode(0, jscObject, &exec->globalData(), jscObject);
            newCallFrame->init(0, /*vPC=*/0, scn, exec, flags | ShouldRestoreCallFrame, argc, callee);
        }
    } else {
        setContextFlags(newCallFrame, flags);
#if ENABLE(JIT)
        exec->registers()[JSC::RegisterFile::Callee] = JSC::JSValue(callee); //JIT let the callee set the 'callee'
#endif
        if (calledAsConstructor) {
            //update the new created this
            JSC::Register* thisRegister = thisRegisterForFrame(newCallFrame);
            *thisRegister = thisObject;
        }
    }
    currentFrame = newCallFrame;
    return newCallFrame;
}


/*!
  Pops the current execution context and restores the previous one.
  This function must be used in conjunction with pushContext().

  \sa pushContext()
*/
void QScriptEngine::popContext()
{
    if (agent())
        agent()->contextPop();
    Q_D(QScriptEngine);
    if (d->currentFrame->returnPC() != 0 || d->currentFrame->codeBlock() != 0
        || !currentContext()->parentContext()) {
        qWarning("QScriptEngine::popContext() doesn't match with pushContext()");
        return;
    }

    d->popContext();
}

/*! \internal
    counter part of QScriptEnginePrivate::pushContext
 */
void QScriptEnginePrivate::popContext()
{
    uint flags = contextFlags(currentFrame);
    bool hasScope = flags & HasScopeContext;
    if (flags & ShouldRestoreCallFrame) { //normal case
        JSC::RegisterFile &registerFile = currentFrame->interpreter()->registerFile();
        JSC::Register *const newEnd = currentFrame->registers() - JSC::RegisterFile::CallFrameHeaderSize - currentFrame->argumentCount();
        if (hasScope)
            currentFrame->scopeChain()->pop()->deref();
        registerFile.shrink(newEnd);
    } else if(hasScope) { //the stack frame was created by the Interpreter, we don't need to rewind it.
        currentFrame->setScopeChain(currentFrame->scopeChain()->pop());
        currentFrame->scopeChain()->deref();
    }
    currentFrame = currentFrame->callerFrame();
}

/*!
  Returns true if the last script evaluation resulted in an uncaught
  exception; otherwise returns false.

  The exception state is cleared when evaluate() is called.

  \sa uncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
bool QScriptEngine::hasUncaughtException() const
{
    Q_D(const QScriptEngine);
    JSC::ExecState* exec = d->globalExec();
    return exec->hadException() || d->currentException().isValid();
}

/*!
  Returns the current uncaught exception, or an invalid QScriptValue
  if there is no uncaught exception.

  The exception value is typically an \c{Error} object; in that case,
  you can call toString() on the return value to obtain an error
  message.

  \sa hasUncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
QScriptValue QScriptEngine::uncaughtException() const
{
    Q_D(const QScriptEngine);
    QScriptValue result;
    JSC::ExecState* exec = d->globalExec();
    if (exec->hadException())
        result = const_cast<QScriptEnginePrivate*>(d)->scriptValueFromJSCValue(exec->exception());
    else
        result = d->currentException();
    return result;
}

/*!
  Returns the line number where the last uncaught exception occurred.

  Line numbers are 1-based, unless a different base was specified as
  the second argument to evaluate().

  \sa hasUncaughtException(), uncaughtExceptionBacktrace()
*/
int QScriptEngine::uncaughtExceptionLineNumber() const
{
    if (!hasUncaughtException())
        return -1;
    return uncaughtException().property(QLatin1String("lineNumber")).toInt32();
}

/*!
  Returns a human-readable backtrace of the last uncaught exception.

  Each line is of the form \c{<function-name>(<arguments>)@<file-name>:<line-number>}.

  \sa uncaughtException()
*/
QStringList QScriptEngine::uncaughtExceptionBacktrace() const
{
    if (!hasUncaughtException())
        return QStringList();
// ### currently no way to get a full backtrace from JSC without installing a
// debugger that reimplements exception() and store the backtrace there.
    QScriptValue value = uncaughtException();
    if (!value.isError())
        return QStringList();
    QStringList result;
    result.append(QString::fromLatin1("<anonymous>()@%0:%1")
                  .arg(value.property(QLatin1String("fileName")).toString())
                  .arg(value.property(QLatin1String("lineNumber")).toInt32()));
    return result;
}

/*!
  \since 4.4

  Clears any uncaught exceptions in this engine.

  \sa hasUncaughtException()
*/
void QScriptEngine::clearExceptions()
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    exec->clearException();
    d->clearCurrentException();
}

/*!
  Returns the default prototype associated with the given \a metaTypeId,
  or an invalid QScriptValue if no default prototype has been set.

  \sa setDefaultPrototype()
*/
QScriptValue QScriptEngine::defaultPrototype(int metaTypeId) const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->scriptValueFromJSCValue(d->defaultPrototype(metaTypeId));
}

/*!
  Sets the default prototype of the C++ type identified by the given
  \a metaTypeId to \a prototype.

  The default prototype provides a script interface for values of
  type \a metaTypeId when a value of that type is accessed from script
  code.  Whenever the script engine (implicitly or explicitly) creates
  a QScriptValue from a value of type \a metaTypeId, the default
  prototype will be set as the QScriptValue's prototype.

  The \a prototype object itself may be constructed using one of two
  principal techniques; the simplest is to subclass QScriptable, which
  enables you to define the scripting API of the type through QObject
  properties and slots.  Another possibility is to create a script
  object by calling newObject(), and populate the object with the
  desired properties (e.g. native functions wrapped with
  newFunction()).

  \sa defaultPrototype(), qScriptRegisterMetaType(), QScriptable, {Default Prototypes Example}
*/
void QScriptEngine::setDefaultPrototype(int metaTypeId, const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    d->setDefaultPrototype(metaTypeId, d->scriptValueToJSCValue(prototype));
}

/*!
  \typedef QScriptEngine::FunctionSignature
  \relates QScriptEngine

  The function signature \c{QScriptValue f(QScriptContext *, QScriptEngine *)}.

  A function with such a signature can be passed to
  QScriptEngine::newFunction() to wrap the function.
*/

/*!
  \typedef QScriptEngine::FunctionWithArgSignature
  \relates QScriptEngine

  The function signature \c{QScriptValue f(QScriptContext *, QScriptEngine *, void *)}.

  A function with such a signature can be passed to
  QScriptEngine::newFunction() to wrap the function.
*/

/*!
    \typedef QScriptEngine::MarshalFunction
    \internal
*/

/*!
    \typedef QScriptEngine::DemarshalFunction
    \internal
*/

/*!
    \internal
*/
QScriptValue QScriptEngine::create(int type, const void *ptr)
{
    Q_D(QScriptEngine);
    return d->create(type, ptr);
}

QScriptValue QScriptEnginePrivate::create(int type, const void *ptr)
{
    Q_Q(QScriptEngine);
    Q_ASSERT(ptr != 0);
    QScriptValue result;
    QScriptTypeInfo *info = m_typeInfos.value(type);
    if (info && info->marshal) {
        result = info->marshal(q, ptr);
    } else {
        // check if it's one of the types we know
        switch (QMetaType::Type(type)) {
        case QMetaType::Void:
            return QScriptValue(q, QScriptValue::UndefinedValue);
        case QMetaType::Bool:
            return QScriptValue(q, *reinterpret_cast<const bool*>(ptr));
        case QMetaType::Int:
            return QScriptValue(q, *reinterpret_cast<const int*>(ptr));
        case QMetaType::UInt:
            return QScriptValue(q, *reinterpret_cast<const uint*>(ptr));
        case QMetaType::LongLong:
            return QScriptValue(q, qsreal(*reinterpret_cast<const qlonglong*>(ptr)));
        case QMetaType::ULongLong:
#if defined(Q_OS_WIN) && defined(_MSC_FULL_VER) && _MSC_FULL_VER <= 12008804
#pragma message("** NOTE: You need the Visual Studio Processor Pack to compile support for 64bit unsigned integers.")
            return QScriptValue(q, qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(ptr)));
#elif defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
            return QScriptValue(q, qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(ptr)));
#else
            return QScriptValue(q, qsreal(*reinterpret_cast<const qulonglong*>(ptr)));
#endif
        case QMetaType::Double:
            return QScriptValue(q, qsreal(*reinterpret_cast<const double*>(ptr)));
        case QMetaType::QString:
            return QScriptValue(q, *reinterpret_cast<const QString*>(ptr));
        case QMetaType::Float:
            return QScriptValue(q, *reinterpret_cast<const float*>(ptr));
        case QMetaType::Short:
            return QScriptValue(q, *reinterpret_cast<const short*>(ptr));
        case QMetaType::UShort:
            return QScriptValue(q, *reinterpret_cast<const unsigned short*>(ptr));
        case QMetaType::Char:
            return QScriptValue(q, *reinterpret_cast<const char*>(ptr));
        case QMetaType::UChar:
            return QScriptValue(q, *reinterpret_cast<const unsigned char*>(ptr));
        case QMetaType::QChar:
            return QScriptValue(q, (*reinterpret_cast<const QChar*>(ptr)).unicode());
        case QMetaType::QStringList:
            result = arrayFromStringList(*reinterpret_cast<const QStringList *>(ptr));
            break;
        case QMetaType::QVariantList:
            result = arrayFromVariantList(*reinterpret_cast<const QVariantList *>(ptr));
            break;
        case QMetaType::QVariantMap:
            result = objectFromVariantMap(*reinterpret_cast<const QVariantMap *>(ptr));
            break;
        case QMetaType::QDateTime:
            result = q->newDate(*reinterpret_cast<const QDateTime *>(ptr));
            break;
        case QMetaType::QDate:
            result = q->newDate(QDateTime(*reinterpret_cast<const QDate *>(ptr)));
            break;
#ifndef QT_NO_REGEXP
        case QMetaType::QRegExp:
            result = q->newRegExp(*reinterpret_cast<const QRegExp *>(ptr));
            break;
#endif
#ifndef QT_NO_QOBJECT
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
            result = q->newQObject(*reinterpret_cast<QObject* const *>(ptr));
            break;
#endif
        default:
            if (type == qMetaTypeId<QScriptValue>()) {
                result = *reinterpret_cast<const QScriptValue*>(ptr);
                if (!result.isValid())
                    return QScriptValue(q, QScriptValue::UndefinedValue);
            }

#ifndef QT_NO_QOBJECT
            // lazy registration of some common list types
            else if (type == qMetaTypeId<QObjectList>()) {
                qScriptRegisterSequenceMetaType<QObjectList>(q);
                return create(type, ptr);
            }
#endif
            else if (type == qMetaTypeId<QList<int> >()) {
                qScriptRegisterSequenceMetaType<QList<int> >(q);
                return create(type, ptr);
            }

            else {
                QByteArray typeName = QMetaType::typeName(type);
                if (typeName == "QVariant")
                    result = scriptValueFromVariant(*reinterpret_cast<const QVariant*>(ptr));
                if (typeName.endsWith('*') && !*reinterpret_cast<void* const *>(ptr))
                    return QScriptValue(q, QScriptValue::NullValue);
                else
                    result = q->newVariant(QVariant(type, ptr));
            }
        }
    }
    if (result.isObject() && info && info->prototype
        && JSC::JSValue::strictEqual(scriptValueToJSCValue(result.prototype()), originalGlobalObject()->objectPrototype())) {
        result.setPrototype(scriptValueFromJSCValue(info->prototype));
    }
    return result;
}

bool QScriptEnginePrivate::convert(const QScriptValue &value,
                                   int type, void *ptr,
                                   QScriptEnginePrivate *eng)
{
    if (!eng)
        eng = QScriptValuePrivate::getEngine(value);
    if (eng) {
        QScriptTypeInfo *info = eng->m_typeInfos.value(type);
        if (info && info->demarshal) {
            info->demarshal(value, ptr);
            return true;
        }
    }

    // check if it's one of the types we know
    switch (QMetaType::Type(type)) {
    case QMetaType::Bool:
        *reinterpret_cast<bool*>(ptr) = value.toBoolean();
        return true;
    case QMetaType::Int:
        *reinterpret_cast<int*>(ptr) = value.toInt32();
        return true;
    case QMetaType::UInt:
        *reinterpret_cast<uint*>(ptr) = value.toUInt32();
        return true;
    case QMetaType::LongLong:
        *reinterpret_cast<qlonglong*>(ptr) = qlonglong(value.toInteger());
        return true;
    case QMetaType::ULongLong:
        *reinterpret_cast<qulonglong*>(ptr) = qulonglong(value.toInteger());
        return true;
    case QMetaType::Double:
        *reinterpret_cast<double*>(ptr) = value.toNumber();
        return true;
    case QMetaType::QString:
        if (value.isUndefined() || value.isNull())
            *reinterpret_cast<QString*>(ptr) = QString();
        else
            *reinterpret_cast<QString*>(ptr) = value.toString();
        return true;
    case QMetaType::Float:
        *reinterpret_cast<float*>(ptr) = value.toNumber();
        return true;
    case QMetaType::Short:
        *reinterpret_cast<short*>(ptr) = short(value.toInt32());
        return true;
    case QMetaType::UShort:
        *reinterpret_cast<unsigned short*>(ptr) = value.toUInt16();
        return true;
    case QMetaType::Char:
        *reinterpret_cast<char*>(ptr) = char(value.toInt32());
        return true;
    case QMetaType::UChar:
        *reinterpret_cast<unsigned char*>(ptr) = (unsigned char)(value.toInt32());
        return true;
    case QMetaType::QChar:
        if (value.isString()) {
            QString str = value.toString();
            *reinterpret_cast<QChar*>(ptr) = str.isEmpty() ? QChar() : str.at(0);
        } else {
            *reinterpret_cast<QChar*>(ptr) = QChar(value.toUInt16());
        }
        return true;
    case QMetaType::QDateTime:
        if (value.isDate()) {
            *reinterpret_cast<QDateTime *>(ptr) = value.toDateTime();
            return true;
        } break;
    case QMetaType::QDate:
        if (value.isDate()) {
            *reinterpret_cast<QDate *>(ptr) = value.toDateTime().date();
            return true;
        } break;
#ifndef QT_NO_REGEXP
    case QMetaType::QRegExp:
        if (value.isRegExp()) {
            *reinterpret_cast<QRegExp *>(ptr) = value.toRegExp();
            return true;
        } break;
#endif
#ifndef QT_NO_QOBJECT
    case QMetaType::QObjectStar:
        if (value.isQObject() || value.isNull()) {
            *reinterpret_cast<QObject* *>(ptr) = value.toQObject();
            return true;
        } break;
    case QMetaType::QWidgetStar:
        if (value.isQObject() || value.isNull()) {
            QObject *qo = value.toQObject();
            if (!qo || qo->isWidgetType()) {
                *reinterpret_cast<QWidget* *>(ptr) = reinterpret_cast<QWidget*>(qo);
                return true;
            }
        } break;
#endif
    case QMetaType::QStringList:
        if (value.isArray()) {
            *reinterpret_cast<QStringList *>(ptr) = stringListFromArray(value);
            return true;
        } break;
    case QMetaType::QVariantList:
        if (value.isArray()) {
            *reinterpret_cast<QVariantList *>(ptr) = variantListFromArray(value);
            return true;
        } break;
    case QMetaType::QVariantMap:
        if (value.isObject()) {
            *reinterpret_cast<QVariantMap *>(ptr) = variantMapFromObject(value);
            return true;
        } break;
    default:
    ;
    }

    QByteArray name = QMetaType::typeName(type);
#ifndef QT_NO_QOBJECT
    if (convertToNativeQObject(value, name, reinterpret_cast<void* *>(ptr)))
        return true;
#endif
    if (value.isVariant() && name.endsWith('*')) {
        int valueType = QMetaType::type(name.left(name.size()-1));
        QVariant &var = QScriptValuePrivate::get(value)->variantValue();
        if (valueType == var.userType()) {
            *reinterpret_cast<void* *>(ptr) = var.data();
            return true;
        } else {
            // look in the prototype chain
            QScriptValue proto = value.prototype();
            while (proto.isObject()) {
                bool canCast = false;
                if (proto.isVariant()) {
                    canCast = (type == proto.toVariant().userType())
                              || (valueType && (valueType == proto.toVariant().userType()));
                }
#ifndef QT_NO_QOBJECT
                else if (proto.isQObject()) {
                    QByteArray className = name.left(name.size()-1);
                    if (QObject *qobject = proto.toQObject())
                        canCast = qobject->qt_metacast(className) != 0;
                }
#endif
                if (canCast) {
                    QByteArray varTypeName = QMetaType::typeName(var.userType());
                    if (varTypeName.endsWith('*'))
                        *reinterpret_cast<void* *>(ptr) = *reinterpret_cast<void* *>(var.data());
                    else
                        *reinterpret_cast<void* *>(ptr) = var.data();
                    return true;
                }
                proto = proto.prototype();
            }
        }
    } else if (value.isNull() && name.endsWith('*')) {
        *reinterpret_cast<void* *>(ptr) = 0;
        return true;
    } else if (type == qMetaTypeId<QScriptValue>()) {
        if (!eng)
            return false;
        *reinterpret_cast<QScriptValue*>(ptr) = value;
        return true;
    } else if (name == "QVariant") {
        *reinterpret_cast<QVariant*>(ptr) = value.toVariant();
        return true;
    }

    // lazy registration of some common list types
#ifndef QT_NO_QOBJECT
    else if (type == qMetaTypeId<QObjectList>()) {
        if (!eng)
            return false;
        qScriptRegisterSequenceMetaType<QObjectList>(eng->q_func());
        return convert(value, type, ptr, eng);
    }
#endif
    else if (type == qMetaTypeId<QList<int> >()) {
        if (!eng)
            return false;
        qScriptRegisterSequenceMetaType<QList<int> >(eng->q_func());
        return convert(value, type, ptr, eng);
    }

#if 0
    if (!name.isEmpty()) {
        qWarning("QScriptEngine::convert: unable to convert value to type `%s'",
                 name.constData());
    }
#endif
    return false;
}

bool QScriptEnginePrivate::hasDemarshalFunction(int type) const
{
    QScriptTypeInfo *info = m_typeInfos.value(type);
    return info && (info->demarshal != 0);
}

/*!
    \internal
*/
bool QScriptEngine::convert(const QScriptValue &value, int type, void *ptr)
{
    Q_D(QScriptEngine);
    return QScriptEnginePrivate::convert(value, type, ptr, d);
}

/*!
    \internal
*/
bool QScriptEngine::convertV2(const QScriptValue &value, int type, void *ptr)
{
    return QScriptEnginePrivate::convert(value, type, ptr, /*engine=*/0);
}

/*!
    \internal
*/
void QScriptEngine::registerCustomType(int type, MarshalFunction mf,
                                       DemarshalFunction df,
                                       const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    QScriptTypeInfo *info = d->m_typeInfos.value(type);
    if (!info) {
        info = new QScriptTypeInfo();
        d->m_typeInfos.insert(type, info);
    }
    info->marshal = mf;
    info->demarshal = df;
    info->prototype = d->scriptValueToJSCValue(prototype);
}

/*!
  \since 4.5

  Installs translator functions on the given \a object, or on the Global
  Object if no object is specified.

  The relation between Qt Script translator functions and C++ translator
  functions is described in the following table:

    \table
    \header \o Script Function \o Corresponding C++ Function
    \row    \o qsTr()       \o QObject::tr()
    \row    \o QT_TR_NOOP() \o QT_TR_NOOP()
    \row    \o qsTranslate() \o QCoreApplication::translate()
    \row    \o QT_TRANSLATE_NOOP() \o QT_TRANSLATE_NOOP()
    \endtable

  \sa {Internationalization with Qt}
*/
void QScriptEngine::installTranslatorFunctions(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue jscObject = d->scriptValueToJSCValue(object);
    JSC::JSGlobalObject *glob = d->originalGlobalObject();
    if (!jscObject || !jscObject.isObject())
        jscObject = glob;
//    unsigned attribs = JSC::DontEnum;
    JSC::asObject(jscObject)->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, glob->prototypeFunctionStructure(), 5, JSC::Identifier(exec, "qsTranslate"), QScript::functionQsTranslate));
    JSC::asObject(jscObject)->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, glob->prototypeFunctionStructure(), 2, JSC::Identifier(exec, "QT_TRANSLATE_NOOP"), QScript::functionQsTranslateNoOp));
    JSC::asObject(jscObject)->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, glob->prototypeFunctionStructure(), 3, JSC::Identifier(exec, "qsTr"), QScript::functionQsTr));
    JSC::asObject(jscObject)->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, glob->prototypeFunctionStructure(), 1, JSC::Identifier(exec, "QT_TR_NOOP"), QScript::functionQsTrNoOp));

    glob->stringPrototype()->putDirectFunction(exec, new (exec)JSC::NativeFunctionWrapper(exec, glob->prototypeFunctionStructure(), 1, JSC::Identifier(exec, "arg"), QScript::stringProtoFuncArg));
}

/*!
    Imports the given \a extension into this QScriptEngine.  Returns
    undefinedValue() if the extension was successfully imported. You
    can call hasUncaughtException() to check if an error occurred; in
    that case, the return value is the value that was thrown by the
    exception (usually an \c{Error} object).

    QScriptEngine ensures that a particular extension is only imported
    once; subsequent calls to importExtension() with the same extension
    name will do nothing and return undefinedValue().

    \sa availableExtensions(), QScriptExtensionPlugin, {Creating QtScript Extensions}
*/
QScriptValue QScriptEngine::importExtension(const QString &extension)
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(extension);
#else
    Q_D(QScriptEngine);
    if (d->importedExtensions.contains(extension))
        return undefinedValue(); // already imported

    QScriptContext *context = currentContext();
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return context->throwError(QLatin1String("No application object"));

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    QStringList libraryPaths = app->libraryPaths();
    QString dot = QLatin1String(".");
    QStringList pathComponents = extension.split(dot);
    QString initDotJs = QLatin1String("__init__.js");

    QString ext;
    for (int i = 0; i < pathComponents.count(); ++i) {
        if (!ext.isEmpty())
            ext.append(dot);
        ext.append(pathComponents.at(i));
        if (d->importedExtensions.contains(ext))
            continue; // already imported

        if (d->extensionsBeingImported.contains(ext)) {
            return context->throwError(QString::fromLatin1("recursive import of %0")
                                       .arg(extension));
        }
        d->extensionsBeingImported.insert(ext);

        QScriptExtensionInterface *iface = 0;
        QString initjsContents;
        QString initjsFileName;

        // look for the extension in static plugins
        for (int j = 0; j < staticPlugins.size(); ++j) {
            iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(j));
            if (!iface)
                continue;
            if (iface->keys().contains(ext))
                break; // use this one
            else
                iface = 0; // keep looking
        }

        {
            // look for __init__.js resource
            QString path = QString::fromLatin1(":/qtscriptextension");
            for (int j = 0; j <= i; ++j) {
                path.append(QLatin1Char('/'));
                path.append(pathComponents.at(j));
            }
            path.append(QLatin1Char('/'));
            path.append(initDotJs);
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream ts(&file);
                initjsContents = ts.readAll();
                initjsFileName = path;
                file.close();
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            // look for the extension in library paths
            for (int j = 0; j < libraryPaths.count(); ++j) {
                QString libPath = libraryPaths.at(j) + QDir::separator() + QLatin1String("script");
                QDir dir(libPath);
                if (!dir.exists(dot))
                    continue;

                // look for C++ plugin
                QFileInfoList files = dir.entryInfoList(QDir::Files);
                for (int k = 0; k < files.count(); ++k) {
                    QFileInfo entry = files.at(k);
                    QString filePath = entry.canonicalFilePath();
                    QPluginLoader loader(filePath);
                    iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
                    if (iface) {
                        if (iface->keys().contains(ext))
                            break; // use this one
                        else
                            iface = 0; // keep looking
                    }
                }

                // look for __init__.js in the corresponding dir
                QDir dirdir(libPath);
                bool dirExists = dirdir.exists();
                for (int k = 0; dirExists && (k <= i); ++k)
                    dirExists = dirdir.cd(pathComponents.at(k));
                if (dirExists && dirdir.exists(initDotJs)) {
                    QFile file(dirdir.canonicalPath()
                               + QDir::separator() + initDotJs);
                    if (file.open(QIODevice::ReadOnly)) {
                        QTextStream ts(&file);
                        initjsContents = ts.readAll();
                        initjsFileName = file.fileName();
                        file.close();
                    }
                }

                if (iface || !initjsContents.isEmpty())
                    break;
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            d->extensionsBeingImported.remove(ext);
            return context->throwError(
                QString::fromLatin1("Unable to import %0: no such extension")
                .arg(extension));
        }

        // initialize the extension in a new context
        QScriptContext *ctx = pushContext();
        ctx->setThisObject(globalObject());
        ctx->activationObject().setProperty(QLatin1String("__extension__"), ext,
                                            QScriptValue::ReadOnly | QScriptValue::Undeletable);
        ctx->activationObject().setProperty(QLatin1String("__setupPackage__"),
                                            newFunction(QScript::__setupPackage__));
        ctx->activationObject().setProperty(QLatin1String("__postInit__"), QScriptValue(QScriptValue::UndefinedValue));

        // the script is evaluated first
        if (!initjsContents.isEmpty()) {
            QScriptValue ret = evaluate(initjsContents, initjsFileName);
            if (hasUncaughtException()) {
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        // next, the C++ plugin is called
        if (iface) {
            iface->initialize(ext, this);
            if (hasUncaughtException()) {
                QScriptValue ret = uncaughtException(); // ctx_p->returnValue();
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        // if the __postInit__ function has been set, we call it
        QScriptValue postInit = ctx->activationObject().property(QLatin1String("__postInit__"));
        if (postInit.isFunction()) {
            postInit.call(globalObject());
            if (hasUncaughtException()) {
                QScriptValue ret = uncaughtException(); // ctx_p->returnValue();
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        popContext();

        d->importedExtensions.insert(ext);
        d->extensionsBeingImported.remove(ext);
    } // for (i)
#endif // QT_NO_QOBJECT
    return undefinedValue();
}

/*!
    \since 4.4

    Returns a list naming the available extensions that can be
    imported using the importExtension() function. This list includes
    extensions that have been imported.

    \sa importExtension(), importedExtensions()
*/
QStringList QScriptEngine::availableExtensions() const
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    return QStringList();
#else
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return QStringList();

    QSet<QString> result;

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (int i = 0; i < staticPlugins.size(); ++i) {
        QScriptExtensionInterface *iface;
        iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(i));
        if (iface) {
            QStringList keys = iface->keys();
            for (int j = 0; j < keys.count(); ++j)
                result << keys.at(j);
        }
    }

    QStringList libraryPaths = app->libraryPaths();
    for (int i = 0; i < libraryPaths.count(); ++i) {
        QString libPath = libraryPaths.at(i) + QDir::separator() + QLatin1String("script");
        QDir dir(libPath);
        if (!dir.exists())
            continue;

        // look for C++ plugins
        QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (int j = 0; j < files.count(); ++j) {
            QFileInfo entry = files.at(j);
            QString filePath = entry.canonicalFilePath();
            QPluginLoader loader(filePath);
            QScriptExtensionInterface *iface;
            iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
            if (iface) {
                QStringList keys = iface->keys();
                for (int k = 0; k < keys.count(); ++k)
                    result << keys.at(k);
            }
        }

        // look for scripts
        QString initDotJs = QLatin1String("__init__.js");
        QList<QFileInfo> stack;
        stack << dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        while (!stack.isEmpty()) {
            QFileInfo entry = stack.takeLast();
            QDir dd(entry.canonicalFilePath());
            if (dd.exists(initDotJs)) {
                QString rpath = dir.relativeFilePath(dd.canonicalPath());
                QStringList components = rpath.split(QLatin1Char('/'));
                result << components.join(QLatin1String("."));
                stack << dd.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            }
        }
    }

    QStringList lst = result.toList();
    qSort(lst);
    return lst;
#endif
}

/*!
    \since 4.4

    Returns a list naming the extensions that have been imported
    using the importExtension() function.

    \sa availableExtensions()
*/
QStringList QScriptEngine::importedExtensions() const
{
    Q_D(const QScriptEngine);
    QStringList lst = d->importedExtensions.toList();
    qSort(lst);
    return lst;
}

/*! \fn QScriptValue QScriptEngine::toScriptValue(const T &value)

    Creates a QScriptValue with the given \a value.

    Note that the template type \c{T} must be known to QMetaType.

    See \l{Conversion Between QtScript and C++ Types} for a
    description of the built-in type conversion provided by
    QtScript. By default, the types that are not specially handled by
    QtScript are represented as QVariants (e.g. the \a value is passed
    to newVariant()); you can change this behavior by installing your
    own type conversion functions with qScriptRegisterMetaType().

    \warning This function is not available with MSVC 6. Use
    qScriptValueFromValue() instead if you need to support that
    version of the compiler.

    \sa fromScriptValue(), qScriptRegisterMetaType()
*/

/*! \fn T QScriptEngine::fromScriptValue(const QScriptValue &value)

    Returns the given \a value converted to the template type \c{T}.

    Note that \c{T} must be known to QMetaType.

    See \l{Conversion Between QtScript and C++ Types} for a
    description of the built-in type conversion provided by
    QtScript.

    \warning This function is not available with MSVC 6. Use
    qScriptValueToValue() or qscriptvalue_cast() instead if you need
    to support that version of the compiler.

    \sa toScriptValue(), qScriptRegisterMetaType()
*/

/*!
    \fn QScriptValue qScriptValueFromValue(QScriptEngine *engine, const T &value)
    \since 4.3
    \relates QScriptEngine

    Creates a QScriptValue using the given \a engine with the given \a
    value of template type \c{T}.

    This function is equivalent to QScriptEngine::toScriptValue().
    It is provided as a work-around for MSVC 6, which doesn't support
    member template functions.

    \sa qScriptValueToValue()
*/

/*!
    \fn T qScriptValueToValue(const QScriptValue &value)
    \since 4.3
    \relates QScriptEngine

    Returns the given \a value converted to the template type \c{T}.

    This function is equivalent to QScriptEngine::fromScriptValue().
    It is provided as a work-around for MSVC 6, which doesn't
    support member template functions.

    \sa qScriptValueFromValue()
*/

/*!
    \fn QScriptValue qScriptValueFromSequence(QScriptEngine *engine, const Container &container)
    \since 4.3
    \relates QScriptEngine

    Creates an array in the form of a QScriptValue using the given \a engine
    with the given \a container of template type \c{Container}.

    The \c Container type must provide a \c const_iterator class to enable the
    contents of the container to be copied into the array.

    Additionally, the type of each element in the sequence should be
    suitable for conversion to a QScriptValue.  See
    \l{Conversion Between QtScript and C++ Types} for more information
    about the restrictions on types that can be used with QScriptValue.

    \sa qScriptValueFromValue()
*/

/*!
    \fn void qScriptValueToSequence(const QScriptValue &value, Container &container)
    \since 4.3
    \relates QScriptEngine

    Copies the elements in the sequence specified by \a value to the given
    \a container of template type \c{Container}.

    The \a value used is typically an array, but any container can be copied
    as long as it provides a \c length property describing how many elements
    it contains.

    Additionally, the type of each element in the sequence must be
    suitable for conversion to a C++ type from a QScriptValue.  See
    \l{Conversion Between QtScript and C++ Types} for more information
    about the restrictions on types that can be used with
    QScriptValue.

    \sa qscriptvalue_cast()
*/

/*!
    \fn T qscriptvalue_cast(const QScriptValue &value)
    \since 4.3
    \relates QScriptValue

    Returns the given \a value converted to the template type \c{T}.

    \sa qScriptRegisterMetaType(), QScriptEngine::toScriptValue()
*/

/*! \fn int qScriptRegisterMetaType(
            QScriptEngine *engine,
            QScriptValue (*toScriptValue)(QScriptEngine *, const T &t),
            void (*fromScriptValue)(const QScriptValue &, T &t),
            const QScriptValue &prototype = QScriptValue())
    \relates QScriptEngine

    Registers the type \c{T} in the given \a engine. \a toScriptValue must
    be a function that will convert from a value of type \c{T} to a
    QScriptValue, and \a fromScriptValue a function that does the
    opposite. \a prototype, if valid, is the prototype that's set on
    QScriptValues returned by \a toScriptValue.

    Returns the internal ID used by QMetaType.

    You only need to call this function if you want to provide custom
    conversion of values of type \c{T}, i.e. if the default
    QVariant-based representation and conversion is not
    appropriate. (Note that custom QObject-derived types also fall in
    this category; e.g. for a QObject-derived class called MyObject,
    you probably want to define conversion functions for MyObject*
    that utilize QScriptEngine::newQObject() and
    QScriptValue::toQObject().)

    If you only want to define a common script interface for values of
    type \c{T}, and don't care how those values are represented
    (i.e. storing them in QVariants is fine), use
    \l{QScriptEngine::setDefaultPrototype()}{setDefaultPrototype}()
    instead; this will minimize conversion costs.

    You need to declare the custom type first with
    Q_DECLARE_METATYPE().

    After a type has been registered, you can convert from a
    QScriptValue to that type using
    \l{QScriptEngine::fromScriptValue()}{fromScriptValue}(), and
    create a QScriptValue from a value of that type using
    \l{QScriptEngine::toScriptValue()}{toScriptValue}(). The engine
    will take care of calling the proper conversion function when
    calling C++ slots, and when getting or setting a C++ property;
    i.e. the custom type may be used seamlessly on both the C++ side
    and the script side.

    The following is an example of how to use this function. We will
    specify custom conversion of our type \c{MyStruct}. Here's the C++
    type:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 20

    We must declare it so that the type will be known to QMetaType:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 21

    Next, the \c{MyStruct} conversion functions. We represent the
    \c{MyStruct} value as a script object and just copy the properties:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 22

    Now we can register \c{MyStruct} with the engine:
    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 23

    Working with \c{MyStruct} values is now easy:
    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 24

    If you want to be able to construct values of your custom type
    from script code, you have to register a constructor function for
    the type. For example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 25

    \sa qScriptRegisterSequenceMetaType(), qRegisterMetaType()
*/

/*!
    \macro Q_SCRIPT_DECLARE_QMETAOBJECT(QMetaObject, ArgType)
    \since 4.3
    \relates QScriptEngine

    Declares the given \a QMetaObject. Used in combination with
    QScriptEngine::scriptValueFromQMetaObject() to make enums and
    instantiation of \a QMetaObject available to script code. The
    constructor generated by this macro takes a single argument of
    type \a ArgType; typically the argument is the parent type of the
    new instance, in which case \a ArgType is \c{QWidget*} or
    \c{QObject*}. Objects created by the constructor will have
    QScriptEngine::AutoOwnership ownership.
*/

/*! \fn int qScriptRegisterSequenceMetaType(
            QScriptEngine *engine,
            const QScriptValue &prototype = QScriptValue())
    \relates QScriptEngine

    Registers the sequence type \c{T} in the given \a engine. This
    function provides conversion functions that convert between \c{T}
    and Qt Script \c{Array} objects. \c{T} must provide a
    const_iterator class and begin(), end() and push_back()
    functions. If \a prototype is valid, it will be set as the
    prototype of \c{Array} objects due to conversion from \c{T};
    otherwise, the standard \c{Array} prototype will be used.

    Returns the internal ID used by QMetaType.

    You need to declare the container type first with
    Q_DECLARE_METATYPE(). If the element type isn't a standard Qt/C++
    type, it must be declared using Q_DECLARE_METATYPE() as well.
    Example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 26

    \sa qScriptRegisterMetaType()
*/

/*!
  Runs the garbage collector.

  The garbage collector will attempt to reclaim memory by locating and
  disposing of objects that are no longer reachable in the script
  environment.

  Normally you don't need to call this function; the garbage collector
  will automatically be invoked when the QScriptEngine decides that
  it's wise to do so (i.e. when a certain number of new objects have
  been created). However, you can call this function to explicitly
  request that garbage collection should be performed as soon as
  possible.
*/
void QScriptEngine::collectGarbage()
{
    Q_D(QScriptEngine);
    d->collectGarbage();
}

/*!

  Sets the interval between calls to QCoreApplication::processEvents
  to \a interval milliseconds.

  While the interpreter is running, all event processing is by default
  blocked. This means for instance that the gui will not be updated
  and timers will not be fired. To allow event processing during
  interpreter execution one can specify the processing interval to be
  a positive value, indicating the number of milliseconds between each
  time QCoreApplication::processEvents() is called.

  The default value is -1, which disables event processing during
  interpreter execution.

  You can use QCoreApplication::postEvent() to post an event that
  performs custom processing at the next interval. For example, you
  could keep track of the total running time of the script and call
  abortEvaluation() when you detect that the script has been running
  for a long time without completing.

  \sa processEventsInterval()
*/
void QScriptEngine::setProcessEventsInterval(int interval)
{
    Q_D(QScriptEngine);
    d->processEventsInterval = interval;

    if (interval > 0)
        d->globalData->timeoutChecker->setCheckInterval(interval);

    d->timeoutChecker()->setShouldProcessEvents(interval > 0);
}

/*!

  Returns the interval in milliseconds between calls to
  QCoreApplication::processEvents() while the interpreter is running.

  \sa setProcessEventsInterval()
*/
int QScriptEngine::processEventsInterval() const
{
    Q_D(const QScriptEngine);
    return d->processEventsInterval;
}

/*!
  \since 4.4

  Returns true if this engine is currently evaluating a script,
  otherwise returns false.

  \sa evaluate(), abortEvaluation()
*/
bool QScriptEngine::isEvaluating() const
{
    Q_D(const QScriptEngine);
    return (d->currentFrame != d->globalExec()) || d->inEval;
}

/*!
  \since 4.4

  Aborts any script evaluation currently taking place in this engine.
  The given \a result is passed back as the result of the evaluation
  (i.e. it is returned from the call to evaluate() being aborted).

  If the engine isn't evaluating a script (i.e. isEvaluating() returns
  false), this function does nothing.

  Call this function if you need to abort a running script for some
  reason, e.g.  when you have detected that the script has been
  running for several seconds without completing.

  \sa evaluate(), isEvaluating(), setProcessEventsInterval()
*/
void QScriptEngine::abortEvaluation(const QScriptValue &result)
{
    Q_D(QScriptEngine);

    d->timeoutChecker()->setShouldAbort(true);
    d->abortResult = result;
}

#ifndef QT_NO_QOBJECT

/*!
  \since 4.4
  \relates QScriptEngine

  Creates a connection from the \a signal in the \a sender to the
  given \a function. If \a receiver is an object, it will act as the
  `this' object when the signal handler function is invoked. Returns
  true if the connection succeeds; otherwise returns false.

  \sa qScriptDisconnect(), QScriptEngine::signalHandlerException()
*/
bool qScriptConnect(QObject *sender, const char *signal,
                    const QScriptValue &receiver, const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction())
        return false;
    if (receiver.isObject() && (receiver.engine() != function.engine()))
        return false;
    QScriptEnginePrivate *engine = QScriptEnginePrivate::get(function.engine());
    JSC::JSValue jscReceiver = engine->scriptValueToJSCValue(receiver);
    JSC::JSValue jscFunction = engine->scriptValueToJSCValue(function);
    return engine->scriptConnect(sender, signal, jscReceiver, jscFunction,
                                 Qt::AutoConnection);
}

/*!
  \since 4.4
  \relates QScriptEngine

  Disconnects the \a signal in the \a sender from the given (\a
  receiver, \a function) pair. Returns true if the connection is
  successfully broken; otherwise returns false.

  \sa qScriptConnect()
*/
bool qScriptDisconnect(QObject *sender, const char *signal,
                       const QScriptValue &receiver, const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction())
        return false;
    if (receiver.isObject() && (receiver.engine() != function.engine()))
        return false;
    QScriptEnginePrivate *engine = QScriptEnginePrivate::get(function.engine());
    JSC::JSValue jscReceiver = engine->scriptValueToJSCValue(receiver);
    JSC::JSValue jscFunction = engine->scriptValueToJSCValue(function);
    return engine->scriptDisconnect(sender, signal, jscReceiver, jscFunction);
}

/*!
    \since 4.4
    \fn void QScriptEngine::signalHandlerException(const QScriptValue &exception)

    This signal is emitted when a script function connected to a signal causes
    an \a exception.

    \sa qScriptConnect()
*/

QT_BEGIN_INCLUDE_NAMESPACE
#include "moc_qscriptengine.cpp"
QT_END_INCLUDE_NAMESPACE

#endif // QT_NO_QOBJECT

/*!
  \since 4.4

  Installs the given \a agent on this engine. The agent will be
  notified of various events pertaining to script execution. This is
  useful when you want to find out exactly what the engine is doing,
  e.g. when evaluate() is called. The agent interface is the basis of
  tools like debuggers and profilers.

  The engine maintains ownership of the \a agent.

  Calling this function will replace the existing agent, if any.

  \sa agent()
*/
void QScriptEngine::setAgent(QScriptEngineAgent *agent)
{
    Q_D(QScriptEngine);
    if (agent && (agent->engine() != this)) {
        qWarning("QScriptEngine::setAgent(): "
                 "cannot set agent belonging to different engine");
        return;
    }
    if (d->activeAgent)
        QScriptEngineAgentPrivate::get(d->activeAgent)->detach();
    d->activeAgent = agent;
    if (agent) {
        QScriptEngineAgentPrivate::get(agent)->attach();
    }
}

/*!
  \since 4.4

  Returns the agent currently installed on this engine, or 0 if no
  agent is installed.

  \sa setAgent()
*/
QScriptEngineAgent *QScriptEngine::agent() const
{
    Q_D(const QScriptEngine);
    return d->activeAgent;
}

/*!
  \since 4.4

  Returns a handle that represents the given string, \a str.

  QScriptString can be used to quickly look up properties, and
  compare property names, of script objects.

  \sa QScriptValue::property()
*/
QScriptString QScriptEngine::toStringHandle(const QString &str)
{
    Q_D(QScriptEngine);
    QScriptString result;
    QScriptStringPrivate *p = new QScriptStringPrivate(d, JSC::Identifier(d->currentFrame, str), QScriptStringPrivate::HeapAllocated);
    QScriptStringPrivate::init(result, p);
    d->registerScriptString(p);
    return result;
}

/*!
  \since 4.5

  Converts the given \a value to an object, if such a conversion is
  possible; otherwise returns an invalid QScriptValue. The conversion
  is performed according to the following table:

    \table
    \header \o Input Type \o Result
    \row    \o Undefined  \o An invalid QScriptValue.
    \row    \o Null       \o An invalid QScriptValue.
    \row    \o Boolean    \o A new Boolean object whose internal value is set to the value of the boolean.
    \row    \o Number     \o A new Number object whose internal value is set to the value of the number.
    \row    \o String     \o A new String object whose internal value is set to the value of the string.
    \row    \o Object     \o The result is the object itself (no conversion).
    \endtable

    \sa newObject()
*/
QScriptValue QScriptEngine::toObject(const QScriptValue &value)
{
    Q_D(QScriptEngine);
    JSC::JSValue jscValue = d->scriptValueToJSCValue(value);
    if (!jscValue || jscValue.isUndefined() || jscValue.isNull())
        return QScriptValue();
    JSC::ExecState* exec = d->currentFrame;
    JSC::JSValue result = jscValue.toObject(exec);
    return d->scriptValueFromJSCValue(result);
}

/*!
  \internal

  Returns the object with the given \a id, or an invalid
  QScriptValue if there is no object with that id.

  \sa QScriptValue::objectId()
*/
QScriptValue QScriptEngine::objectById(qint64 id) const
{
    Q_D(const QScriptEngine);
    // Assumes that the cell was not been garbage collected
    return const_cast<QScriptEnginePrivate*>(d)->scriptValueFromJSCValue((JSC::JSCell*)id);
}

/*!
  \since 4.5
  \class QScriptSyntaxCheckResult

  \brief The QScriptSyntaxCheckResult class provides the result of a script syntax check.

  \ingroup script
  \mainclass

  QScriptSyntaxCheckResult is returned by QScriptEngine::checkSyntax() to
  provide information about the syntactical (in)correctness of a script.
*/

/*!
    \enum QScriptSyntaxCheckResult::State

    This enum specifies the state of a syntax check.

    \value Error The program contains a syntax error.
    \value Intermediate The program is incomplete.
    \value Valid The program is a syntactically correct Qt Script program.
*/

/*!
  Constructs a new QScriptSyntaxCheckResult from the \a other result.
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(const QScriptSyntaxCheckResult &other)
    : d_ptr(other.d_ptr)
{
}

/*!
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(QScriptSyntaxCheckResultPrivate *d)
    : d_ptr(d)
{
}

/*!
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult()
    : d_ptr(0)
{
}

/*!
  Destroys this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::~QScriptSyntaxCheckResult()
{
}

/*!
  Returns the state of this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::State QScriptSyntaxCheckResult::state() const
{
    Q_D(const QScriptSyntaxCheckResult);
    if (!d)
        return Valid;
    return d->state;
}

/*!
  Returns the error line number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorMessage()
*/
int QScriptSyntaxCheckResult::errorLineNumber() const
{
    Q_D(const QScriptSyntaxCheckResult);
    if (!d)
        return -1;
    return d->errorLineNumber;
}

/*!
  Returns the error column number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorLineNumber()
*/
int QScriptSyntaxCheckResult::errorColumnNumber() const
{
    Q_D(const QScriptSyntaxCheckResult);
    if (!d)
        return -1;
    return d->errorColumnNumber;
}

/*!
  Returns the error message of this QScriptSyntaxCheckResult, or an empty
  string if there is no error.

  \sa state(), errorLineNumber()
*/
QString QScriptSyntaxCheckResult::errorMessage() const
{
    Q_D(const QScriptSyntaxCheckResult);
    if (!d)
        return QString();
    return d->errorMessage;
}

/*!
  Assigns the \a other result to this QScriptSyntaxCheckResult, and returns a
  reference to this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult &QScriptSyntaxCheckResult::operator=(const QScriptSyntaxCheckResult &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

#ifdef QT_BUILD_INTERNAL
Q_AUTOTEST_EXPORT bool qt_script_isJITEnabled()
{
#if ENABLE(JIT)
    return true;
#else
    return false;
#endif
}
#endif

QT_END_NAMESPACE
