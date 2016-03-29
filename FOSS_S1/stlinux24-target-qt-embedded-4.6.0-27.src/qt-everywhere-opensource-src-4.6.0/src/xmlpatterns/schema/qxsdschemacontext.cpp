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

#include "qxsdschemacontext_p.h"

#include "qderivedinteger_p.h"
#include "qderivedstring_p.h"
#include "qxsdschematypesfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdSchemaContext::XsdSchemaContext(const NamePool::Ptr &namePool)
    : m_namePool(namePool)
    , m_networkAccessManager(0)
    , m_uriResolver(0)
    , m_messageHandler(0)
{
}

NamePool::Ptr XsdSchemaContext::namePool() const
{
    return m_namePool;
}

QUrl XsdSchemaContext::baseURI() const
{
    return m_baseURI;
}

void XsdSchemaContext::setBaseURI(const QUrl &uri)
{
    m_baseURI = uri;
}

void XsdSchemaContext::setNetworkAccessManager(QNetworkAccessManager *accessManager)
{
    m_networkAccessManager = accessManager;
}

QNetworkAccessManager* XsdSchemaContext::networkAccessManager() const
{
    return m_networkAccessManager;
}

void XsdSchemaContext::setMessageHandler(QAbstractMessageHandler *handler)
{
    m_messageHandler = handler;
}

QAbstractMessageHandler* XsdSchemaContext::messageHandler() const
{
    return m_messageHandler;
}

QSourceLocation XsdSchemaContext::locationFor(const SourceLocationReflection *const) const
{
    return QSourceLocation();
}

void XsdSchemaContext::setUriResolver(const QAbstractUriResolver *uriResolver)
{
    m_uriResolver = uriResolver;
}

const QAbstractUriResolver* XsdSchemaContext::uriResolver() const
{
    return m_uriResolver;
}

XsdFacet::Hash XsdSchemaContext::facetsForType(const AnySimpleType::Ptr &type) const
{
    if (type->isDefinedBySchema())
        return XsdSimpleType::Ptr(type)->facets();
    else {
        if (m_builtinTypesFacetList.isEmpty())
            m_builtinTypesFacetList = setupBuiltinTypesFacetList();

        return m_builtinTypesFacetList.value(type);
    }
}

SchemaTypeFactory::Ptr XsdSchemaContext::schemaTypeFactory() const
{
    if (!m_schemaTypeFactory)
        m_schemaTypeFactory = SchemaTypeFactory::Ptr(new XsdSchemaTypesFactory(m_namePool));

    return m_schemaTypeFactory;
}

