/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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
handler.cpp

Provides a handler for processing XML elements found by the reader.

The handler looks for <title> and <link> elements within <item> elements,
and records the text found within them. Link information stored within
rdf:about attributes of <item> elements is also recorded when it is
available.

For each item found, a signal is emitted which specifies its title and
link information. This may be used by user interfaces for the purpose of
displaying items as they are read.
*/

#include <QtGui>

#include "handler.h"

/*
    Reset the state of the handler to ensure that new documents are
    read correctly.

    We return true to indicate that parsing should continue.
*/

bool Handler::startDocument()
{
    inItem = false;
    inTitle = false;
    inLink = false;

    return true;
}

/*
    Process each starting element in the XML document.

    Nested item, title, or link elements are not allowed, so we return false
    if we encounter any of these. We also prohibit multiple definitions of
    title strings.

    Link destinations are read by this function if they are specified as
    attributes in item elements.

    For all cases not explicitly checked for, we return true to indicate that
    the element is acceptable, and that parsing should continue. By doing
    this, we can ignore elements in which we are not interested.
*/

bool Handler::startElement(const QString &, const QString &,
    const QString & qName, const QXmlAttributes &attr)
{
    if (qName == "item") {

        if (inItem)
            return false;
        else {
            inItem = true;
            linkString = attr.value("rdf:about");
        }
    }
    else if (qName == "title") {

        if (inTitle)
            return false;
        else if (!titleString.isEmpty())
            return false;
        else if (inItem)
            inTitle = true;
    }
    else if (qName == "link") {

        if (inLink)
            return false;
        else if (inItem)
            inLink = true;
    }

    return true;
}

/*
    Process each ending element in the XML document.

    For recognized elements, we reset flags to ensure that we can read new
    instances of these elements. If we have read an item element, emit a
    signal to indicate that a new item is available for display.

    We return true to indicate that parsing should continue.
*/

bool Handler::endElement(const QString &, const QString &,
    const QString & qName)
{
    if (qName == "title" && inTitle)
        inTitle = false;
    else if (qName == "link" && inLink)
        inLink = false;
    else if (qName == "item") {
        if (!titleString.isEmpty() && !linkString.isEmpty())
            emit newItem(titleString, linkString);
        inItem = false;
        titleString = "";
        linkString = "";
    }

    return true;
}

/*
    Collect characters when reading the contents of title or link elements
    when they occur within an item element.

    We return true to indicate that parsing should continue.
*/

bool Handler::characters (const QString &chars)
{
    if (inTitle)
        titleString += chars;
    else if (inLink)
        linkString += chars;

    return true;
}

/*
    Report a fatal parsing error, and return false to indicate to the reader
    that parsing should stop.
*/

//! [0]
bool Handler::fatalError (const QXmlParseException & exception)
{
    qWarning() << "Fatal error on line" << exception.lineNumber()
               << ", column" << exception.columnNumber() << ":"
               << exception.message();

    return false;
}
//! [0]
