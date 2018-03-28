/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: bl0
 * File		: nx_sss.h
 * Description	:
 * Author	: Hans
 * History	: 2017.10.18	Hans first draft
 */


#ifndef __NX_SSS_H__
#define __NX_SSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nx_type.h"

/* Mailbox Hardware Register */
struct NX_MBRegisterSet {
	volatile U32 MB_STATUS;			/* 000 */
	volatile U32 CM0_HRESET;		/* 004 */
	volatile U32 CM0_SRAM_ACCESS_CONTROL;	/* 008 */
	volatile U32 _Rev0;			/* 00C */
	volatile U32 LOCKUP_SYSRSTREQ_CON;	/* 010 */
	volatile U32 CM0_FW_STATUS;		/* 014 */
	volatile U32 _Rev1[2];			/* 018 */
	volatile U32 MBINTSTAT;			/* 020 */
	volatile U32 MBINTENSET;		/* 024 */
	volatile U32 MBINTENCLR;		/* 028 */
	volatile U32 MBINTPEND;			/* 02C */
	volatile U32 CRYPTOE_STATUS;		/* 030 */
	volatile U32 CM0_DEBUG_CONTROL;		/* 034 */
	volatile U32 CM0_ANTI_RBCNT;		/* 038 */
	volatile U32 _Rev2;			/* 03C */
	volatile U32 CM0_SECURE_BOOT_KEY[16];	/* 040 */
	volatile U32 IDMA_MODE_SEL;		/* 080 */
	volatile U32 _Rev3[3];			/* 084 */
	volatile U32 IDMA_RX_ADDR;		/* 090 */
	volatile U32 IDMA_RX_LENGTH;		/* 094 */
	volatile U32 IDMA_RX_CONTROL;		/* 098 */
	volatile U32 IDMA_RX_STATUS;		/* 09C */
	volatile U32 IDMA_TX_ADDR;		/* 0A0 */
	volatile U32 IDMA_TX_LENGTH;		/* 0A4 */
	volatile U32 IDMA_TX_CONTROL;		/* 0A8 */
	volatile U32 IDMA_TX_STATUS;		/* 0AC */
	volatile U32 _Rev4[20];			/* 0B0 */
	volatile U32 MB_DATA[64];		/* 100 */
	volatile U8 _Rev10[0x400 - 128 * 4];
};

/* Key Manager Engine Hardware Register */
struct NX_KMRegisterSet {
	volatile U32 KM_COMMAND;		/* 000 */
	volatile U32 KM_STATUS;			/* 004 */
	volatile U32 KM_CTRL;			/* 008 */
	volatile U32 KM_KEY_INIT;		/* 00C */
	volatile U32 KM_INTSTAT;		/* 010 */
	volatile U32 KM_INTENSET;		/* 014 */
	volatile U32 KM_INTENCLR;		/* 018 */
	volatile U32 KM_INTENPEND;		/* 01C */
	volatile U32 _Rev0[24];			/* 020 */
	volatile U32 BK_DATA[16];		/* 080 */
	volatile U32 KDF_INPUT[15];		/* 100 */
	volatile U32 _Rev1;			/* 13C */
	volatile U32 KDF_INPUT_LENGTH;		/* 140 */
	volatile U32 _Rev2[47];			/* 144 */
	volatile U32 KM_BEK_CONTROL;		/* 200 */
	volatile U8 _Rev10[0x400 - 129 * 4];
};


