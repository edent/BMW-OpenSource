/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  node.h
*/

#ifndef NODE_H
#define NODE_H

#include <qdir.h>
#include <qmap.h>
#include <qpair.h>
#include <qstringlist.h>

#include "codechunk.h"
#include "doc.h"
#include "location.h"
#include "text.h"

QT_BEGIN_NAMESPACE

class InnerNode;

class Node
{
 public:
    enum Type { 
        Namespace, 
        Class, 
        Fake, 
        Enum, 
        Typedef, 
        Function, 
        Property,
        Variable, 
#ifdef QDOC_QML
        Target,
        QmlProperty,
        QmlSignal,
        QmlMethod,
        LastType
#else
        Target,
        LastType
#endif
    };

    enum SubType { 
        NoSubType,
        Example, 
        HeaderFile, 
        File,
        Group,
        Module,
        Page,
#ifdef QDOC_QML
        ExternalPage,
        QmlClass,
        QmlPropertyGroup
#else
        ExternalPage
#endif
    };
    
    enum Access { Public, Protected, Private };
    
    enum Status { 
        Compat, 
        Obsolete, 
        Deprecated, 
        Preliminary, 
        Commendable, 
        Main, 
        Internal 
    }; // don't reorder thisw enum
    
    enum ThreadSafeness { 
        UnspecifiedSafeness, 
        NonReentrant, 
        Reentrant, 
        ThreadSafe 
    };
    
    enum LinkType { 
        StartLink, 
        NextLink, 
        PreviousLink,
        ContentsLink, 
        IndexLink,
        InheritsLink /*, 
        GlossaryLink, 
        CopyrightLink,
        ChapterLink, 
        SectionLink, 
        SubsectionLink, 
        AppendixLink */ 
    };

    virtual ~Node();

    void setAccess(Access access) { acc = access; }
    void setLocation(const Location& location) { loc = location; }
    void setDoc(const Doc& doc, bool replace = false);
    void setStatus(Status status) { sta = status; }
    void setThreadSafeness(ThreadSafeness safeness) { saf = safeness; }
    void setSince(const QString &since) { sinc = since; }
    void setRelates(InnerNode *pseudoParent);
    void setModuleName(const QString &module) { mod = module; }
    void setLink(LinkType linkType, const QString &link, const QString &desc);
    void setUrl(const QString &url);
    void setTemplateStuff(const QString &templateStuff) { tpl = templateStuff; }

    virtual bool isInnerNode() const = 0;
    virtual bool isReimp() const { return false; }
    Type type() const { return typ; }
    virtual SubType subType() const { return NoSubType; }
    InnerNode *parent() const { return par; }
    InnerNode *relates() const { return rel; }
    const QString& name() const { return nam; }
    QMap<LinkType, QPair<QString,QString> > links() const { return linkMap; }
    QString moduleName() const;
    QString url() const;
    virtual QString nameForLists() const { return nam; }

    Access access() const { return acc; }
    const Location& location() const { return loc; }
    const Doc& doc() const { return d; }
    Status status() const { return sta; }
    Status inheritedStatus() const;
    ThreadSafeness threadSafeness() const;
    ThreadSafeness inheritedThreadSafeness() const;
    QString since() const { return sinc; }
    QString templateStuff() const { return tpl; }

    void clearRelated() { rel = 0; }

    virtual QString fileBase() const;

 protected:
    Node(Type type, InnerNode *parent, const QString& name);

 private:

#ifdef Q_WS_WIN
    Type typ;
    Access acc;
    Status sta;
    ThreadSafeness saf;
#else
    Type typ : 4;
    Access acc : 2;
    Status sta : 3;
    ThreadSafeness saf : 2;
#endif
    InnerNode *par;
    InnerNode *rel;
    QString nam;
    Location loc;
    Doc d;
    QMap<LinkType, QPair<QString, QString> > linkMap;
    QString mod;
    QString u;
    QString sinc;
    QString tpl;
};

class FunctionNode;
class EnumNode;

typedef QList<Node *> NodeList;

class InnerNode : public Node
{
 public:
    virtual ~InnerNode();

    Node *findNode(const QString& name);
    Node *findNode(const QString& name, Type type);
    FunctionNode *findFunctionNode(const QString& name);
    FunctionNode *findFunctionNode(const FunctionNode *clone);
    void addInclude(const QString &include);
    void setIncludes(const QStringList &includes);
    void setOverload(const FunctionNode *func, bool overlode);
    void normalizeOverloads();
    void makeUndocumentedChildrenInternal();
    void deleteChildren();
    void removeFromRelated();

