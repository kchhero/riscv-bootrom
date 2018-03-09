//;-----------------------------------------------------------------------------
//;
//; Copyright (C) 2009 Nexell Co., All Rights Reserved
//; Nexell Co. Proprietary & Confidential
//;
//; NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//; AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//; BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//; FOR A PARTICULAR PURPOSE.
//;
//; Module	: bl0
//; File	: nx_antares_bootoption.h
//; Description	: core define
//; Author	: Hans
//; History	:
//;	2017-09-25	Hans, for NXP3220
//;-----------------------------------------------------------------------------

#ifndef NX_BOOTOPTION_H
#define NX_BOOTOPTION_H


/* ========================= rst cfg =====================*/
#define BOOTMODE		0	// 1, 2

#define USBBOOT			1
#define SDBOOT			2	// data sector boot
#define XIP			3


#define PORTNUMBER		5

#define SERIALFLASHBUS		3	// 0: 1 bit  1: 4 bit
#define SERIALFLASHSPEED	4	// 0: 16MHz, 1: 1MHz
#define SELSPIPORT		5 // 6	// 0, 1, 2

#define eMMCBOOTMODE		3	// 0: cmd line low, 1: boot cmd(0xFFFFFFFA)(ALT)
#define eMMCBUSWIDTH		4	// 0: 4 bit, 1: 8 bit
#define SELSDPORT		5 // 6	// 0, 1, 2: port number, 3: hispeed p 0

#define NANDTYPE		3	// 0: large, 1: small
#define NANDADDRSTEP		4	// small: 1:3, 0:4, large: 0:5, 1:4
#define NANDPAGE		5 // 6	// 0: 2k, 1:4K, 2:8K, 3:16K ~

#define UARTBAUDRATE		3	// 0: 115200, 1:921600
#define UARTPORT		4 // 5, 6	// 0 ~ 6 port

#define ICACHE			7	// rstcfg only. 0: disable, 1: enable
#define EXNOBOOTMSG		8	// 0: no message, 1:uart0 message output
/*
 * EXNOBOOTMSG	NOBOOTMSG
 *	0	    0		uart out
 *	0	    1		memory save
 *	1	    0		memory save
 *	1	    1		no message out
 */

/* ========================= efuse =====================*/

#define NEXTTRYPORT		7 // 8	// 0, 1, 2: port number, 3: not try

#define NOBOOTMSG		10	// 0: boot message, 1: no boot message

#define VALIDFIELD		11	// 0: valid, 1: invalid
#define BOOTCFGUSE		12	// 0: not use, 1: use

#define BOOTHALT		13	// 1: boot stop
#define EXNOBOOTMSG_SAVE	13	// saved external boot mode pin status

#define VERIFY			11 // 12 // 0: 0, 1: 1, 2:2, 3: no verify

#define SPEEDUP			14	// system speed up

#endif
