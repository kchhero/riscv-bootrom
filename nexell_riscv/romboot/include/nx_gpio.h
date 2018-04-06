/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: gpio
 * File		: nx_gpio.h
 * Description	: gpio register map
 * Author	: Hans
 * History	: 2018.01.06 First Implementation
 */

#ifndef __NX_GPIO_H__
#define __NX_GPIO_H__

struct	NX_GPIO_RegisterSet
{
	volatile unsigned int GPIOx_OUT;			// 0x000: Output Register
	volatile unsigned int GPIOx_OUTENB;		// 0x004: Output Enable Register
	volatile unsigned int GPIOx_DETMODE[2];		// 0x008: Event Detect Mode Register
	volatile unsigned int GPIOx_INTENB;		// 0x010: Interrupt Enable Register
	volatile unsigned int GPIOx_DET;			// 0x014: Event Detect Register
	volatile unsigned int GPIOx_PAD;			// 0x018: PAD Status Register
	volatile unsigned int GPIOx_PUENB;		// 0x01C: Pull Up Enable Register /// @note charles - THIS REGISTER IS NOT USED
	volatile unsigned int GPIOx_ALTFN[2];		// 0x020: Alternate Function Select Register
	volatile unsigned int __Reserved[5];		// 0x028:
	volatile unsigned int GPIOx_DETENB;		// 0x03C: IntPend Detect Enable Register
	volatile unsigned int GPIOx_SLEW;		// 0x040
	volatile unsigned int GPIOx_SLEW_DISABLE_DEFAULT;	// 0x044
	volatile unsigned int GPIOx_DRV1;			// 0x048
	volatile unsigned int GPIOx_DRV1_DISABLE_DEFAULT;	// 0x04C
	volatile unsigned int GPIOx_DRV0;			// 0x050
	volatile unsigned int GPIOx_DRV0_DISABLE_DEFAULT;	// 0x054
	volatile unsigned int GPIOx_PULLSEL;			// 0x058
	volatile unsigned int GPIOx_PULLSEL_DISABLE_DEFAULT;	// 0x05C
	volatile unsigned int GPIOx_PULLENB;			// 0x060
	volatile unsigned int GPIOx_PULLENB_DISABLE_DEFAULT;	// 0x064
	volatile unsigned int GPIOx_InputMuxSelect[2];		// 0x068
	volatile unsigned int GPIOx_SECURE_MARKING;		// 0x070
	volatile unsigned int GPIOx_INPUTENB;			// 0x074
	volatile unsigned int GPIOx_INPUTENB_DISABLE_DEFAULT;	// 0x078
	volatile unsigned int GPIOx_ALTFN_2;			// 0x07C

	unsigned char __Reserved_MID0[0x80];
};

struct NX_GPIO
{
	struct NX_GPIO_RegisterSet RWGPIO;
	struct NX_GPIO_RegisterSet SGPIO;
	struct NX_GPIO_RegisterSet CGPIO;
	unsigned char __Rev[0x1000 - 3 * sizeof(struct NX_GPIO_RegisterSet)];
};