    virtual bool isInnerNode() const;
    const Node *findNode(const QString& name) const;
    const Node *findNode(const QString& name, Type type) const;
    const FunctionNode *findFunctionNode(const QString& name) const;
    const FunctionNode *findFunctionNode(const FunctionNode *clone) const;
    const EnumNode *findEnumNodeForValue(const QString &enumValue) const;
    const NodeList & childNodes() const { return children; }
    const NodeList & relatedNodes() const { return related; }
    int count() const { return children.size(); }
    int overloadNumber(const FunctionNode *func) const;
    int numOverloads(const QString& funcName) const;
    NodeList overloads(const QString &funcName) const;
    const QStringList& includes() const { return inc; }

 protected:
    InnerNode(Type type, InnerNode *parent, const QString& name);

 private:
    friend class Node;

    static bool isSameSignature(const FunctionNode *f1, const FunctionNode *f2);
    void addChild(Node *child);
    void removeChild(Node *child);
    void removeRelated(Node *pseudoChild);

    QStringList inc;
    NodeList children;
    NodeList enumChildren;
    NodeList related;
    QMap<QString, Node *> childMap;
    QMap<QString, Node *> primaryFunctionMap;
    QMap<QString, NodeList> secondaryFunctionMap;
};

class LeafNode : public Node
{
 public:
    LeafNode();
    virtual ~LeafNode() { }

    virtual bool isInnerNode() const;

 protected:
    LeafNode(Type type, InnerNode* parent, const QString& name);
};

class NamespaceNode : public InnerNode
{
 public:
    NamespaceNode(InnerNode *parent, const QString& name);
    virtual ~NamespaceNode() { }
};

class ClassNode;

struct RelatedClass
{
    RelatedClass() { }
    RelatedClass(Node::Access access0, 
                 ClassNode* node0,
                 const QString& dataTypeWithTemplateArgs0 = "")
      : access(access0), 
        node(node0),
        dataTypeWithTemplateArgs(dataTypeWithTemplateArgs0) { }

    Node::Access        access;
    ClassNode*          node;
    QString             dataTypeWithTemplateArgs;
};

class ClassNode : public InnerNode
{
 public:
    ClassNode(InnerNode *parent, const QString& name);
    virtual ~ClassNode() { }

    void addBaseClass(Access access, 
                      ClassNode *node, 
                      const QString &dataTypeWithTemplateArgs = "");
    void fixBaseClasses();

    const QList<RelatedClass> &baseClasses() const { return bas; }
    const QList<RelatedClass> &derivedClasses() const { return der; }

    bool hideFromMainList() const { return hidden; }
    void setHideFromMainList(bool value) { hidden = value; }

    QString serviceName() const { return sname; }
    void setServiceName(const QString& value) { sname = value; }
    QString qmlElement() const { return qmlelement; }
    void setQmlElement(const QString& value) { qmlelement = value; }

 private:
    QList<RelatedClass> bas;
    QList<RelatedClass> der;
    bool hidden;
    QString sname;
    QString qmlelement;
};

class FakeNode : public InnerNode
{
 public:

    FakeNode(InnerNode *parent, const QString& name, SubType subType);
    virtual ~FakeNode() { }

    void setTitle(const QString &title) { tle = title; }
    void setSubTitle(const QString &subTitle) { stle = subTitle; }
    void addGroupMember(Node *node) { gr.append(node); }

    SubType subType() const { return sub; }
    QString title() const { return tle; }
    QString fullTitle() const;
    QString subTitle() const;
    const NodeList &groupMembers() const { return gr; }
    virtual QString nameForLists() const { return title(); }

 private:
    SubType sub;
    QString tle;
    QString stle;
    NodeList gr;
};

#ifdef QDOC_QML
class QmlClassNode : public FakeNode
{
 public:
    QmlClassNode(InnerNode *parent, 
                 const QString& name, 
                 const ClassNode* cn);
    virtual ~QmlClassNode() { }

    const ClassNode* classNode() const { return cnode; }
    virtual QString fileBase() const;

    static bool qmlOnly;

 private:
    const ClassNode* cnode;
};

class QmlPropGroupNode : public FakeNode
{
 public:
    QmlPropGroupNode(QmlClassNode* parent, 
                     const QString& name,
                     bool attached);
    virtual ~QmlPropGroupNode() { }

    const QString& element() const { return parent()->name(); }
    void setDefault() { isdefault = true; }
    bool isDefault() const { return isdefault; }
    bool isAttached() const { return att; }

 private:
    bool    isdefault;
    bool    att;
};

class QmlPropertyNode : public LeafNode
{
 public:
    QmlPropertyNode(QmlPropGroupNode* parent, 
                    const QString& name,
                    const QString& type,
                    bool attached);
    virtual ~QmlPropertyNode() { }

    void setDataType(const QString& dataType) { dt = dataType; }
    void setStored(bool stored) { sto = toTrool(stored); }
    void setDesignable(bool designable) { des = toTrool(designable); }
    void setWritable(bool writable) { wri = toTrool(writable); }

