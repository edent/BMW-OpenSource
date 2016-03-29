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
Q3SqlPropertyMap *myMap  = new Q3SqlPropertyMap();
Q3SqlForm        *myForm = new Q3SqlForm(this);
MyEditor myEditor(this);

// Set the Q3SqlForm's record buffer to the update buffer of
// a pre-existing Q3SqlCursor called 'cur'.
myForm->setRecord(cur->primeUpdate());

// Install the customized map
myMap->insert("MyEditor", "content");
myForm->installPropertyMap(myMap); // myForm now owns myMap
...
// Insert a field into the form that uses a myEditor to edit the
// field 'somefield'
myForm->insert(&myEditor, "somefield");

// Update myEditor with the value from the mapped database field
myForm->readFields();
...
// Let the user edit the form
...
// Update the database fields with the values in the form
myForm->writeFields();
...
//! [0]


//! [1]
Q3SqlPropertyMap *myMap = new Q3SqlPropertyMap;

myMap->insert("MyEditor", "content");
Q3SqlPropertyMap::installDefaultMap(myMap);
...
//! [1]
