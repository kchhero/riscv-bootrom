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
#include <nx_antares.h>
#include "nx_antares_bootoption.h"
#include "nx_antares_bootheader.h"
#include <nx_type.h>
#include "debug.h"
#include "libplat.h"

#ifdef NXP4330
#include <nx_chip.h>
#include <nx_clkpwr.h>
#include <nx_alive.h>
#endif
#ifdef NXP3220
#include <nx_chip_sfr.h>
#include <nx_cmu.h>
#endif
#include <nx_gpio.h>
#include <nx_ecid.h>

#include "printf.h"

#include "libarm.h"
#include "fnptr.h"

#ifdef NXP4330
//struct NX_ALIVE_RegisterSet * const pALIVEReg =
//	(struct NX_ALIVE_RegisterSet *)PHY_BASEADDR_ALIVE_MODULE;
struct NX_ECID_RegisterSet * const pECIDReg =
	(struct NX_ECID_RegisterSet *)PHY_BASEADDR_ECID_MODULE;
#define PHY_BASEADDR_CAN0_MODULE_RAM 0xA4000000
#define PHY_BASEADDR_CAN1_MODULE_RAM 0xA4800000
#endif

#ifdef NXP3220
struct NX_ECID_RegisterSet * const pECIDReg =
	(struct NX_ECID_RegisterSet *)PHY_BASEADDR_ECID_MODULE_SECURE;

const struct cmu_device_clk sssclk[2] = {
	{0x200, 0, 11, NX_CLKSRC_PLL0, 5},	// axi
	{0x200, 0, 6, NX_CLKSRC_PLL0, 10}	// apb
};
#endif

//	73FC7B44B996F9990261A01C9CB93C8F
const unsigned int iv[4] = {
//	0x73FC7B44, 0xB996F999, 0x0261A01C, 0x9CB93C8F
	0x447BFC73, 0x99F996B9, 0x1CA06102, 0x8F3CB99C
};
#ifdef NXP3220
struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_SRAM;
#endif

#ifdef NXP4330
struct nx_bootmm *const pbm = (struct nx_bootmm * const)0xFFFF0000;
#endif

#ifdef NXP4330
const unsigned int bootkeyhash[4][8] = {
	{0XD73A01A4, 0X0848E9B4, 0XDD1AE57E, 0X206C24E4, 
	 0X4C4B09FF, 0X17A4D4DD, 0X8D6DA22A, 0X845A9DEC},
	{0X00000000, 0X00000000, 0X00000000, 0X00000000, 
	 0X00000000, 0X00000000, 0X00000000, 0X00000000},
	{0XD73A01A4, 0X0848E9B4, 0XDD1AE57E, 0X206C24E4, 
	 0X4C4B09FF, 0X17A4D4DD, 0X8D6DA22A, 0X845A9DEC},
	{0XD73A01A4, 0X0848E9B4, 0XDD1AE57E, 0X206C24E4, 
	 0X4C4B09FF, 0X17A4D4DD, 0X8D6DA22A, 0X845A9DEC},
};
const unsigned int (*const pbootkeyhash)[4][8] =
	(const unsigned int (*)[4][8])bootkeyhash;
#endif
#ifdef NXP3220
const unsigned int (*const pbootkeyhash)[4][8] =
	(const unsigned int (*const)[4][8])0x20070150;
//	(const unsigned int (*const)[4][8])&pECIDReg->SBOOTHASH0[0];
#endif

//------------------------------------------------------------------------------
unsigned int iROMBOOT(unsigned int OrgBootOption)
{
#if 1
	OrgBootOption =
		0 << EXNOBOOTMSG | 2 << SELSDPORT | UARTBOOT << BOOTMODE;
#endif
	unsigned int option = OrgBootOption & 0x1FF;
	unsigned int soption;
#ifdef NXP3220
	unsigned int eBootOption = pECIDReg->BOOT_CFG;
	unsigned int eSecureOption = pECIDReg->EFUSE_CFG;
#endif
#ifdef NXP4330
	unsigned int eBootOption;
	unsigned int eSecureOption;
#endif

#if 1
	const unsigned int eRSTCFG = 1 << BOOTCFGUSE | 0 << VALIDFIELD | 0 << BOOTHALT |
		0 << NOBOOTMSG | 0 << USE_SDFS | 2 << NEXTTRYPORT |
		0 << PORTNUMBER | SDBOOT << BOOTMODE;

	const unsigned int eSECURE = 1 << AESENB |	// AES-Enable
			2 << VERIFYENB;	// Verifaction-Enable
	eBootOption = eRSTCFG;
	eSecureOption = eSECURE;
#endif


	if (eSecureOption & 1 << AESENB)
		option |= 1 << DECRYPT;
	
	if (eSecureOption & 1 << (VERIFYENB + 2))
		option |= 3 << VERIFY;
	else if (eSecureOption & 1 << (VERIFYENB + 1))
		option |= 2 << VERIFY;
	else if (eSecureOption & 1 << (VERIFYENB + 0))
		option |= 1 << VERIFY;

	if (option & (1 << EXNOBOOTMSG))
		option = (option & ~(1 << EXNOBOOTMSG)) | 1 << EXNOBOOTMSG_SAVE;

	// check low boot config written and valid
	if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) {
		soption = eBootOption;
		eBootOption &= 0x7FF;	// mask VALIDFIELD & BOOTCFGUSE, HALT, SPEED
		option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY;
		option |= eBootOption;
	} else {
		// check high boot config and valid
		eBootOption >>= 13;
		if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) {
			soption = eBootOption;
			eBootOption &= 0x7FF;
			option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY;
			option |= eBootOption;
		} else {
			// any efuse value does not written.
			// ext rstcfg has no next port information.
			// default: port 1
			option |= (0x1 + 1) << NEXTTRYPORT;	// 0: no next try
		}
	}

	SetBootOption(option);

	SetMempoolPtr(PHY_BASEADDR_CAN0_MODULE_RAM);
	SetStringPtr((unsigned int *)PHY_BASEADDR_CAN1_MODULE_RAM);
	Setbl0fnPtr(&bl0fn);

	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();

