/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module          : ARM
 * File            : lib_armv7.S
 * Description     :
 * Author          : Firmware Team
 * History         : 2017.10.09 Hans first draft
 */
#include "armv7.h"
#include "nx_antares.h"

.text
.align 4
.global SetICache
//; void SetICache(int enable);
	mrc	p15, 0, r1, c1, c0, 0
	cmp	r0, #0
	biceq	r1, r1, #BIT1_I
	orrne	r1, r1, #BIT1_I
	mcr	p15, 0, r1, c1, c0, 0
	bx	lr

.global GetBootOption
.type	GetBootOption, function
//; unsigned int GetBootOption(void);
GetBootOption:
	mrc	p15, 0, r0, c13, c0, 1	//; CONTEXTIDR
	bx	lr

.global SetBootOption
.type	SetBootOption, function
//; unsigned int SetBootOption(void);
SetBootOption:
	mcr	p15, 0, r0, c13, c0, 1
	bx	lr

.global	GetMempoolPtr
.type	GetMempoolPtr, function
//; unsigned int *GetMempoolPtr(void);
GetMempoolPtr:
	mrc	p15, 0, r0, c13, c0, 2	//; TPIDRURW tp id user rw
	bx	lr

.global	SetMempoolPtr
.type	SetMempoolPtr, function
//; void SetMempoolPtr(unsigned int mempoolptr);
SetMempoolPtr:
	mcr	p15, 0, r0, c13, c0, 2
	bx	lr

.global	GetStringPtr
.type	GetStringPtr, function
//; unsigned int *GetStringPtr(void);
GetStringPtr:
	mrc	p15, 0, r0, c13, c0, 3	//; TPIDRURO tp id user ro
	bx	lr

.global	SetStringPtr
.type	SetStringPtr, function
//; void SetStringPtr(unsigned int *StringPtr);
SetStringPtr:
	mcr	p15, 0, r0, c13, c0, 3
	bx	lr

.global Getbl0fnPtr
.type Getbl0fnPtr, function
//; struct nx_bl0_fn *Getbl0fnPtr(void);
Getbl0fnPtr:
	mrc	p15, 0, r0, c13, c0, 4	//; TPIDRPRW pl1 only thread id rw reg
	bx	lr

.global Setbl0fnPtr
.type Setbl0fnPtr, function
//; void Setbl0fnPtr(struct nx_bl0_fn *fnptr);
Setbl0fnPtr:
	mcr	p15, 0, r0, c13, c0, 4
	bx	lr

.global GetPMCnt
.type GetPMCnt, function
//; unsigned int GetPMCnt(void);
GetPMCnt:
	mrc	p15, 0, r0, c9, c12, 0	//; pmu cnt reg
	bx	lr

.global asmudelay
.type asmudelay, function
//; void asmudelay(unsigned int utime);
asmudelay:
	mrc     p15, 0, r1, c9, c12, 0
	add	r0, r0, r1
delaywait:
	mrc     p15, 0, r1, c9, c12, 0
	cmp	r0, r1
	bne	delaywait
	bx	lr

