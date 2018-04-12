/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module	: bl0
 * File		: nx_antares_bootheader.h
 * Description	: This must be synchronized with NSIH.txt
 * Author	: hans
 * History	: 2017.10.09 create
 */

#ifndef __NX_BOOTHEADER_H__
#define __NX_BOOTHEADER_H__

struct nx_bootinfo {
	unsigned int vector[8];				/* 0x000 ~ 0x01f */
	unsigned int vector_rel[8];			/* 0x020 ~ 0x03f */

	unsigned int LoadSize;				/* 0x040 */
	unsigned int CRC32;				/* 0x044 */
	unsigned int LoadAddr;				/* 0x048 */
	unsigned int StartAddr;				/* 0x04C */

	unsigned char _reserved3[512 - 4 * 22];		/* 0x050 ~ 0x1f7 */

	/* version */
	unsigned int buildinfo;				/* 0x1f8 */

	/* "NSIH": nexell system infomation header */
	unsigned int signature;				/* 0x1fc */
} __attribute__ ((packed, aligned(16)));

/* struct nx_bootheader { */
/* 	struct nx_bootinfo bi; */
/* 	unsigned int image[1]; */
/* }; */

struct nx_bootmm {
	struct nx_bootinfo bi;
	unsigned int image[(32768 - 512 - 3 * 256) / 4];
};

/* ========================= rst cfg =====================*/
#define BOOTMODE		0	// 1, 2

#define USBBOOT			11
#define SDBOOT			22	// data sector boot
#define XIP			33


#define PORTNUMBER		5

#define SERIALFLASHBUS		3	// 0: 1 bit  1: 4 bit
#define SERIALFLASHSPEED	4	// 0: 16MHz, 1: 1MHz

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

#endif
