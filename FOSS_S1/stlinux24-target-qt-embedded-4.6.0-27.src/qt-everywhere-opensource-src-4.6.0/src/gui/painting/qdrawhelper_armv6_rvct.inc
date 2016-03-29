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

;-----------------------------------------------------------------------------
; Globals.
; Earch marcro expects that caller has loaded 0x800080 to r14.
;-----------------------------------------------------------------------------

ComponentHalf	EQU	0x800080

;-----------------------------------------------------------------------------
; ARM assembly implementations of accelerated graphics operations.
;
; Conventions:
;
; 	- r0 = Target buffer pointer
;	- r1 = Source buffer pointer
; 	- r2 = Length of the buffer to blend
;	- r3 = Constant alpha for source buffer
;
;-----------------------------------------------------------------------------

; A macro for transparently defining ARM functions
		MACRO
$func	Function
		AREA Function_$func, CODE
		GLOBAL $func
		ALIGN 4
		CODE32
$func
		MEND


;-----------------------------------------------------------------------------
; Armv6 boosted implementation of BYTE_MUL(...) function found in qdrawhelper_p.h.
;
;  @param dst	Destination register where to store the result
;  @param x     Value to multiply
;  @param a     Multiplicator byte
;  @param r14   Component half 0x800080
;
;  @note Trashes x, r8
;-----------------------------------------------------------------------------
		MACRO
		ByteMul $dst, $x, $a

		; static inline uint BYTE_MUL(uint x, uint a)

		; uint r8 = (x & 0xff00ff) * a + 0x800080
		uxtb16	r8, $x	; r8 = r8 & 0x00FF00FF
		mla		r8, r8, $a, r14

		; x = ((r >> 8) & 0xff00ff) * a + 0x800080
		uxtb16	$x, $x, ror #8
		mla		$x, $x, $a, r14


		; r8 = (r8 + ((r8 >> 8) & 0xff00ff) ) >> 8
		; r8 &= 0xff00ff
		uxtab16	r8, r8, r8, ror #8
		uxtb16	r8, r8, ror #8

		; x = x + ((x >>8) & 0xff00ff)
		uxtab16	$x, $x, $x, ror #8

		; x &= 0xff00ff00
		; x |= r8
		uxtb16	$x, $x, ror #8
		orr		$dst, r8, $x, lsl #8

		MEND

;-----------------------------------------------------------------------------
; Armv6 boosted implementation of INTERPOLATE_PIXEL_255(...) function found in
; qdrawhelper_p.h.
;
;  @param dst	Destination register where to store the result
;  @param x     First value to multiply
;  @param a		Multiplicator byte for first value
;  @param y     Second value to multiply
;  @param b     Multiplicator byte for second value
;  @param r14   Component half 0x800080
;
;
;  @note Trashes x, r8, r14
;-----------------------------------------------------------------------------
		MACRO
		InterpolatePixel255 $dst, $x, $a, $y, $b

		; static inline uint INTERPOLATE_PIXEL_255(uint x, uint a, uint y, uint b)

		; First calculate the parts where we need 0x800080

		; uint r8 = (((x & 0xff00ff) * a) + 0x800080)
		uxtb16	r8, $x	; r8 = r8 & 0x00FF00FF
		mla		r8, r8, $a, r14

		; x = ((((x >> 8) & 0xff00ff) * a) + 0x800080)
		uxtb16	$x, $x, ror #8
		mla		$x, $x, $a, r14

		; Now we are trashing r14 to free it for other purposes

		; uint r14 = (y & 0xff00ff) * b
		uxtb16	r14, $y	; r14 = y & 0x00FF00FF
		mul		r14, r14, $b

		; r8 = r8 + r14
		add		r8, r8, r14

		; r8 = (r8 + ((r8 >> 8) & 0xff00ff) ) >> 8
		; r8 &= 0xff00ff
		uxtab16	r8, r8, r8, ror #8
		uxtb16	r8, r8, ror #8

		; r14 = ((y >> 8) & 0xff00ff) * b
		uxtb16	r14, $y, ror #8	; r14 = ((y >> 8) & 0xFF00FF)
		mul		r14, r14, $b

		; x = x + r14
		add		$x, $x, r14

		; x = x + ((x >>8) & 0xff00ff)
		uxtab16	$x, $x, $x, ror #8

		; x &= 0xff00ff00
		; x |= r8
		uxtb16	$x, $x, ror #8
		orr		$dst, r8, $x, lsl #8

		MEND

