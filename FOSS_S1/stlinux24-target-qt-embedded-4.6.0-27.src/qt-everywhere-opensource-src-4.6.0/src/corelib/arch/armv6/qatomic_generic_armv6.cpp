/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** This file implements the generic atomics interface using ARMv6 assembly
** instructions. It is more efficent than the inline versions when Qt is
** built for the THUMB instruction set, as the required instructions are
** only available in ARM state.
****************************************************************************/

#include <QtCore/qglobal.h>

#ifdef QT_HAVE_ARMV6

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE

#ifdef Q_CC_RVCT
#pragma push
#pragma arm
Q_CORE_EXPORT asm
bool QBasicAtomicInt_testAndSetOrdered(volatile int *_q_value, int expectedValue, int newValue)
{
    CODE32
    //R0 = _q_value
    //R1 = expectedValue
    //R2 = newValue
retry_testAndSetOrdered
    LDREX    r3,[r0]        //r3 = *_q_value
    EORS     r3,r3,r1       //if (r3 == expectedValue) {
    STREXEQ  r3,r2,[r0]     //*_q_value = newvalue, r3 = error
    TEQEQ    r3,#1          //if error
    BEQ      retry_testAndSetOrdered          //then goto retry }
    RSBS     r0,r3,#1       //return (r3 == 0)
    MOVCC    r0,#0
    BX       r14
}

Q_CORE_EXPORT asm
int QBasicAtomicInt_fetchAndStoreOrdered(volatile int *_q_value, int newValue)
{
    CODE32
//R0 = _q_value
//R1 = newValue
retry_fetchAndStoreOrdered
    LDREX    r3,[r0]        //r3 = *_q_value
    STREX    r2,r1,[r0]     //*_q_value = newValue, r2 = error
    TEQ      r2,#0          //if error
    BNE      retry_fetchAndStoreOrdered          //then goto retry
    MOV      r0,r3          //return r3
    BX       r14
}

Q_CORE_EXPORT asm
int QBasicAtomicInt_fetchAndAddOrdered(volatile int *_q_value, int valueToAdd)
{
    CODE32
    //R0 = _q_value
    //R1 = valueToAdd
    STMDB    sp!,{r12,lr}
retry_fetchAndAddOrdered
    LDREX    r2,[r0]        //r2 = *_q_value
    ADD      r3,r2,r1       //r3 = r2 + r1
    STREX    r12,r3,[r0]    //*_q_value = r3, r12 = error
    TEQ      r12,#0         //if error
    BNE      retry_fetchAndAddOrdered          //then retry
    MOV      r0,r2          //return r2
    LDMIA    sp!,{r12,pc}
}

Q_CORE_EXPORT asm
bool QBasicAtomicPointer_testAndSetOrdered(void * volatile *_q_value,
                                           void *expectedValue,
                                           void *newValue)
{
    CODE32
    //R0 = _q_value
    //R1 = expectedValue
    //R2 = newValue
retryPointer_testAndSetOrdered
    LDREX    r3,[r0]        //r3 = *_q_value
    EORS     r3,r3,r1       //if (r3 == expectedValue) {
    STREXEQ  r3,r2,[r0]     //*_q_value = newvalue, r3 = error
    TEQEQ    r3,#1          //if error
    BEQ      retryPointer_testAndSetOrdered          //then goto retry }
    RSBS     r0,r3,#1       //return (r3 == 0)
    MOVCC    r0,#0
    BX       r14
}

Q_CORE_EXPORT asm
void *QBasicAtomicPointer_fetchAndStoreOrdered(void * volatile *_q_value, void *newValue)
{
    CODE32
    //R0 = _q_value
    //R1 = newValue
retryPointer_fetchAndStoreOrdered
    LDREX    r3,[r0]        //r3 = *_q_value
    STREX    r2,r1,[r0]     //*_q_value = newValue, r2 = error
    TEQ      r2,#0          //if error
    BNE      retryPointer_fetchAndStoreOrdered          //then goto retry
    MOV      r0,r3          //return r3
    BX       r14
}

Q_CORE_EXPORT asm
void *QBasicAtomicPointer_fetchAndAddOrdered(void * volatile *_q_value, qptrdiff valueToAdd)
{
    CODE32
    //R0 = _q_value
    //R1 = valueToAdd
    STMDB    sp!,{r12,lr}
retryPointer_fetchAndAddOrdered
    LDREX    r2,[r0]        //r2 = *_q_value
    ADD      r3,r2,r1       //r3 = r2 + r1
    STREX    r12,r3,[r0]    //*_q_value = r3, r12 = error
    TEQ      r12,#0         //if error
    BNE      retryPointer_fetchAndAddOrdered          //then retry
    MOV      r0,r2          //return r2
    LDMIA    sp!,{r12,pc}
}

