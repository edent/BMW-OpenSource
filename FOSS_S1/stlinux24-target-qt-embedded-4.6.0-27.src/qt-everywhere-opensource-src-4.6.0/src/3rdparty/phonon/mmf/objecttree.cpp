/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QTextStream>
#include <QWidget>
#include "objecttree.h"

QT_BEGIN_NAMESPACE

namespace ObjectTree
{

DepthFirstConstIterator::DepthFirstConstIterator()
    :   m_pointee(0)
{

}

DepthFirstConstIterator::DepthFirstConstIterator
    (const QObject& root)
    :   m_pointee(&root)
{

}

DepthFirstConstIterator&
    DepthFirstConstIterator::operator++()
{
    const QObjectList& children = m_pointee->children();

    if (children.count() == 0) {
        backtrack();
    }
    else {
        m_history.push(0);
        m_pointee = children.first();
    }

    return *this;
}

void DepthFirstConstIterator::backtrack()
{
    if (m_history.count()) {
        const int index = m_history.top();
        m_history.pop();

        const QObjectList& siblings = m_pointee->parent()->children();
        if (siblings.count() > index + 1) {
            m_history.push(index + 1);
            m_pointee = siblings[index + 1];
        }
        else {
            m_pointee = m_pointee->parent();
            backtrack();
        }
    }
    else {
        // Reached end of search
        m_pointee = 0;
    }
}



AncestorConstIterator::AncestorConstIterator()
{

}

AncestorConstIterator::AncestorConstIterator(const QObject& leaf)
{
    m_ancestors.push(&leaf);
    QObject* ancestor = leaf.parent();
    while(ancestor)
    {
        m_ancestors.push(ancestor);
        ancestor = ancestor->parent();
    }
}

} // namespace ObjectTree

QT_END_NAMESPACE