;-----------------------------------------------------------------------------
;
;-----------------------------------------------------------------------------
		MACRO
$label	Blend4Pixels $BlendPixel

		; Blend first 4 pixels

		ldmia	r1!, {r4-r7}
		ldm		r0, {r9-r12}

b4p1_$label	$BlendPixel r9, r4, r3
b4p2_$label	$BlendPixel r10, r5, r3
b4p3_$label	$BlendPixel r11, r6, r3
b4p4_$label	$BlendPixel r12, r7, r3

		stmia 	r0!, {r9-r12}

		MEND

;-----------------------------------------------------------------------------
;
;-----------------------------------------------------------------------------
		MACRO
$label	Blend8Pixels $BlendPixel

b8p1_$label Blend4Pixels $BlendPixel
b8p2_$label Blend4Pixels $BlendPixel

		MEND

;-----------------------------------------------------------------------------
;
;-----------------------------------------------------------------------------
		MACRO
$label	Blend16Pixels $BlendPixel

b16p1_$label Blend8Pixels $BlendPixel
b16p2_$label Blend8Pixels $BlendPixel

		MEND

;-----------------------------------------------------------------------------
;
;-----------------------------------------------------------------------------
		MACRO
$label	Blend32Pixels $BlendPixel

b32p1_$label Blend16Pixels $BlendPixel
b32p2_$label Blend16Pixels $BlendPixel

		MEND

;-----------------------------------------------------------------------------
; A macro for source over compositing one row of pixels and saving the results
; to destination buffer.
;
; @param dest			Destination	buffer						(r0)
; @param src			Source buffer							(r1)
; @param length			Length									(r2)
; @param const_alpha	Constant alpha					  		(r3)
; @param r14 			Component Half (0x800080)		  		(r14)
;
;  @note Advances r0, r1
;  @note Trashes r2, r4-r12
;-----------------------------------------------------------------------------
		MACRO
$label	BlendRow $BlendPixel

		pld		[r1]

bloop_$label
		; Blend 32 pixels per loop iteration
		subs	r2, r2, #32
		bmi		b_remaining_$label

brp1_$label	Blend32Pixels $BlendPixel

		b		bloop_$label

b_remaining_$label

		; Remaining 31 pixels

		addmi	r2, r2, #32

		; Blend 16 pixels
		tst		r2, #16
		beq		b_remaining8_$label

brp2_$label Blend16Pixels $BlendPixel

b_remaining8_$label

		; Blend 8 pixels
		tst		r2, #8
		beq		b_remaining4_$label

brp3_$label Blend8Pixels $BlendPixel

b_remaining4_$label

		; Blend 4 pixels
		tst		r2, #4
		beq		b_remaining3_$label

brp4_$label Blend4Pixels $BlendPixel

b_remaining3_$label

		; Remaining 3 pixels

		tst		r2, #2
		beq		b_last_$label

		ldmia 	r1!, {r4-r5}
		ldm	  	r0,  {r9-r10}

brp5_$label	$BlendPixel r9, r4, r3
brp6_$label	$BlendPixel r10, r5, r3

		stmia 	r0!, {r9-r10}

b_last_$label

		tst		r2, #1
		beq		bexit_$label

		ldr		r4, [r1]
		ldr		r9, [r0]

bpl_$label	$BlendPixel r9, r4, r3

		str 	r9, [r0]

bexit_$label

		MEND

;-----------------------------------------------------------------------------
; A macro for source over compositing one row of pixels and saving the results
; to destination buffer. Restores all registers.
;
; @param dest			Destination	buffer						(r0)
; @param src			Source buffer							(r1)
; @param length			Length									(r2)
; @param const_alpha	Constant alpha					  		(r3)
; @param r14 			Component Half (0x800080)		  		(r14)
;
;  @note Advances r0, r1
;  @note Trashes r2, r4-r12
;-----------------------------------------------------------------------------
		MACRO
$label	BlendRowSafe $BlendPixel

		stmfd	sp!, {r0-r6} ; Preserves registers only up to r6

brs_$label  BlendRow $BlendPixel

		ldmfd	sp!, {r0-r6}

		MEND


;-----------------------------------------------------------------------------
; Pix Copy.
; NOTE! Cache line size of ARM1136JF-S and ARM1136J-S is 32 bytes (8 pixels).
;
; @param dst			Destination	pixels						(r0)
; @param src			Source pixels							(r1)
; @param len			Length									(r2)
;
;  @note Trashes r3-r10
;-----------------------------------------------------------------------------
		MACRO
