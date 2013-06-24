;;;
;;;  softurr32.asm -- this file is part of SoftURR
;;;  Copyright (C) 2006 Takayuki Usui
;;;
;;;  This program is free software; you can redistribute it and/or modify
;;;  it under the terms of the GNU General Public License as published by
;;;  the Free Software Foundation; either version 2 of the License, or
;;;  (at your option) any later version.
;;;
;;;  This program is distributed in the hope that it will be useful,
;;;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;  GNU General Public License for more details.
;;;
;;;  You should have received a copy of the GNU General Public License
;;;  along with this program; if not, write to the Free Software
;;;  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
;;;
;;; Reference:
;;;  "Write Great Code: Volume 1: Understanding the Machine"
;;;    Randall Hyde, No Starch Press, 2004
;;;
	%define arg(n) [ebp+((n+2)*4)]
	%define tmp(n,x) [ebp-x-(4*(n+1))]

	global urr32_decode:function
	global urr32_encode:function
	global urr32_add:function
	global urr32_sub:function
	global urr32_mul:function
	global urr32_div:function
	
	section .text
	align 4
urr32_decode:
	push	ebp
	mov	ebp,esp
	push	ebx
	push	esi
	push	edi
	xor	ebx,ebx		; exponent
	xor	edx,edx		; mantissa
	xor	esi,esi		; signs (high 2bit) and degree (low 8bit)
	xor	edi,edi		; maintain mantissa mask here
	not	edi
	mov	eax,arg(3)	; argument: input
	test	eax,eax		; zero
	jz	.90
	cmp	eax,0x80000000	; (proposal) nan
	je	.err
.10:
	shl	edi,1		; you must sync eax and its current mask
	shl	eax,1		; examine sign bit of the number
	jnc	.20
	or	esi,0x80000000	; maintain sign of the number here
	neg	eax
.20:
	shl	edi,1
	shl	eax,1		; examine sign of exponent
	jc	.30
	or	esi,0x40000000	; maintain sign of exponent here
	not	eax
.30:
	xor	ecx,ecx		; count the rightmost continuous set bit(s)
.31:				; this value is 'degree' of the exponent
	shl	edi,1
	shl	eax,1
	jnc	.40
	inc	ecx
	jmp	.31
.40:
	or	esi,ecx		; save degree of exponent
	mov	ebx,1		; calc major part of exponent
	shl	ebx,cl
	shr	ebx,1
	cmp	ecx,2
	jb	.70
.50:
	dec	ecx
.51:
	test	edi,edi
	jnz	.52
	mov	edi,esi		; generate bits over register width
	and	edi,0x40000000	; if 0 <= exp then zero else one
	shl	edi,2
	rcl	edx,1
	loop	.51
	jmp	.70
.52:
	shl	edi,1		; calc minor part of exponent
	shl	eax,1		; copy the rightmost bit(s) of eax to edx
	rcl	edx,1
	loop	.51
.70:
	or	ebx,edx		; build exponent
	test	esi,0x40000000	; deal with sign of exponent
	jz	.80
	inc	ebx
	neg	ebx
	not	eax
.80:
	mov	edx,edi
	and	edx,eax		; build mantissa
	shr	edx,1		; prepend hidden bit
	or	edx,0x80000000
.90:
	mov	eax,ebx
	mov	ebx,arg(1)
	mov	[ebx],eax	; return: exponent
	mov	ebx,arg(2)
	mov	[ebx],edx	; return: mantissa
	mov	ebx,arg(0)
	mov	[ebx],esi	; return: signs and degree
	xor	eax,eax
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret
.err:
	xor	eax,eax
	not	eax
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret

		align 4
urr32_encode:
	push	ebp
	mov	ebp,esp
	push	ebx
	push	esi
	push	edi
	xor	ebx,ebx
	xor	esi,esi
	mov	eax,arg(1)	; argument: sign of the number
	test	eax,0x80000000
	jz	.10
	or	esi,0x80000000	; save sign of the number here
.10:
	mov	eax,arg(2)	; argument: exponent
	mov	edx,arg(3)	; argument: mantissa
	test	edx,edx		; zero
	jz	.99