#pragma pop
#elif defined (Q_CC_GCCE)
Q_CORE_EXPORT __declspec( naked )
bool QBasicAtomicInt_testAndSetOrdered(volatile int *_q_value, int expectedValue, int newValue)
{
    //R0 = _q_value
    //R1 = expectedValue
    //R2 = newValue
    asm("retry_testAndSetOrdered:");
    asm("    LDREX    r3,[r0]");        //r3 = *_q_value
    asm("    EORS     r3,r3,r1");       //if (r3 == expectedValue) {
    asm("    STREXEQ  r3,r2,[r0]");     //*_q_value = newvalue, r3 = error
    asm("    TEQEQ    r3,#1");          //if error
    asm("    BEQ      retry_testAndSetOrdered");          //then goto retry }
    asm("    RSBS     r0,r3,#1");       //return (r3 == 0)
    asm("    MOVCC    r0,#0");
    asm("    BX       r14");
}

Q_CORE_EXPORT __declspec( naked )
int QBasicAtomicInt_fetchAndStoreOrdered(volatile int *_q_value, int newValue)
{
//R0 = _q_value
//R1 = newValue
    asm("retry_fetchAndStoreOrdered:");
    asm("    LDREX    r3,[r0]");        //r3 = *_q_value
    asm("    STREX    r2,r1,[r0]");     //*_q_value = newValue, r2 = error
    asm("    TEQ      r2,#0");          //if error
    asm("    BNE      retry_fetchAndStoreOrdered");          //then goto retry
    asm("    MOV      r0,r3");          //return r3
    asm("    BX       r14");
}

Q_CORE_EXPORT __declspec( naked )
int QBasicAtomicInt_fetchAndAddOrdered(volatile int *_q_value, int valueToAdd)
{
    //R0 = _q_value
    //R1 = valueToAdd
    asm("    STMDB    sp!,{r12,lr}");
    asm("retry_fetchAndAddOrdered:");
    asm("    LDREX    r2,[r0]");        //r2 = *_q_value
    asm("    ADD      r3,r2,r1 ");      //r3 = r2 + r1
    asm("    STREX    r12,r3,[r0]");    //*_q_value = r3, r12 = error
    asm("    TEQ      r12,#0");         //if error
    asm("    BNE      retry_fetchAndAddOrdered");          //then retry
    asm("    MOV      r0,r2");          //return r2
    asm("    LDMIA    sp!,{r12,pc}");
}

Q_CORE_EXPORT __declspec( naked )
bool QBasicAtomicPointer_testAndSetOrdered(void * volatile *_q_value,
                                           void *expectedValue,
                                           void *newValue)
{
    //R0 = _q_value
    //R1 = expectedValue
    //R2 = newValue
    asm("retryPointer_testAndSetOrdered:");
    asm("    LDREX    r3,[r0]");        //r3 = *_q_value
    asm("    EORS     r3,r3,r1");       //if (r3 == expectedValue) {
    asm("    STREXEQ  r3,r2,[r0]");     //*_q_value = newvalue, r3 = error
    asm("    TEQEQ    r3,#1");          //if error
    asm("    BEQ      retryPointer_testAndSetOrdered");          //then goto retry }
    asm("    RSBS     r0,r3,#1");       //return (r3 == 0)
    asm("    MOVCC    r0,#0");
    asm("    BX       r14");
}

Q_CORE_EXPORT __declspec( naked )
void *QBasicAtomicPointer_fetchAndStoreOrdered(void * volatile *_q_value, void *newValue)
{
    //R0 = _q_value
    //R1 = newValue
    asm("retryPointer_fetchAndStoreOrdered:");
    asm("    LDREX    r3,[r0]");        //r3 = *_q_value
    asm("    STREX    r2,r1,[r0]");     //*_q_value = newValue, r2 = error
    asm("    TEQ      r2,#0");          //if error
    asm("    BNE      retryPointer_fetchAndStoreOrdered");          //then goto retry
    asm("    MOV      r0,r3");          //return r3
    asm("    BX       r14");
}

Q_CORE_EXPORT __declspec( naked )
void *QBasicAtomicPointer_fetchAndAddOrdered(void * volatile *_q_value, qptrdiff valueToAdd)
{
    //R0 = _q_value
    //R1 = valueToAdd
    asm("    STMDB    sp!,{r12,lr}");
    asm("retryPointer_fetchAndAddOrdered:");
    asm("    LDREX    r2,[r0]");        //r2 = *_q_value
    asm("    ADD      r3,r2,r1");       //r3 = r2 + r1
    asm("    STREX    r12,r3,[r0]");    //*_q_value = r3, r12 = error
    asm("    TEQ      r12,#0");         //if error
    asm("    BNE      retryPointer_fetchAndAddOrdered");          //then retry
    asm("    MOV      r0,r2");          //return r2
    asm("    LDMIA    sp!,{r12,pc}");
}
#else
#error unknown arm compiler
#endif
QT_END_NAMESPACE
#endif
