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

#include "include/nx_type.h"
#include "libplat.h"
#ifdef QEMU_RISCV
#include "kprintf.h"
#include "include/qemu_platform.h"
#else
#include "printf.h"
#include "include/platform.h"
#endif
#include "fnptr.h"

#include "include/nx_alive.h"
//#include "include/nx_chip_sfr.h"

//#define SIM_TEST
#define CNTSPEED	24000000	/* overflow time 2^32 / 24000000 = 178.957 */
					/* maybe usb & uart boot make error */
//#define CLKCNTUS	41666667
#define CLKCNTUS	41667
//unsigned int GetPMCnt(void);
void udelay(U32 utime)
{
/* #if 1 */
/* 	register unsigned int tcnt = (utime * CLKCNTUS) / 1000;	 */
/* 	tcnt += GetPMCnt(); */
/* 	/\* maybe GetPMCnt count speed will not be so fast compare to cpu *\/ */
/* //	while (tcnt != GetPMCnt()) */
/* 	while (tcnt > GetPMCnt()) */
/* 		;		 */
/* #else */
	register volatile U32 i;
	register U32 j = utime;
	for ( ; j > 0; j--)
/* #ifdef SIM_TEST */
/* 		for (i = 6; i > 0; i--); */
/* #else */
//		for (i = 106; i > 0; i--);
		for (i = 1; i > 0; i--);
        //#endif
        //#endif
}

#ifdef QEMU_RISCV
struct NX_GPIO (*const pGPIOReg)[1] = (struct NX_GPIO (*)[])GPIO_CTRL_ADDR;
struct NX_ALIVE_RegisterSet * const pAlive =
  (struct NX_ALIVE_RegisterSet *)PHY_BASEADDR_SYSCTRLTOP_MODULE;
#else
struct NX_GPIO (*const pGPIOReg)[1] =
  (struct NX_GPIO (*)[])PHY_BASEADDR_GPIO0_MODULE;
struct NX_ALIVE_RegisterSet * const pAlive =
  (struct NX_ALIVE_RegisterSet *)PHY_BASEADDR_SYSCTRLTOP_MODULE;
#endif
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

void *nx_memcpy(void *dest, const void *src, size_t n)
{
	const char *s = src;
	char *d = dest;

	while (n--)
		*d++ = *s++;

	return dest;
}

void *nx_memset(void *str, int c, size_t n)
{
	char *pdata = str;
	while (n--)
		*pdata++ = c;
	return str;
}

int nx_memcmp(const void* s1, const void* s2, size_t n)
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
        pbl0fn->_dprintf("divide by zero, halt!\r\n");

	while (1);
}