.11:
	test	edx,0x80000000	; normalized mantissa
	jnz	.12
	dec	eax
	shl	edx,1
	jmp	.11
.12:
	push	edx
	push	eax
	call	round_mantissa
	mov	ebx,eax
	pop	eax
	pop	edx
	test	ebx,ebx
	jnz	.err
	lea	ebx,arg(2)	; update exponent and mantissa
	mov	[ebx],eax
	lea	ebx,arg(3)
	mov	[ebx],edx
.13:
	push	edx
	push	eax
	call	round_exponent
	mov	ebx,eax
	pop	eax
	pop	edx
	test	ebx,ebx
	jnz	.err
	lea	ebx,arg(2)	; update exponent
	mov	[ebx],eax
.20:	
	test	eax,0x80000000	; deal with sign of exponent
	jz	.21
	or	esi,0x40000000	; save sign of exponent here
	neg	eax
	dec	eax
.21:
	bsr	ecx,eax		; search most significant set bit
	jz	.23		; exponent is 0
	inc	ecx
	cmp	ecx,30		; validate exponent value
	ja	.err
	test	esi,0x40000000
	jz	.22
	cmp	ecx,29
	ja	.err
.22:
	or	esi,ecx		; save degree of exponent here
.23:
	xor	ebx,ebx		; result code is stored in ebx
	xor	edi,edi		; maintain current mask for ebx
	not	edi
.30:
	shl	edi,2		; start with prefix 01 bits
	or	ebx,1
	mov	ecx,esi		; extract degree of exponent
	and	ecx,0xFF
	cmp	ecx,0
	je	.41		; exponent is 0
	cmp	ecx,1
	je	.40		; exponent is 1
	mov	edx,ecx		; exponent is 2 and above
.31:
	shl	edi,1		; major exponent
	shl	ebx,1
	or	ebx,1
	loop	.31
	test	edi,edi
	jz	.50
	mov	ecx,edx		; minor exponent
	dec	ecx
	shl	edi,1
	shl	ebx,1
	mov	edx,1
	shl	edx,cl
.32:
	test	edi,edi
	jz	.50
	shr	edx,1
	shl	edi,1		; copy lower part of exponent
	shl	ebx,1
	test	eax,edx
	jz	.33
	or	ebx,1
.33:
	loop	.32
	jmp	.50
.40:
	shl	edi,2		; when exponent is 1 (0110...)
	shl	ebx,2
	or	ebx,2
	jmp	.50
.41:
	shl	edi,1		; when exponent is 0 (010...)
	shl	ebx,1
	jmp	.50
.50:
	test	esi,0x40000000
	jz	.60
	not	ebx		; deal with sign of exponent
	mov	edx,edi
	shr	edx,1		; reverse exponent part
	or	edx,0x80000000	; but, keep sing of the number intact
	xor	ebx,edx
.60:
	mov	eax,arg(3)	; fetch normalized mantissa
	test	eax,0x80000000
	jz	.err
	shl	eax,1		; strip hidden bit
.61:
	test	edi,edi		; append mantissa to ebx
	jz	.70
	shl	edi,1
	shl	eax,1
	rcl	ebx,1
	jmp	.61
.70:
	test	esi,0x80000000	; deal with sign of the number
	jz	.80
	neg	ebx
.80:
	mov	eax,ebx
	mov	ebx,arg(0)
	mov	[ebx],eax
	xor	eax,eax
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret
.99:
	mov	ebx,arg(0)
	mov	[ebx],dword 0
	xor	eax,eax
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret
.err:
	xor	eax,eax
	not	eax
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret

	align 4	
round_exponent:
	push	ebp
	mov	ebp,esp
	push	ebx
	push	esi
.10:
	xor	esi,esi
	mov	eax,arg(0)	; argument: exponent
	test	eax,0x80000000
	jz	.20
	or	esi,0x40000000	; save sign of exponent here
	neg	eax
	dec	eax
.20:
	bsr	ecx,eax		; search most significant set bit
	jz	.91		; exponent is 0
	inc	ecx
	cmp	ecx,16		; validate exponent value
	jb	.91