/* Feeder Hardware Register */
struct NX_FEED {
	volatile U32 INTSTAT;		/* 000 */
	volatile U32 INTENSET;		/* 004 */
	volatile U32 INTENCLR;		/* 008 */
	volatile U32 INTPEND;		/* 00C */
	volatile U32 FIFOSTAT;		/* 010 */
	volatile U32 FIFOCTRL;		/* 014 */
	volatile U32 GLOBAL;		/* 018 */
	volatile U32 CM0_LP_CON;	/* 01C */
	volatile U32 BRDMAS;		/* 020 */
	volatile U32 BRDMAL;		/* 024 */
	volatile U32 BRDMAC;		/* 028 */
	volatile U32 BRDMASH;		/* 02C */
	volatile U32 BTDMAS;		/* 030 */
	volatile U32 BTDMAL;		/* 034 */
	volatile U32 BTDMAC;		/* 038 */
	volatile U32 BTDMASH;		/* 03C */
	volatile U32 HRDMAS;		/* 040 */
	volatile U32 HRDMAL;		/* 044 */
	volatile U32 HRDMAC;		/* 048 */
	volatile U32 HRDMASH;		/* 04C */
	volatile U32 BRDMASTAT;		/* 050 */
	volatile U32 BTDMASTAT;		/* 054 */
	volatile U32 HRDMASTAT;		/* 058 */
	volatile U32 BDRDMASTAT;	/* 05C */
	volatile U32 HDRDMASTAT;	/* 060 */
	volatile U32 _Rev0[3];		/* 064 */
	volatile U32 SSS_VERSION;	/* 070 */
	volatile U32 _Rev1[3];		/* 074 */
	volatile U32 BDRDMAS;		/* 080 */
	volatile U32 BDRDMAL;		/* 084 */
	volatile U32 BDRDMAC;		/* 088 */
	volatile U32 BDRDMASH;		/* 08C */
	volatile U32 HDRDMAS;		/* 090 */
	volatile U32 HDRDMAL;		/* 094 */
	volatile U32 HDRDMAC;		/* 098 */
	volatile U32 HDRDMASH;		/* 09C */
	volatile U32 DDMACTRL;		/* 0A0 */
	volatile U32 DDMASTATUS;	/* 0A4 */
	volatile U8 _Rev10[0x400 - 42 * 4];
};
struct NX_TDES {
	volatile U32 CONF;		/* 000 */
	volatile U32 STAT;		/* 004 */
	volatile U32 _Rev0[2];
	volatile U32 KEY[2][3];		/* 010 */
	volatile U32 IV[2];		/* 028 */
	volatile U32 INPUT[2];		/* 030 */
	volatile U32 OUTPUT[2];		/* 038 */
	volatile U8 _Rev10[0x400 - 16 * 4];
};

struct NX_RC4 {
	volatile U32 CONTROL;		/* 000 */
	volatile U32 BYTE_SWAP;		/* 004 */
	volatile U32 KEY_LEN;		/* 008 */
	volatile U32 STATUS;		/* 00C */
	volatile U32 MSG_SIZE_LOW;	/* 010 */
	volatile U32 MSG_SIZE_HIGH;	/* 014 */
	volatile U32 _Rev0[2];		/* 018 */
	volatile U32 DATA_IN;		/* 020 */
	volatile U32 DATA_OUT;		/* 024 */
	volatile U32 _Rev1[2];		/* 028 */
	volatile U32 KEY[4];		/* 030 */
	volatile U8 _Rev10[0x400 - 16 * 4];
};


struct NX_TRNG {
	volatile U32 CLKDIV;		/* 000 */
	volatile U32 _Rev0[3];		/* 004 */
	volatile U32 AEN;		/* 010 */
	volatile U32 _Rev1[3];		/* 014 */
	volatile U32 CTRL;		/* 020 */
	volatile U32 _Rev2[3];		/* 024 */
	volatile U32 POST_CTRL;		/* 030 */
	volatile U32 _Rev3[3];		/* 034 */
	volatile U32 TEST_CTRL;		/* 040 */
	volatile U32 TEST_STAT;		/* 044 */
	volatile U32 HT_CUTOFF;		/* 048 */
	volatile U32 STARTUP_CTRL;	/* 04C */
	volatile U32 FIFO_CTRL;		/* 050 */
	volatile U32 BT_THRESHOLD;	/* 054 */
	volatile U32 OT_THRESHOLD;	/* 058 */
	volatile U32 MB_THRESHOLD;	/* 05C */
	volatile U32 TEST_DONE;		/* 060 */
	volatile U32 _Rev4[7];		/* 064 */
	volatile U32 FIFO[8];		/* 080 */
	volatile U8 _Rev10[0x400 - 40 * 4];
};

struct NX_PKA {
	volatile U32 TCR[6];			/* 000 */
	volatile U8 _Rev10[0x400 - 6 * 4];
};

