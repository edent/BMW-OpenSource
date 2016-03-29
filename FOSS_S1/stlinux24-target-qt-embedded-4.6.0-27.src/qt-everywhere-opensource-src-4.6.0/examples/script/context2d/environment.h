/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <qobject.h>
#include <qlist.h>
#include <qhash.h>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScriptEngine>
#include <QScriptable>
class QContext2DCanvas;

//! [0]
class Environment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QScriptValue document READ document)
public:
    Environment(QObject *parent = 0);
    ~Environment();

    QScriptValue document() const;

    void addCanvas(QContext2DCanvas *canvas);
    QContext2DCanvas *canvasByName(const QString &name) const;
    QList<QContext2DCanvas*> canvases() const;

    QScriptValue evaluate(const QString &code,
                          const QString &fileName = QString());

    QScriptValue toWrapper(QObject *object);

    void handleEvent(QContext2DCanvas *canvas, QMouseEvent *e);
    void handleEvent(QContext2DCanvas *canvas, QKeyEvent *e);

    void reset();
//! [0]

    QScriptEngine *engine() const;

//! [1]
public slots:
    int setInterval(const QScriptValue &expression, int delay);
    void clearInterval(int timerId);

    int setTimeout(const QScriptValue &expression, int delay);
    void clearTimeout(int timerId);
//! [1]

//! [2]
signals:
    void scriptError(const QScriptValue &error);
//! [2]

protected:
    void timerEvent(QTimerEvent *event);

private:
    QScriptValue eventHandler(QContext2DCanvas *canvas,
                              const QString &type, QScriptValue *who);
    QScriptValue newFakeDomEvent(const QString &type,
                                 const QScriptValue &target);
    void maybeEmitScriptError();

    QScriptEngine *m_engine;
    QScriptValue m_originalGlobalObject;
    QScriptValue m_document;
    QList<QContext2DCanvas*> m_canvases;
    QHash<int, QScriptValue> m_intervalHash;
    QHash<int, QScriptValue> m_timeoutHash;
};

//! [3]
class Document : public QObject
{
    Q_OBJECT
public:
    Document(Environment *env);
    ~Document();

public slots:
    QScriptValue getElementById(const QString &id) const;
    QScriptValue getElementsByTagName(const QString &name) const;

    // EventTarget
    void addEventListener(const QString &type, const QScriptValue &listener,
                          bool useCapture);
};
//! [3]

class CanvasGradientPrototype : public QObject, public QScriptable
{
    Q_OBJECT
protected:
    CanvasGradientPrototype(QObject *parent = 0);
public:
    static void setup(QScriptEngine *engine);

public slots:
    void addColorStop(qreal offset, const QString &color);
};

#endif
