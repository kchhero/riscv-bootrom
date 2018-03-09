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
#include "libarm.h"
#include "libplat.h"
#include <printf.h>
#include "fnptr.h"

#ifdef NXP3220
#include <nx_alive.h>
#include <nx_chip_sfr.h>
#endif

#ifdef NXP4330
#include <nx_rstcon.h>
#include <nx_clkpwr.h>

struct NX_GPIO_RegisterSet (* const pGPIOReg)[1] =
	(struct NX_GPIO_RegisterSet (*)[])PHY_BASEADDR_GPIOA_MODULE;
	
struct NX_RSTCON_RegisterSet * const pReg_RstCon =
		(struct NX_RSTCON_RegisterSet * const)PHY_BASEADDR_RSTCON_MODULE;


void ResetCon(U32 devicenum, CBOOL en)
{
	if (en)
		ClearIO32(&pReg_RstCon->REGRST[(devicenum >> 5) & 0x3],
			  (0x1 << (devicenum & 0x1F))); // reset
	else
		SetIO32(&pReg_RstCon->REGRST[(devicenum >> 5) & 0x3],
			(0x1 << (devicenum & 0x1F))); // reset negate
}

struct NXPERIDOT_GPIO_RegSet
{
	struct NX_GPIO_RegisterSet NXGPIO;
	U8 Reserved[0x1000 - sizeof(struct NX_GPIO_RegisterSet)];
};

#if !DIRECT_IO
static struct NXPERIDOT_GPIO_RegSet (* const pBaseGPIOReg)[1] =
	(struct NXPERIDOT_GPIO_RegSet (*)[])(PHY_BASEADDR_GPIOA_MODULE);
void GPIOSetAltFunction(U32 AltFunc)
{
	register struct NX_GPIO_RegisterSet *pGPIOReg =
			&pBaseGPIOReg[(AltFunc >> 8) & 0x7]->NXGPIO;
	pGPIOReg->GPIOx_ALTFN[(AltFunc >> 6) & 0x1] =
			(pGPIOReg->GPIOx_ALTFN[(AltFunc >> 6) & 0x1] &
				~(0x3UL << (((AltFunc >> 3) & 0xF) * 2)))
			| ((AltFunc & 0x3) << (((AltFunc >> 3) & 0xF) * 2));
}
#endif
#endif	// NXP4330

//#define SIM_TEST
#define CNTSPEED	24000000	/* overflow time 2^32 / 24000000 = 178.957 */
					/* maybe usb & uart boot make error */
//#define CLKCNTUS	41666667
#define CLKCNTUS	41667
unsigned int GetPMCnt(void);
void udelay(U32 utime)
{
#if 1
	register unsigned int tcnt = (utime * CLKCNTUS) / 1000;	
	tcnt += GetPMCnt();
	/* maybe GetPMCnt count speed will not be so fast compare to cpu */
//	while (tcnt != GetPMCnt())
	while (tcnt > GetPMCnt())
		;		
#else
	register volatile U32 i;
	register U32 j = utime;
	for ( ; j > 0; j--)
#ifdef SIM_TEST
		for (i = 6; i > 0; i--);
#else
//		for (i = 106; i > 0; i--);
		for (i = 1; i > 0; i--);
#endif
#endif
}

#ifdef NXP3220
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
#if 0
void GPIO_SetIO(struct padi *pad, CBOOL inout)	/* 0: out, 1: in */
{
	struct NX_GPIO_RegisterSet *pGPIOReg = pNXGPIO[pad->group];
	if (!pad->flag)
		return;
	if (inout)
		pGPIOReg->GPIOx_CLR_OUTENB = 1 << pad->pin;
	else
		pGPIOReg->GPIOx_SET_OUTENB = 1 << pad->pin;
}
#endif
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
#endif

#ifdef NXP4330
#define PLL_P	18
#define PLL_M	8
#define PLL_S	0
#define PLL_K	16

#define __g_OSC_KHz	24000
static struct NX_CLKPWR_RegisterSet *pclkpwr =
	(struct NX_CLKPWR_RegisterSet *)PHY_BASEADDR_CLKPWR_MODULE;
U32 NX_CLKPWR_GetPLLFrequency(U32 PllNumber)
{
	U32 regvalue, regvalue1, nP, nM, nS, nK;
	U32 temp = 0;

	regvalue = pclkpwr->PLLSETREG[PllNumber];
	regvalue1 = pclkpwr->PLLSETREG_SSCG[PllNumber];
	nP = (regvalue >> PLL_P) & 0x3F;
	nM = (regvalue >> PLL_M) & 0x3FF;
	nS = (regvalue >> PLL_S) & 0xFF;
	nK = (regvalue1 >> PLL_K) & 0xFFFF;

	if ((PllNumber > 1) && nK)
		temp = ((((nK * 1000) / 65536) * __g_OSC_KHz) / nP) >> nS;

	temp = ((((nM * __g_OSC_KHz) / nP) >> nS) * 1000) + temp;
	return temp;
}
#endif

void *memcpy(void *dest, const void *src, size_t n)
{
	const char *s = src;
	char *d = dest;

	while (n--)
		*d++ = *s++;

	return dest;
}

void *memset(void *str, int c, size_t n)
{
	char *pdata = str;
	while (n--)
		*pdata++ = c;
	return str;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
	const char *src1 = s1, *src2 = s2;

	while (n--) {
		char res = *src1++ - *src2++;
		if (res)
			return (res);
	}
	return 0;
}

void __div0(void)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->printf("divide by zero, halt!\r\n");
	while (1);
}

unsigned int __udivmodsi4(unsigned int num, unsigned int den)
{
	unsigned int quot = 0, qbit = 1;

	if (den == 0) {
		__div0();
		return -1;
	}

	/* Left-justify denominator and count shift */
	while ((int) den >= 0) {
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit) {
		if (den <= num) {
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	return quot;
}

unsigned int __aeabi_uidiv(unsigned int num, unsigned int den)
{
	return __udivmodsi4(num, den);
}
