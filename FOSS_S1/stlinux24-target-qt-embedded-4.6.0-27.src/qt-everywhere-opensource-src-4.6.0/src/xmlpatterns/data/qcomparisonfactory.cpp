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

#include "qatomiccomparators_p.h"
#include "qatomicstring_p.h"
#include "qcomparisonplatform_p.h"
#include "qvaluefactory_p.h"

#include "qcomparisonfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/**
 * @short Helper class for ComparisonFactory::fromLexical() which exposes
 * CastingPlatform appropriately.
 *
 * @relates ComparisonFactory
 */
class PerformComparison : public ComparisonPlatform<PerformComparison, true>
                        , public SourceLocationReflection
{
public:
    PerformComparison(const SourceLocationReflection *const sourceLocationReflection,
                      const AtomicComparator::Operator op) : m_sourceReflection(sourceLocationReflection)
                                                           , m_operator(op)
    {
        Q_ASSERT(m_sourceReflection);
    }

    bool operator()(const AtomicValue::Ptr &operand1,
                    const AtomicValue::Ptr &operand2,
                    const SchemaType::Ptr &type,
                    const ReportContext::Ptr &context)
    {
        const ItemType::Ptr asItemType((AtomicType::Ptr(type)));

        /* One area where the Query Transform world differs from the Schema
         * world is that @c xs:duration is not considedered comparable, because
         * it's according to Schema is partially comparable. This means
         * ComparisonPlatform::fetchComparator() flags it as impossible, and
         * hence we need to override that.
         *
         * SchemaType::wxsTypeMatches() will return true for sub-types of @c
         * xs:duration as well, but that's ok since AbstractDurationComparator
         * works for them too. */
        if(BuiltinTypes::xsDuration->wxsTypeMatches(type))
            prepareComparison(AtomicComparator::Ptr(new AbstractDurationComparator()));
        else if (BuiltinTypes::xsGYear->wxsTypeMatches(type) ||
                 BuiltinTypes::xsGYearMonth->wxsTypeMatches(type) ||
                 BuiltinTypes::xsGMonth->wxsTypeMatches(type) ||
                 BuiltinTypes::xsGMonthDay->wxsTypeMatches(type) ||
                 BuiltinTypes::xsGDay->wxsTypeMatches(type))
            prepareComparison(AtomicComparator::Ptr(new AbstractDateTimeComparator()));
        else
            prepareComparison(fetchComparator(asItemType, asItemType, context));

        return flexibleCompare(operand1, operand2, context);
    }

    const SourceLocationReflection *actualReflection() const
    {
        return m_sourceReflection;
    }

    AtomicComparator::Operator operatorID() const
    {
        return m_operator;
    }

private:
    const SourceLocationReflection *const m_sourceReflection;
    const AtomicComparator::Operator      m_operator;
};

bool ComparisonFactory::compare(const AtomicValue::Ptr &operand1,
                                const AtomicComparator::Operator op,
                                const AtomicValue::Ptr &operand2,
                                const SchemaType::Ptr &type,
                                const ReportContext::Ptr &context,
                                const SourceLocationReflection *const sourceLocationReflection)
{
    Q_ASSERT(operand1);
    Q_ASSERT(operand2);
    Q_ASSERT(context);
    Q_ASSERT(sourceLocationReflection);
    Q_ASSERT(type);
    Q_ASSERT_X(type->category() == SchemaType::SimpleTypeAtomic, Q_FUNC_INFO,
               "We can only compare atomic values.");

    return PerformComparison(sourceLocationReflection, op)(operand1, operand2, type, context);
}

bool ComparisonFactory::constructAndCompare(const DerivedString<TypeString>::Ptr &operand1,
                                            const AtomicComparator::Operator op,
                                            const DerivedString<TypeString>::Ptr &operand2,
                                            const SchemaType::Ptr &type,
                                            const ReportContext::Ptr &context,
                                            const SourceLocationReflection *const sourceLocationReflection)
{
    Q_ASSERT(operand1);
    Q_ASSERT(operand2);
    Q_ASSERT(context);
    Q_ASSERT(sourceLocationReflection);
    Q_ASSERT(type);
    Q_ASSERT_X(type->category() == SchemaType::SimpleTypeAtomic, Q_FUNC_INFO,
               "We can only compare atomic values.");

    const AtomicValue::Ptr value1 = ValueFactory::fromLexical(operand1->stringValue(), type, context, sourceLocationReflection);
    const AtomicValue::Ptr value2 = ValueFactory::fromLexical(operand2->stringValue(), type, context, sourceLocationReflection);

    return compare(value1, op, value2, type, context, sourceLocationReflection);
}

QT_END_NAMESPACE
