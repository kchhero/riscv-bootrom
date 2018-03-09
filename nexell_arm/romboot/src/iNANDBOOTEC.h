//------------------------------------------------------------------------------
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// NEXELL Co. Proprietary & Confidential
//
// NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
// AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Module	: bl0
// File		: iNANDBOOT.h
// Description	: Bootloader for NAND flash memory with H/W BCH error correction.
// Author	: hans
// History	:
// 	2018-02-17	Hans	add
//------------------------------------------------------------------------------

#ifndef __INANDBOOTEC_H__
#define __INANDBOOTEC_H__

struct nandbootinfo {
	unsigned char *buf;		// read buffer
	unsigned int zeroimage_page;	// zero base nand offset
	unsigned int copyimage_page;	// if nand data cannot error correction
	unsigned int sector_ptr;	// error correct sector ptr
	unsigned short pagesize;	// nand page size
	unsigned short sectorsize;	// 512 or 1024
	unsigned short datasize;	// unit byte (sectorsize - eccsize)
	unsigned short eccsize;		// unit byte
	unsigned short remainbufsize;
	unsigned char state;
	unsigned char pagecnt;
	unsigned char sectorptr;
	unsigned char address_step;
	char nextretry;			// if none correctable error
	unsigned char imageblockcnt;
	unsigned char checkimagecnt;
};

#endif /* __INANDBOOTEC_H__ */
