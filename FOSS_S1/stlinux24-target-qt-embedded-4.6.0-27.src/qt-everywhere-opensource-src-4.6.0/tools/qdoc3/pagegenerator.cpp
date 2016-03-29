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
  pagegenerator.cpp
*/

#include <QtCore>
#include <qfile.h>
#include <qfileinfo.h>

#include "pagegenerator.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

/*!
  Nothing to do in the constructor.
 */
PageGenerator::PageGenerator()
{
    // nothing.
}

/*!
  The destructor 
 */
PageGenerator::~PageGenerator()
{
    while (!outStreamStack.isEmpty())
	endSubPage();
}

/*!
  This function is recursive.
 */
void PageGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    generateInnerNode(tree->root(), marker);
}

QString PageGenerator::fileBase(const Node *node)
{
    if (node->relates())
	node = node->relates();
    else if (!node->isInnerNode())
	node = node->parent();
#ifdef QDOC_QML
    if (node->subType() == Node::QmlPropertyGroup) {
        node = node->parent();
    }
#endif        

    QString base = node->doc().baseName();
    if (!base.isEmpty())
        return base;

    const Node *p = node;

    forever {
        const Node *pp = p->parent();
        base.prepend(p->name());
#ifdef QDOC_QML
        /*
          To avoid file name conflicts in the html directory,
          we prepend "qml-" to the file name of QML element doc
          files.
         */
        if (p->subType() == Node::QmlClass) {
            base.prepend("qml-");
        }
#endif        
        if (!pp || pp->name().isEmpty() || pp->type() == Node::Fake)
            break;
        base.prepend(QLatin1Char('-'));
        p = pp;
    }
    
    if (node->type() == Node::Fake) {
#ifdef QDOC2_COMPAT
        if (base.endsWith(".html"))
            base.truncate(base.length() - 5);
#endif
    }

    // the code below is effectively equivalent to:
    //   base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    //   base = base.trimmed();
    //   base.replace(" ", "-");
    //   base = base.toLower();
    // as this function accounted for ~8% of total running time
    // we optimize a bit...

    QString res;
    // +5 prevents realloc in fileName() below
    res.reserve(base.size() + 5);
    bool begun = false;
    for (int i = 0; i != base.size(); ++i) {
        QChar c = base.at(i);
        uint u = c.unicode();
        if (u >= 'A' && u <= 'Z')
            u -= 'A' - 'a';
        if ((u >= 'a' &&  u <= 'z') || (u >= '0' && u <= '9')) {
            res += QLatin1Char(u);
            begun = true;
        }
        else if (begun) {
            res += QLatin1Char('-');
            begun = false;
        }
    }
    while (res.endsWith(QLatin1Char('-')))
        res.chop(1);
    return res;
}

QString PageGenerator::fileName(const Node *node)
{
    if (!node->url().isEmpty())
        return node->url();

    QString name = fileBase(node);
    name += QLatin1Char('.');
    name += fileExtension(node);
    return name;
}

QString PageGenerator::outFileName()
{
    return QFileInfo(static_cast<QFile *>(out().device())->fileName()).fileName();
}

void PageGenerator::beginSubPage(const Location& location,
                                 const QString& fileName)
{
    QFile *outFile = new QFile(outputDir() + "/" + fileName);
    if (!outFile->open(QFile::WriteOnly))
	location.fatal(tr("Cannot open output file '%1'")
			.arg(outFile->fileName()));
    QTextStream *out = new QTextStream(outFile);
    out->setCodec("ISO-8859-1");
    outStreamStack.push(out);
}

void PageGenerator::endSubPage()
{
    outStreamStack.top()->flush();
    delete outStreamStack.top()->device();
    delete outStreamStack.pop();
}

QTextStream &PageGenerator::out()
{
    return *outStreamStack.top();
}

/*!
  Recursive writing of html files from the root \a node.
 */
void PageGenerator::generateInnerNode(const InnerNode *node,
                                      CodeMarker *marker)
{
    if (!node->url().isNull())
        return;

    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
        if (fakeNode->subType() == Node::ExternalPage)
            return;
#ifdef QDOC_QML            
        if (fakeNode->subType() == Node::QmlPropertyGroup)
            return;
#endif            
    }

    if (node->parent() != 0) {
	beginSubPage(node->location(), fileName(node));
	if (node->type() == Node::Namespace || node->type() == Node::Class) {
	    generateClassLikeNode(node, marker);
	}
        else if (node->type() == Node::Fake) {
	    generateFakeNode(static_cast<const FakeNode *>(node), marker);
	}
	endSubPage();
    }

    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
	if ((*c)->isInnerNode() && (*c)->access() != Node::Private)
	    generateInnerNode((const InnerNode *) *c, marker);
	++c;
    }
}

QT_END_NAMESPACE
