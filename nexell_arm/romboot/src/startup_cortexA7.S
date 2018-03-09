/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: bl0
 * File		: startup_cortexA7.S
 * Description	:
 * Author	: Hans
 * History	: 2017.10.09	Hans first draft
 */

//;==================================================================
//; cortex A7
//;==================================================================
#include "armv7.h"
#include "nx_antares.h"
#include "nx_antares_bootoption.h"


	.align 4

//;==================================================================
//; Vectors
//;==================================================================
.global Vectors
Vectors:
	LDR	PC, ResetV	//; 00 - Reset
	LDR	PC, UndefV	//; 04 - Undefined instructions
	LDR	PC, SWIV	//; 08 - SWI instructions
	LDR	PC, PAbortV	//; 0C - Instruction fetch aborts
	LDR	PC, DAbortV	//; 10 - Data access aborts
	LDR	PC, UnusedV	//; 14 - Reserved (was address exception)
	LDR	PC, IRQV	//; 18 - IRQ interrupts
	LDR	PC, FIQV	//; 1C - FIQ interrupts

ResetV:
	.word	Reset_Handler
UndefV:
	.word	(BASEADDR_SRAM + 0x04)	//; 04 - undef
SWIV:
	.word	(BASEADDR_SRAM + 0x08)	//; 08 - software interrupt
PAbortV:
	.word	(BASEADDR_SRAM + 0x0C)	//; 0C - prefectch abort
DAbortV:
	.word	(BASEADDR_SRAM + 0x10)	//; 10 - data abort
UnusedV:
	.word	0			//; 14 - will reset if called...
IRQV:
	.word	(BASEADDR_SRAM + 0x18)	//; 18 - IRQ
FIQV:
	.word	(BASEADDR_SRAM + 0x1C)	//; 1C - FIQ

//;=============================================================================
//; Imports
//;=============================================================================

.global iROMBOOT

//;=============================================================================
//; Reset Handler - Generic initialization, run by all CPUs
//;=============================================================================

Reset_Handler:
	//; if MMU/MPU enabled - disable (useful for ARMulator tests)
//;=============================================================================
//; Disable caches, MMU and branch prediction in case they were left enabled
//; from an earlier run
//; This does not need to be done from a cold reset
//;=============================================================================
	MRC	P15, 0, R0, C1, C0, 0		//; Read CP15 System Control register
	BIC	R0, R0, #(BIT1_V|BIT1_I|BIT1_Z)	//; Clear V:13, I:12, Z:11 bit to disable High Vector, I-Cache, Branch Prediction
	BIC	R0, R0, #(BIT1_C|BIT1_M)	//; Clear C:2, M:0 bit to disable D Cache, MMU
	MCR	P15, 0, R0, C1, C0, 0		//; Write value back to CP15 System Control register

#if 0	// no need in romboot. this will be bl1.
	MRC	P15, 0, R0, C1, C0, 1		//; read CP15 ACTLR
	ORR	R0, R0, #(1 << 6)		//; set ACTLR.SMP bit
	MCR	P15, 0, R0, C1, C0, 1		//; write CP15 ACTLR
#endif

	MSR	CPSR_c, #(Mode_MON|I_Bit|F_Bit)	//; Set CPU to MON Mode


	//; in general enable all counters
	//; reset all counters to zero
	//; reset cycle counter to zero
	MOV	R0, #((1 << 2) | (1 << 1) | (1 << 0))
	MCR	P15, 0, R0, C9, C12, 0	//; PMCR
	MOV	R0, #0x8000000F		//; enable all counter
	MCR	P15, 0, R0, C9, C12, 1	//; PMCNTENSET
	MCR	P15, 0, R0, C9, C12, 3	//; PMOVSR, clear overflows
	MCR	P15, 0, R0, C9, C14, 2	//; PMINTENCLR, disable counter overflow int

	MRC	P15, 0, R0, C0, C0, 5		//; Get cpu id
	ANDS	R0, R0, #0x3			//; check master cpu
	BNE	SUBCPU

	MOV	R0, #0
	MCR	P15, 0, R0, C8, C7, 0		//; I-TLB and D-TLB invalidation
	MCR	P15, 0, R0, C7, C5, 6		//; BPIALL - Invalidate entire branch predictor array

	LDR	R0, =Vectors
	MCR	P15, 0, R0, C12, C0, 0		//; Set Secure MON Vector

