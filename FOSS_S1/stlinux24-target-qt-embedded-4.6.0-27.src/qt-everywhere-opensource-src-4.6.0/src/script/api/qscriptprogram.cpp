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
#include "qscriptprogram.h"
#include "qscriptprogram_p.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"

#include "Executable.h"

QT_BEGIN_NAMESPACE

/*!
  \internal

  \since 4.6
  \class QScriptProgram

  \brief The QScriptProgram class encapsulates a Qt Script program.

  \ingroup script

  QScriptProgram retains the compiled representation of the script if
  possible. Thus, QScriptProgram can be used to evaluate the same
  script multiple times more efficiently.

  \code
  QScriptEngine engine;
  QScriptProgram program("1 + 2");
  QScriptValue result = engine.evaluate(program);
  \endcode
*/

QScriptProgramPrivate::QScriptProgramPrivate(const QString &src,
                                             const QString &fn,
                                             int ln)
    : sourceCode(src), fileName(fn), firstLineNumber(ln),
      engine(0), _executable(0), sourceId(-1), isCompiled(false)
{
    ref = 0;
}

QScriptProgramPrivate::~QScriptProgramPrivate()
{
    delete _executable;
}

QScriptProgramPrivate *QScriptProgramPrivate::get(const QScriptProgram &q)
{
    return const_cast<QScriptProgramPrivate*>(q.d_func());
}

JSC::EvalExecutable *QScriptProgramPrivate::executable(JSC::ExecState *exec,
                                                       QScriptEnginePrivate *eng)
{
    if (_executable) {
        if (eng == engine)
            return _executable;
        delete _executable;
    }
    WTF::PassRefPtr<QScript::UStringSourceProviderWithFeedback> provider
        = QScript::UStringSourceProviderWithFeedback::create(sourceCode, fileName, firstLineNumber, eng);
    sourceId = provider->asID();
    JSC::SourceCode source(provider, firstLineNumber); //after construction of SourceCode provider variable will be null.
    _executable = new JSC::EvalExecutable(exec, source);
    engine = eng;
    isCompiled = false;
    return _executable;
}

/*!
  Constructs a null QScriptProgram.
*/
QScriptProgram::QScriptProgram()
    : d_ptr(0)
{
}

/*!
  Constructs a new QScriptProgram with the given \a sourceCode, \a
  fileName and \a firstLineNumber.
*/
QScriptProgram::QScriptProgram(const QString &sourceCode,
                               const QString fileName,
                               int firstLineNumber)
    : d_ptr(new QScriptProgramPrivate(sourceCode, fileName, firstLineNumber))
{
}

/*!
  Constructs a new QScriptProgram that is a copy of \a other.
*/
QScriptProgram::QScriptProgram(const QScriptProgram &other)
    : d_ptr(other.d_ptr)
{
}

/*!
  Destroys this QScriptProgram.
*/
QScriptProgram::~QScriptProgram()
{
    Q_D(QScriptProgram);
    //    if (d->engine && (d->ref == 1))
    //      d->engine->unregisterScriptProgram(d);
}

/*!
  Assigns the \a other value to this QScriptProgram.
*/
QScriptProgram &QScriptProgram::operator=(const QScriptProgram &other)
{
  //    if (d_func() && d_func()->engine && (d_func()->ref == 1))
      //        d_func()->engine->unregisterScriptProgram(d_func());
  //    }
    d_ptr = other.d_ptr;
    return *this;
}

/*!
  Returns true if this QScriptProgram is null; otherwise
  returns false.
*/
bool QScriptProgram::isNull() const
{
    Q_D(const QScriptProgram);
    return (d == 0);
}

/*!
  Returns the source code of this program.
*/
QString QScriptProgram::sourceCode() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return QString();
    return d->sourceCode;
}

/*!
  Returns the filename associated with this program.
*/
QString QScriptProgram::fileName() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return QString();
    return d->fileName;
}

/*!
  Returns the line number associated with this program.
*/
int QScriptProgram::firstLineNumber() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return -1;
    return d->firstLineNumber;
}

/*!
  Returns true if this QScriptProgram is equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator==(const QScriptProgram &other) const
{
    Q_D(const QScriptProgram);
    if (d == other.d_func())
        return true;
    return (sourceCode() == other.sourceCode())
        && (fileName() == other.fileName())
        && (firstLineNumber() == other.firstLineNumber());
}

/*!
  Returns true if this QScriptProgram is not equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator!=(const QScriptProgram &other) const
{
    return !operator==(other);
}

QT_END_NAMESPACE
