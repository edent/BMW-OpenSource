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

#include "qaxwidgetextrainfo.h"
#include "qdesigneraxwidget.h"
#include "qaxwidgetpropertysheet.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/private/ui4_p.h>

QT_BEGIN_NAMESPACE

QAxWidgetExtraInfo::QAxWidgetExtraInfo(QDesignerAxWidget *widget, QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent), m_widget(widget), m_core(core)
{
}

QWidget *QAxWidgetExtraInfo::widget() const
{
    return m_widget;
}

QDesignerFormEditorInterface *QAxWidgetExtraInfo::core() const
{
    return m_core;
}

bool QAxWidgetExtraInfo::saveUiExtraInfo(DomUI *)
{
    return false;
}

bool QAxWidgetExtraInfo::loadUiExtraInfo(DomUI *)
{
    return false;
}

bool QAxWidgetExtraInfo::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    /* Turn off standard setters and make sure "control" is in front,
     * otherwise, previews will not work as the properties are not applied via
     * the caching property sheet, them. */
    typedef QList<DomProperty *> DomPropertyList;
    DomPropertyList props = ui_widget->elementProperty();
    const int size = props.size();
    const QString controlProperty = QLatin1String(QAxWidgetPropertySheet::controlPropertyName);
    for (int i = 0; i < size; i++) {
        props.at(i)->setAttributeStdset(false);
        if (i > 0 && props.at(i)->attributeName() == controlProperty) {
            qSwap(props[0], props[i]);
            ui_widget->setElementProperty(props);
        }

    }
    return true;
}

bool QAxWidgetExtraInfo::loadWidgetExtraInfo(DomWidget *)
{
    return false;
}

QAxWidgetExtraInfoFactory::QAxWidgetExtraInfoFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent)
    : QExtensionFactory(parent), m_core(core)
{
}

QObject *QAxWidgetExtraInfoFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
        return 0;

    if (QDesignerAxWidget *w = qobject_cast<QDesignerAxWidget*>(object))
        return new QAxWidgetExtraInfo(w, m_core, parent);

    return 0;
}

QT_END_NAMESPACE
