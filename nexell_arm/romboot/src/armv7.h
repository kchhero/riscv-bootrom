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
 * File		: armv7.h
 * Description	:
 * Author	: Hans
 * History	: 2017.10.09	Hans first draft
 */

#define BIT1_XP	(1 << 23)	//; Subpage AP
#define BIT1_U	(1 << 22)	//; Enables unaligned data access
#define BIT1_L4	(1 << 15)	//; for ARM4V architecture
#define BIT1_RR	(1 << 14)	//; Replacement strategy for ICache & DCache
#define BIT1_V	(1 << 13)	//; Loaction of exception vectors
#define BIT1_I	(1 << 12)	//; ICache enable/disable
#define BIT1_Z	(1 << 11)	//; Branch prediction
#define BIT1_R	(1 <<  9)	//; ROM protection
#define BIT1_S	(1 <<  8)	//; system protection
#define BIT1_B	(1 <<  7)	//; Endianness(Bin/Little : 1/0)
#define BIT1_C	(1 <<  2)	//; DCache enable/disable
#define BIT1_A	(1 <<  1)	//; Alignment fault enable/disable
#define BIT1_M	(1 <<  0)	//; MMU enable/disable

#define Mode_USR	0x10
#define Mode_FIQ	0x11
#define Mode_IRQ	0x12
#define Mode_SVC	0x13
#define Mode_MON	0x16
#define Mode_ABT	0x17
#define Mode_HYP	0x1A
#define Mode_UNDEF	0x1B
#define Mode_SYS	0x1F

#define A_Bit		(1 << 8)	//; when A bit is set, Abort is disabled
#define I_Bit		(1 << 7)	//; when I bit is set, IRQ is disabled
#define F_Bit		(1 << 6)	//; when F bit is set, FIQ is disabled

