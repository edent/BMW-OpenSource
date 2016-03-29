/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef QACTIVEXPROPERTYSHEET_H
#define QACTIVEXPROPERTYSHEET_H

#include <QtDesigner/private/qdesigner_propertysheet_p.h>

QT_BEGIN_NAMESPACE

class QDesignerAxWidget;
class QDesignerFormWindowInterface;

/* The propertysheet has a method to delete itself and repopulate
 * if the "control" property changes. Pre 4.5, the control property
 * might not be the first one, so, the properties are stored and
 * re-applied. If the "control" is the first one, it should be
 * sufficient to reapply the changed flags, however, care must be taken when
 * resetting the control.
 * Resetting a control: The current behaviour is that the modified Active X properties are added again
 * as Fake-Properties, which is a nice side-effect as not cause a loss. */

class QAxWidgetPropertySheet: public QDesignerPropertySheet
{
    Q_OBJECT
    Q_INTERFACES(QDesignerPropertySheetExtension)
public:
    explicit QAxWidgetPropertySheet(QDesignerAxWidget *object, QObject *parent = 0);

    virtual bool isEnabled(int index) const;
    virtual void setProperty(int index, const QVariant &value);
    virtual bool reset(int index);
    int indexOf(const QString &name) const;
    bool dynamicPropertiesAllowed() const;

    static const char *controlPropertyName;

public slots:
    void updatePropertySheet();

private:
    QDesignerAxWidget *axWidget() const;

    const QString m_controlProperty;
    const QString m_propertyGroup;
    int m_controlIndex;
    struct SavedProperties {
        typedef QMap<QString, QVariant> NamePropertyMap;
        NamePropertyMap changedProperties;
        QWidget *widget;
        QString clsid;
    } m_currentProperties;

    static void reloadPropertySheet(const struct SavedProperties &properties, QDesignerFormWindowInterface *formWin);
};

typedef QDesignerPropertySheetFactory<QDesignerAxWidget, QAxWidgetPropertySheet> ActiveXPropertySheetFactory;

QT_END_NAMESPACE

#endif
