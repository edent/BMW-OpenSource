/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>


//![0]
struct StringEvent : public QEvent
{
    StringEvent(const QString &val)
    : QEvent(QEvent::Type(QEvent::User+1)),
      value(val) {}

    QString value;
};
//![0]

//![1]
class StringTransition : public QAbstractTransition
{
public:
    StringTransition(const QString &value)
        : m_value(value) {}

protected:
    virtual bool eventTest(QEvent *e) const
    {
        if (e->type() != QEvent::Type(QEvent::User+1)) // StringEvent
            return false;
        StringEvent *se = static_cast<StringEvent*>(e);
        return (m_value == se->value);
    }
    
    virtual void onTransition(QEvent *) {}

private:
    QString m_value;
};
//![1]

int main(int argv, char **args)
{
    QApplication app(argv, args);

//![2]
    QStateMachine machine;
    QState *s1 = new QState();
    QState *s2 = new QState();
    QFinalState *done = new QFinalState();

    StringTransition *t1 = new StringTransition("Hello");
    t1->setTargetState(s2);
    s1->addTransition(t1);
    StringTransition *t2 = new StringTransition("world");
    t2->setTargetState(done);
    s2->addTransition(t2);

    machine.addState(s1);
    machine.addState(s2);
    machine.addState(done);
    machine.setInitialState(s1);
//![2]

//![3]
    machine.postEvent(new StringEvent("Hello"));
    machine.postEvent(new StringEvent("world"));
//![3]

    return app.exec();
}

#include "main4.moc"