#ifdef NXP3220
	int speedup = 0;
	if ((soption & 1 << SPEEDUP) || (OrgBootOption & 1 << ICACHE))
		speedup = 1;
	else if (soption & 1 << SPEEDUP)
		speedup = 2;
	
	pbl0fn->setcpuclock(speedup);
	pbl0fn->setsystemclock(speedup);

	if (option & 1 << DECRYPT)
		pbl0fn->setdeviceclock(sssclk, 2, 1);
#endif
#ifdef NXP4330
	soption = soption;
#endif
//--------------------------------------------------------------------------
// Debug Console
//--------------------------------------------------------------------------
	pbl0fn->DebugInit();

	pbl0fn->buildinfo();

	pbl0fn->printf("OrgBootOption:%x, eRSTCFG:%x, eSecure:%x, option:%x\r\n",
			OrgBootOption, eRSTCFG, eSecureOption, option);

#if 0
	if (boption & 1 << BOOTHALT) {
		printf("cpu halt!!!\r\n");
		while (1)
			__asm__ __volatile__ ("wfi");
	}
#endif

#ifndef BL1SIMUL
	// external usb boot is top priority, always first checked.
	if ((OrgBootOption & 0x7 << BOOTMODE) == (USBBOOT << BOOTMODE)) {
		pbl0fn->printf("force usb boot\r\n");
		goto lastboot;
	}

	do {
		CBOOL Result = CFALSE;
//--------------------------------------------------------------------------
// iROMBOOT mode : option [2:0]
//--------------------------------------------------------------------------
		switch ((option >> BOOTMODE) & 0x7) {
		default:
			pbl0fn->printf("no support boot mode(%x)\r\n", option);
		case USBBOOT:
			Result = pbl0fn->iUSBBOOT(option);
			break;
		case SDFSBOOT:	// iSDHCFSBOOT (SD/MMC/eSD/eMMC)
			Result = pbl0fn->iSDXCFSBOOT(option);
			break;
		case SPIBOOT:	// iSPIBOOT (Serial Flash memory)
			Result = pbl0fn->iSPIBOOT(option);
			break;
		case SDBOOT:	// iSDHCBOOT (SD/MMC/eSD/eMMC)
			Result = pbl0fn->iSDXCBOOT(option);
			break;
		case EMMCBOOT:
			Result = pbl0fn->ieMMCBOOT(option);
			break;
		case NANDECBOOT:
			Result = pbl0fn->iNANDBOOTEC(option);
			break;
		case UARTBOOT:
			Result = pbl0fn->iUARTBOOT(option);
			break;
		}

		if (Result)
			break;

		unsigned int next_port = (option >> NEXTTRYPORT) & 0x3;
		if (0 == next_port)
			goto lastboot;

		option &= ~0x67UL;	// mask boot mode, portnumber for SDBOOT
		option |= next_port << PORTNUMBER;

		pbl0fn->printf("update boot\r\n");
		if (option & 1 << USE_SDFS)
			Result = pbl0fn->iSDXCFSBOOT(option);
		else
			Result = pbl0fn->iSDXCBOOT(option);

		if (Result)
			break;

lastboot:
		pbl0fn->iUSBBOOT(option);
	} while (0);


	int ret = 1;
	if (option & 0x3 << VERIFY) {
#ifdef NXP3220
		unsigned int *pCMU_SYS_APB_SRST = (unsigned int *)0x27010234;
		*pCMU_SYS_APB_SRST = 0x1 << 2;	// set register
		pbl0fn->memset((void*)GetMempoolPtr(), 0, 8192);
#endif
		ret = pbl0fn->authenticate_image(
				pbm->rsa_public.rsaencryptedsha256hash, 
				pbm->rsa_public.rsapublicbootkey,
				(unsigned char *)pbm->image,
				(int)pbm->bi.LoadSize);
	}

#ifdef NXP3220
	if (option & 1 << DECRYPT)
		pbl0fn->setdeviceclock(sssclk, 2, 0);
#endif

	if (ret) {
		struct nx_bootinfo *pbi = (struct nx_bootinfo *)0xFFFF0000;
		pbl0fn->printf("Launch to 0x%X\r\n", pbi->StartAddr);
//		void (*plaunch)(void) = (void (*)(void))pbi->StartAddr;
//		plaunch();
		return pbi->StartAddr;
	} else
		goto lastboot;
#else
	while (1);
	return 0;
#endif
}
