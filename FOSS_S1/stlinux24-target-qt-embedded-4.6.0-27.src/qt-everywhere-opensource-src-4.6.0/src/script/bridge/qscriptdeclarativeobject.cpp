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

#include "config.h"
#include "qscriptdeclarativeobject_p.h"

#include "../api/qscriptengine.h"
#include "../api/qscriptengine_p.h"
#include "../api/qscriptcontext.h"
#include "../api/qscriptcontext_p.h"
#include "../api/qscriptclass.h"
#include "../api/qscriptclasspropertyiterator.h"

#include "Error.h"
#include "PropertyNameArray.h"

#include <QtCore/qstringlist.h>

Q_DECLARE_METATYPE(QScriptContext*)
Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QScriptValueList)

QT_BEGIN_NAMESPACE

namespace QScript
{

DeclarativeObjectDelegate::DeclarativeObjectDelegate(QScriptDeclarativeClass *c, 
                                                     QScriptDeclarativeClass::Object *o)
: m_class(c), m_object(o)
{
}

DeclarativeObjectDelegate::~DeclarativeObjectDelegate()
{
    delete m_object;
}

QScriptObjectDelegate::Type DeclarativeObjectDelegate::type() const
{
    return DeclarativeClassObject;
}

bool DeclarativeObjectDelegate::getOwnPropertySlot(QScriptObject* object,
                                                   JSC::ExecState *exec,
                                                   const JSC::Identifier &propertyName,
                                                   JSC::PropertySlot &slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScriptDeclarativeClass::Identifier identifier = (void *)propertyName.ustring().rep();

    QScriptDeclarativeClassPrivate *p = QScriptDeclarativeClassPrivate::get(m_class);
    p->context = reinterpret_cast<QScriptContext *>(exec);
    QScriptClass::QueryFlags flags = 
        m_class->queryProperty(m_object, identifier, QScriptClass::HandlesReadAccess);
    if (flags & QScriptClass::HandlesReadAccess) {
        QScriptValue value = m_class->property(m_object, identifier);
        p->context = 0;
        slot.setValue(engine->scriptValueToJSCValue(value));
        return true;
    }
    p->context = 0;

    return QScriptObjectDelegate::getOwnPropertySlot(object, exec, propertyName, slot);
}

void DeclarativeObjectDelegate::put(QScriptObject* object, JSC::ExecState *exec,
                                    const JSC::Identifier &propertyName,
                                    JSC::JSValue value, JSC::PutPropertySlot &slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    QScriptDeclarativeClass::Identifier identifier = (void *)propertyName.ustring().rep();

    QScriptDeclarativeClassPrivate *p = QScriptDeclarativeClassPrivate::get(m_class);
    p->context = reinterpret_cast<QScriptContext *>(exec);
    QScriptClass::QueryFlags flags = 
        m_class->queryProperty(m_object, identifier, QScriptClass::HandlesWriteAccess);
    if (flags & QScriptClass::HandlesWriteAccess) {
        m_class->setProperty(m_object, identifier, engine->scriptValueFromJSCValue(value));
        p->context = 0;
        return;
    }
    p->context = 0;

    QScriptObjectDelegate::put(object, exec, propertyName, value, slot);
}

bool DeclarativeObjectDelegate::deleteProperty(QScriptObject* object, JSC::ExecState *exec,
                                               const JSC::Identifier &propertyName,
                                               bool checkDontDelete)
{
    return QScriptObjectDelegate::deleteProperty(object, exec, propertyName, checkDontDelete);
}

bool DeclarativeObjectDelegate::getPropertyAttributes(const QScriptObject* object, 
                                                      JSC::ExecState *exec,
                                                      const JSC::Identifier &propertyName,
                                                      unsigned &attribs) const
{
    QScriptDeclarativeClass::Identifier identifier = (void *)propertyName.ustring().rep();

    QScriptClass::QueryFlags flags = 
        m_class->queryProperty(m_object, identifier, QScriptClass::HandlesReadAccess);
    if (flags & QScriptClass::HandlesReadAccess) {
        QScriptValue::PropertyFlags flags = m_class->propertyFlags(m_object, identifier);
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

void DeclarativeObjectDelegate::getOwnPropertyNames(QScriptObject* object, JSC::ExecState *exec,
                                                    JSC::PropertyNameArray &propertyNames,
                                                    bool includeNonEnumerable)
{
    QStringList properties = m_class->propertyNames(m_object);
    for (int ii = 0; ii < properties.count(); ++ii) {
        const QString &name = properties.at(ii);
        propertyNames.add(JSC::Identifier(exec, name));
    }

    QScriptObjectDelegate::getOwnPropertyNames(object, exec, propertyNames, includeNonEnumerable);
}

JSC::CallType DeclarativeObjectDelegate::getCallData(QScriptObject *object, JSC::CallData &callData)
{
    return QScriptObjectDelegate::getCallData(object, callData);
}

JSC::ConstructType DeclarativeObjectDelegate::getConstructData(QScriptObject* object, JSC::ConstructData &constructData)
{
    return QScriptObjectDelegate::getConstructData(object, constructData);
}

bool DeclarativeObjectDelegate::hasInstance(QScriptObject* object, JSC::ExecState *exec,
                                            JSC::JSValue value, JSC::JSValue proto)
{
    return QScriptObjectDelegate::hasInstance(object, exec, value, proto);
}

} // namespace QScript

QT_END_NAMESPACE
