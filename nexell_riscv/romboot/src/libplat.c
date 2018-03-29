/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: library
 * File		: libplat.c
 * Description	: various chip library
 * Author	: Hans
 * History	: 2016.08.12 First Implementation
 */

#include <nx_type.h>
#include "libplat.h"
#include <kprintf.h>
//#include "fnptr.h"

#include <nx_alive.h>
//#include <nx_chip_sfr.h>

//#define SIM_TEST
#define CNTSPEED	24000000	/* overflow time 2^32 / 24000000 = 178.957 */
					/* maybe usb & uart boot make error */
//#define CLKCNTUS	41666667
#define CLKCNTUS	41667

struct NX_GPIO (*const pGPIOReg)[1] =
	(struct NX_GPIO (*)[])PHY_BASEADDR_GPIO0_MODULE;
struct NX_ALIVE_RegisterSet * const pAlive =
	(struct NX_ALIVE_RegisterSet *)PHY_BASEADDR_SYSCTRLTOP_MODULE;
void GPIOReleasePAD(const struct nxpadi *pad)
{
	pAlive->GPIO_PWR[pad->grp].DOWN &= ~(1 << pad->pin);	// pad retention
}
void GPIOSetAltFunction(const struct nxpadi *pad, CBOOL setalt)
{
	U32 regvalue;
	U32 alt = setalt ? pad->alt : 0;

	if (!pad->flag)
		return;

	regvalue = pGPIOReg[pad->grp]->RWGPIO.GPIOx_ALTFN[pad->pin >> 4];
	pGPIOReg[pad->grp]->RWGPIO.GPIOx_ALTFN[pad->pin >> 4] =
		(regvalue & ~(3 << ((pad->pin & 0xF) * 2))) |
			(alt << ((pad->pin & 0xF) * 2));
}

void GPIOSetDrvSt(const struct nxpadi *pad, NX_GPIO_DRVSTRENGTH str)
{
	struct NX_GPIO *pGPIOxReg = pGPIOReg[pad->grp];
	U32 d0 = str & 1, d1 = (str >> 1) & 1;

	if (!pad->flag)
		return;

	if (d0) {
		pGPIOxReg->SGPIO.GPIOx_DRV0 = 0x1 << pad->pin;
		pGPIOxReg->SGPIO.GPIOx_DRV0_DISABLE_DEFAULT = 0x1 << pad->pin;
	} else {
		pGPIOxReg->CGPIO.GPIOx_DRV0 = 0x1 << pad->pin;
		pGPIOxReg->CGPIO.GPIOx_DRV0_DISABLE_DEFAULT = 0x1 << pad->pin;
	}
	if (d1) {
		pGPIOxReg->SGPIO.GPIOx_DRV1 = 0x1 << pad->pin;
		pGPIOxReg->SGPIO.GPIOx_DRV1_DISABLE_DEFAULT = 0x1 << pad->pin;
	} else {
		pGPIOxReg->CGPIO.GPIOx_DRV1 = 0x1 << pad->pin;
		pGPIOxReg->CGPIO.GPIOx_DRV1_DISABLE_DEFAULT = 0x1 << pad->pin;
	}
}
void GPIOSetPullup(const struct nxpadi *pad, NX_GPIO_PULL pull)
{
	struct NX_GPIO *pGPIOxReg = pGPIOReg[pad->grp];
	U32 pullupdown = pull & 1, pullen = !((pull >> 1) & 1);

	if (!pad->flag)
		return;

	if (pullupdown) {
		pGPIOxReg->SGPIO.GPIOx_PULLSEL = 1 << pad->pin;
		pGPIOxReg->SGPIO.GPIOx_PULLSEL_DISABLE_DEFAULT = 1 << pad->pin;
	} else {
		pGPIOxReg->CGPIO.GPIOx_PULLSEL = 1 << pad->pin;
		pGPIOxReg->CGPIO.GPIOx_PULLSEL_DISABLE_DEFAULT = 1 << pad->pin;
	}
	if (pullen) {
		pGPIOxReg->SGPIO.GPIOx_PULLENB = 1 << pad->pin;
		pGPIOxReg->SGPIO.GPIOx_PULLENB_DISABLE_DEFAULT = 1 << pad->pin;
	} else {
		pGPIOxReg->CGPIO.GPIOx_PULLENB = 1 << pad->pin;
		pGPIOxReg->CGPIO.GPIOx_PULLENB_DISABLE_DEFAULT = 1 << pad->pin;
	}
}
void setpad(const union nxpad *const ppad, int num, int enable)
{
	int i;
	for (i = 0; i < num; i++) {
		if (enable) {
			GPIOSetAltFunction(&ppad[i].padi, CTRUE);
			GPIOSetDrvSt(&ppad[i].padi, NX_GPIO_DRVSTRENGTH_3);
			GPIOSetPullup(&ppad[i].padi, NX_GPIO_PULL_OFF);
			if (ppad[i].padd)
				GPIOReleasePAD(&ppad[i].padi);
		} else {
			GPIOSetAltFunction(&ppad[i].padi, CFALSE);
			GPIOSetDrvSt(&ppad[i].padi, NX_GPIO_DRVSTRENGTH_3);
		}
	}
}
