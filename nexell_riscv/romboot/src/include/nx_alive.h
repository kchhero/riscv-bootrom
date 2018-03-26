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

#include "nx_type.h"

struct REG_ACC {
	volatile U32 RST;
	volatile U32 SET;
	volatile U32 READ;
};
struct PAD_PWR {
	volatile U32 DOWN;
	volatile U32 OUTENB;
	volatile U32 OUT;
};
struct NX_ALIVE_RegisterSet			// 0xD000
{
	volatile U32 PWRGATE;			// 0x000
	struct REG_ACC GPIO_LL_ASYNC_DETECT;	// 0x004
	struct REG_ACC GPIO_HL_ASYNC_DETECT;	// 0x010
	struct REG_ACC GPIO_FE_DETECT;		// 0x01C
	struct REG_ACC GPIO_RE_DETECT;		// 0x028
	struct REG_ACC GPIO_LL_DETECT;		// 0x034
	struct REG_ACC GPIO_HL_DETECT;		// 0x040
	struct REG_ACC GPIO_DETECT_ENB;		// 0x04C
	struct REG_ACC GPIO_INT_ENB;		// 0x058
	volatile U32 GPIO_INT_PENDING;		// 0x064
	volatile U32 __Rev0[2];			// 0x068
	volatile U32 SCRATCH;			// 0x070
	struct REG_ACC GPIO_PAD_OUT_ENB;	// 0x074
	struct REG_ACC GPIO_PAD_PU_ENB;		// 0x080
	struct REG_ACC GPIO_PAD_OUT_VAL;	// 0x08C
	volatile U32 __Rev1[2];			// 0x098
	volatile U32 VDD_CTRL;			// 0x0A0
	volatile U32 WAKEUP_STATUS_CLR;		// 0x0A4
	volatile U32 WAKEUP_STATUS;		// 0x0A8
	struct REG_ACC SCRATCHx[8];		// 0x0AC - 0x108
	struct REG_ACC VDDOFF_DELAY;		// 0x10C
	volatile U32 __Rev2;			// 0x118
	volatile U32 GPIO_INPUT_VALUE;		// 0x11C
	volatile U32 __Rev3[2];			// 0x120
	volatile U32 NPADHOLD_ENB[3];		// 0x128
	volatile U32 NPADHOLD;			// 0x134
	struct REG_ACC PULL_SEL;		// 0x138
	struct REG_ACC DRV[2];			// 0x144
	struct REG_ACC INPUT_ENB;		// 0x15C
	volatile U32 __Rev4[38];		// 0x168
	struct PAD_PWR GPIO_PWR[7];		// 0x200 - 0x25C
};

struct NX_PBI_RegisterSet       // power block isolation
{
	volatile U32 NISOLATE;                  // 0xC400
	volatile U32 NPWRUPPRE;                 // 0xC404
	volatile U32 NPWRUP;                    // 0xC408
	volatile U32 NPWRUPACK;                 // 0xC40C
};

struct NX_PWR_RegisterSet
{
	volatile U32 VDD_OFF_DELAY;		// 0xC800
	volatile U32 VDD_OFF_START;		// 0xC804
	volatile U32 VDD_PWRON_XTI;		// 0xC808
	volatile U32 __Rev0;			// 0xC80C
	volatile U32 GPIO_ALTFN_L;		// 0xC810
	volatile U32 ALIVE_SW_RESET;		// 0xC814
	volatile U32 ALIVE_SW_RESET_ENB;	// 0xC818
	volatile U32 RESET_STATUS;		// 0xC81C
	volatile U32 VDD_PWRON_CNT;		// 0xC820
	volatile U32 RESET_CNT;			// 0xC824
	volatile U32 GPIO_ALTFN_H;		// 0xC828
	volatile U32 DISABLE_CPU_WFI;		// 0xC82C
	volatile U32 SLOW_OSC_ENB;		// 0xC830
	volatile U32 FORCE_SLOW_OSC_ENB;	// 0xC834
	volatile U32 OSC_STABLE_VAL;		// 0xC838
	volatile U32 ALIVE_RST_ENB;		// 0xC83C
	volatile U32 VDDOFF_REPWRON_DELAY;	// 0xC840
	volatile U32 CORE_SW_RST;		// 0xC844
	volatile U32 CORE_SW_RST_ENB;		// 0xC848
	volatile U32 CORE_WDT_RST_ENB;		// 0xC84C
	volatile U32 REPWRON_ENB;		// 0xC850
	volatile U32 CORE_RESET_STATUS;		// 0xC854
	volatile U32 PAD_STABLE;		// 0xC858
	volatile U32 NORM_DIV;			// 0xC85C
	volatile U32 STOP_DIV;			// 0xC860
	volatile U32 CLKEN_DEBUG;		// 0xC864
	volatile U32 CLKSEL_DONE;		// 0xC868
	volatile U32 RESET_CONFIG;		// 0xC86C
	volatile U32 __Rev1[36];		// 0xC870
	volatile U32 SCRATCH[32];		// 0xC900
};

#endif //__NX_ALIVE_H__
