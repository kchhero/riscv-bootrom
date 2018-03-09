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
 * File		: startup_cortexA9.S
 * Description	:
 * Author	: Hans
 * History	: 2017.10.09	Hans first draft
 */

//;==================================================================
//; cortex A9 V7
//;==================================================================
#include "armv7.h"
#include "nx_antares.h"
#include "nx_antares_bootoption.h"

		.align 9	// 2^9 = 512

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
	.word	smc_handler	//; 08 - software interrupt
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
	LDR	R0, =0xCF000000		//; L2C BASE
	MOV	R1, #0
	STR	R1, [R0, #0x100]	//; disable L2C
	//; if MMU/MPU enabled - disable (useful for ARMulator tests)
//;=============================================================================
//; Disable caches, MMU and branch prediction in case they were left enabled
//; from an earlier run
//; This does not need to be done from a cold reset
//;=============================================================================
	MRC	P15, 0, R0, C1, C0, 0
	//; Clear V:13, I:12, Z:11 bit to disable High Vector, I-Cache, Branch Prediction
	BIC	R0, R0, #(BIT1_V|BIT1_I|BIT1_Z)
	//; Clear C:2, M:0 bit to disable D Cache, MMU
	BIC	R0, R0, #(BIT1_C|BIT1_A|BIT1_M)
	MCR	P15, 0, R0, C1, C0, 0

	MSR	CPSR_c, #(Mode_MON|I_Bit|F_Bit)	//; Set CPU to MON Mode

//;=============================================================================
//; Get CPU and Core ID	CPU0 & Core0 ID is 0, CPU1 & Core3 ID is 7
//;=============================================================================
	MRC	P15, 0, R12, C0, C0, 5	//; Get our cpu id
	AND	R12, R12, #0xF		//; Save CPU id

	CMP	R12, #PRIMARY_CPU
	BNE	CommonInit	//; CPU 1, 2, 3 goes to Wait For Interrupt Loop

//;=============================================================================
//; boot delay for cache auto initialize 10000ns
//;=============================================================================
#if 0
	MOV	R0, #0x20000000
	ORR	R0, R0, #0x002C0000	//; ARM efuse status register address
EfuseInitRecheck:
	LDR	R1, [R0, #(1*4)]
	AND	R1, R1, #(1<<24)	//; 16: ARMTOP Efuse Initial Done, 17: ARMTOP_P1 Efuse Initial Done
	CMP	R1, #(1<<24)
	BNE	EfuseInitRecheck
#endif

//;=============================================================================
//; Get Boot Config
//;=============================================================================
	MOV	R0, #0xC0000000
	ORR	R0, R0, #0x00010000	//; syscon address
	LDR	R11, [R0, #0x27C]	//; Get Boot Config

//;=============================================================================
//; Set Cache EMA for Each Core Voltage
//;=============================================================================
#if 0
	MOV	R0, #0x20000000		//;
	ORR	R0, R0, #0x002C0000	//; tie-off reg base address

	LDR	R1, [R0, #(18*4)]	//; Get CPU0 EMA

	BIC	R1, R1, #(7<<17)	//; clear CPU0 EMA
	BIC	R1, R1, #(7<<23)	//; clear CPU0 L2EMA

	//; 0: 1.0 volt	3 apply		1: 1.1 volt 1 apply - reset config
	TST	R11, #1<<COREVOLTAGE
	MOV	R3, #0
	MOVNE	R3, #1<<17		//; 1 ==> 1.1 volt
	ORRNE	R3, R3, #1<<23
	MOVEQ	R3, #3<<17		//; 0 ==> 1.0 volt
	ORREQ	R3, R3, #3<<23

	ORR	R1, R1, R3		//; apply CPU0 EMA

	STR	R1, [R0, #(18*4)]	//; Set CPU0 EMA
#endif

CommonInit:
//;=============================================================================
//; Set Vector Base Address Register (VBAR) to point to this application's vector table
//;=============================================================================
	LDR	R0, =Vectors
	MCR	P15, 0, R0, C12, C0, 0	//; Write Secure Vector Base Address
#if 0	// no need already set first
	MRC	P15, 0, R0, C1, C0, 0	//; Read SCTLR
	BIC	R0, #(1 << 13)		//; Clear V-bit
	MCR	P15, 0, R0, C1, C0, 0	//; Write SCTLR
#endif

//;=============================================================================
//; Invalidate instruction caches and entire branch predictor and TLB
//;=============================================================================
	MOV	R0, #0
	MCR     P15, 0, R0, C7, C5, 0	//; invalidate all instruction caches
	MCR	P15, 0, R0, C7, C5, 6	//; Invalidate entire branch predictor array
	MCR	P15, 0, R0, C8, C7, 0	//; Invalidate entire Unified TLB

//;=============================================================================
//; Set SMP Start Address to Invalid
//;=============================================================================
#if 0
	MOV	R0, #0x20000000		//; peri base
	ORR	R0, R0, #0x00010000	//; clk N pwr offset
	MOV	R1, #0xFFFFFFFF		//; mark to invalid address
	STR	R1, [R0, #0x230]	//; set invalid jump address
#endif

//;=============================================================================
//; Enable caches and branch prediction
//;=============================================================================
	MOV	R0, R11
	TST	R0, #(0x1 << ICACHE)	//; check sd-icache bit

	MRCNE	P15, 0, R1, C1, C0, 0	//; Read System Control Register
	ORRNE	R1, R1, #(BIT1_Z | BIT1_I)	//; Set I bit 12 to enable I Cache
	MCRNE	P15, 0, R1, C1, C0, 0	//; Write System Control Register


//;=============================================================================
//; Setup stacks
//;=============================================================================
	MOV	SP, #0xAF000000
	ORR	SP, #0x00FF0000
	ADD	SP, SP, #INTERNAL_SRAM_SIZE	//; PHY_BASEADDR_SRAM 0xAFFF0000 + INTERNAL_SRAM_SIZE

	BL	iROMBOOT			//; save this in register for possible long jump

        BX      R0				//; jump to R0
	MOV	R12, #0x01			//; usb boot
	MOV	PC, #0				//; Never Get Here

//;=============================================================================
//; End of startup.s
//;=============================================================================