.21:
	cmp	ecx,30
	jb	.22
	ja	.err
	mov	ecx,29
	jmp	.23
.22:	
	shl	ecx,1
	sub	ecx,30		; (degree*2-30) bits will be lost
.23:
	mov	edx,ecx
	mov	ebx,eax
	shr	eax,cl
	sub	ecx,32
	neg	ecx
	shl	ebx,cl
.30:
	and	esi,0x40000000
	jz	.40
	test	ebx,0x80000000	; special treatment for negative
	jnz	.32
	cmp	ebx,0x7FFFFFFF
	jne	.31
	test	eax,1
	jz	.32
.31:
	dec	eax		; round down
.32:
	mov	ecx,edx
	shl	eax,cl
	xor	edx,edx
	not	edx
	shl	edx,cl
	not	edx
	or	eax,edx
	inc	eax
	neg	eax
	jmp	.90
.40:
	test	ebx,0x80000000  ; break tie on round-to-even
	jz	.42
	test	ebx,0x7FFFFFFF
	jnz	.41
	test	eax,1
	jz	.42
.41:
	inc	eax		; round up
.42:
	mov	ecx,edx
	shl	eax,cl
.90:
	lea	ebx,arg(0)
	mov	[ebx],eax
.91:
	xor	eax,eax
	pop	esi
	pop	ebx
	leave
	ret
.err:
	xor	eax,eax
	not	eax
	pop	esi
	pop	ebx
	leave
	ret
	
	align 4
round_mantissa:
	push	ebp
	mov	ebp,esp
	push	ebx
.10:
	mov	eax,arg(0)	; argument: exponent
	test	eax,0x80000000
	jz	.20
	neg	eax
	dec	eax
.20:
	bsr	ecx,eax		; search most significant set bit
	jz	.30		; exponent is 0
	inc	ecx
	cmp	ecx,15		; validate exponent value
	ja	.22
.21:
	shl	ecx,1		; exponent 1 and above
	inc	ecx		; (degree*2+1) bits will be lost
	jmp	.31
.22:
	mov	ecx,31
	jmp	.31
.30:
	mov	ecx,2
.31:
	mov	edx,ecx
	mov	eax,arg(1)	; argument: normalized mantissa
	mov	ebx,eax
	shr	eax,cl
	sub	ecx,32
	neg	ecx
	shl	ebx,cl		; this part is shift out
.40:
	test	ebx,0x80000000  ; break tie on round-to-even
	jz	.50
	test	ebx,0x7FFFFFFF
	jnz	.41
	test	eax,1
	jz	.50
.41:
	inc	eax		; round up
	mov	ecx,edx
	shl	eax,cl
	jnc	.42
	rcr	eax,1		; mantissa overflow
	lea	ebx,arg(1)	; in this case, mantissa should be 0x80000000
	mov	[ebx],eax
	mov	eax,arg(0)	; increment exponent
	inc	eax
	lea	ebx,arg(0)
	mov	[ebx],eax
	jmp	.50
.42:
	lea	ebx,arg(1)
	mov	[ebx],eax
.50:
	xor	eax,eax
	pop	ebx
	leave
	ret
.err:
	xor	eax,eax
	not	eax
	pop	ebx
	leave
	ret

	align 4
urr32_add:
	push	ebp
	mov	ebp,esp
	push	ebx
	sub	esp,24
.10:
	push	dword arg(1)	; argument: left operand
	lea	eax,tmp(2,4)	; local: left mantissa
	push	eax
	lea	eax,tmp(1,4)	; local: left exponent
	push	eax
	lea	eax,tmp(0,4)	; local: left sign
	push	eax
	call	urr32_decode	; extract left operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.11:
	push	dword arg(2)	; argument: right operand
	lea	eax,tmp(5,4)	; local: right mantissa
	push	eax
	lea	eax,tmp(4,4)	; local: right exponent
	push	eax
	lea	eax,tmp(3,4)	; local: right sign
	push	eax
	call	urr32_decode	; extract right operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.20:
	mov	eax,tmp(1,4)	; compare exponents
	mov	edx,tmp(4,4)
	sub	eax,edx		; normalize exponent on stack
	jg	.21		; right > left
	jl	.22		; left < right
	jmp	.30
