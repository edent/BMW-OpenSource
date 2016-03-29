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

#include "qatomiccaster_p.h"
#include "qatomicstring_p.h"
#include "qcastingplatform_p.h"
#include "qvaluefactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/**
 * @short Helper class for ValueFactory::fromLexical() which exposes
 * CastingPlatform appropriately.
 *
 * @relates ValueFactory
 */
class PerformValueConstruction : public CastingPlatform<PerformValueConstruction, false>
                               , public SourceLocationReflection
{
public:
    PerformValueConstruction(const SourceLocationReflection *const sourceLocationReflection,
                             const SchemaType::Ptr &toType) : m_sourceReflection(sourceLocationReflection)
                                                            , m_targetType(AtomicType::Ptr(toType))
    {
        Q_ASSERT(m_sourceReflection);
    }

    AtomicValue::Ptr operator()(const AtomicValue::Ptr &lexicalValue,
                                const SchemaType::Ptr & /*type*/,
                                const ReportContext::Ptr &context)
    {
        prepareCasting(context, BuiltinTypes::xsString);
        return AtomicValue::Ptr(const_cast<AtomicValue *>(cast(lexicalValue, context).asAtomicValue()));
    }

    const SourceLocationReflection *actualReflection() const
    {
        return m_sourceReflection;
    }

    ItemType::Ptr targetType() const
    {
        return m_targetType;
    }

private:
    const SourceLocationReflection *const m_sourceReflection;
    const ItemType::Ptr                   m_targetType;
};

AtomicValue::Ptr ValueFactory::fromLexical(const QString &lexicalValue,
                                           const SchemaType::Ptr &type,
                                           const ReportContext::Ptr &context,
                                           const SourceLocationReflection *const sourceLocationReflection)
{
    Q_ASSERT(context);
    Q_ASSERT(type);
    Q_ASSERT_X(type->category() == SchemaType::SimpleTypeAtomic, Q_FUNC_INFO,
               "We can only construct for atomic values.");

    return PerformValueConstruction(sourceLocationReflection, type)(AtomicString::fromValue(lexicalValue),
                                                                    type,
                                                                    context);
}

QT_END_NAMESPACE