//;=============================================================================
//; Get Boot Config
//;=============================================================================
#ifdef NXP4330
	MOVW	R0, #0
	MOVT	R0, #0xC001		//; syscon address
	LDR	R12, [R0, #0x27C]	//; Get Boot Config
#endif
#ifdef NXP3220
	MOVW	R0, #0
	MOVT	R0, #0x2207		//; cpu pll
	MOVW	R2, #0
	MOVT	R2, #0x2702		//; pll0
	ORR	R4, R2, #0x10000	//; pll1
	MOVW	R6, #0
	MOVT	R6, #0x2307		//; pll_ddr0
	ADD	R8, R6, #0x10000	//; pll_ddr1

	LDR	R1, [R0, #0]		//; cpu pll ctrl
	LDR	R3, [R2, #0]
	LDR	R5, [R4, #0]
	LDR	R7, [R6, #0]
	LDR	R9, [R8, #0]

	ORR	R1, R1, #(1 << 3)	//; bypass off
	ORR	R3, R3, #(1 << 3)
	ORR	R5, R5, #(1 << 3)
	ORR	R7, R7, #(1 << 3)
	ORR	R9, R9, #(1 << 3)

	STR	R1, [R0, #0]
	STR	R3, [R2, #0]
	STR	R5, [R4, #0]
	STR	R7, [R6, #0]
	STR	R9, [R8, #0]

	MOVW	R0, #0xC800
	MOVT	R0, #0x2008		//; syscon address
	LDR	R12, [R0, #0x6C]	//; Get Boot Config
#endif


CacheInit:
//;=============================================================================
//; Set Vector Base Address Register (VBAR) to point to this application's vector table
//;=============================================================================
	LDR	R0, =Vectors
	MCR	P15, 0, R0, C12, C0, 0	//; Write Secure or Non-secure Vector Base Address
	MRC	P15, 0, R0, C1, C0, 0	//; Read SCTLR
	BIC	R0, #(1 << 13)	        //; Clear V-bit
	MCR	P15, 0, R0, C1, C0, 0	//; Write SCTLR

//;=============================================================================
//; Invalidate entire branch predictor
//;=============================================================================
	MOV	R0, #0
	MCR     P15, 0, R0, C7, C5, 0	//; invalidate all instruction caches
	MCR	P15, 0, R0, C7, C5, 6	//; BPIALL - Invalidate entire branch predictor array

//;=============================================================================
//; Enable caches and branch prediction
//;=============================================================================
	MOV	R0, R12
	TST	R0, #(0x1 << ICACHE )	//; if selsdex==0 check sd-icache bit

	MRCNE	P15, 0, R1, C1, C0, 0	//; Read System Control Register
	ORRNE	R1, R1, #(BIT1_I|BIT1_Z)//; Set I bit 12 to enable I Cache 11: branch
	MCRNE	P15, 0, R1, C1, C0, 0	//; Write System Control Register


//;=============================================================================
//; Setup stacks
//;=============================================================================
	MOVW	SP, #0xFD00		//; 3 * 256 bytes
	MOVT	SP, #0xFFFF

	BL	iROMBOOT		//; save this in register for possible long jump

        BX      R0			//; jump to R0
	MOV	R12, #0x01		//; usb boot
	MOV	PC, #0			//; Never Get Here

SUBCPU:
	WFI
	B	SUBCPU
//;=============================================================================
//; End of startup.s
//;=============================================================================
