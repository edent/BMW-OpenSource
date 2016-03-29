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

#include "qxsdwildcard_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString XsdWildcard::absentNamespace()
{
    return QLatin1String("__ns_absent");
}

void XsdWildcard::NamespaceConstraint::setVariety(Variety variety)
{
    m_variety = variety;
}

XsdWildcard::NamespaceConstraint::Variety XsdWildcard::NamespaceConstraint::variety() const
{
    return m_variety;
}

void XsdWildcard::NamespaceConstraint::setNamespaces(const QSet<QString> &namespaces)
{
    m_namespaces = namespaces;
}

QSet<QString> XsdWildcard::NamespaceConstraint::namespaces() const
{
    return m_namespaces;
}

void XsdWildcard::NamespaceConstraint::setDisallowedNames(const QSet<QString> &names)
{
    m_disallowedNames = names;
}

QSet<QString> XsdWildcard::NamespaceConstraint::disallowedNames() const
{
    return m_disallowedNames;
}

XsdWildcard::XsdWildcard()
    : m_namespaceConstraint(new NamespaceConstraint())
    , m_processContents(Strict)
{
    m_namespaceConstraint->setVariety(NamespaceConstraint::Any);
}

bool XsdWildcard::isWildcard() const
{
    return true;
}

void XsdWildcard::setNamespaceConstraint(const NamespaceConstraint::Ptr &namespaceConstraint)
{
    m_namespaceConstraint = namespaceConstraint;
}

XsdWildcard::NamespaceConstraint::Ptr XsdWildcard::namespaceConstraint() const
{
    return m_namespaceConstraint;
}

void XsdWildcard::setProcessContents(ProcessContents contents)
{
    m_processContents = contents;
}

XsdWildcard::ProcessContents XsdWildcard::processContents() const
{
    return m_processContents;
}

QT_END_NAMESPACE
