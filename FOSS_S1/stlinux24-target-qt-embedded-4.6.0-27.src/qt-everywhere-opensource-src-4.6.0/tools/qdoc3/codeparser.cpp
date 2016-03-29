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
  codeparser.cpp
*/

#include <QtCore>
#include "codeparser.h"
#include "node.h"
#include "tree.h"
#include "config.h"

QT_BEGIN_NAMESPACE

#define COMMAND_COMPAT                  Doc::alias(QLatin1String("compat"))
#define COMMAND_DEPRECATED              Doc::alias(QLatin1String("deprecated")) // ### don't document
#define COMMAND_INGROUP                 Doc::alias(QLatin1String("ingroup"))
#define COMMAND_INMODULE                Doc::alias(QLatin1String("inmodule"))  // ### don't document
#define COMMAND_INTERNAL                Doc::alias(QLatin1String("internal"))
#define COMMAND_MAINCLASS               Doc::alias(QLatin1String("mainclass"))
#define COMMAND_NONREENTRANT            Doc::alias(QLatin1String("nonreentrant"))
#define COMMAND_OBSOLETE                Doc::alias(QLatin1String("obsolete"))
#define COMMAND_PRELIMINARY             Doc::alias(QLatin1String("preliminary"))
#define COMMAND_INPUBLICGROUP           Doc::alias(QLatin1String("inpublicgroup"))
#define COMMAND_REENTRANT               Doc::alias(QLatin1String("reentrant"))
#define COMMAND_SINCE                   Doc::alias(QLatin1String("since"))
#define COMMAND_SUBTITLE                Doc::alias(QLatin1String("subtitle"))
#define COMMAND_THREADSAFE              Doc::alias(QLatin1String("threadsafe"))
#define COMMAND_TITLE                   Doc::alias(QLatin1String("title"))

QList<CodeParser *> CodeParser::parsers;
bool CodeParser::showInternal = false;

/*!
  The constructor adds this code parser to the static
  list of code parsers.
 */
CodeParser::CodeParser()
{
    parsers.prepend(this);
}

/*!
  The destructor removes this code parser from the static
  list of code parsers.
 */
CodeParser::~CodeParser()
{
    parsers.removeAll(this);
}

/*!
  Initialize the code parser base class.
 */
void CodeParser::initializeParser(const Config& config)
{
    showInternal = config.getBool(QLatin1String(CONFIG_SHOWINTERNAL));
}

/*!
  Teerminating a code parser is trivial.
 */
void CodeParser::terminateParser()
{
    // nothing.
}

QString CodeParser::headerFileNameFilter()
{
    return sourceFileNameFilter();
}

void CodeParser::parseHeaderFile(const Location& location,
                                 const QString& filePath,
                                 Tree *tree)
{
    parseSourceFile(location, filePath, tree);
}

void CodeParser::doneParsingHeaderFiles(Tree *tree)
{
    doneParsingSourceFiles(tree);
}

/*!
  All the code parsers in the static list are initialized here,
  after the qdoc configuration variables have been set.
 */
void CodeParser::initialize(const Config& config)
{
    QList<CodeParser *>::ConstIterator p = parsers.begin();
    while (p != parsers.end()) {
	(*p)->initializeParser(config);
	++p;
    }
}

/*!
  All the code parsers in the static list are terminated here.
 */
void CodeParser::terminate()
{
    QList<CodeParser *>::ConstIterator p = parsers.begin();
    while (p != parsers.end()) {
	(*p)->terminateParser();
	++p;
    }
}

CodeParser *CodeParser::parserForLanguage(const QString& language)
{
    QList<CodeParser *>::ConstIterator p = parsers.begin();
    while (p != parsers.end()) {
	if ((*p)->language() == language)
	    return *p;
	++p;
    }
    return 0;
}

/*!
  Returns the set of strings representing the common metacommands.
 */
QSet<QString> CodeParser::commonMetaCommands()
{
    return QSet<QString>() << COMMAND_COMPAT
                           << COMMAND_DEPRECATED
                           << COMMAND_INGROUP
                           << COMMAND_INMODULE
                           << COMMAND_INTERNAL
                           << COMMAND_MAINCLASS
                           << COMMAND_NONREENTRANT
                           << COMMAND_OBSOLETE
                           << COMMAND_PRELIMINARY
                           << COMMAND_INPUBLICGROUP
                           << COMMAND_REENTRANT
                           << COMMAND_SINCE
                           << COMMAND_SUBTITLE
                           << COMMAND_THREADSAFE
                           << COMMAND_TITLE;
}

/*!
  The topic command has been processed. Now process the other
  metacommands that were found. These are not the text markup
  commands. 
 */
void CodeParser::processCommonMetaCommand(const Location &location,
                                          const QString &command,
					  const QString &arg,
                                          Node *node,
                                          Tree *tree)
{
    if (command == COMMAND_COMPAT) {
        node->setStatus(Node::Compat);
    }
    else if (command == COMMAND_DEPRECATED) {
	node->setStatus(Node::Deprecated);
    }
    else if (command == COMMAND_INGROUP) {
	tree->addToGroup(node, arg);
    }
    else if (command == COMMAND_INPUBLICGROUP) {
        tree->addToPublicGroup(node, arg);
    }
    else if (command == COMMAND_INMODULE) {
	node->setModuleName(arg);
    }
    else if (command == COMMAND_MAINCLASS) {
	node->setStatus(Node::Main);
    }
    else if (command == COMMAND_OBSOLETE) {
        if (node->status() != Node::Compat)
            node->setStatus(Node::Obsolete);
    }
    else if (command == COMMAND_NONREENTRANT) {
	node->setThreadSafeness(Node::NonReentrant);
    }
    else if (command == COMMAND_PRELIMINARY) {
	node->setStatus(Node::Preliminary);
    }
    else if (command == COMMAND_INTERNAL) {
        if (!showInternal) {
            node->setAccess(Node::Private);
            node->setStatus(Node::Internal);
        }
    }
    else if (command == COMMAND_REENTRANT) {
	node->setThreadSafeness(Node::Reentrant);
    }
    else if (command == COMMAND_SINCE) {
        node->setSince(arg);
    }
    else if (command == COMMAND_SUBTITLE) {
	if (node->type() == Node::Fake) {
	    FakeNode *fake = static_cast<FakeNode *>(node);
            fake->setSubTitle(arg);
        }
        else
            location.warning(tr("Ignored '\\%1'").arg(COMMAND_SUBTITLE));
    }
    else if (command == COMMAND_THREADSAFE) {
	node->setThreadSafeness(Node::ThreadSafe);
    }
    else if (command == COMMAND_TITLE) {
	if (node->type() == Node::Fake) {
	    FakeNode *fake = static_cast<FakeNode *>(node);
            fake->setTitle(arg);
        }
        else
	    location.warning(tr("Ignored '\\%1'").arg(COMMAND_TITLE));
    }
}

QT_END_NAMESPACE
