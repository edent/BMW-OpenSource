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
; qt_blend_rgb32_on_rgb32_arm
;
; @brief
;
; @param dest			Destination	pixels						(r0)
; @param dbpl			Destination bytes per line				(r1)
; @param src			Source pixels							(r2)
; @param sbpl			Source bytes per line					(r3)
; @param w				Width							  (s0 -> r4)
; @param h				Height							  (s1 -> r5)
; @param const_alpha	Constant alpha					  (s2 -> r6)
;
;---------------------------------------------------------------------------
qt_blend_rgb32_on_rgb32_armv6 Function
		stmfd	sp!, {r4-r12, r14}

		; read arguments off the stack
		add		r8, sp, #10 * 4
		ldmia	r8, {r9-r11}

		; Reorganize registers

		mov		r4, r10
		mov		r5, r1
		mov		r6, r3

		mov		r1, r2
		mov		r2, r9
		mov		r3, r11

		; Now we have registers
		; @param dest			Destination	pixels						(r0)
		; @param src			Source pixels							(r1)
		; @param w				Width							  		(r2)
		; @param const_alpha	Constant alpha					  		(r3)
		; @param h				Height							  		(r4)
		; @param dbpl			Destination bytes per line				(r5)
		; @param sbpl			Source bytes per line					(r6)

		cmp		r3, #256 ; test if we have fully opaque constant alpha value
		bne		rgb32_blend_const_alpha ; branch if not

rgb32_blend_loop

		subs	r4, r4, #1
		bmi		rgb32_blend_exit	; while(h--)

rgb321	PixCpySafe r0, r1, r2

		add		r0, r0, r5 ; dest = dest + dbpl
		add		r1, r1, r6 ; src = src + sbpl

		b		rgb32_blend_loop


rgb32_blend_const_alpha

		;ldr		r14, =ComponentHalf ; load 0x800080 to r14
		mov		r14, #0x800000
		add		r14, r14, #0x80

		sub		r3, r3, #1	; const_alpha -= 1;

rgb32_blend_loop_const_alpha

		subs	r4, r4, #1
		bmi		rgb32_blend_exit	; while(h--)

rgb322	BlendRowSafe PixelSourceOverConstAlpha

		add		r0, r0, r5 ; dest = dest + dbpl
		add		r1, r1, r6 ; src = src + sbpl

		b		rgb32_blend_loop_const_alpha

rgb32_blend_exit

		ldmfd	sp!, {r4-r12, pc}		; pop and return



;-----------------------------------------------------------------------------
; qt_blend_argb32_on_argb32_arm
;
; @brief
;
; @param dest			Destination	pixels						(r0)
; @param dbpl			Destination bytes per line				(r1)
; @param src			Source pixels							(r2)
; @param sbpl			Source bytes per line					(r3)
; @param w				Width							  (s0 -> r4)
; @param h				Height							  (s1 -> r5)
; @param const_alpha	Constant alpha					  (s2 -> r6)
;
;---------------------------------------------------------------------------
qt_blend_argb32_on_argb32_armv6 Function
		stmfd	sp!, {r4-r12, r14}

	 	; read arguments off the stack
		add		r8, sp, #10 * 4
		ldmia	r8, {r9-r11}

		; Reorganize registers

		mov		r4, r10
		mov		r5, r1
		mov		r6, r3

		mov		r1, r2
		mov		r2, r9
		mov		r3, r11

		; Now we have registers
		; @param dest			Destination	pixels						(r0)
		; @param src			Source pixels							(r1)
		; @param w				Width							  		(r2)
		; @param const_alpha	Constant alpha					  		(r3)
		; @param h				Height							  		(r4)
		; @param dbpl			Destination bytes per line				(r5)
		; @param sbpl			Source bytes per line					(r6)

		;ldr		r14, =ComponentHalf ; load 0x800080 to r14
		mov		r14, #0x800000
		add		r14, r14, #0x80

		cmp		r3, #256 ; test if we have fully opaque constant alpha value
		bne		argb32_blend_const_alpha ; branch if not

argb32_blend_loop

		subs	r4, r4, #1
		bmi		argb32_blend_exit	; while(h--)

argb321	 BlendRowSafe PixelSourceOver

		add		r0, r0, r5 ; dest = dest + dbpl
		add		r1, r1, r6 ; src = src + sbpl

		b		argb32_blend_loop

argb32_blend_const_alpha

		sub		r3, r3, #1	; const_alpha -= 1;

argb32_blend_loop_const_alpha

		subs	r4, r4, #1
		bmi		argb32_blend_exit	; while(h--)

argb322	 BlendRowSafe PixelSourceOverConstAlpha

		add		r0, r0, r5 ; dest = dest + dbpl
		add		r1, r1, r6 ; src = src + sbpl

		b		argb32_blend_loop_const_alpha

argb32_blend_exit

		ldmfd	sp!, {r4-r12, pc}		; pop and return


		END ; File end