    const QString &dataType() const { return dt; }
    QString qualifiedDataType() const { return dt; }
    bool isStored() const { return fromTrool(sto,true); }
    bool isDesignable() const { return fromTrool(des,false); }
    bool isWritable() const { return fromTrool(wri,true); }
    bool isAttached() const { return att; }

    const QString& element() const { return static_cast<QmlPropGroupNode*>(parent())->element(); }

 private:
    enum Trool { Trool_True, Trool_False, Trool_Default };

    static Trool toTrool(bool boolean);
    static bool fromTrool(Trool troolean, bool defaultValue);

    QString dt;
    Trool   sto;
    Trool   des;
    Trool   wri;
    bool    att;
};

class QmlSignalNode : public LeafNode
{
 public:
    QmlSignalNode(QmlClassNode* parent, 
                  const QString& name,
                  bool attached);
    virtual ~QmlSignalNode() { }

    const QString& element() const { return parent()->name(); }
    bool isAttached() const { return att; }

 private:
    bool    att;
};

class QmlMethodNode : public LeafNode
{
 public:
    QmlMethodNode(QmlClassNode* parent,
                  const QString& name,
                  bool attached);
    virtual ~QmlMethodNode() { }

    const QString& element() const { return parent()->name(); }
    bool isAttached() const { return att; }

 private:
    bool    att;
};
#endif

class EnumItem
{
 public:
    EnumItem() { }
    EnumItem(const QString& name, const QString& value)
	: nam(name), val(value) { }
    EnumItem(const QString& name, const QString& value, const Text &txt)
	: nam(name), val(value), txt(txt) { }

    const QString& name() const { return nam; }
    const QString& value() const { return val; }
    const Text &text() const { return txt; }

 private:
    QString nam;
    QString val;
    Text txt;
};

class TypedefNode;

class EnumNode : public LeafNode
{
 public:
    EnumNode(InnerNode *parent, const QString& name);
    virtual ~EnumNode() { }

    void addItem(const EnumItem& item);
    void setFlagsType(TypedefNode *typedeff);
    bool hasItem(const QString &name) const { return names.contains(name); }

    const QList<EnumItem>& items() const { return itms; }
    Access itemAccess(const QString& name) const;
    const TypedefNode *flagsType() const { return ft; }
    QString itemValue(const QString &name) const;

 private:
    QList<EnumItem> itms;
    QSet<QString> names;
    const TypedefNode *ft;
};

class TypedefNode : public LeafNode
{
 public:
    TypedefNode(InnerNode *parent, const QString& name);
    virtual ~TypedefNode() { }

    const EnumNode *associatedEnum() const { return ae; }

 private:
    void setAssociatedEnum(const EnumNode *enume);

    friend class EnumNode;

    const EnumNode *ae;
};

inline void EnumNode::setFlagsType(TypedefNode *typedeff)
{
    ft = typedeff;
    typedeff->setAssociatedEnum(this);
}


class Parameter
{
 public:
    Parameter() {}
    Parameter(const QString& leftType, 
              const QString& rightType = "",
              const QString& name = "", 
              const QString& defaultValue = "");
    Parameter(const Parameter& p);

    Parameter& operator=(const Parameter& p);

    void setName(const QString& name) { nam = name; }

    bool hasType() const { return lef.length() + rig.length() > 0; }
    const QString& leftType() const { return lef; }
    const QString& rightType() const { return rig; }
    const QString& name() const { return nam; }
    const QString& defaultValue() const { return def; }

    QString reconstruct(bool value = false) const;

 private:
    QString lef;
    QString rig;
    QString nam;
    QString def;
};

class PropertyNode;

class FunctionNode : public LeafNode
{
 public:
    enum Metaness { 
        Plain, 
        Signal, 
        Slot,
        Ctor,
        Dtor, 
        MacroWithParams,
        MacroWithoutParams, 
        Native };
    enum Virtualness { NonVirtual, ImpureVirtual, PureVirtual };

    FunctionNode(InnerNode *parent, const QString &name);
    virtual ~FunctionNode() { }

    void setReturnType(const QString& returnType) { rt = returnType; }
    void setParentPath(const QStringList& parentPath) { pp = parentPath; }
    void setMetaness(Metaness metaness) { met = metaness; }
    void setVirtualness(Virtualness virtualness) { vir = virtualness; }
    void setConst(bool conste) { con = conste; }
    void setStatic(bool statique) { sta = statique; }
    void setOverload(bool overlode);
    void setReimp(bool r);
    void addParameter(const Parameter& parameter);
    inline void setParameters(const QList<Parameter>& parameters);
    void borrowParameterNames(const FunctionNode *source);
    void setReimplementedFrom(FunctionNode *from);

