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

//! [0]
QDomDocument doc;
QDomImplementation impl;

// This will create the element, but the resulting XML document will
// be invalid, because '~' is not a valid character in a tag name.
impl.setInvalidDataPolicy(QDomImplementation::AcceptInvalidData);
QDomElement elt1 = doc.createElement("foo~bar");

// This will create an element with the tag name "foobar".
impl.setInvalidDataPolicy(QDomImplementation::DropInvalidData);
QDomElement elt2 = doc.createElement("foo~bar");

// This will create a null element.
impl.setInvalidDataPolicy(QDomImplementation::ReturnNullNode);
QDomElement elt3 = doc.createElement("foo~bar");
//! [0]


//! [1]
QDomDocument d;
d.setContent(someXML);
QDomNode n = d.firstChild();
while (!n.isNull()) {
    if (n.isElement()) {
        QDomElement e = n.toElement();
        cout << "Element name: " << e.tagName() << endl;
        break;
    }
    n = n.nextSibling();
}
//! [1]


//! [2]
QDomDocument document;
QDomElement element1 = document.documentElement();
QDomElement element2 = element1;
//! [2]


//! [3]
QDomElement element3 = document.createElement("MyElement");
QDomElement element4 = document.createElement("MyElement");
//! [3]


//! [4]
<body>
<h1>Heading</h1>
<p>Hello <b>you</b></p>
</body>
//! [4]


//! [5]
<h1>Heading</h1>
<p>The text...</p>
<h2>Next heading</h2>
//! [5]


//! [6]
<h1>Heading</h1>
<p>The text...</p>
<h2>Next heading</h2>
//! [6]


//! [7]
<link href="http://qt.nokia.com" color="red" />
//! [7]


//! [8]
QDomElement e = //...
//...
QDomAttr a = e.attributeNode("href");
cout << a.value() << endl;                // prints "http://qt.nokia.com"
a.setValue("http://qt.nokia.com/doc"); // change the node's attribute
QDomAttr a2 = e.attributeNode("href");
cout << a2.value() << endl;               // prints "http://qt.nokia.com/doc"
//! [8]


//! [9]
QDomElement e = //...
//...
QString s = e.text()
//! [9]


//! [10]
QString text;
QDomElement element = doc.documentElement();
for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
{
    QDomText t = n.toText();
    if (!t.isNull())
        text += t.data();
}
//! [10]


//! [11]
QDomDocument doc = // ...
QDomElement root = doc.firstChildElement("database");
QDomElement elt = root.firstChildElement("entry");
for (; !elt.isNull(); elt = elt.nextSiblingElement("entry")) {
    // ...
}
//! [11]


//! [12]
<img src="myimg.png">
//! [12]


//! [13]
<h1>Hello <b>Qt</b> <![CDATA[<xml is cool>]]></h1>
//! [13]


//! [14]
Hello Qt <xml is cool>
//! [14]


//! [15]
<!-- this is a comment -->
//! [15]


//! [16]
QDomDocument doc("mydocument");
QFile file("mydocument.xml");
if (!file.open(QIODevice::ReadOnly))
    return;
if (!doc.setContent(&file)) {
    file.close();
    return;
}
file.close();

// print out the element names of all elements that are direct children
// of the outermost element.
QDomElement docElem = doc.documentElement();

QDomNode n = docElem.firstChild();
while(!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull()) {
        cout << qPrintable(e.tagName()) << endl; // the node really is an element.
    }
    n = n.nextSibling();
}

// Here we append a new element to the end of the document
QDomElement elem = doc.createElement("img");
elem.setAttribute("src", "myimage.png");
docElem.appendChild(elem);
//! [16]


//! [17]
QDomDocument doc("MyML");
QDomElement root = doc.createElement("MyML");
doc.appendChild(root);

QDomElement tag = doc.createElement("Greeting");
root.appendChild(tag);

QDomText t = doc.createTextNode("Hello World");
tag.appendChild(t);

QString xml = doc.toString();
//! [17]
