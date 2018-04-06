/*
 * Copyright (C) 2018 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module	: SDMMC
 * File		: nx_sdmmc.h
 * Description  :
 * ReferenceDoc : DesignWare Cores Mobile Storage Host Controller Databook, Version 1.60a
 * Author	: S/W A, suker
 * History	:
 */
#ifndef __NX_SDMMC_H__
#define __NX_SDMMC_H__

#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct  NX_SDMMC_RegisterSet_2_9
{
	volatile unsigned int CTRL;			// 0x000 : Control
	volatile unsigned int PWREN;			// 0x004 : Power Enable
	volatile unsigned int CLKDIV;			// 0x008 : Clock Divider
	volatile unsigned int CLKSRC;			// 0x00C : Clock Source
	volatile unsigned int CLKENA;			// 0x010 : Clock Enable
	volatile unsigned int TMOUT;			// 0x014 : Time-Out
	volatile unsigned int CTYPE;			// 0x018 : Card Type
	volatile unsigned int BLKSIZ;			// 0x01C : Block Size
	volatile unsigned int BYTCNT;			// 0x020 : Byte Count
	volatile unsigned int INTMASK;			// 0x024 : Interrupt Mask
	volatile unsigned int CMDARG;			// 0x028 : Command Argument
	volatile unsigned int CMD;			// 0x02C : Command
	volatile unsigned int RESP[4];			// 0x030 : Response 0
	volatile unsigned int MINTSTS;			// 0x040 : Masked Interrupt Status
	volatile unsigned int RINTSTS;			// 0x044 : Raw Interrupt Status
	volatile unsigned int STATUS;			// 0x048 : Status - Mainly for Debug Purpose
	volatile unsigned int FIFOTH;			// 0x04C : FIFO Threshold
	volatile unsigned int CDETECT;			// 0x050 : Card Detect
	volatile unsigned int WRTPRT;			// 0x054 : Write Protect
	volatile unsigned int GPIO;			// 0x058 : General Purpose Input/Output
	volatile unsigned int TCBCNT;			// 0x05C : Transferred CIU card byte count
	volatile unsigned int TBBCNT;			// 0x060 : Transferred Host Byte Count
	volatile unsigned int DEBNCE;			// 0x064 : Card Detect Debounce
	volatile unsigned int USRID;			// 0x068 : User ID
	volatile unsigned int VERID;			// 0x06C : Version ID
	volatile unsigned int HCON;			// 0x070 : Hardware Configuration
	volatile unsigned int UHS_REG;			// 0x074 : UHS_REG register
	volatile unsigned int RSTn;			// 0x078 : Hardware reset register
	volatile unsigned int _Rev0;			// 0x07C
	volatile unsigned int BMOD;			// 0x080 : Bus Mode
	volatile unsigned int PLDMND;			// 0x084 : Poll Demand
	volatile unsigned int DBADDR;			// 0x088 : Descriptor List Base Address
	volatile unsigned int IDSTS;			// 0x08C : Internal DMAC Status
	volatile unsigned int IDINTEN;			// 0x090 : Internal DMAC Interrupt Enable
	volatile unsigned int DSCADDR;			// 0x094 : Current Host Descriptor Address
	volatile unsigned int BUFADDR;			// 0x098 : Current Buffer Descriptor Address
	volatile unsigned char  _Rev1[0x100-0x09C];	// 0x09C ~ 0x100 reserved area
	volatile unsigned int CARDTHRCTL;		// 0x100 : Card Read Threshold Enable
	volatile unsigned int BACKEND_POWER;		// 0x104 : Back-end Power
	volatile unsigned int UHS_REG_EXT;		// 0x108 : eMMC 4.5 1.2V
	volatile unsigned int EMMC_DDR_REG;		// 0x10C : eMMC DDR START bit detection control
	volatile unsigned int ENABLE_SHIFT;		// 0x110 : Phase shift control
	volatile unsigned int CLKCTRL;			// 0x114 : External clock phase & delay control
	volatile unsigned char  _Rev2[0x200-0x118];	// 0x118 ~ 0x200
	volatile unsigned int DATA;			// 0x200 : Data
	volatile unsigned char  _Rev3[0x400-0x204];	// 0x204 ~ 0x400
	volatile unsigned int TIEMODE;			// 0x400
	volatile unsigned int TIESRAM;			// 0x404
	volatile unsigned int TIEDRVPHASE;		// 0x408
	volatile unsigned int TIESMPPHASE;		// 0x40C
	volatile unsigned int TIEDSDELAY;		// 0x410
#ifdef QEMU_RISCV
        volatile unsigned int sd_body_size;   
        volatile unsigned char sd_body[505];
#endif
} NX_SDMMC_RegisterSet;


/// @brief  SDMMC clock phase shift
typedef enum
{
	NX_SDMMC_CLOCK_SHIFT_0          = 0,
	NX_SDMMC_CLOCK_SHIFT_90         = 1,
	NX_SDMMC_CLOCK_SHIFT_180        = 2,
	NX_SDMMC_CLOCK_SHIFT_270        = 3
} NX_SDMMC_CLKSHIFT;

#ifdef  __cplusplus
}
#endif

#endif  // __NX_SDMMC_H__

