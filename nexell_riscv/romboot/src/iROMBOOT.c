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

#if defined(QEMU_RISCV) || defined(SOC_SIM)
#include <nx_qemu_sim_printf.h>
#else
#include <nx_swallow_printf.h>
#endif

void __riscv_synch_thread(void) {
    __asm__ __volatile__ ("fence" : : : "memory");
}


//------------------------------------------------------------------------------
int romboot(void)
{
    int result = 0;
    int option = getBootMode();

#ifdef DEBUG
    _dprintf("ROMBOOT Start\n");
    _dprintf("<<bootrom>>romboot start with option = 0x%x\n",option);
#endif

#ifndef QEMU_RISCV
    nxSetClockInit();
    
    do {
        switch (option) {
        case SDBOOT:	// iSDHCBOOT (SD/MMC/eSD/eMMC)
            result = iSDBOOT(option);
            break;
        case SPIBOOT:
            _dprintf("SPIBOOT welcome\n");
            break;
        default: //default bootmode is sdcard
            result = iSDBOOT(option);
            //            _dprintf("no support boot mode(%x)\r\n", option);
            break;
        }

        if (result)
            break;

    } while (0);

#else

    nxSetClockInit();

    if (option != 0) {
        result = iSDBOOT(option);
    }
    else
        _dprintf("<<bootrom>> boot option is strange!\n");
#endif

    __riscv_synch_thread();

#ifdef DEBUG
    _dprintf(">> sdboot result = 0x%x <<\n",result);
#endif
    
    if (result) {
        struct nx_bootinfo *pbi = (struct nx_bootinfo *)BASEADDR_SRAM;
#ifdef DEBUG
        _dprintf(">> Launch to 0x%x\n", pbi->StartAddr);
#endif
        return pbi->StartAddr; //0x40000200
    }

    while(1);
    return 0;
}

//------------------------------------------------------------------------------
int getBootMode(void)
{
    volatile unsigned int* pSysConReg = (unsigned int*)PHY_BASEADDR_SYS_CON0_MODULE;

    unsigned int oneReg = *pSysConReg+0;
    unsigned int bootMode = (oneReg & 0x00000020) >> 5;
#ifdef DEBUG
    if (bootMode == 0) {
        _dprintf("SDMMC Boot Mode\n");
    } else {
        _dprintf("SPI Boot Mode\n");
    }
#endif
    return bootMode;
}
