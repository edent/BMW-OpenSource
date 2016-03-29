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

#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QTextStream>
#include <QWidget>

#include "objectdump.h"
#include "objecttree.h"

QT_BEGIN_NAMESPACE

namespace ObjectDump
{

//-----------------------------------------------------------------------------
// QObjectAnnotator
//-----------------------------------------------------------------------------

QAnnotator::~QAnnotator()
{

}


//-----------------------------------------------------------------------------
// Annotators
//-----------------------------------------------------------------------------

QList<QByteArray> QAnnotatorBasic::annotation(const QObject& object)
{
    QList<QByteArray> result;

    QByteArray array;
    QTextStream stream(&array);

    stream << '[' << &object << ']';
    stream << ' ';
    stream << object.metaObject()->className();

    if (object.objectName() != "")
        stream << " \"" << object.objectName() << '"';

    if (object.isWidgetType())
        stream << " isWidget";

    stream.flush();
    result.append(array);
    return result;
}

QList<QByteArray> QAnnotatorWidget::annotation(const QObject& object)
{
    QList<QByteArray> result;

    const QWidget* widget = qobject_cast<const QWidget*>(&object);
    if (widget) {

        QByteArray array;
        QTextStream stream(&array);

        stream << "widget: ";

        if (widget->isVisible())
            stream << "visible ";
        else
            stream << "invisible ";

        stream << widget->x() << ',' << widget->y() << ' ';
        stream << widget->size().width() << 'x'<< widget->size().height() << ' ';

        stream << "hint " << widget->sizeHint().width() << 'x' << widget->sizeHint().height();

        stream.flush();
        result.append(array);
    }

    return result;
}


//-----------------------------------------------------------------------------
// Base class for QDumperPrivate, QVisitorPrivate
//-----------------------------------------------------------------------------

class QDumperBase
{
public:
    QDumperBase();
    ~QDumperBase();

    void setPrefix(const QString& prefix);
    void addAnnotator(QAnnotator* annotator);

protected:
    QByteArray m_prefix;
    QList<QAnnotator*> m_annotators;

};

QDumperBase::QDumperBase()
{

}

QDumperBase::~QDumperBase()
{
    QAnnotator* annotator;
    foreach(annotator, m_annotators)
        delete annotator;
}

void QDumperBase::setPrefix(const QString& prefix)
{
    m_prefix = prefix.count()
        ? (prefix + " ").toAscii()
        : prefix.toAscii();
}

void QDumperBase::addAnnotator(QAnnotator* annotator)
{
    // Protect against an exception occurring during QList::append
    QScopedPointer<QAnnotator> holder(annotator);
    m_annotators.append(annotator);
    holder.take();
}


//-----------------------------------------------------------------------------
// QDumper
//-----------------------------------------------------------------------------

class QDumperPrivate : public QDumperBase
{
public:
    QDumperPrivate();
    ~QDumperPrivate();

    void dumpObject(const QObject& object);

};


QDumperPrivate::QDumperPrivate()
{

}

QDumperPrivate::~QDumperPrivate()
{

}

void QDumperPrivate::dumpObject(const QObject& object)
{
    QAnnotator* annotator;
    foreach(annotator, m_annotators) {

        const QList<QByteArray> annotations = annotator->annotation(object);
        QByteArray annotation;
        foreach(annotation, annotations) {
            QByteArray buffer(m_prefix);
            buffer.append(annotation);
            qDebug() << buffer.constData();
        }
    }
}


QDumper::QDumper()
    : d_ptr(new QDumperPrivate)
{

}

QDumper::~QDumper()
{

}

void QDumper::setPrefix(const QString& prefix)
{
    d_func()->setPrefix(prefix);
}

void QDumper::addAnnotator(QAnnotator* annotator)
{
    d_func()->addAnnotator(annotator);
}

void QDumper::dumpObject(const QObject& object)
{
    d_func()->dumpObject(object);
}


//-----------------------------------------------------------------------------
// QVisitor
//-----------------------------------------------------------------------------

class QVisitorPrivate : public QDumperBase
{
public:
    QVisitorPrivate();
    ~QVisitorPrivate();

    void setIndent(unsigned indent);

    void visitNode(const QObject& object);
    void visitComplete();

private:
    class Node
    {
    public:
        Node();
        ~Node();

        QList<QByteArray>   m_annotation;
        QList<Node*>        m_children;

        typedef QList<Node*>::const_iterator child_iterator;
    };

private:
    Node* findNode(const QObject* object) const;
    QByteArray branchBuffer(const QList<bool>& branches, bool isNodeLine, bool isLastChild) const;
    void dumpRecursive(const Node& node, QList<bool> branches, bool isLastChild);
    void dumpNode(const Node& node, const QList<bool>& branches, bool isLastChild);

private:
    unsigned m_indent;

    QScopedPointer<Node> m_root;

