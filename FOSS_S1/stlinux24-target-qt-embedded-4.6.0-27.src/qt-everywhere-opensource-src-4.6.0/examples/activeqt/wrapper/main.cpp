/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QAxFactory>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QToolButton>
#include <QPixmap>

/* XPM */
static const char *fileopen[] = {
"    16    13        5            1",
". c #040404",
"# c #808304",
"a c None",
"b c #f3f704",
"c c #f3f7f3",
"aaaaaaaaa...aaaa",
"aaaaaaaa.aaa.a.a",
"aaaaaaaaaaaaa..a",
"a...aaaaaaaa...a",
".bcb.......aaaaa",
".cbcbcbcbc.aaaaa",
".bcbcbcbcb.aaaaa",
".cbcb...........",
".bcb.#########.a",
".cb.#########.aa",
".b.#########.aaa",
"..#########.aaaa",
"...........aaaaa"
};


//! [0]
class ActiveQtFactory : public QAxFactory
{
public:
    ActiveQtFactory( const QUuid &lib, const QUuid &app ) 
	: QAxFactory( lib, app ) 
    {}
    QStringList featureList() const
    {
	QStringList list;
	list << "QCheckBox";
	list << "QRadioButton";
	list << "QPushButton";
	list << "QToolButton";
	return list;
    }
    QObject *createObject(const QString &key)
    {
	if ( key == "QCheckBox" )
	    return new QCheckBox(0);
	if ( key == "QRadioButton" )
	    return new QRadioButton(0);
	if ( key == "QPushButton" )
	    return new QPushButton(0 );
	if ( key == "QToolButton" ) {
	    QToolButton *tb = new QToolButton(0);
//	    tb->setIcon( QPixmap(fileopen) );
	    return tb;
	}

	return 0;
    }
    const QMetaObject *metaObject( const QString &key ) const
    {
	if ( key == "QCheckBox" )
	    return &QCheckBox::staticMetaObject;
	if ( key == "QRadioButton" )
	    return &QRadioButton::staticMetaObject;
	if ( key == "QPushButton" )
	    return &QPushButton::staticMetaObject;
	if ( key == "QToolButton" )
	    return &QToolButton::staticMetaObject;

	return 0;
    }
    QUuid classID( const QString &key ) const
    {
	if ( key == "QCheckBox" )
	    return "{6E795DE9-872D-43CF-A831-496EF9D86C68}";
	if ( key == "QRadioButton" )
	    return "{AFCF78C8-446C-409A-93B3-BA2959039189}";
	if ( key == "QPushButton" )
	    return "{2B262458-A4B6-468B-B7D4-CF5FEE0A7092}";
	if ( key == "QToolButton" )
	    return "{7c0ffe7a-60c3-4666-bde2-5cf2b54390a1}";

	return QUuid();
    }
    QUuid interfaceID( const QString &key ) const
    {
	if ( key == "QCheckBox" )
	    return "{4FD39DD7-2DE0-43C1-A8C2-27C51A052810}";
	if ( key == "QRadioButton" )
	    return "{7CC8AE30-206C-48A3-A009-B0A088026C2F}";
	if ( key == "QPushButton" )
	    return "{06831CC9-59B6-436A-9578-6D53E5AD03D3}";
	if ( key == "QToolButton" )
	    return "{6726080f-d63d-4950-a366-9bf33e5cdf84}";

	return QUuid();
    }
    QUuid eventsID( const QString &key ) const
    {
	if ( key == "QCheckBox" )
	    return "{FDB6FFBE-56A3-4E90-8F4D-198488418B3A}";
	if ( key == "QRadioButton" )
	    return "{73EE4860-684C-4A66-BF63-9B9EFFA0CBE5}";
	if ( key == "QPushButton" )
	    return "{3CC3F17F-EA59-4B58-BBD3-842D467131DD}";
	if ( key == "QToolButton" )
	    return "{f4d421fd-4ead-4fd9-8a25-440766939639}";

	return QUuid();
    }
};
//! [0] //! [1]

QAXFACTORY_EXPORT( ActiveQtFactory, "{3B756301-0075-4E40-8BE8-5A81DE2426B7}", "{AB068077-4924-406a-BBAF-42D91C8727DD}" )
//! [1]
