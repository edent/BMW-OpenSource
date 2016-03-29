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
struct MyStructure
{
    int count;
    QString name;
};
Q_DECLARE_METATYPE(MyStructure)

// Marshall the MyStructure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
{
    argument.beginStructure();
    argument << mystruct.count << mystruct.name;
    argument.endStructure();
    return argument;
}

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, MyStructure &mystruct)
{
    argument.beginStructure();
    argument >> mystruct.count >> mystruct.name;
    argument.endStructure();
    return argument;
}
//! [0]


//! [1]
qDBusRegisterMetaType<MyStructure>();
//! [1]


//! [2]
MyType item = qdbus_cast<Type>(argument);
//! [2]


//! [3]
MyType item;
argument >> item;
//! [3]


//! [4]
QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
{
    argument.beginStructure();
    argument << mystruct.member1 << mystruct.member2 << ... ;
    argument.endStructure();
    return argument;
}
//! [4]


//! [5]
QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
{
    argument.beginStructure();
    argument << mystruct.member1 << mystruct.member2;

    argument.beginStructure();
    argument << mystruct.member3.subMember1 << mystruct.member3.subMember2;
    argument.endStructure();

    argument << mystruct.member4;
    argument.endStructure();
    return argument;
}
//! [5]


//! [6]
// append an array of MyElement types
QDBusArgument &operator<<(QDBusArgument &argument, const MyArray &myarray)
{
    argument.beginArray( qMetaTypeId<MyElement>() );
    for ( int i = 0; i < myarray.length; ++i )
        argument << myarray.elements[i];
    argument.endArray();
    return argument;
}
//! [6]


//! [7]
// append a dictionary that associates ints to MyValue types
QDBusArgument &operator<<(QDBusArgument &argument, const MyDictionary &mydict)
{
    argument.beginMap( QVariant::Int, qMetaTypeId<MyValue>() );
    for ( int i = 0; i < mydict.length; ++i ) {
        argument.beginMapEntry();
        argument << mydict.data[i].key << mydict.data[i].value;
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}
//! [7]


//! [8]
const QDBusArgument &operator>>(const QDBusArgument &argument, MyStructure &mystruct)
{
    argument.beginStructure()
    argument >> mystruct.member1 >> mystruct.member2 >> mystruct.member3 >> ...;
    argument.endStructure();
    return argument;
}
//! [8]


//! [9]
// extract a MyArray array of MyElement elements
const QDBusArgument &operator>>(const QDBusArgument &argument, MyArray &myarray)
{
    argument.beginArray();
    myarray.clear();

    while ( !argument.atEnd() ) {
        MyElement element;
        argument >> element;
        myarray.append( element );
    }

    argument.endArray();
    return argument;
}
//! [9]


//! [10]
// extract a MyDictionary map that associates ints to MyValue elements
const QDBusArgument &operator>>(const QDBusArgument &argument, MyDictionary &mydict)
{
    argument.beginMap();
    mydict.clear();

    while ( !argMap.atEnd() ) {
        int key;
        MyValue value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        mydict.append( key, value );
    }

    argument.endMap();
    return argument;
}
//! [10]
