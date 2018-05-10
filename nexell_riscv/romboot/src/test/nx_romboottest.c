////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Nexell Co., Ltd All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	:
// File		: iROMBOOT.c
// Description	:
// Author	: Hans
// History	:
//		2014-01-12	Hans create
//		2017-10-09	Hans modified for NXP3220
//		2018-01-15	Hans sim done
////////////////////////////////////////////////////////////////////////////////
#include <nx_swallow.h>
#include <nx_bootheader.h>
#include <nx_clock.h>
#include <nx_debug.h>
#include <iSDBOOT.h>

#include <nx_swallow_printf.h>

/* void _fencei(void) { */
/*     __asm__ volatile ("fence.i"); */
/* } */
/* void _sfencem(void) { */
/*     __asm__ volatile ("sfence.vm"); */
/* } */
/* void _fencerd(void) { */
/*     __asm__ volatile ("fence.rd"); */
/* } */
/* void _fenceri(void) { */
/*     __asm__ volatile ("fence.ri"); */
/* } */

//#define _fencem() asm volatile ("fence" ::: "memory")

void __riscv_flush_icache(void) {
    __asm__ volatile ("fence.i");
}
void __riscv_synch_thread(void) {
    __asm__ volatile ("fence");
}

//struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_OF_PBM;
//------------------------------------------------------------------------------
int romboottest(int bootmode)
{
    int option = bootmode;
    int result = 0;

    nxSetClockInit();

    unsigned int* volatile temp = (unsigned int*)(BASEADDR_SRAM);
    _dprintf("First 128byte values are below ---\n");
    for (volatile unsigned int i = 0; i < 1024; i++) {
        *(temp+i) = i;
    }

    //__asm__ volatile ("fence");
    //    __asm__ volatile ("fence");
    _fencem();
    //_sfencem();
    _dprintf("Write DONE!\n");
    //    __asm__ volatile ("fence.i");
 
    for (volatile unsigned int i = 0; i < 8; i++) {
        _dprintf("%x ",*(temp+i));
    }
    //    __asm__ volatile ("fence.i");
    _dprintf("\nRead DONE!\n");
    
    return 0;
}