    // Hash table used to associate internal nodes with QObjects
    typedef QHash<const QObject*, Node*> Hash;
    Hash m_hash;
};

static const unsigned DefaultIndent = 2;

QVisitorPrivate::QVisitorPrivate()
    :   m_indent(DefaultIndent)
{

}

QVisitorPrivate::~QVisitorPrivate()
{

}

void QVisitorPrivate::setIndent(unsigned indent)
{
    m_indent = indent;
}

// Builds up a mirror of the object tree, rooted in m_root, with each node
// storing annotations generated by
void QVisitorPrivate::visitNode(const QObject& object)
{
    QObject* const objectParent = object.parent();
    Node* const nodeParent = objectParent ? findNode(objectParent) : 0;

    // Create a new node and store in scoped pointer for exception safety
    Node* node = new Node;
    QScopedPointer<Node> nodePtr(node);

    // Associate node with QObject
    m_hash.insert(&object, node);

    // Insert node into internal tree
    if (nodeParent)
    {
        nodeParent->m_children.append(nodePtr.take());
    }
    else
    {
        Q_ASSERT(m_root.isNull());
        m_root.reset(nodePtr.take());
    }

    // Generate and store annotations
    QAnnotator* annotator;
    foreach(annotator, m_annotators)
        node->m_annotation.append( annotator->annotation(object) );
}

void QVisitorPrivate::visitComplete()
{
    QList<bool> branches;
    static const bool isLastChild = true;
    dumpRecursive(*m_root, branches, isLastChild);
    m_root.reset(0);
}

QVisitorPrivate::Node* QVisitorPrivate::findNode(const QObject* object) const
{
    Hash::const_iterator i = m_hash.find(object);
    return (m_hash.end() == i) ? 0 : *i;
}

QByteArray QVisitorPrivate::branchBuffer
    (const QList<bool>& branches, bool isNodeLine, bool isLastChild) const
{
    const int depth = branches.count();

    const QByteArray indent(m_indent, ' ');
    const QByteArray horiz(m_indent, '-');

    QByteArray buffer;
    QTextStream stream(&buffer);

    for (int i=0; i<depth-1; ++i) {
        if (branches[i])
            stream << '|';
        else
            stream << ' ';
        stream << indent;
    }

    if (depth) {
        if (isNodeLine)
            stream << '+' << horiz;
        else {
            if (!isLastChild)
                stream << '|';
            else
                stream << ' ';
            stream << indent;
        }
    }

    stream.flush();
    buffer.push_front(m_prefix);

    return buffer;
}

void QVisitorPrivate::dumpRecursive
    (const Node& node, QList<bool> branches, bool isLastChild)
{
    dumpNode(node, branches, isLastChild);

    // Recurse down tree
    const Node::child_iterator begin = node.m_children.begin();
    const Node::child_iterator end = node.m_children.end();
    for (Node::child_iterator i = begin; end != i; ++i) {

        isLastChild = (end == i + 1);

        if (begin == i)
            branches.push_back(!isLastChild);
        else
            branches.back() = !isLastChild;

        static const bool isNodeLine = false;
        const QByteArray buffer = branchBuffer(branches, isNodeLine, false);
        qDebug() << buffer.constData();

        dumpRecursive(**i, branches, isLastChild);
    }
}

void QVisitorPrivate::dumpNode
    (const Node& node, const QList<bool>& branches, bool isLastChild)
{
    const QList<QByteArray>::const_iterator
        begin = node.m_annotation.begin(), end = node.m_annotation.end();

    if (begin == end) {
        // No annotations - just dump the object pointer
        const bool isNodeLine = true;
        QByteArray buffer = branchBuffer(branches, isNodeLine, isLastChild);
        qDebug() << 0; // TODO
    }
    else {
        // Dump annotations
        for (QList<QByteArray>::const_iterator i = begin; end != i; ++i) {
            const bool isNodeLine = (begin == i);
            QByteArray buffer = branchBuffer(branches, isNodeLine, isLastChild);
            buffer.append(*i);
            qDebug() << buffer.constData();
        }
    }
}


// QVisitorPrivate::Node

QVisitorPrivate::Node::Node()
{

}

QVisitorPrivate::Node::~Node()
{
    Node* child;
    foreach(child, m_children)
        delete child;
}


// QVisitor

QVisitor::QVisitor()
    : d_ptr(new QVisitorPrivate)
{

}

QVisitor::~QVisitor()
{

}

void QVisitor::setPrefix(const QString& prefix)
{
    d_func()->setPrefix(prefix);
}

void QVisitor::setIndent(unsigned indent)
{
    d_func()->setIndent(indent);
}

void QVisitor::addAnnotator(QAnnotator* annotator)
{
    d_func()->addAnnotator(annotator);
}

void QVisitor::visitPrepare()
{
    // Do nothing
}

void QVisitor::visitNode(const QObject& object)
{
    d_func()->visitNode(object);
}

void QVisitor::visitComplete()
{
    d_func()->visitComplete();
}


//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

void addDefaultAnnotators_sys(QDumper& visitor);
void addDefaultAnnotators_sys(QVisitor& visitor);

void addDefaultAnnotators(QDumper& dumper)
{
    dumper.addAnnotator(new QAnnotatorBasic);
    dumper.addAnnotator(new QAnnotatorWidget);

    // Add platform-specific annotators
    addDefaultAnnotators_sys(dumper);
}

void addDefaultAnnotators(QVisitor& visitor)
{
    visitor.addAnnotator(new QAnnotatorBasic);
    visitor.addAnnotator(new QAnnotatorWidget);

    // Add platform-specific annotators
    addDefaultAnnotators_sys(visitor);
}

void dumpTreeFromRoot(const QObject& root, QVisitor& visitor)
{
    // Set up iteration range
    ObjectTree::DepthFirstConstIterator begin(root), end;

    // Invoke generic visitor algorithm
    ObjectTree::visit(begin, end, visitor);
}

void dumpTreeFromLeaf(const QObject& leaf, QVisitor& visitor)
{
    // Walk up to root
    const QObject* root = &leaf;
    while(root->parent())
    {
        root = root->parent();
    }

    dumpTreeFromRoot(*root, visitor);
}

void dumpAncestors(const QObject& leaf, QVisitor& visitor)
{
    // Set up iteration range
    ObjectTree::AncestorConstIterator begin(leaf), end;

    // Invoke generic visitor algorithm
    ObjectTree::visit(begin, end, visitor);
}


} // namespace ObjectDump

QT_END_NAMESPACE

