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

#define HEADER_ID				\
		((((unsigned int)'N')<< 0) |	\
		 (((unsigned int)'S')<< 8) |	\
		 (((unsigned int)'I')<<16) |	\
		 (((unsigned int)'H')<<24))


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

struct asymmetrickey {
	unsigned char rsaencryptedsha256hash[2048/8];	/* 0x400 ~ 0x4ff */
	unsigned char rsapublicbootkey[2048/8];		/* 0x200 ~ 0x2ff */
	unsigned char rsapublicuserkey[2048/8];		/* 0x300 ~ 0x3ff */
};

struct nx_bootheader {
	struct nx_bootinfo bi;
	struct asymmetrickey rsa_public;
	unsigned int image[1];
};

struct nx_bootmm {
	struct nx_bootinfo bi;
//	unsigned int image[(65536 - 512 - 3 * 256) / 4];
	unsigned int image[(32768 - 512 - 3 * 256) / 4];
	struct asymmetrickey rsa_public;
};
#endif
