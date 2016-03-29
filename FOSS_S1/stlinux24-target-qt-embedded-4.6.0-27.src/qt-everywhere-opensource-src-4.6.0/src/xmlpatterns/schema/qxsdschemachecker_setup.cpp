/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qxsdschemachecker_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdSchemaChecker::setupAllowedAtomicFacets()
{
    // string
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsString->name(m_namePool), facets);
    }

    // boolean
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsBoolean->name(m_namePool), facets);
    }

    // float
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsFloat->name(m_namePool), facets);
    }

    // double
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsDouble->name(m_namePool), facets);
    }

    // decimal
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::TotalDigits
               << XsdFacet::FractionDigits
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsDecimal->name(m_namePool), facets);
    }

    // duration
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsDuration->name(m_namePool), facets);
    }

    // dateTime
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsDateTime->name(m_namePool), facets);
    }

    // time
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsTime->name(m_namePool), facets);
    }

    // date
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsDate->name(m_namePool), facets);
    }

    // gYearMonth
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsGYearMonth->name(m_namePool), facets);
    }

    // gYear
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsGYear->name(m_namePool), facets);
    }

    // gMonthDay
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsGMonthDay->name(m_namePool), facets);
    }

    // gDay
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsGDay->name(m_namePool), facets);
    }

    // gMonth
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::MaximumInclusive
               << XsdFacet::MaximumExclusive
               << XsdFacet::MinimumInclusive
               << XsdFacet::MinimumExclusive
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsGMonth->name(m_namePool), facets);
    }

    // hexBinary
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsHexBinary->name(m_namePool), facets);
    }

    // base64Binary
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsBase64Binary->name(m_namePool), facets);
    }

    // anyURI
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsAnyURI->name(m_namePool), facets);
    }

    // QName
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsQName->name(m_namePool), facets);
    }

    // NOTATION
    {
        QSet<XsdFacet::Type> facets;
        facets << XsdFacet::Length
               << XsdFacet::MinimumLength
               << XsdFacet::MaximumLength
               << XsdFacet::Pattern
               << XsdFacet::Enumeration
               << XsdFacet::WhiteSpace
               << XsdFacet::Assertion;

        m_allowedAtomicFacets.insert(BuiltinTypes::xsNOTATION->name(m_namePool), facets);
    }
}

QT_END_NAMESPACE
