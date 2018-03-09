/*
 * Copyright (C) 2016  Nexell Co., Ltd. All Rights Reserved.
 * Nexell Co. Proprietary & Confidential
 *
 * Nexell informs that this code and information is provided "as is" base
 * and without warranty of any kind, either expressed or implied, including
 * but not limited to the implied warranties of merchantability and/or
 * fitness for a particular puporse.
 * This code is not allowed to redistribute without permission or
 * open to the public.
 * 
 * Module	:
 * File		:
 * Description	:
 * Author	: Hans
 * History	: 2017.02.28 new release
 */

/*
 * A, Q = r0 + (r1 << 32)
 * B, R = r2 + (r3 << 32)
 * A / B = Q ... R
 */
.text
.global	__aeabi_uldivmod
.type	__aeabi_uldivmod, function

.align	0

A_0	.req	r0
A_1	.req	r1
B_0	.req	r2
B_1	.req	r3
C_0	.req	r4
C_1	.req	r5
D_0	.req	r6
D_1	.req	r7
Q_0	.req	r0
Q_1	.req	r1
R_0	.req	r2
R_1	.req	r3

/*
 * __aeabi_idivmod.S
 */

.globl	__aeabi_uidivmod
.align	4

/*;===========================================================================*/
__aeabi_uidivmod:
/**/	stmfd	sp!, {r0, r1, ip, lr}
/**/	bl	__aeabi_uidiv
/**/	ldmfd	sp!, {r1, r2, ip, lr}
/**/	mul	r3, r0, r2
/**/	sub	r1, r1, r3
/**/	mov	pc, lr
/*;===========================================================================*/
__aeabi_uldivmod:
/**/	stmfd	sp!, {r4, r5, r6, r7, lr}
/*	Test if B == 0*/
/**/	orrs	ip, B_0, B_1	/*	Z set -> B == 0*/
/**/	beq	L_div_by_0
/*	Test if B is power of 2: (B & (B - 1)) == 0*/
/**/	subs	C_0, B_0, #1
/**/	sbc	C_1, B_1, #0
/**/	tst	C_0, B_0
/**/	tsteq	B_1, C_1
/**/	beq	L_pow2
/*	Test if A_1 == B_1 == 0*/
/**/	orrs	ip, A_1, B_1
/**/	beq	L_div_32_32
L_div_64_64:
/**/	mov	C_0, #1
/**/	mov	C_1, #0
/*	D_0 = clz A*/
/**/	teq	A_1, #0
/**/	clz	D_0, A_1
/**/	clzeq	ip, A_0
/**/	addeq	D_0, D_0, ip
/*	D_1 = clz B*/
/**/	teq	B_1, #0
/**/	clz	D_1, B_1
/**/	clzeq	ip, B_0
/**/	addeq	D_1, D_1, ip
/*	if clz B - clz A > 0*/
/**/	subs	D_0, D_1, D_0
/**/	bls	L_done_shift
/*	B <<= (clz B - clz A)*/
/**/	subs	D_1, D_0, #32
/**/	rsb	ip, D_0, #32
/**/	movmi	B_1, B_1, lsl D_0
/**/	orrmi	B_1, B_1, B_0, lsr ip
/**/	movpl	B_1, B_0, lsl D_1
/**/	mov	B_0, B_0, lsl D_0
/*	C = 1 << (clz B - clz A)*/
/**/	movmi	C_1, C_1, lsl D_0
/**/	orrmi	C_1, C_1, C_0, lsr ip
/**/	movpl	C_1, C_0, lsl D_1
/**/	mov	C_0, C_0, lsl D_0
L_done_shift:
/**/	mov	D_0, #0
/**/	mov	D_1, #0
/*	C: current bit; D: result*/
L_subtract:
/*	if A >= B*/
/**/	cmp	A_1, B_1
/**/	cmpeq	A_0, B_0
/**/	bcc	L_update
/*	A -= B*/
/**/	subs	A_0, A_0, B_0
/**/	sbc	A_1, A_1, B_1
/*	D |= C*/
/**/	orr	D_0, D_0, C_0
/**/	orr	D_1, D_1, C_1
L_update:
/*	if A == 0: break*/
/**/	orrs	ip, A_1, A_0
/**/	beq	L_exit
/*	C >>= 1*/
/**/	movs	C_1, C_1, lsr #1
/**/	movs	C_0, C_0, rrx
/*	if C == 0: break*/
/**/	orrs	ip, C_1, C_0
/**/	beq	L_exit
/*	B >>= 1*/
/**/	movs	B_1, B_1, lsr #1
/**/	mov	B_0, B_0, rrx
/**/	b	L_subtract
L_exit:
/*	Note: A, B & Q, R are aliases*/
/**/	mov	R_0, A_0
/**/	mov	R_1, A_1
/**/	mov	Q_0, D_0
/**/	mov	Q_1, D_1
/**/	ldmfd	sp!, {r4, r5, r6, r7, pc}
L_div_32_32:
/*	Note:	A_0 &	r0 are aliases*/
/*		Q_1	r1*/
/**/	mov	r1, B_0
/**/	bl	__aeabi_uidivmod
/**/	mov	R_0, r1
/**/	mov	R_1, #0
/**/	mov	Q_1, #0
/**/	ldmfd	sp!, {r4, r5, r6, r7, pc}
L_pow2:
/*	Note: A, B and Q, R are aliases*/
/*	R = A & (B - 1)*/
/**/	and	C_0, A_0, C_0
/**/	and	C_1, A_1, C_1
/*	Q = A >> log2(B)*/
/*	Note: B must not be 0 here!*/
/**/	clz	D_0, B_0
/**/	add	D_1, D_0, #1
/**/	rsbs	D_0, D_0, #31
/**/	bpl	L_1
/**/	clz	D_0, B_1
/**/	rsb	D_0, D_0, #31
/**/	mov	A_0, A_1, lsr D_0
/**/	add	D_0, D_0, #32
L_1:
/**/	movpl	A_0, A_0, lsr D_0
/**/	orrpl	A_0, A_0, A_1, lsl D_1
/**/	mov	A_1, A_1, lsr D_0
/*	Mov back C to R*/
/**/	mov	R_0, C_0
/**/	mov	R_1, C_1
/**/	ldmfd	sp!, {r4, r5, r6, r7, pc}
L_div_by_0:
/**/	bl	__div0
/*	As wrong as it could be*/
/**/	mov	Q_0, #0
/**/	mov	Q_1, #0
/**/	mov	R_0, #0
/**/	mov	R_1, #0
/**/	ldmfd	sp!, {r4, r5, r6, r7, pc}

