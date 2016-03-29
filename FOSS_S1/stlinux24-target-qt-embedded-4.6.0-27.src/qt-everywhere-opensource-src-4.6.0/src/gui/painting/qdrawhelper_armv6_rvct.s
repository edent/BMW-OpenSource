;****************************************************************************
;**
;** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
;** All rights reserved.
;** Contact: Nokia Corporation (qt-info@nokia.com)
;**
;** This file is part of the QtGui module of the Qt Toolkit.
;**
;** $QT_BEGIN_LICENSE:LGPL$
;** Commercial Usage
;** Licensees holding valid Qt Commercial licenses may use this file in
;** accordance with the Qt Commercial License Agreement provided with the
;** Software or, alternatively, in accordance with the terms contained in
;** a written agreement between you and Nokia.
;**
;** GNU Lesser General Public License Usage
;** Alternatively, this file may be used under the terms of the GNU Lesser
;** General Public License version 2.1 as published by the Free Software
;** Foundation and appearing in the file LICENSE.LGPL included in the
;** packaging of this file.  Please review the following information to
;** ensure the GNU Lesser General Public License version 2.1 requirements
;** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;**
;** In addition, as a special exception, Nokia gives you certain additional
;** rights.  These rights are described in the Nokia Qt LGPL Exception
;** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
;**
;** GNU General Public License Usage
;** Alternatively, this file may be used under the terms of the GNU
;** General Public License version 3.0 as published by the Free Software
;** Foundation and appearing in the file LICENSE.GPL included in the
;** packaging of this file.  Please review the following information to
;** ensure the GNU General Public License version 3.0 requirements will be
;** met: http://www.gnu.org/copyleft/gpl.html.
;**
;** If you have questions regarding the use of this file, please contact
;** Nokia at qt-info@nokia.com.
;** $QT_END_LICENSE$
;**
;****************************************************************************

;
;  W A R N I N G
;  -------------
;
; This file is not part of the Qt API.  It exists purely as an
; implementation detail.  This header file may change from version to
; version without notice, or even be removed.
;
; We mean it.
;

		ARM
		PRESERVE8

		INCLUDE		qdrawhelper_armv6_rvct.inc

;-----------------------------------------------------------------------------
; qt_memfill32_armv6
;
; @brief Not yet in use!
;
; @param dest			Destination	buffer						(r0)
; @param value			Value									(r1)
; @param count			Count									(r2)
;
;---------------------------------------------------------------------------
qt_memfill32_armv6 Function
		stmfd	sp!, {r4-r12, r14}

		mov		r3, r1
		mov		r4, r1
		mov		r5, r1
		mov		r6, r1
		mov		r7, r1
		mov		r8, r1
		mov		r9, r1

mfill_loop
		; Fill 32 pixels per loop iteration
		subs	r2, r2, #32
		stmgeia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}
		stmgeia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}
		stmgeia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}
		stmgeia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}
		bgt		mfill_loop

mfill_remaining

		; Fill up to 31 remaining pixels

		; Fill 16 pixels
		tst		r2, #16
		stmneia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}
		stmneia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}

		; Fill 8 pixels
		tst		r2, #8
		stmneia	r0!, {r1, r3, r4, r5, r6, r7, r8, r9}

		; Fill 4 pixels
		tst		r2, #4
		stmneia	r0!, {r1, r3, r4, r5}

		; Fill 2 pixels
		tst		r2, #2
		stmneia	r0!, {r1, r3}

		; Fill last one
		tst		r2, #1
		strne 	r1, [r0]

		ldmfd	sp!, {r4-r12, pc}		; pop and return

;-----------------------------------------------------------------------------
; comp_func_Source_arm
;
; @brief
;
; @param dest			Destination	buffer						(r0)
; @param src			Source buffer							(r1)
; @param length			Length									(r2)
; @param const_alpha	Constant alpha							(r3)
;
;---------------------------------------------------------------------------
comp_func_Source_armv6 Function
		stmfd	sp!, {r4-r12, r14}

		cmp		r3, #255 ; if(r3 == 255)
		bne		src2 ; branch if not

src1	PixCpy	r0, r1, r2

		ldmfd	sp!, {r4-r12, pc}		; pop and return

src2
		;ldr		r14, =ComponentHalf ; load 0x800080 to r14
		mov		r14, #0x800000
		add		r14, r14, #0x80

src22   BlendRow PixelSourceConstAlpha

		ldmfd	sp!, {r4-r12, pc}		; pop and return

;-----------------------------------------------------------------------------
; comp_func_SourceOver_arm
;
; @brief
;
; @param dest			Destination	buffer						(r0)
; @param src			Source buffer							(r1)
; @param length			Length									(r2)
; @param const_alpha	Constant alpha							(r3)
;
;---------------------------------------------------------------------------
comp_func_SourceOver_armv6 Function
		stmfd	sp!, {r4-r12, r14}

		;ldr		r14, =ComponentHalf ; load 0x800080 to r14
		mov		r14, #0x800000
		add		r14, r14, #0x80

		cmp		r3, #255 ; if(r3 == 255)
		bne		srcovr2 ; branch if not

srcovr1	 BlendRow PixelSourceOver

		ldmfd	sp!, {r4-r12, pc}		; pop and return

srcovr2

srcovr22  BlendRow PixelSourceOverConstAlpha

		ldmfd	sp!, {r4-r12, pc}		; pop and return


		END	; File end