QHash<SchemaType::Ptr, XsdFacet::Hash> XsdSchemaContext::setupBuiltinTypesFacetList() const
{
    QHash<SchemaType::Ptr, XsdFacet::Hash> hash;

    const XsdFacet::Ptr fixedCollapseWhiteSpace(new XsdFacet());
    fixedCollapseWhiteSpace->setType(XsdFacet::WhiteSpace);
    fixedCollapseWhiteSpace->setValue(DerivedString<TypeString>::fromLexical(m_namePool, XsdSchemaToken::toString(XsdSchemaToken::Collapse)));
    fixedCollapseWhiteSpace->setFixed(true);

    const XsdFacet::Ptr collapseWhiteSpace(new XsdFacet());
    collapseWhiteSpace->setType(XsdFacet::WhiteSpace);
    collapseWhiteSpace->setValue(DerivedString<TypeString>::fromLexical(m_namePool, XsdSchemaToken::toString(XsdSchemaToken::Collapse)));
    collapseWhiteSpace->setFixed(false);

    const XsdFacet::Ptr preserveWhiteSpace(new XsdFacet());
    preserveWhiteSpace->setType(XsdFacet::WhiteSpace);
    preserveWhiteSpace->setValue(DerivedString<TypeString>::fromLexical(m_namePool, XsdSchemaToken::toString(XsdSchemaToken::Preserve)));
    preserveWhiteSpace->setFixed(false);

    const XsdFacet::Ptr replaceWhiteSpace(new XsdFacet());
    replaceWhiteSpace->setType(XsdFacet::WhiteSpace);
    replaceWhiteSpace->setValue(DerivedString<TypeString>::fromLexical(m_namePool, XsdSchemaToken::toString(XsdSchemaToken::Replace)));
    replaceWhiteSpace->setFixed(false);

    const XsdFacet::Ptr fixedZeroFractionDigits(new XsdFacet());
    fixedZeroFractionDigits->setType(XsdFacet::FractionDigits);
    fixedZeroFractionDigits->setValue(DerivedInteger<TypeNonNegativeInteger>::fromValue(m_namePool, 0));
    fixedZeroFractionDigits->setFixed(true);

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsString];
        facets.insert(preserveWhiteSpace->type(), preserveWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsBoolean];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDecimal];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsFloat];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDouble];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDuration];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDateTime];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsTime];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDate];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsGYearMonth];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsGYear];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsGMonthDay];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsGDay];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsGMonth];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsHexBinary];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsBase64Binary];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsAnyURI];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsQName];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNOTATION];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNormalizedString];
        facets.insert(replaceWhiteSpace->type(), replaceWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsToken];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsLanguage];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);

        const XsdFacet::Ptr pattern(new XsdFacet());
        pattern->setType(XsdFacet::Pattern);
        pattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("[a-zA-Z]{1,8}(-[a-zA-Z0-9]{1,8})*")));
        facets.insert(pattern->type(), pattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNMTOKEN];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);

        const XsdFacet::Ptr pattern(new XsdFacet());
        pattern->setType(XsdFacet::Pattern);
        pattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("\\c+")));
        facets.insert(pattern->type(), pattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsName];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);

        const XsdFacet::Ptr pattern(new XsdFacet());
        pattern->setType(XsdFacet::Pattern);
        pattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("\\i\\c*")));
        facets.insert(pattern->type(), pattern);
    }

    const XsdFacet::Ptr ncNamePattern(new XsdFacet());
    {
        ncNamePattern->setType(XsdFacet::Pattern);
        AtomicValue::List patterns;
        patterns << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("\\i\\c*"));
        patterns << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("[\\i-[:]][\\c-[:]]*"));
        ncNamePattern->setMultiValue(patterns);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNCName];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);
        facets.insert(ncNamePattern->type(), ncNamePattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsID];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);
        facets.insert(ncNamePattern->type(), ncNamePattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsIDREF];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);
        facets.insert(ncNamePattern->type(), ncNamePattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsENTITY];
        facets.insert(collapseWhiteSpace->type(), collapseWhiteSpace);
        facets.insert(ncNamePattern->type(), ncNamePattern);
    }

    const XsdFacet::Ptr integerPattern(new XsdFacet());
    integerPattern->setType(XsdFacet::Pattern);
    integerPattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("[\\-+]?[0-9]+")));

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsInteger];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNonPositiveInteger];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("0")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNegativeInteger];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("-1")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsLong];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("9223372036854775807")));
        facets.insert(maxInclusive->type(), maxInclusive);

        const XsdFacet::Ptr minInclusive(new XsdFacet());
        minInclusive->setType(XsdFacet::MinimumInclusive);
        minInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("-9223372036854775808")));
        facets.insert(minInclusive->type(), minInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsInt];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("2147483647")));
        facets.insert(maxInclusive->type(), maxInclusive);

        const XsdFacet::Ptr minInclusive(new XsdFacet());
        minInclusive->setType(XsdFacet::MinimumInclusive);
        minInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("-2147483648")));
        facets.insert(minInclusive->type(), minInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsShort];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("32767")));
        facets.insert(maxInclusive->type(), maxInclusive);

        const XsdFacet::Ptr minInclusive(new XsdFacet());
        minInclusive->setType(XsdFacet::MinimumInclusive);
        minInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("-32768")));
        facets.insert(minInclusive->type(), minInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsByte];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("127")));
        facets.insert(maxInclusive->type(), maxInclusive);

        const XsdFacet::Ptr minInclusive(new XsdFacet());
        minInclusive->setType(XsdFacet::MinimumInclusive);
        minInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("-128")));
        facets.insert(minInclusive->type(), minInclusive);
    }

    const XsdFacet::Ptr unsignedMinInclusive(new XsdFacet());
    unsignedMinInclusive->setType(XsdFacet::MinimumInclusive);
    unsignedMinInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("0")));

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsNonNegativeInteger];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
        facets.insert(unsignedMinInclusive->type(), unsignedMinInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsUnsignedLong];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
        facets.insert(unsignedMinInclusive->type(), unsignedMinInclusive);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("18446744073709551615")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsUnsignedInt];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
        facets.insert(unsignedMinInclusive->type(), unsignedMinInclusive);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("4294967295")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsUnsignedShort];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
        facets.insert(unsignedMinInclusive->type(), unsignedMinInclusive);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("65535")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsUnsignedByte];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);
        facets.insert(integerPattern->type(), integerPattern);
        facets.insert(unsignedMinInclusive->type(), unsignedMinInclusive);

        const XsdFacet::Ptr maxInclusive(new XsdFacet());
        maxInclusive->setType(XsdFacet::MaximumInclusive);
        maxInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("255")));
        facets.insert(maxInclusive->type(), maxInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsPositiveInteger];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);
        facets.insert(fixedZeroFractionDigits->type(), fixedZeroFractionDigits);

        const XsdFacet::Ptr minInclusive(new XsdFacet());
        minInclusive->setType(XsdFacet::MinimumInclusive);
        minInclusive->setValue(DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("1")));
        facets.insert(minInclusive->type(), minInclusive);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsYearMonthDuration];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);

        const XsdFacet::Ptr pattern(new XsdFacet());
        pattern->setType(XsdFacet::Pattern);
        pattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("[^DT]*")));
        facets.insert(pattern->type(), pattern);
    }

    {
        XsdFacet::Hash &facets = hash[BuiltinTypes::xsDayTimeDuration];
        facets.insert(fixedCollapseWhiteSpace->type(), fixedCollapseWhiteSpace);

        const XsdFacet::Ptr pattern(new XsdFacet());
        pattern->setType(XsdFacet::Pattern);
        pattern->setMultiValue(AtomicValue::List() << DerivedString<TypeString>::fromLexical(m_namePool, QString::fromLatin1("[^YM]*(T.*)?")));
        facets.insert(pattern->type(), pattern);
    }

    return hash;
}

QT_END_NAMESPACE