struct NX_WDT {
	volatile U32 WT_CON;			/* 000 */
	volatile U32 WT_DAT;			/* 004 */
	volatile U32 WT_CNT;			/* 008 */
	volatile U32 WT_CLRINT;			/* 00C */
	volatile U8 _Rev10[0x200 - 4 * 4];
};

struct NX_SSS_RegisterSet {
	struct NX_FEED feeder;			/* 0x0000 Feeder */
	struct NX_TDES tdes;			/* 0x0800 TDES */
	struct NX_RC4 arc4;			/* 0x0C00 ARC4 */
	struct NX_TRNG trng;			/* 0x1400 TRNG */
	struct NX_WDT wdt[2];			/* 0x1800 Secure WDT */
	U8 _Rev0[0x4000 - 0x1800 - 0x0400];
	struct NX_PKA pka;			/* 0x4000 PKA */
	volatile U8 NX_PKASRAM[2880];		/* 0x4400 2880 bytes */
	U8 _Rev1[0x8000 - 0x4400 - 2880];
	union {
		volatile U8 bNX_CM0SRAM[32768];	/* 0x8000 32KB */
		volatile U32 wNX_CM0SRAM[32768 / 4];
	};
	volatile U8 NX_SRAMDATA[4];		/* 0x10000 */
};

struct NX_SKM_RegisterSet {
	volatile unsigned int CTRL;
	volatile unsigned int MODE;
};

struct NX_BRTDMADESC {
	U32 BRDMAC;	// 00 control of block cipher receiving DMA
	U32 BRDMASH;	// 04 start address high
	U32 BRDMAS;	// 08 start address low
	U32 BRDMAL;	// 0C length
	U32 BTDMAC;	// 10 control fo block ciper transmitting DMA
	U32 BTDMASH;	// 14 start address high
	U32 BTDMAS;	// 18 start address low
	U32 BTDMAL;	// 1C length
};

struct NX_HRDMADESC {
	U32 HRDMAC;	// 00 control of hash receiving DMA
	U32 HRDMASH;	// 04 start address high
	U32 HRDMAS;	// 08 start address low
	U32 HRDMAL;	// 0C length
};

#define CCM_QLEN_2B		(1 << 16)
#define CCM_QLEN_3B		(2 << 16)
#define CCM_QLEN_4B		(3 << 16)
#define CCM_QLEN_5B		(4 << 16)
#define CCM_QLEN_6B		(5 << 16)
#define CCM_QLEN_7B		(6 << 16)
#define CCM_QLEN_8B		(7 << 16)
#define AES_CCM_SIZE_T		(7 << 19)
#define CCM_TAG_4B		(1 << 19)
#define CCM_TAG_6B		(2 << 19)
#define CCM_TAG_8B		(3 << 19)
#define CCM_TAG_10B		(4 << 19)
#define CCM_TAG_12B		(5 << 19)
#define CCM_TAG_14B		(6 << 19)
#define CCM_TAG_16B		(7 << 19)

#define FCBYTESWAP	(1 << 1)
#define BURST_1		(0 << 14)
#define BURST_2		(1 << 14)
#define BURST_4		(2 << 14)
#define BURST_8		(3 << 14)
#define BURST_16	(4 << 14)

#define WAKEUP_INT		(1 << 12)
#define WFI_INT			(1 << 11)
#define BRDMAINT		(1 << 3)
#define BTDMAINT		(1 << 2)
#define HRDMAINT		(1 << 1)
#define PKDMAINT		(1 << 0)

#define BDDMA_DONE		(1 << 16)
#define BDRDMA_DONE		(1 << 15)
#define HDDMA_DONE		(1 << 14)
#define HDRDMA_DONE		(1 << 13)

#define CMAC_DONE		(1 << 10)
#define BTEDMA_DONE		(1 << 9)
#define RNG_ERROR		(1 << 8)
#define PARTIAL_DONE		(1 << 7)
#define PRNG_DONE		(1 << 6)
#define MSG_DONE 		(1 << 5)
#define PRNG_ERROR		(1 << 4)
#define BRDMA_DONE		(1 << 3)
#define BTDMA_DONE		(1 << 2)
#define HRDMA_DONE		(1 << 1)
#define PKA_DONE		(1 << 0)
#ifdef cplusplus
}
#endif

#endif	//__NX_SSS_H__
