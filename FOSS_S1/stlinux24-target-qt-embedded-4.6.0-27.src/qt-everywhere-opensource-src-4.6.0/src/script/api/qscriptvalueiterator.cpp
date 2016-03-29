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
#include "qscriptvalueiterator.h"

#include "qscriptstring.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalue_p.h"
#include "qlinkedlist.h"


#include "JSObject.h"
#include "PropertyNameArray.h"
#include "JSArray.h"
#include "JSFunction.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptValueIterator

  \brief The QScriptValueIterator class provides a Java-style iterator for QScriptValue.

  \ingroup script


  The QScriptValueIterator constructor takes a QScriptValue as
  argument.  After construction, the iterator is located at the very
  beginning of the sequence of properties. Here's how to iterate over
  all the properties of a QScriptValue:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 0

  The next() advances the iterator. The name(), value() and flags()
  functions return the name, value and flags of the last item that was
  jumped over.

  If you want to remove properties as you iterate over the
  QScriptValue, use remove(). If you want to modify the value of a
  property, use setValue().

  Note that QScriptValueIterator only iterates over the QScriptValue's
  own properties; i.e. it does not follow the prototype chain. You can
  use a loop like this to follow the prototype chain:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 1

  Note that QScriptValueIterator will not automatically skip over
  properties that have the QScriptValue::SkipInEnumeration flag set;
  that flag only affects iteration in script code.  If you want, you
  can skip over such properties with code like the following:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 2

  \sa QScriptValue::property()
*/

class QScriptValueIteratorPrivate
{
public:
    QScriptValueIteratorPrivate()
        : initialized(false)
    {}
    void ensureInitialized()
    {
        if (initialized)
            return;
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(object.engine());
        JSC::ExecState *exec = eng_p->globalExec();
        JSC::PropertyNameArray propertyNamesArray(exec);
        propertyNamesArray.setShouldCache(false);
        JSC::asObject(QScriptValuePrivate::get(object)->jscValue)->getOwnPropertyNames(exec, propertyNamesArray, /*includeNonEnumerable=*/true);

        JSC::PropertyNameArray::const_iterator propertyNamesIt = propertyNamesArray.begin();
        for(; propertyNamesIt != propertyNamesArray.end(); ++propertyNamesIt) {
            propertyNames.append(propertyNamesIt->ustring());
        }
        it = propertyNames.begin();
        initialized = true;
    }

    QScriptValue object;
    QLinkedList<JSC::UString> propertyNames;
    QLinkedList<JSC::UString>::iterator it;
    QLinkedList<JSC::UString>::iterator current;
    bool initialized;
};

/*!
  Constructs an iterator for traversing \a object. The iterator is
  set to be at the front of the sequence of properties (before the
  first property).
*/
QScriptValueIterator::QScriptValueIterator(const QScriptValue &object)
    : d_ptr(0)
{
    if (object.isObject()) {
        d_ptr.reset(new QScriptValueIteratorPrivate());
        d_ptr->object = object;
    }
}

/*!
  Destroys the iterator.
*/
QScriptValueIterator::~QScriptValueIterator()
{
}

/*!
  Returns true if there is at least one item ahead of the iterator
  (i.e. the iterator is \e not at the back of the property sequence);
  otherwise returns false.

  \sa next(), hasPrevious()
*/
bool QScriptValueIterator::hasNext() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return false;

    const_cast<QScriptValueIteratorPrivate*>(d)->ensureInitialized();
    return d->it != d->propertyNames.end();
}

/*!
  Advances the iterator by one position.

  Calling this function on an iterator located at the back of the
  container leads to undefined results.

  \sa hasNext(), previous(), name()
*/
void QScriptValueIterator::next()
{
    Q_D(QScriptValueIterator);
    if (!d)
        return;
    d->ensureInitialized();

    d->current = d->it;
    ++(d->it);
}

/*!
  Returns true if there is at least one item behind the iterator
  (i.e. the iterator is \e not at the front of the property sequence);
  otherwise returns false.

  \sa previous(), hasNext()
*/
bool QScriptValueIterator::hasPrevious() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return false;

    const_cast<QScriptValueIteratorPrivate*>(d)->ensureInitialized();
    return d->it != d->propertyNames.begin();
}

/*!
  Moves the iterator back by one position.

  Calling this function on an iterator located at the front of the
  container leads to undefined results.

  \sa hasPrevious(), next(), name()
*/
void QScriptValueIterator::previous()
{
    Q_D(QScriptValueIterator);
    if (!d)
        return;
    d->ensureInitialized();
    --(d->it);
    d->current = d->it;
}

/*!
  Moves the iterator to the front of the QScriptValue (before the
  first property).

  \sa toBack(), next()
*/
void QScriptValueIterator::toFront()
{
    Q_D(QScriptValueIterator);
    if (!d)
        return;
    d->ensureInitialized();
    d->it = d->propertyNames.begin();
}

/*!
  Moves the iterator to the back of the QScriptValue (after the
  last property).

  \sa toFront(), previous()
*/
void QScriptValueIterator::toBack()
{
    Q_D(QScriptValueIterator);
    if (!d)
        return;
    d->ensureInitialized();
    d->it = d->propertyNames.end();
}

/*!
  Returns the name of the last property that was jumped over using
  next() or previous().

  \sa value(), flags()
*/
QString QScriptValueIterator::name() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->initialized)
        return QString();
    return *d->current;
}

/*!
  \since 4.4

  Returns the name of the last property that was jumped over using
  next() or previous().
*/
QScriptString QScriptValueIterator::scriptName() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->initialized)
        return QScriptString();
    return d->object.engine()->toStringHandle(name());
}

/*!
  Returns the value of the last property that was jumped over using
  next() or previous().

  \sa setValue(), name()
*/
QScriptValue QScriptValueIterator::value() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->initialized)
        return QScriptValue();
    return d->object.property(name());
}

/*!
  Sets the \a value of the last property that was jumped over using
  next() or previous().

  \sa value(), name()
*/
void QScriptValueIterator::setValue(const QScriptValue &value)
{
    Q_D(QScriptValueIterator);
    if (!d || !d->initialized)
        return;
    d->object.setProperty(name(), value);
}

/*!
  Returns the flags of the last property that was jumped over using
  next() or previous().

  \sa value()
*/
QScriptValue::PropertyFlags QScriptValueIterator::flags() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->initialized)
        return 0;
    return d->object.propertyFlags(name());
}

/*!
  Removes the last property that was jumped over using next()
  or previous().

  \sa setValue()
*/
void QScriptValueIterator::remove()
{
    Q_D(QScriptValueIterator);
    if (!d || !d->initialized)
        return;
    d->object.setProperty(name(), QScriptValue());
    d->propertyNames.erase(d->current);
}

/*!
  Makes the iterator operate on \a object. The iterator is set to be
  at the front of the sequence of properties (before the first
  property).
*/
QScriptValueIterator& QScriptValueIterator::operator=(QScriptValue &object)
{
    d_ptr.reset();
    if (object.isObject()) {
        d_ptr.reset(new QScriptValueIteratorPrivate());
        d_ptr->object = object;
    }
    return *this;
}

QT_END_NAMESPACE
