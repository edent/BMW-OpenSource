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

#include "qxsdschematypesfactory_p.h"

#include "qbasictypesfactory_p.h"
#include "qbuiltintypes_p.h"
#include "qderivedinteger_p.h"
#include "qderivedstring_p.h"
#include "qcommonnamespaces_p.h"
#include "qxsdschematoken_p.h"
#include "qxsdsimpletype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdSchemaTypesFactory::XsdSchemaTypesFactory(const NamePool::Ptr &namePool)
    : m_namePool(namePool)
{
    m_types.reserve(3);

    const XsdFacet::Ptr whiteSpaceFacet(new XsdFacet());
    whiteSpaceFacet->setType(XsdFacet::WhiteSpace);
    whiteSpaceFacet->setFixed(true);
    whiteSpaceFacet->setValue(DerivedString<TypeString>::fromLexical(m_namePool, XsdSchemaToken::toString(XsdSchemaToken::Collapse)));

    const XsdFacet::Ptr minLengthFacet(new XsdFacet());
    minLengthFacet->setType(XsdFacet::MinimumLength);
    minLengthFacet->setValue(DerivedInteger<TypeNonNegativeInteger>::fromLexical(namePool, QLatin1String("1")));

    XsdFacet::Hash facets;
    facets.insert(whiteSpaceFacet->type(), whiteSpaceFacet);
    facets.insert(minLengthFacet->type(), minLengthFacet);

    {
        const QXmlName typeName = m_namePool->allocateQName(CommonNamespaces::WXS, QLatin1String("NMTOKENS"));
        const XsdSimpleType::Ptr type(new XsdSimpleType());
        type->setName(typeName);
        type->setWxsSuperType(BuiltinTypes::xsAnySimpleType);
        type->setCategory(XsdSimpleType::SimpleTypeList);
        type->setItemType(BuiltinTypes::xsNMTOKEN);
        type->setDerivationMethod(XsdSimpleType::DerivationRestriction);
        type->setFacets(facets);
        m_types.insert(typeName, type);
    }
    {
        const QXmlName typeName = m_namePool->allocateQName(CommonNamespaces::WXS, QLatin1String("IDREFS"));
        const XsdSimpleType::Ptr type(new XsdSimpleType());
        type->setName(typeName);
        type->setWxsSuperType(BuiltinTypes::xsAnySimpleType);
        type->setCategory(XsdSimpleType::SimpleTypeList);
        type->setItemType(BuiltinTypes::xsIDREF);
        type->setDerivationMethod(XsdSimpleType::DerivationRestriction);
        type->setFacets(facets);
        m_types.insert(typeName, type);
    }
    {
        const QXmlName typeName = m_namePool->allocateQName(CommonNamespaces::WXS, QLatin1String("ENTITIES"));
        const XsdSimpleType::Ptr type(new XsdSimpleType());
        type->setName(typeName);
        type->setWxsSuperType(BuiltinTypes::xsAnySimpleType);
        type->setCategory(XsdSimpleType::SimpleTypeList);
        type->setItemType(BuiltinTypes::xsENTITY);
        type->setDerivationMethod(XsdSimpleType::DerivationRestriction);
        type->setFacets(facets);
        m_types.insert(typeName, type);
    }
}

SchemaType::Ptr XsdSchemaTypesFactory::createSchemaType(const QXmlName name) const
{
    if (m_types.contains(name)) {
        return m_types.value(name);
    } else {
        if (!m_basicTypesFactory)
            m_basicTypesFactory = BasicTypesFactory::self(m_namePool);

        return m_basicTypesFactory->createSchemaType(name);
    }
}

SchemaType::Hash XsdSchemaTypesFactory::types() const
{
    return m_types;
}

QT_END_NAMESPACE
