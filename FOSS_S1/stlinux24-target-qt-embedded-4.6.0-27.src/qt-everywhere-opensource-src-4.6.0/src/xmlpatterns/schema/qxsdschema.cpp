/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qxsdschema_p.h"

#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdSchema::XsdSchema(const NamePool::Ptr &namePool)
    : m_namePool(namePool)
{
}

XsdSchema::~XsdSchema()
{
}

NamePool::Ptr XsdSchema::namePool() const
{
    return m_namePool;
}

void XsdSchema::setTargetNamespace(const QString &targetNamespace)
{
    m_targetNamespace = targetNamespace;
}

QString XsdSchema::targetNamespace() const
{
    return m_targetNamespace;
}

void XsdSchema::addElement(const XsdElement::Ptr &element)
{
    const QWriteLocker locker(&m_lock);

    m_elements.insert(element->name(m_namePool), element);
}

XsdElement::Ptr XsdSchema::element(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_elements.value(name);
}

XsdElement::List XsdSchema::elements() const
{
    const QReadLocker locker(&m_lock);

    return m_elements.values();
}

void XsdSchema::addAttribute(const XsdAttribute::Ptr &attribute)
{
    const QWriteLocker locker(&m_lock);

    m_attributes.insert(attribute->name(m_namePool), attribute);
}

XsdAttribute::Ptr XsdSchema::attribute(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_attributes.value(name);
}

XsdAttribute::List XsdSchema::attributes() const
{
    const QReadLocker locker(&m_lock);

    return m_attributes.values();
}

void XsdSchema::addType(const SchemaType::Ptr &type)
{
    const QWriteLocker locker(&m_lock);

    m_types.insert(type->name(m_namePool), type);
}

SchemaType::Ptr XsdSchema::type(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_types.value(name);
}

SchemaType::List XsdSchema::types() const
{
    const QReadLocker locker(&m_lock);

    return m_types.values();
}

XsdSimpleType::List XsdSchema::simpleTypes() const
{
    QReadLocker locker(&m_lock);

    XsdSimpleType::List retval;

    const SchemaType::List types = m_types.values();
    for (int i = 0; i < types.count(); ++i) {
        if (types.at(i)->isSimpleType() && types.at(i)->isDefinedBySchema())
            retval.append(types.at(i));
    }

    return retval;
}

XsdComplexType::List XsdSchema::complexTypes() const
{
    QReadLocker locker(&m_lock);

    XsdComplexType::List retval;

    const SchemaType::List types = m_types.values();
    for (int i = 0; i < types.count(); ++i) {
        if (types.at(i)->isComplexType() && types.at(i)->isDefinedBySchema())
            retval.append(types.at(i));
    }

    return retval;
}

void XsdSchema::addAnonymousType(const SchemaType::Ptr &type)
{
    const QWriteLocker locker(&m_lock);

    // search for not used anonymous type name
    QXmlName typeName = type->name(m_namePool);
    while (m_anonymousTypes.contains(typeName)) {
        typeName = m_namePool->allocateQName(QString(), QLatin1String("merged_") + m_namePool->stringForLocalName(typeName.localName()), QString());
    }

    m_anonymousTypes.insert(typeName, type);
}

SchemaType::List XsdSchema::anonymousTypes() const
{
    const QReadLocker locker(&m_lock);

    return m_anonymousTypes.values();
}

void XsdSchema::addAttributeGroup(const XsdAttributeGroup::Ptr &group)
{
    const QWriteLocker locker(&m_lock);

    m_attributeGroups.insert(group->name(m_namePool), group);
}

XsdAttributeGroup::Ptr XsdSchema::attributeGroup(const QXmlName name) const
{
    const QReadLocker locker(&m_lock);

    return m_attributeGroups.value(name);
}

XsdAttributeGroup::List XsdSchema::attributeGroups() const
{
    const QReadLocker locker(&m_lock);

    return m_attributeGroups.values();
}

void XsdSchema::addElementGroup(const XsdModelGroup::Ptr &group)
{
    const QWriteLocker locker(&m_lock);

    m_elementGroups.insert(group->name(m_namePool), group);
}

XsdModelGroup::Ptr XsdSchema::elementGroup(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_elementGroups.value(name);
}

XsdModelGroup::List XsdSchema::elementGroups() const
{
    const QReadLocker locker(&m_lock);

    return m_elementGroups.values();
}

void XsdSchema::addNotation(const XsdNotation::Ptr &notation)
{
    const QWriteLocker locker(&m_lock);

    m_notations.insert(notation->name(m_namePool), notation);
}

XsdNotation::Ptr XsdSchema::notation(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_notations.value(name);
}

XsdNotation::List XsdSchema::notations() const
{
    const QReadLocker locker(&m_lock);

    return m_notations.values();
}

void XsdSchema::addIdentityConstraint(const XsdIdentityConstraint::Ptr &constraint)
{
    const QWriteLocker locker(&m_lock);

    m_identityConstraints.insert(constraint->name(m_namePool), constraint);
}

XsdIdentityConstraint::Ptr XsdSchema::identityConstraint(const QXmlName &name) const
{
    const QReadLocker locker(&m_lock);

    return m_identityConstraints.value(name);
}

XsdIdentityConstraint::List XsdSchema::identityConstraints() const
{
    const QReadLocker locker(&m_lock);

    return m_identityConstraints.values();
}

QT_END_NAMESPACE
