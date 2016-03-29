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


void wrapInFunction()
{

//! [0]
QVERIFY(1 + 1 == 2);
//! [0]


//! [1]
QVERIFY2(1 + 1 == 2, "A breach in basic arithmetic occured.");
//! [1]


//! [2]
QCOMPARE(QString("hello").toUpper(), QString("HELLO"));
//! [2]


//! [3]
void TestQString::toInt_data()
{
    QTest::addColumn<QString>("aString");
    QTest::addColumn<int>("expected");

    QTest::newRow("positive value") << "42" << 42;
    QTest::newRow("negative value") << "-42" << -42;
    QTest::newRow("zero") << "0" << 0;
}
//! [3]


//! [4]
void TestQString::toInt()
{
     QFETCH(QString, aString);
     QFETCH(int, expected);

     QCOMPARE(aString.toInt(), expected);
}
//! [4]


//! [5]
if (sizeof(int) != 4)
    QFAIL("This test has not been ported to this platform yet.");
//! [5]


//! [6]
QFETCH(QString, myString);
QCOMPARE(QString("hello").toUpper(), myString);
//! [6]


//! [7]
QTEST(QString("hello").toUpper(), "myString");
//! [7]


//! [8]
if (!QSqlDatabase::drivers().contains("SQLITE"))
    QSKIP("This test requires the SQLITE database driver", SkipAll);
//! [8]


//! [9]
QEXPECT_FAIL("", "Will fix in the next release", Continue);
QCOMPARE(i, 42);
QCOMPARE(j, 43);
//! [9]


//! [10]
QEXPECT_FAIL("data27", "Oh my, this is soooo broken", Abort);
QCOMPARE(i, 42);
//! [10]


//! [11]
class TestQString: public QObject { ... };
QTEST_MAIN(TestQString)
//! [11]


//! [12]
#ifdef Q_WS_X11
    QTEST_MAIN(MyX11Test)
#else
    // do nothing on non-X11 platforms
    QTEST_NOOP_MAIN
#endif
//! [12]


//! [13]
QTest::keyClick(myWidget, 'a');
//! [13]


//! [14]
QTest::keyClick(myWidget, Qt::Key_Escape);

QTest::keyClick(myWidget, Qt::Key_Escape, Qt::ShiftModifier, 200);
//! [14]


//! [15]
QTest::keyClicks(myWidget, "hello world");
//! [15]


//! [16]
namespace QTest {
    template<>
    char *toString(const MyPoint &point)
    {
        QByteArray ba = "MyPoint(";
        ba += QByteArray::number(point.x()) + ", " + QByteArray::number(point.y());
        ba += ")";
        return qstrdup(ba.data());
    }
}
//! [16]


//! [17]
int i = 0;
while (myNetworkServerNotResponding() && i++ < 50)
    QTest::qWait(250);
//! [17]


//! [18]
MyFirstTestObject test1;
QTest::qExec(&test1);

MySecondTestObject test2;
QTest::qExec(&test2);
//! [18]


//! [19]
QDir dir;

QTest::ignoreMessage(QtWarningMsg, "QDir::mkdir: Empty or null file name(s)");
dir.mkdir("");
//! [19]


//! [20]
void myTestFunction_data()
{
    QTest::addColumn<QString>("aString");
    QTest::newRow("just hello") << QString("hello");
    QTest::newRow("a null string") << QString();
}
//! [20]


//! [21]
void myTestFunction_data() {
    QTest::addColumn<int>("intval");
    QTest::addColumn<QString>("str");
    QTest::addColumn<double>("dbl");

    QTest::newRow("row1") << 1 << "hello" << 1.5;
}
//! [21]


//! [22]
void MyTestClass::cleanup()
{
    if (qstrcmp(currentTestFunction(), "myDatabaseTest") == 0) {
        // clean up all database connections
        closeAllDatabases();
    }
}
//! [22]


//! [23]
QTest::qSleep(250);
//! [23]

//! [24]
QWidget widget;
widget.show();
QTest::qWaitForWindowShown(&widget);
//! [24]

}

