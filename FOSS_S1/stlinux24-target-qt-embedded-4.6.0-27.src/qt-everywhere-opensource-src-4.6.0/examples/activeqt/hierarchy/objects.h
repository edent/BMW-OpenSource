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

#ifndef OBJECTS_H
#define OBJECTS_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE
class QSubWidget;

//! [0]
class QParentWidget : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("ClassID", "{d574a747-8016-46db-a07c-b2b4854ee75c}");
    Q_CLASSINFO("InterfaceID", "{4a30719d-d9c2-4659-9d16-67378209f822}");
    Q_CLASSINFO("EventsID", "{4a30719d-d9c2-4659-9d16-67378209f823}");
public:
    QParentWidget(QWidget *parent = 0);

    QSize sizeHint() const;

public slots:
    void createSubWidget( const QString &name );

    QSubWidget *subWidget( const QString &name );

private:
    QVBoxLayout *vbox;
};
//! [0]

//! [1]
class QSubWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QString label READ label WRITE setLabel )

    Q_CLASSINFO("ClassID", "{850652f4-8f71-4f69-b745-bce241ccdc30}");
    Q_CLASSINFO("InterfaceID", "{2d76cc2f-3488-417a-83d6-debff88b3c3f}");
    Q_CLASSINFO("ToSuperClass", "QSubWidget");
    
public:
    QSubWidget(QWidget *parent = 0, const QString &name = QString());

    void setLabel( const QString &text );
    QString label() const;

    QSize sizeHint() const;

protected:
    void paintEvent( QPaintEvent *e );

private:
    QString lbl;
};
//! [1]

#endif // OBJECTS_H
