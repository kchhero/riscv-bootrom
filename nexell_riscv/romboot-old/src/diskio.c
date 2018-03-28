/*==============================================================================
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	: bl0
// File		: diskio.c
// Description	:
// Author	: Hans
// History	:
// 		2014.01.12 First implementation
==============================================================================*/

#include "include/nx_swallow.h"
#include "include/nx_swallow_bootoption.h"
#include "include/nx_type.h"
#include "diskio.h"

#include "iSDHCBOOT.h"

//#include "libarm.h"
#include "fnptr.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define MMC		0
#define USB		1
#define ATA		2



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	U8 drv				/* Physical drive nmuber (0..) */
)
{
//	DSTATUS stat = RES_OK;
//	int result;

	switch (drv) {
	case MMC :
//		result = MMC_disk_initialize();
		// translate the reslut code here

		return RES_OK;//stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	U8 drv		/* Physical drive nmuber (0..) */
)
{
//	DSTATUS stat;
//	int result;

	switch (drv) {
	case MMC :
//		result = MMC_disk_status();
		// translate the reslut code here

		return RES_OK;//stat;
	}
	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	U8 drv,			/* Physical drive nmuber (0..) */
	U8 *buff,		/* Data buffer to store read data */
	U32 sector,		/* Sector address (LBA) */
	U8 count,		/* Number of sectors to read (1..255) */
	U32 *diskhandle)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	CBOOL	Result;
	SDXCBOOTSTATUS *pSDXCBootStatus = (SDXCBOOTSTATUS *)diskhandle;

	switch (drv) {
	case MMC:
		Result = pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, sector,
							count, (U32*)buff);
		if (Result)
			return RES_OK;
	}
	return RES_PARERR;
}