.21:
	add	tmp(4,4),eax	; raise right exponent on stack
	push	eax
	push	dword tmp(5,4)
	call	shift		; shift right mantissa
	mov	ebx,eax
	pop	eax
	add	esp,4
	test	ebx,ebx
	jnz	.err
	mov	tmp(5,4),eax	; update right mantissa on stack
	jmp	.30
.22:
	neg	eax		; raise left exponent
	add	tmp(1,4),eax
	push	eax
	push	dword tmp(2,4)
	call	shift
	mov	ebx,eax
	pop	eax
	add	esp,4
	test	ebx,ebx
	jnz	.err
	mov	tmp(2,4),eax
.30:
	mov	eax,tmp(0,4)	; compare signs
	mov	edx,tmp(3,4)
	xor	eax,edx
	and	eax,0x80000000
	jz	.50
.40:
	mov	eax,tmp(2,4)	; compare mantissa
	mov	edx,tmp(5,4)
	cmp	eax,edx
	jb	.41
	sub	eax,edx
	push	eax
	push	dword tmp(1,4)
	push	dword tmp(0,4)
	push	dword arg(0)
	jmp	.60
.41:
	sub	edx,eax
	push	edx
	push	dword tmp(4,4)
	push	dword tmp(3,4)
	push	dword arg(0)
	jmp	.60
.50:
	mov	eax,tmp(2,4)
	mov	edx,tmp(5,4)
	add	eax,edx
	jnc	.51
	rcr	eax,1		; mantissa overflow
	push	eax
	mov	eax,tmp(1,4)
	inc	eax		; increment exponent
	push	eax
	push	dword tmp(0,4)
	push	dword arg(0)
	jmp	.60
.51:
	push	eax
	push	dword tmp(1,4)
	push	dword tmp(0,4)
	push	dword arg(0)
.60:
	call	urr32_encode
	add	esp,16
	test	eax,eax
	jnz	.err
	add	esp,24
	xor	eax,eax
	pop	ebx
	leave
	ret
.err:
	add	esp,24
	xor	eax,eax
	not	eax
	pop	ebx
	leave
	ret

	align 4
urr32_sub:
	push	ebp
	mov	ebp,esp
	mov	eax,arg(2)
	neg	eax
	push	eax
	push	dword arg(1)
	push	dword arg(0)
	call	urr32_add
	add	esp,12
	leave
	ret

	align 4
shift:
	push	ebp
	mov	ebp,esp
	push	ebx
	mov	eax,arg(0)	; argument: mantissa
	mov	ebx,eax
	mov	ecx,arg(1)	; argument: shift
	cmp	ecx,0
	jz	.30
	cmp	ecx,32
	ja	.40
.10:
	shr	eax,cl
	sub	ecx,32
	neg	ecx
	shl	ebx,cl
	test	ebx,0x80000000	; round-to-even
	jz	.30
	test	ebx,0x7FFFFFFF
	jnz	.20
	test	eax,1
	jz	.30
.20:
	inc	eax		; round up
.30:
	lea	ebx,arg(0)
	mov	[ebx],eax
	xor	eax,eax
	pop	ebx
	leave
	ret
.40:
	xor	eax,eax		; wipe out
	lea	ebx,arg(0)
	mov	[ebx],eax
	pop	ebx
	leave
	ret

	align 4
urr32_mul:
	push	ebp
	mov	ebp,esp
	push	ebx
	sub	esp,24
.10:
	push	dword arg(1)	; argument: left operand
	lea	eax,tmp(2,4)	; local: left mantissa
	push	eax
	lea	eax,tmp(1,4)	; local: left exponent
	push	eax
	lea	eax,tmp(0,4)	; local: left sign
	push	eax
	call	urr32_decode	; extract left operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.11:
	push	dword arg(2)	; argument: right operand
	lea	eax,tmp(5,4)	; local: right mantissa
	push	eax
	lea	eax,tmp(4,4)	; local: right exponent
	push	eax
	lea	eax,tmp(3,4)	; local: right sign
	push	eax
	call	urr32_decode	; extract right operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.20:
	mov	eax,tmp(2,4)	; multiply mantissa
	test	eax,eax
	jz	.90
	mov	edx,tmp(5,4)
	test	edx,edx
	jz	.90
