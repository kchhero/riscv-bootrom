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
#include "include/nx_swallow.h"
#include "include/nx_swallow_bootoption.h"
#include "include/nx_swallow_bootheader.h"
#include "include/nx_type.h"
#include "debug.h"
#include "libplat.h"


//#include "include/nx_chip_sfr.h"
#include "include/nx_cmu.h"

#include "include/nx_gpio.h"
//#include <nx_ecid.h>

#ifdef QEMU_RISCV
#include "kprintf.h"
#include "include/qemu_platform.h"
#else
#include "printf.h"
#include "include/platform.h"
#endif

#include "fnptr.h"

#include "include/common.h"


struct NX_ECID_RegisterSet * const pECIDReg =
  (struct NX_ECID_RegisterSet *)0x700000;//PHY_BASEADDR_ECID_MODULE_SECURE;

const struct cmu_device_clk sssclk[2] = {
	{0x200, 0, 11, NX_CLKSRC_PLL0, 5},	// axi
	{0x200, 0, 6, NX_CLKSRC_PLL0, 10}	// apb
};


//	73FC7B44B996F9990261A01C9CB93C8F
const unsigned int iv[4] = {
//	0x73FC7B44, 0xB996F999, 0x0261A01C, 0x9CB93C8F
	0x447BFC73, 0x99F996B9, 0x1CA06102, 0x8F3CB99C
};

struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_SRAM;

const unsigned int (*const pbootkeyhash)[4][8] =
	(const unsigned int (*const)[4][8])0x20070150;
//	(const unsigned int (*const)[4][8])&pECIDReg->SBOOTHASH0[0];


//------------------------------------------------------------------------------
unsigned int iROMBOOT(unsigned int OrgBootOption)
{
#if 1
        OrgBootOption = SDBOOT;
#endif
        NXBL0FN *pbl0fn = Getbl0fnPtr();
	unsigned int option = OrgBootOption & 0x1FF;
        //unsigned int soption = 0;

#ifdef DEBUG
        REG32(uart, UART_REG_TXCTRL) = UART_TXEN;
        kputs("[SUKER-bootrom] iROMBOOT launch\n");
        kputs("[SUKER-bootrom] bootmode ---------*");
        kputs("[SUKER-bootrom] USBBOOT		 1");
        kputs("[SUKER-bootrom] SDBOOT		 2");
        kputs("[SUKER-bootrom] XIP		 3");
        kputs("[SUKER-bootrom] ------------------*");
        _dprintf("[SUKER-bootrom] OrgBootOption : 0x%x\n",OrgBootOption);
        _dprintf("[SUKER-bootrom] option        : 0x%x\n",option);
#endif
/* #if 1 */
/* 	const unsigned int eRSTCFG = 1 << BOOTCFGUSE | 0 << VALIDFIELD | 0 << BOOTHALT | */
/* 		0 << NOBOOTMSG | 0 << USE_SDFS | 2 << NEXTTRYPORT | */
/* 		0 << PORTNUMBER | SDBOOT << BOOTMODE; */

/* #endif */

/* 	if (option & (1 << EXNOBOOTMSG)) */
/* 		option = (option & ~(1 << EXNOBOOTMSG)) | 1 << EXNOBOOTMSG_SAVE; */


        //	SetBootOption(option);

	/* SetMempoolPtr(PHY_BASEADDR_CAN0_MODULE_RAM); */
	/* SetStringPtr((unsigned int *)PHY_BASEADDR_CAN1_MODULE_RAM); */
	/* Setbl0fnPtr(&bl0fn); */

        //	NXBL0FN *pbl0fn = Getbl0fnPtr();

	/* int speedup = 0; */
	/* if ((soption & 1 << SPEEDUP) || (OrgBootOption & 1 << ICACHE)) */
	/* 	speedup = 1; */
	/* else if (soption & 1 << SPEEDUP) */
	/* 	speedup = 2; */
	
        //	pbl0fn->setcpuclock(speedup);
        //	pbl0fn->setsystemclock(speedup);

	/* if (option & 1 << DECRYPT) */
	/* 	pbl0fn->setdeviceclock(sssclk, 2, 1); */

//--------------------------------------------------------------------------
// Debug Console
//--------------------------------------------------------------------------
//	pbl0fn->DebugInit();
//        DebugInit();

        //	pbl0fn->buildinfo();

	//pbl0fn->_dprintf("OrgBootOption:%x, eRSTCFG:%x, option:%x\r\n",
	//		OrgBootOption, eRSTCFG, option);
        //        while(1);
        //        return 0;
        /* _dprintf("OrgBootOption:%x, eRSTCFG:%x, option:%x\r\n", */
	/* 		OrgBootOption, eRSTCFG, option); */
        
#if 0
	if (boption & 1 << BOOTHALT) {
		printf("cpu halt!!!\r\n");
		while (1)
			__asm__ __volatile__ ("wfi");
	}
#endif

#ifndef QEMU_RISCV
	// external usb boot is top priority, always first checked.
	if ((OrgBootOption & 0x7 << BOOTMODE) == (USBBOOT << BOOTMODE)) {
                pbl0fn->_dprintf("force usb boot\r\n");
		goto lastboot;
	}

	do {
		CBOOL Result = CFALSE;
//--------------------------------------------------------------------------
// iROMBOOT mode : option [2:0]
//--------------------------------------------------------------------------
		switch ((option >> BOOTMODE) & 0x7) {
		default:
                        pbl0fn->_dprintf("no support boot mode(%x)\r\n", option);

		case USBBOOT:
			Result = pbl0fn->iUSBBOOT(option);
			break;
		case SDBOOT:	// iSDHCBOOT (SD/MMC/eSD/eMMC)
			Result = pbl0fn->iSDMMCBOOT(option);
			break;
		}

		if (Result)
			break;

		unsigned int next_port = (option >> NEXTTRYPORT) & 0x3;
		if (0 == next_port)
			goto lastboot;

		option &= ~0x67UL;	// mask boot mode, portnumber for SDBOOT
		option |= next_port << PORTNUMBER;

                pbl0fn->_dprintf("update boot\r\n");

		Result = pbl0fn->iSDMMCBOOT(option);

		if (Result)
			break;

lastboot:
		pbl0fn->iUSBBOOT(option);
	} while (0);


	int ret = 1;
	if (ret) {
		struct nx_bootinfo *pbi = (struct nx_bootinfo *)0xFFFF0000;
                pbl0fn->_dprintf("Launch to 0x%X\r\n", pbi->StartAddr);

//		void (*plaunch)(void) = (void (*)(void))pbi->StartAddr;
//		plaunch();
		return pbi->StartAddr;
	} else
		goto lastboot;
#else
        pbl0fn->iSDMMCBOOT(1);
	while (1);
	return 0;
#endif
}
