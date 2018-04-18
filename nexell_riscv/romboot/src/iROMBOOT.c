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

#ifdef QEMU_RISCV
#include <nx_qemu_printf.h>
#else
#include <nx_swallow_printf.h>
#endif

//struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_OF_PBM;
//------------------------------------------------------------------------------
int romboot(int bootmode)
{
    int option = bootmode;
#ifndef QEMU_RISCV
     /* volatile char *reg = (char*)PHY_BASEADDR_DUMMY_MODULE; */
     /* char *s = "test rom"; */
     /* char c = ' '; */
     /* for (; (c = *s) != '\0'; s++) */
     /*    *reg = (char)c; */

     //     _dprintf("ROMBOOT");
    nxSetClockInit();

    //todo
    // DDR1 setup
    
    do {
        int Result = 0;
        switch (option) {
            /* case XIPBOOT: */
            /* 	break; */
        case SDBOOT:	// iSDHCBOOT (SD/MMC/eSD/eMMC)
            Result = iSDBOOT(option);
            break;
        default:
            //            _dprintf("no support boot mode(%x)\r\n", option);
            break;
        }

        if (Result)
            break;

        //        _dprintf("update boot\r\n");
        Result = iSDBOOT(option);

        if (Result)
            break;

    } while (0);
#else
    /* struct nx_bootinfo *pbi = (struct nx_bootinfo *)BASEADDR_OF_PBM; */
    unsigned int result;
    _dprintf("ROMBOOT Start\n");
    nxSetClockInit();
    _dprintf("<<bootrom>> option %s = 0x%x\n", __func__, option); 
    if (option != 0) {
        _dprintf("<<bootrom>> iSDBOOT go\n");            
        result = iSDBOOT(option);

    }
    else
        _dprintf("<<bootrom>> boot option is strange!\n");
#endif        
    __asm__ __volatile__ ("fence.i" : : : "memory");

    return 1;
}