.30:
	mul	edx
	test	edx,0x80000000
	jnz	.31
	shl	eax,1
	rcl	edx,1
	jmp	.32
.31:
	inc	dword tmp(1,4)
.32:
	test	eax,eax		; condense eax into the low bit of edx
	setnz	al
	movzx	eax,al
	or	edx,eax
	mov	tmp(2,4),edx
.40:
	mov	eax,tmp(1,4)	; add exponents
	mov	edx,tmp(4,4)
	add	eax,edx
	jo	.err
	mov	tmp(1,4),eax
.50:
	mov	eax,tmp(0,4)	; compare signs
	mov	edx,tmp(3,4)
	xor	eax,edx
	and	eax,0x80000000
	sar	eax,31
	or	eax,1
	mov	tmp(0,4),eax
.60:
	push	dword tmp(2,4)
	push	dword tmp(1,4)
	push	dword tmp(0,4)
	push	dword arg(0)
	call	urr32_encode
	add	esp,16
	test	eax,eax
	jnz	.err
	add	esp,24
	xor	eax,eax
	pop	ebx
	leave
	ret
.90:
	mov	ebx,arg(0)
	mov	[ebx],dword 0
	add	esp,24
	xor	eax,eax
	pop	ebx
	leave
	ret
.err:
	add	esp,24
	xor	eax,eax
	not	eax
	pop	ebx
	leave
	ret

	align 4
urr32_div:
	push	ebp
	mov	ebp,esp
	push	ebx
	sub	esp,24
.10:
	push	dword arg(1)	; argument: left operand
	lea	eax,tmp(2,4)	; local: left mantissa
	push	eax
	lea	eax,tmp(1,4)	; local: left exponent
	push	eax
	lea	eax,tmp(0,4)	; local: left sign
	push	eax
	call	urr32_decode	; extract left operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.11:
	push	dword arg(2)	; argument: right operand
	lea	eax,tmp(5,4)	; local: right mantissa
	push	eax
	lea	eax,tmp(4,4)	; local: right exponent
	push	eax
	lea	eax,tmp(3,4)	; local: right sign
	push	eax
	call	urr32_decode	; extract right operand here
	add	esp,16
	test	eax,eax
	jnz	.err
.20:
	mov	eax,tmp(5,4)
	test	eax,eax		; detect zero division
	jz	.err
	mov	edx,tmp(2,4)
	test	edx,edx
	jz	.90
.30:
	mov	ebx,eax		; divide mantissa
	xor	eax,eax
	shr	edx,1
	rcr	eax,1
	div	ebx
	test	edx,edx
	setnz	dl		; condense edx into the low bit of eax
	movzx	edx,dl
	or	eax,edx
	mov	tmp(2,4),eax
.40:
	mov	eax,tmp(1,4)	; subtract exponents
	mov	edx,tmp(4,4)
	sub	eax,edx
	jo	.err
	mov	tmp(1,4),eax
.50:
	mov	eax,tmp(0,4)	; compare signs
	mov	edx,tmp(3,4)
	xor	eax,edx
	and	eax,0x80000000
	sar	eax,31
	or	eax,1
	mov	tmp(0,4),eax
.60:
	push	dword tmp(2,4)
	push	dword tmp(1,4)
	push	dword tmp(0,4)
	push	dword arg(0)
	call	urr32_encode
	add	esp,16
	test	eax,eax
	jnz	.err	
	add	esp,24
	xor	eax,eax
	pop	ebx
	leave
	ret
.90:
	mov	ebx,arg(0)
	mov	[ebx],dword 0
	add	esp,24
	xor	eax,eax
	pop	ebx
	leave
	ret
.err:
	add	esp,24
	xor	eax,eax
	not	eax
	pop	ebx
	leave
	ret
