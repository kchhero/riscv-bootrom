/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: alive
 * File		: nx_alive.h
 * Description	: alive register map
 * Author	: Hans
 * History	: 2018.01.09 First Implementation
 */

#ifndef __NX_ALIVE_H__
#define __NX_ALIVE_H__

struct REG_ACC {
	volatile unsigned int RST;
	volatile unsigned int SET;
	volatile unsigned int READ;
};
struct PAD_PWR {
	volatile unsigned int DOWN;
	volatile unsigned int OUTENB;
	volatile unsigned int OUT;
};
struct NX_ALIVE_RegisterSet			// 0xD000
{
	volatile unsigned int PWRGATE;			// 0x000
	struct REG_ACC GPIO_LL_ASYNC_DETECT;	// 0x004
	struct REG_ACC GPIO_HL_ASYNC_DETECT;	// 0x010
	struct REG_ACC GPIO_FE_DETECT;		// 0x01C
	struct REG_ACC GPIO_RE_DETECT;		// 0x028
	struct REG_ACC GPIO_LL_DETECT;		// 0x034
	struct REG_ACC GPIO_HL_DETECT;		// 0x040
	struct REG_ACC GPIO_DETECT_ENB;		// 0x04C
	struct REG_ACC GPIO_INT_ENB;		// 0x058
	volatile unsigned int GPIO_INT_PENDING;		// 0x064
	volatile unsigned int __Rev0[2];			// 0x068
	volatile unsigned int SCRATCH;			// 0x070
	struct REG_ACC GPIO_PAD_OUT_ENB;	// 0x074
	struct REG_ACC GPIO_PAD_PU_ENB;		// 0x080
	struct REG_ACC GPIO_PAD_OUT_VAL;	// 0x08C
	volatile unsigned int __Rev1[2];			// 0x098
	volatile unsigned int VDD_CTRL;			// 0x0A0
	volatile unsigned int WAKEUP_STATUS_CLR;		// 0x0A4
	volatile unsigned int WAKEUP_STATUS;		// 0x0A8
	struct REG_ACC SCRATCHx[8];		// 0x0AC - 0x108
	struct REG_ACC VDDOFF_DELAY;		// 0x10C
	volatile unsigned int __Rev2;			// 0x118
	volatile unsigned int GPIO_INPUT_VALUE;		// 0x11C
	volatile unsigned int __Rev3[2];			// 0x120
	volatile unsigned int NPADHOLD_ENB[3];		// 0x128
	volatile unsigned int NPADHOLD;			// 0x134
	struct REG_ACC PULL_SEL;		// 0x138
	struct REG_ACC DRV[2];			// 0x144
	struct REG_ACC INPUT_ENB;		// 0x15C
	volatile unsigned int __Rev4[38];		// 0x168
	struct PAD_PWR GPIO_PWR[7];		// 0x200 - 0x25C
};

struct NX_PBI_RegisterSet       // power block isolation
{
	volatile unsigned int NISOLATE;                  // 0xC400
	volatile unsigned int NPWRUPPRE;                 // 0xC404
	volatile unsigned int NPWRUP;                    // 0xC408
	volatile unsigned int NPWRUPACK;                 // 0xC40C
};

struct NX_PWR_RegisterSet
{
	volatile unsigned int VDD_OFF_DELAY;		// 0xC800
	volatile unsigned int VDD_OFF_START;		// 0xC804
	volatile unsigned int VDD_PWRON_XTI;		// 0xC808
	volatile unsigned int __Rev0;			// 0xC80C
	volatile unsigned int GPIO_ALTFN_L;		// 0xC810
	volatile unsigned int ALIVE_SW_RESET;		// 0xC814
	volatile unsigned int ALIVE_SW_RESET_ENB;	// 0xC818
	volatile unsigned int RESET_STATUS;		// 0xC81C
	volatile unsigned int VDD_PWRON_CNT;		// 0xC820
	volatile unsigned int RESET_CNT;			// 0xC824
	volatile unsigned int GPIO_ALTFN_H;		// 0xC828
	volatile unsigned int DISABLE_CPU_WFI;		// 0xC82C
	volatile unsigned int SLOW_OSC_ENB;		// 0xC830
	volatile unsigned int FORCE_SLOW_OSC_ENB;	// 0xC834
	volatile unsigned int OSC_STABLE_VAL;		// 0xC838
	volatile unsigned int ALIVE_RST_ENB;		// 0xC83C
	volatile unsigned int VDDOFF_REPWRON_DELAY;	// 0xC840
	volatile unsigned int CORE_SW_RST;		// 0xC844
	volatile unsigned int CORE_SW_RST_ENB;		// 0xC848
	volatile unsigned int CORE_WDT_RST_ENB;		// 0xC84C
	volatile unsigned int REPWRON_ENB;		// 0xC850
	volatile unsigned int CORE_RESET_STATUS;		// 0xC854
	volatile unsigned int PAD_STABLE;		// 0xC858
	volatile unsigned int NORM_DIV;			// 0xC85C
	volatile unsigned int STOP_DIV;			// 0xC860
	volatile unsigned int CLKEN_DEBUG;		// 0xC864
	volatile unsigned int CLKSEL_DONE;		// 0xC868
	volatile unsigned int RESET_CONFIG;		// 0xC86C
	volatile unsigned int __Rev1[36];		// 0xC870
	volatile unsigned int SCRATCH[32];		// 0xC900
};

#endif //__NX_ALIVE_H__
