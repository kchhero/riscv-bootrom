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
#include "nx_peridot.h"

//#include "aarch64_Vectors.inc"

	.align 9, 0    // 2^9 = 512 bytes sector size
	.skip 0x200, 0x00

/*
 * entry point of main function
 */
.global main
.global s5p6818_subcpu_on
.global GetCPUID

//;==================================================================
//; Vectors
//;==================================================================
.global Startup
Startup:

.global Sleep
Sleep:


BuildInfo:


Reset_Handler:

clbss_l:

clbss_e:

CPUBRINGUP:

.global __pllchange
__pllchange:                                        //; r0:data r1:pll address r2:delay count

pllchangedelayloop:                                 //; this code will be already within i-cache. no bus transa
        
notadapt:

postloop:

synchrorous_vector_jump:

sync_vector_value:

.global    remap_vectors
// void remap_vectors(U32 cpuid, U32 *synchrorous_vector_jump, void temp, U32 *__bss_end__);
remap_vectors:

    .global sync_asm_handler_EL3
sync_asm_handler_EL3:

.global ExceptionTest
.global Startup_EL1
Startup_EL1:

//;==================================================================
//; End of startup.s
//;==================================================================
