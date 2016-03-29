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
bool function(const T &t);
//! [0]


//! [1]
bool allLowerCase(const QString &string)
{
    return string.lowered() == string;
}

QStringList strings = ...;
QFuture<QString> lowerCaseStrings = QtConcurrent::filtered(strings, allLowerCase);
//! [1]


//! [2]
QStringList strings = ...;
QFuture<void> future = QtConcurrent::filter(strings, allLowerCase);
//! [2]


//! [3]
V function(T &result, const U &intermediate)
//! [3]


//! [4]
void addToDictionary(QSet<QString> &dictionary, const QString &string)
{
    dictionary.insert(string);
}

QStringList strings = ...;
QFuture<QSet<QString> > dictionary = QtConcurrent::filteredReduced(strings, allLowerCase, addToDictionary);
//! [4]


//! [5]
QStringList strings = ...;
QFuture<QString> lowerCaseStrings = QtConcurrent::filtered(strings.constBegin(), strings.constEnd(), allLowerCase);

// filter in-place only works on non-const iterators
QFuture<void> future = QtConcurrent::filter(strings.begin(), strings.end(), allLowerCase);

QFuture<QSet<QString> > dictionary = QtConcurrent::filteredReduced(strings.constBegin(), strings.constEnd(), allLowerCase, addToDictionary);
//! [5]


//! [6]
QStringList strings = ...;

// each call blocks until the entire operation is finished
QStringList lowerCaseStrings = QtConcurrent::blockingFiltered(strings, allLowerCase);


QtConcurrent::blockingFilter(strings, allLowerCase);

QSet<QString> dictionary = QtConcurrent::blockingFilteredReduced(strings, allLowerCase, addToDictionary);
//! [6]


//! [7]
// keep only images with an alpha channel
QList<QImage> images = ...;
QFuture<void> alphaImages = QtConcurrent::filter(strings, &QImage::hasAlphaChannel);

// keep only gray scale images
QList<QImage> images = ...;
QFuture<QImage> grayscaleImages = QtConcurrent::filtered(images, &QImage::isGrayscale);

// create a set of all printable characters
QList<QChar> characters = ...;
QFuture<QSet<QChar> > set = QtConcurrent::filteredReduced(characters, &QChar::isPrint, &QSet<QChar>::insert);
//! [7]


//! [8]
// can mix normal functions and member functions with QtConcurrent::filteredReduced()

// create a dictionary of all lower cased strings
extern bool allLowerCase(const QString &string);
QStringList strings = ...;
QFuture<QSet<int> > averageWordLength = QtConcurrent::filteredReduced(strings, allLowerCase, QSet<QString>::insert);

// create a collage of all gray scale images
extern void addToCollage(QImage &collage, const QImage &grayscaleImage);
QList<QImage> images = ...;
QFuture<QImage> collage = QtConcurrent::filteredReduced(images, &QImage::isGrayscale, addToCollage);
//! [8]


//! [9]
bool QString::contains(const QRegExp &regexp) const;
//! [9]


//! [10]
boost::bind(&QString::contains, QRegExp("^\\S+$")); // matches strings without whitespace
//! [10]


//! [11]
bool contains(const QString &string)
//! [11]


//! [12]
QStringList strings = ...;
boost::bind(static_cast<bool(QString::*)(const QRegExp&)>( &QString::contains ), QRegExp("..." ));
//! [12]

//! [13]
struct StartsWith
{
    StartsWith(const QString &string)
    : m_string(string) { }

    typedef bool result_type;

    bool operator()(const QString &testString)
    {
        return testString.startsWith(m_string);
    }

    QString m_string;
};

QList<QString> strings = ...;
QFuture<QString> fooString = QtConcurrent::filtered(images, StartsWith(QLatin1String("Foo")));
//! [13]
