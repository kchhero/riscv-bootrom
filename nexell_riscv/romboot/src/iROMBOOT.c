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

#include "include/nx_chip_sfr.h"
#include "include/nx_cmu.h"

#include "include/nx_gpio.h"
#include "include/nx_ecid.h"

#ifdef QEMU_RISCV
#include "kprintf.h"
#include "include/qemu_platform.h"
#else
#include "printf.h"
#include "include/platform.h"
#endif

//#include "libarm.h"
#include "fnptr.h"


struct NX_ECID_RegisterSet * const pECIDReg =
	(struct NX_ECID_RegisterSet *)PHY_BASEADDR_ECID_MODULE_SECURE;

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
    NXBL0FN *pbl0fn = Getbl0fnPtr();
/* #if 0 */
/* 	OrgBootOption = */
/* 		0 << EXNOBOOTMSG | 2 << SELSDPORT | UARTBOOT << BOOTMODE; */
/* #endif */
    unsigned int option = OrgBootOption & 0x1FF;
    unsigned int soption;

    unsigned int eBootOption = pECIDReg->BOOT_CFG;

#ifdef QEMU_RISCV
    pbl0fn->_dprintf("[bootrom] %s : eBootOption = %lu\n",__func__,eBootOption);
#endif

        //	unsigned int eSecureOption = pECIDReg->EFUSE_CFG;

/* #if 0 */
/* 	const unsigned int eRSTCFG = 1 << BOOTCFGUSE | 0 << VALIDFIELD | 0 << BOOTHALT | */
/* 		0 << NOBOOTMSG | 0 << USE_SDFS | 2 << NEXTTRYPORT | */
/* 		0 << PORTNUMBER | SDBOOT << BOOTMODE; */

/* 	const unsigned int eSECURE = 1 << AESENB |	// AES-Enable */
/* 			2 << VERIFYENB;	// Verifaction-Enable */
/* 	eBootOption = eRSTCFG; */
/* 	eSecureOption = eSECURE; */
/* #endif */


	/* if (eSecureOption & 1 << AESENB) */
	/* 	option |= 1 << DECRYPT; */
	
	/* if (eSecureOption & 1 << (VERIFYENB + 2)) */
	/* 	option |= 3 << VERIFY; */
	/* else if (eSecureOption & 1 << (VERIFYENB + 1)) */
	/* 	option |= 2 << VERIFY; */
	/* else if (eSecureOption & 1 << (VERIFYENB + 0)) */
	/* 	option |= 1 << VERIFY; */

	if (option & (1 << EXNOBOOTMSG))
		option = (option & ~(1 << EXNOBOOTMSG)) | 1 << EXNOBOOTMSG_SAVE;

	// check low boot config written and valid
	if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) {
		soption = eBootOption;
		eBootOption &= 0x7FF;	// mask VALIDFIELD & BOOTCFGUSE, HALT, SPEED
                //		option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY;
		option |= eBootOption;
	} else {
		// check high boot config and valid
		eBootOption >>= 13;
		if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) {
			soption = eBootOption;
			eBootOption &= 0x7FF;
                        //			option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY;
			option |= eBootOption;
		} else {
			// any efuse value does not written.
			// ext rstcfg has no next port information.
			// default: port 1
			option |= (0x1 + 1) << NEXTTRYPORT;	// 0: no next try
		}
	}

        //todo check!!!
        //	SetBootOption(option);

        //	SetMempoolPtr(PHY_BASEADDR_CAN0_MODULE_RAM);
        //	SetStringPtr((unsigned int *)PHY_BASEADDR_CAN1_MODULE_RAM);
        //	Setbl0fnPtr(&bl0fn);

	int speedup = 0;
	if ((soption & 1 << SPEEDUP) || (OrgBootOption & 1 << ICACHE))
		speedup = 1;
	else if (soption & 1 << SPEEDUP)
		speedup = 2;
	
	pbl0fn->setcpuclock(speedup);
	pbl0fn->setsystemclock(speedup);

	/* if (option & 1 << DECRYPT) */
	/* 	pbl0fn->setdeviceclock(sssclk, 2, 1); */

//--------------------------------------------------------------------------
// Debug Console
//--------------------------------------------------------------------------
#ifndef QEMU_RISCV
	pbl0fn->DebugInit();
#endif
	//pbl0fn->buildinfo();

	/* pbl0fn->printf("OrgBootOption:%x, eRSTCFG:%x, eSecure:%x, option:%x\r\n", */
	/* 		OrgBootOption, eRSTCFG, eSecureOption, option); */

/* #if 0 */
/* 	if (boption & 1 << BOOTHALT) { */
/* 		_dprintf("cpu halt!!!\r\n"); */
/* 		while (1) */
/* 			__asm__ __volatile__ ("wfi"); */
/* 	} */
/* #endif */

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
			Result = pbl0fn->iSDXCBOOT(option);
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

		Result = pbl0fn->iSDXCBOOT(option);

		if (Result)
			break;

lastboot:
		pbl0fn->iUSBBOOT(option);
	} while (0);


	int ret = 1;
	/* if (option & 0x3 << VERIFY) { */
	/* 	unsigned int *pCMU_SYS_APB_SRST = (unsigned int *)0x27010234; */
	/* 	*pCMU_SYS_APB_SRST = 0x1 << 2;	// set register */
	/* 	pbl0fn->nx_memset((void*)GetMempoolPtr(), 0, 8192); */
	/* 	ret = pbl0fn->authenticate_image( */
	/* 			pbm->rsa_public.rsaencryptedsha256hash,  */
	/* 			pbm->rsa_public.rsapublicbootkey, */
	/* 			(unsigned char *)pbm->image, */
	/* 			(int)pbm->bi.LoadSize); */
	/* } */

	/* if (option & 1 << DECRYPT) */
	/* 	pbl0fn->setdeviceclock(sssclk, 2, 0); */

	if (ret) {
		struct nx_bootinfo *pbi = (struct nx_bootinfo *)0xFFFF0000;
		pbl0fn->_dprintf("Launch to 0x%X\r\n", pbi->StartAddr);
//		void (*plaunch)(void) = (void (*)(void))pbi->StartAddr;
//		plaunch();
		return pbi->StartAddr;
	} else
		goto lastboot;
#else
    {
        CBOOL result = CFALSE;
        result = pbl0fn->iSDXCBOOT(option);
	while (1);
	return 0;
    }
#endif
}
