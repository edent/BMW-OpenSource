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
void myFunction(bool useSubClass)
{
    MyClass *p = useSubClass ? new MyClass() : new MySubClass;
    QIODevice *device = handsOverOwnership();

    if (m_value > 3) {
        delete p;
        delete device;
        return;
    }

    try {
        process(device);
    }
    catch (...) {
        delete p;
        delete device;
        throw;
    }

    delete p;
    delete device;
}
//! [0]

//! [1]
void myFunction(bool useSubClass)
{
    // assuming that MyClass has a virtual destructor
    QScopedPointer<MyClass> p(useSubClass ? new MyClass() : new MySubClass);
    QScopedPointer<QIODevice> device(handsOverOwnership());

    if (m_value > 3)
        return;

    process(device);
}
//! [1]

//! [2]
    const QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<const QWidget> p(new QWidget());

    QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<QWidget> p(new QWidget());

    QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<QWidget> p(new QWidget());

    const QWidget *p = new QWidget();
    // is equivalent to:
    QScopedPointer<const QWidget> p(new QWidget());

//! [2]

//! [3]
if (scopedPointer) {
    ...
}
//! [3]

//! [4]
class MyPrivateClass; // forward declare MyPrivateClass

class MyClass
{
private:
    QScopedPointer<MyPrivateClass> privatePtr; // QScopedPointer to forward declared class

public:
    MyClass(); // OK
    inline ~MyClass() {} // VIOLATION - Destructor must not be inline

private:
    Q_DISABLE_COPY(MyClass) // OK - copy constructor and assignment operators
                             // are now disabled, so the compiler won't implicitely
                             // generate them.
};
//! [4]

//! [5]
// this QScopedPointer deletes its data using the delete[] operator:
QScopedPointer<int, QScopedPointerArrayDeleter<int> > arrayPointer(new int[42]);

// this QScopedPointer frees its data using free():
QScopedPointer<int, QScopedPointerPodDeleter<int> > podPointer(reinterpret_cast<int *>(malloc(42)));

// this struct calls "myCustomDeallocator" to delete the pointer
struct ScopedPointerCustomDeleter
{
    static inline void cleanup(MyCustomClass *pointer)
    {
        myCustomDeallocator(pointer);
    }
};

// QScopedPointer using a custom deleter:
QScopedPointer<MyCustomClass, ScopedPointerCustomDeleter> customPointer(new MyCustomClass);
//! [5]
