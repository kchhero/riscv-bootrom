/*
 * Copyright (C) 2016  Nexell Co., Ltd.
 * Author: Sangjong, Han <hans@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nx_pyrope.h"
#include "cfgBootDefine.h"

        .align
/* Start and e.nd of BSS */

.globl __bss_start__
.globl __bss_end__

/*
 * entry point of main function
 */
.global BootMain
.text
//==================================================================
// vectors
//==================================================================
.global vectors
vectors:
        b       reset_handler 		// 00 - Reset
        b       .          		// 04 - Undefined instructions
        b       .             		// 08 - SWI instructions
        b       .          		// 0C - Instruction fetch aborts
        b       .          		// 10 - Data access aborts
        b       .           		// 14 - Reserved (was address exception)
        b       .            		// 18 - IRQ interrupts
        b       .            		// 1C - FIQ interrupts


reset_handler:
	/* Set to zero, Clear SRAM */
	ldr     r1, =__bss_start__		// this is auto-relocated!
	ldr     r2, =__bss_end__		// this is auto-relocated!

	mov     r3, #0x00000000			// prepare zero to clear BSS

clearbss:
	cmp     r1, r2				// while not at end of BSS
	strlo   r3, [r1]			// clear 32-bit BSS word
	addlo   r1, r1, #4			// move to next
	blo     clearbss

main:
	ldr     sp, =BASEADDR_SRAM
	add     sp, sp, #INTERNAL_SRAM_SIZE	// Add to internal sram size.

	bl	BootMain			//; save this in register for possible long jump
	b	.


	.ltorg
