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
 * File		: libplat.h
 * Description	: various chip library
 * Author	: Hans
 * History	: 2016.08.12 First Implementation
 */
#ifndef __LIBPLAT_H__
#define __LIBPLAT_H__

#include <nx_gpio.h>
#include <nx_type.h>
#ifdef NXP3220
#include <nx_cmu.h>
#endif

void udelay(U32 utime);
#ifdef NXP4330
void ResetCon(U32 devicenum, CBOOL en);
void GPIOSetAltFunction(U32 AltFunc);
U32 NX_CLKPWR_GetPLLFrequency(U32 PllNumber);
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#ifdef NXP3220
struct nxpadi {
	unsigned int alt: 3;    /* alt function */
	unsigned int pin: 5;    /* pin number of group */
	unsigned int grp: 8;    /* gpio group number */
	unsigned int flag: 16;  /* pad property 0: not controlable */
};

union nxpad {
	unsigned int padd;
	struct nxpadi padi;
};

void GPIOSetAltFunction(const struct nxpadi *pad, CBOOL setalt);
void GPIOSetDrvSt(const struct nxpadi *pad, NX_GPIO_DRVSTRENGTH str);
void GPIOSetPullup(const struct nxpadi *pad, NX_GPIO_PULL pull);
void GPIOReleasePAD(const struct nxpadi *pad);
void setpad(const union nxpad *const ppad, int num, int enable);


void setcpuclock(unsigned int clklvl);
void setsystemclock(unsigned int clklvl);
void setdeviceclock(const struct cmu_device_clk *pdclk, int num, int enb);
#endif

#endif /* __LIBPLAT_H__ */