$label	PixCpy $dst, $src, $len

		pld		[$src]

pcpy_loop_$label
		; Copy 8 pixels per loop iteration
		pld		[$src, #96]
		subs	$len, $len, #8
		ldmgeia	$src!, {r3-r10}
		stmgeia	$dst!, {r3-r10}
		bgt		pcpy_loop_$label

pcpy_remaining_$label

		; Copy up to 7 remaining pixels

		; Copy 4 pixels
		tst		$len, #4
		ldmneia	$src!, {r3-r6}
		stmneia	$dst!, {r3-r6}

		tst		$len, #2
		ldmneia	$src!, {r3-r4}
		stmneia	$dst!, {r3-r4}

		tst		$len, #1
		ldrne	r3, [$src]
		strne 	r3, [$dst]

		MEND

;-----------------------------------------------------------------------------
; General Pix Copy. Maximum 8 pixels at time. Restores all registers.
;
; @param dst			Destination	pixels						(r0)
; @param src			Source pixels							(r1)
; @param len			Length									(r2)
;
;  @note Trashes r3-r10
;-----------------------------------------------------------------------------
		MACRO
$label	PixCpySafe $dst, $src, $len

		stmfd	sp!, {r0-r6} ; Preserves registers only up to r6

pcs_$label  PixCpy $dst, $src, $len

		ldmfd	sp!, {r0-r6}		; pop

		MEND


;-----------------------------------------------------------------------------
; A macro for source over compositing one pixel and saving the result to
; dst register.
;
;  @param dst      Destination register, must contain destination pixel upon entry
;  @param src 	   Source register, must contain source pixel upon entry
;  @param const_alpha Constant source alpha
;  @param r14	   Component half 0x800080
;
;  @note Trashes const_alpha, r8
;-----------------------------------------------------------------------------
		MACRO
$label	PixelSourceOver $dst, $src, $const_alpha

		; Negate src and extract alpha
		mvn		$const_alpha, $src ; bitwise not
		uxtb	$const_alpha, $const_alpha, ror #24	; r3 = ((r3 & 0xFF000000) >> 24);

		;cmp		$const_alpha, #255	; test for full transparency ( negated )
		;beq		exit_$label
		cmp     $const_alpha, #0	            ; test for full opacity ( negated )
		moveq	$dst, $src
		beq		exit_$label

		ByteMul	$dst, $dst, $const_alpha
		add		$dst, $src, $dst

exit_$label
		MEND

;-----------------------------------------------------------------------------
; A macro for source over compositing one pixel and saving the result to
; dst register.
;
;  @param dst      Destination register, must contain destination pixel upon entry
;  @param src 	   Source register, must contain source pixel upon entry
;  @param const_alpha Constant source alpha
;  @param r14	   Component half 0x800080
;
;  @note Trashes src, const_alpha, r8
;-----------------------------------------------------------------------------
		MACRO
$label	PixelSourceOverConstAlpha $dst, $src, $const_alpha

		; store alpha because we are going to trash it
		stmfd	sp!, {$const_alpha}

		ByteMul	$src, $src, $const_alpha

		; Negate src and extract alpha
		mvn		$const_alpha, $src ; bitwise not
		uxtb	$const_alpha, $const_alpha, ror #24	; r3 = ((r3 & 0xFF000000) >> 24);

		ByteMul	$dst, $dst, $const_alpha

		add		$dst, $src, $dst

		; recover alpha
		ldmfd	sp!, {$const_alpha}

		MEND

;-----------------------------------------------------------------------------
; A macro for source over compositing one pixel and saving the result to
; a register.
;
;  @param dst      Destination register, must contain destination pixel upon entry
;  @param src 	   Source register, must contain source pixel upon entry
;  @param const_alpha Constant source alpha
;  @param r14	   Component half 0x800080
;
;  @note Trashes src, r8
;-----------------------------------------------------------------------------
		MACRO
$label	PixelSourceConstAlpha $dst, $src, $const_alpha

		; store r2 and r14 because we are going to trash them
		stmfd	sp!, {r2, r14}

		rsb		r2, $const_alpha, #255
		InterpolatePixel255 $dst, $src, $const_alpha, $dst, r2

		; recover r2 and r14
		ldmfd	sp!, {r2, r14}

		MEND

		END ; File end