    const QString& returnType() const { return rt; }
    Metaness metaness() const { return met; }
    bool isMacro() const { 
        return met == MacroWithParams || met == MacroWithoutParams; 
    }
    Virtualness virtualness() const { return vir; }
    bool isConst() const { return con; }
    bool isStatic() const { return sta; }
    bool isOverload() const { return ove; }
    bool isReimp() const { return reimp; }
    int overloadNumber() const;
    int numOverloads() const;
    const QList<Parameter>& parameters() const { return params; }
    QStringList parameterNames() const;
    const FunctionNode *reimplementedFrom() const { return rf; }
    const QList<FunctionNode *> &reimplementedBy() const { return rb; }
    const PropertyNode *associatedProperty() const { return ap; }
    const QStringList& parentPath() const { return pp; }

    QStringList reconstructParams(bool values = false) const;
    QString signature(bool values = false) const;

 private:
    void setAssociatedProperty(PropertyNode *property);

    friend class InnerNode;
    friend class PropertyNode;

    QString     rt;
    QStringList pp;
#ifdef Q_WS_WIN
    Metaness    met;
    Virtualness vir;
#else
    Metaness met : 4;
    Virtualness vir : 2;
#endif
    bool con : 1;
    bool sta : 1;
    bool ove : 1;
    bool reimp: 1; 
    QList<Parameter> params;
    const FunctionNode *rf;
    const PropertyNode *ap;
    QList<FunctionNode *> rb;
};

class PropertyNode : public LeafNode
{
 public:
    enum FunctionRole { Getter, Setter, Resetter, Notifier };
    enum { NumFunctionRoles = Notifier + 1 };

    PropertyNode(InnerNode *parent, const QString& name);
    virtual ~PropertyNode() { }

    void setDataType(const QString& dataType) { dt = dataType; }
    void addFunction(FunctionNode *function, FunctionRole role);
    void addSignal(FunctionNode *function, FunctionRole role);
    void setStored(bool stored) { sto = toTrool(stored); }
    void setDesignable(bool designable) { des = toTrool(designable); }
    void setWritable(bool writable) { wri = toTrool(writable); }
    void setOverriddenFrom(const PropertyNode *baseProperty);

    const QString &dataType() const { return dt; }
    QString qualifiedDataType() const;
    NodeList functions() const;
    NodeList functions(FunctionRole role) const { return funcs[(int)role]; }
    NodeList getters() const { return functions(Getter); }
    NodeList setters() const { return functions(Setter); }
    NodeList resetters() const { return functions(Resetter); }
    NodeList notifiers() const { return functions(Notifier); }
    bool isStored() const { return fromTrool(sto, storedDefault()); }
    bool isDesignable() const { return fromTrool(des, designableDefault()); }
    bool isWritable() const { return fromTrool(wri, writableDefault()); }
    const PropertyNode *overriddenFrom() const { return overrides; }

 private:
    enum Trool { Trool_True, Trool_False, Trool_Default };

    static Trool toTrool(bool boolean);
    static bool fromTrool(Trool troolean, bool defaultValue);

    bool storedDefault() const { return true; }
    bool designableDefault() const { return !setters().isEmpty(); }
    bool writableDefault() const { return !setters().isEmpty(); }

    QString dt;
    NodeList funcs[NumFunctionRoles];
    Trool sto;
    Trool des;
    Trool wri;
    const PropertyNode *overrides;
};

inline void FunctionNode::setParameters(const QList<Parameter> &parameters)
{
    params = parameters;
}

inline void PropertyNode::addFunction(FunctionNode *function, FunctionRole role)
{
    funcs[(int)role].append(function);
    function->setAssociatedProperty(this);
}

inline void PropertyNode::addSignal(FunctionNode *function, FunctionRole role)
{
    funcs[(int)role].append(function);
}

inline NodeList PropertyNode::functions() const
{
    NodeList list;
    for (int i = 0; i < NumFunctionRoles; ++i)
	list += funcs[i];
    return list;
}

class VariableNode : public LeafNode
{
 public:
    VariableNode(InnerNode *parent, const QString &name);
    virtual ~VariableNode() { }

    void setLeftType(const QString &leftType) { lt = leftType; }
    void setRightType(const QString &rightType) { rt = rightType; }
    void setStatic(bool statique) { sta = statique; }

    const QString &leftType() const { return lt; }
    const QString &rightType() const { return rt; }
    QString dataType() const { return lt + rt; }
    bool isStatic() const { return sta; }

 private:
    QString lt;
    QString rt;
    bool sta;
};

inline VariableNode::VariableNode(InnerNode *parent, const QString &name)
    : LeafNode(Variable, parent, name), sta(false)
{
}

class TargetNode : public LeafNode
{
 public:
    TargetNode(InnerNode *parent, const QString& name);
    virtual ~TargetNode() { }

    virtual bool isInnerNode() const;
};

QT_END_NAMESPACE

#endif
