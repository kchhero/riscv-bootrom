/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: ecid
 * File		: nx_ecid.h
 * Description	: ecid register map
 * Author	: Hans
 * History	: 2018.01.02 First Implementation
 */

#ifndef __NX_ECID_H__
#define __NX_ECID_H__

struct NX_ECID_RegisterSet {
	volatile unsigned int CHIP_NAME[12];	// 0x000
	volatile unsigned int _Rev0;		// 0x030
	volatile unsigned int GUID[4];		// 0x034
	volatile unsigned int EC0;		// 0x044
	volatile unsigned int _Rev1;		// 0x048
	volatile unsigned int EC2;		// 0x04C
	volatile unsigned int BLOW[3];		// 0x050
	volatile unsigned int _Rev2;		// 0x05C
	volatile unsigned int BLOWD[4];		// 0x060
	volatile unsigned int _Rev3[36];	// 0x070
	volatile unsigned int ECID[4];		// 0x100
	volatile unsigned int SBOOTKEY0[4];	// 0x110
	volatile unsigned int SBOOTKEY1[4];	// 0x120
	volatile unsigned int _Rev4[8];		// 0x130
	volatile unsigned int SBOOTHASH0[8];	// 0x150
	volatile unsigned int _Rev5[8];		// 0x170
	volatile unsigned int SBOOTHASH1[8];	// 0x190
	volatile unsigned int SBOOTHASH2[8];	// 0x1B0
	volatile unsigned int SJTAG[4];		// 0x1D0
	volatile unsigned int ANTI_RB[4];	// 0x1E0
	volatile unsigned int EFUSE_CFG;	// 0x1F0
	volatile unsigned int EFUSE_PROT;	// 0x1F4
	volatile unsigned int _Rev6[2];		// 0x1F8
	volatile unsigned int BOOT_CFG;		// 0x200
	volatile unsigned int _Rev7[3];		// 0x204
	volatile unsigned int BACK_ENC_EK[8];	// 0x210
	volatile unsigned int ROOT_ENC_KEY[8];	// 0x230
	volatile unsigned int CM0_SBOOT_KEY[16];// 0x250
	volatile unsigned int ROOT_PRIV_KEY[17];// 0x290
	volatile unsigned int _Rev8[11];	// 0x2D4
	volatile unsigned int PUF[136];		// 0x300
	volatile unsigned int PUF_CFG;		// 0x520
	volatile unsigned int CM0_ANTI_RB;	// 0x524
	volatile unsigned int CM0_ANTI_RB_CFG;	// 0x528
	volatile unsigned int _Rev9;		// 0x52C
	volatile unsigned int HPM_IDS[4];	// 0x530
};

#endif //__NX_ECID_H__
