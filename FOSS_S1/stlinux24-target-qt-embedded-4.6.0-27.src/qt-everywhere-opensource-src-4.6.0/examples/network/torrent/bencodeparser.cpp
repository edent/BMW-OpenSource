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

#include "bencodeparser.h"

#include <QList>
#include <QMetaType>

BencodeParser::BencodeParser()
{
}

bool BencodeParser::parse(const QByteArray &content)
{
    if (content.isEmpty()) {
        errString = QString("No content");
        return false;
    }

    this->content = content;
    index = 0;
    infoStart = 0;
    infoLength = 0;
    return getDictionary(&dictionaryValue);
}

QString BencodeParser::errorString() const
{
    return errString;
}

QMap<QByteArray, QVariant> BencodeParser::dictionary() const
{
    return dictionaryValue;
}

QByteArray BencodeParser::infoSection() const
{
    return content.mid(infoStart, infoLength);
}

bool BencodeParser::getByteString(QByteArray *byteString)
{
    const int contentSize = content.size();
    int size = -1;
    do {
	char c = content.at(index);
	if (c < '0' || c > '9') {
	    if (size == -1)
		return false;
	    if (c != ':') {
		errString = QString("Unexpected character at pos %1: %2")
		    .arg(index).arg(c);
		return false;
	    }
	    ++index;
	    break;
	}
	if (size == -1)
	    size = 0;
	size *= 10;
	size += c - '0';
    } while (++index < contentSize);

    if (byteString)
	*byteString = content.mid(index, size);
    index += size;
    return true;
}

bool BencodeParser::getInteger(qint64 *integer)
{
    const int contentSize = content.size();
    if (content.at(index) != 'i')
	return false;

    ++index;
    qint64 num = -1;
    bool negative = false;

    do {
	char c = content.at(index);
	if (c < '0' || c > '9') {
	    if (num == -1) {
		if (c != '-' || negative)
		    return false;
		negative = true;
		continue;
	    } else {
		if (c != 'e') {
		    errString = QString("Unexpected character at pos %1: %2")
			.arg(index).arg(c);
		    return false;
		}
		++index;
		break;
	    }
	}
	if (num == -1)
	    num = 0;
	num *= 10;
	num += c - '0';
    } while (++index < contentSize);

    if (integer)
	*integer = negative ? -num : num;
    return true;
}

bool BencodeParser::getList(QList<QVariant> *list)
{
    const int contentSize = content.size();
    if (content.at(index) != 'l')
	return false;

    QList<QVariant> tmp;    
    ++index;

    do {
	if (content.at(index) == 'e') {
	    ++index;
	    break;
	}

	qint64 number;
	QByteArray byteString;
	QList<QVariant> tmpList;
	QMap<QByteArray, QVariant> dictionary;

	if (getInteger(&number))
	    tmp << number;
	else if (getByteString(&byteString))
	    tmp << byteString;
	else if (getList(&tmpList))
	    tmp << tmpList;
	else if (getDictionary(&dictionary))
	    tmp << qVariantFromValue<QMap<QByteArray, QVariant> >(dictionary);
	else {
	    errString = QString("error at index %1").arg(index);
	    return false;
	}
    } while (index < contentSize);

    if (list)
	*list = tmp;
    return true;
}

bool BencodeParser::getDictionary(QMap<QByteArray, QVariant> *dictionary)
{
    const int contentSize = content.size();
    if (content.at(index) != 'd')
	return false;

    QMap<QByteArray, QVariant> tmp;    
    ++index;

    do {
	if (content.at(index) == 'e') {
	    ++index;
	    break;
	}

	QByteArray key;
	if (!getByteString(&key))
	    break;

	if (key == "info")
	  infoStart = index;

	qint64 number;
	QByteArray byteString;
	QList<QVariant> tmpList;
	QMap<QByteArray, QVariant> dictionary;

	if (getInteger(&number))
	    tmp.insert(key, number);
	else if (getByteString(&byteString))
	    tmp.insert(key, byteString);
	else if (getList(&tmpList))
	    tmp.insert(key, tmpList);
	else if (getDictionary(&dictionary))
	    tmp.insert(key, qVariantFromValue<QMap<QByteArray, QVariant> >(dictionary));
	else {
	    errString = QString("error at index %1").arg(index);
	    return false;
	}

	if (key == "info")
	  infoLength = index - infoStart;

    } while (index < contentSize);

    if (dictionary)
	*dictionary = tmp;
    return true;
}