///@brief	GPIO Interrupts for interrupt interface
enum
{
	NX_GPIO_INT_GPIO0	= 0,	//	GPIO 0 Interrupt
	NX_GPIO_INT_GPIO1	= 1,	//	GPIO 1 Interrupt
	NX_GPIO_INT_GPIO2	= 2,	//	GPIO 2 Interrupt
	NX_GPIO_INT_GPIO3	= 3,	//	GPIO 3 Interrupt
	NX_GPIO_INT_GPIO4	= 4,	//	GPIO 4 Interrupt
	NX_GPIO_INT_GPIO5	= 5,	//	GPIO 5 Interrupt
	NX_GPIO_INT_GPIO6	= 6,	//	GPIO 6 Interrupt
	NX_GPIO_INT_GPIO7	= 7,	//	GPIO 7 Interrupt
	NX_GPIO_INT_GPIO8	= 8,	//	GPIO 8 Interrupt
	NX_GPIO_INT_GPIO9	= 9,	//	GPIO 9 Interrupt
	NX_GPIO_INT_GPIO10	= 10,	//	GPIO 10 Interrupt
	NX_GPIO_INT_GPIO11	= 11,	//	GPIO 11 Interrupt
	NX_GPIO_INT_GPIO12	= 12,	//	GPIO 12 Interrupt
	NX_GPIO_INT_GPIO13	= 13,	//	GPIO 13 Interrupt
	NX_GPIO_INT_GPIO14	= 14,	//	GPIO 14 Interrupt
	NX_GPIO_INT_GPIO15	= 15,	//	GPIO 15 Interrupt
	NX_GPIO_INT_GPIO16	= 16,	//	GPIO 16 Interrupt
	NX_GPIO_INT_GPIO17	= 17,	//	GPIO 17 Interrupt
	NX_GPIO_INT_GPIO18	= 18,	//	GPIO 18 Interrupt
	NX_GPIO_INT_GPIO19	= 19,	//	GPIO 19 Interrupt
	NX_GPIO_INT_GPIO20	= 20,	//	GPIO 20 Interrupt
	NX_GPIO_INT_GPIO21	= 21,	//	GPIO 21 Interrupt
	NX_GPIO_INT_GPIO22	= 22,	//	GPIO 22 Interrupt
	NX_GPIO_INT_GPIO23	= 23,	//	GPIO 23 Interrupt
	NX_GPIO_INT_GPIO24	= 24,	//	GPIO 24 Interrupt
	NX_GPIO_INT_GPIO25	= 25,	//	GPIO 25 Interrupt
	NX_GPIO_INT_GPIO26	= 26,	//	GPIO 26 Interrupt
	NX_GPIO_INT_GPIO27	= 27,	//	GPIO 27 Interrupt
	NX_GPIO_INT_GPIO28	= 28,	//	GPIO 28 Interrupt
	NX_GPIO_INT_GPIO29	= 29,	//	GPIO 29 Interrupt
	NX_GPIO_INT_GPIO30	= 30,	//	GPIO 30 Interrupt
	NX_GPIO_INT_GPIO31	= 31	//	GPIO 31 Interrupt
};

/// @brief	GPIO interrupt mode
typedef enum
{
	NX_GPIO_INTMODE_LOWLEVEL	= 0UL,	// Low level detect
	NX_GPIO_INTMODE_HIGHLEVEL	= 1UL,	// High level detect
	NX_GPIO_INTMODE_FALLINGEDGE	= 2UL,	// Falling edge detect
	NX_GPIO_INTMODE_RISINGEDGE	= 3UL,	// Rising edge detect
	NX_GPIO_INTMODE_BOTHEDGE	= 4UL	// both (rise and falling) edge detect
} NX_GPIO_INTMODE;

/// @brief	I/O mode
typedef enum
{
	NX_GPIO_PADFUNC_GPIO	= 0UL,		// GPIO mode
	NX_GPIO_PADFUNC_1	= 1UL,		// Alternate function 1
	NX_GPIO_PADFUNC_2	= 2UL,		// Alternate function 2
	NX_GPIO_PADFUNC_3	= 3UL,		// Alternate function 3
	NX_GPIO_PADFUNC_4	= 4UL,		// Alternate function 3
	NX_GPIO_PADFUNC_5	= 5UL,		// Alternate function 3
	NX_GPIO_PADFUNC_6	= 6UL,		// Alternate function 3
	NX_GPIO_PADFUNC_7	= 7UL		// Alternate function 3
} NX_GPIO_PADFUNC;

typedef enum
{
	NX_GPIO_DRVSTRENGTH_0	= 0UL,
	NX_GPIO_DRVSTRENGTH_1	= 1UL,
	NX_GPIO_DRVSTRENGTH_2	= 2UL,
	NX_GPIO_DRVSTRENGTH_3	= 3UL
} NX_GPIO_DRVSTRENGTH;

typedef enum
{
	NX_GPIO_PULL_DOWN	= 0UL,
	NX_GPIO_PULL_UP		= 1UL,
	NX_GPIO_PULL_OFF	= 2UL
} NX_GPIO_PULL;

void udelay(unsigned int utime);

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

void GPIOSetAltFunction(const struct nxpadi *pad, int setalt);
void GPIOSetDrvSt(const struct nxpadi *pad, NX_GPIO_DRVSTRENGTH str);
void GPIOSetPullup(const struct nxpadi *pad, NX_GPIO_PULL pull);
void GPIOReleasePAD(const struct nxpadi *pad);
void setpad(const union nxpad *const ppad, int num, int enable);

#endif //__NX_GPIO_H__
