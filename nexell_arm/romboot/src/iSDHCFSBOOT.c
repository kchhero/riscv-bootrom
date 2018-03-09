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
// File		: iSDHCFSBOOT.c
// Description	:
// Author	: Hans
// History	: 2013.02.06 First implementation
//		2017.10.09 Hans modify for nxp3220
==============================================================================*/
#include "nx_antares.h"
#include "nx_antares_bootheader.h"
#include "nx_antares_bootoption.h"
#include <nx_type.h>
#include "printf.h"
#include "libarm.h"

#ifdef NXP4330
#include <nx_chip.h>
#endif

#ifdef NXP3220
#include <nx_chip_sfr.h>
#include "crypto.h"
#endif

#include <fatfs.h>

#include <iSDHCBOOT.h>
#include "fnptr.h"


extern struct NX_CLKGEN_RegisterSet * const pgSDClkGenReg[3];
extern struct NX_SDMMC_RegisterSet * const pgSDXCReg[3];

CBOOL FSBoot(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL Ret = CFALSE;
	FATFS FATFS;	/* Pointer to the file system objects (logical drives) */
	const char *diskname = "0:";

//	pbl0fn->printf("mount to disk 0\r\n");
	FATFS.fs_type = 0;
	FATFS.id = 0;
	FATFS.diskhandle = (U32*)pSDXCBootStatus;

	if (FR_OK != pbl0fn->f_mount(&diskname, &FATFS, 0)) {
		pbl0fn->printf("mount fail\r\n");
		return Ret;
	}

	const char *filename = "nxboot.img";
	FIL lfile;

	if (FR_OK != pbl0fn->f_open(&lfile, filename, FA_READ, &FATFS)) {
		pbl0fn->printf("%s file open fail\r\n", filename);
		return Ret;
	}

	U32 RSize;
	if (FR_OK != pbl0fn->f_read(&lfile, (void*)&(*pbl0fn->pbm)->bi,
				sizeof(struct nx_bootinfo), &RSize)) {
		pbl0fn->printf("file read fail(%d)\r\n", RSize);
		goto errexitl;
	}

	U32 iv[4];
	iv[0] = (*pbl0fn->iv)[0];
	iv[1] = (*pbl0fn->iv)[1];
	iv[2] = (*pbl0fn->iv)[2];
	iv[3] = (*pbl0fn->iv)[3];
#if 1
	if (!pbl0fn->headercheck(iv, option & 1 << DECRYPT)) {
		goto errexitl;
	}
#else
	struct nx_bootinfo *pbi = (struct nx_bootinfo *)(*pbl0fn->pbm);
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->bi,
				(U32*)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));
	}

	if (pbi->signature != HEADER_ID) {
		pbl0fn->printf("not bootable image(%04X)\r\n", pbi->signature);
		goto errexitl;
	}
#endif

	U32 BootSize = lfile.fsize - sizeof(struct asymmetrickey);
	if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK)
		BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;

	if (FR_OK != pbl0fn->f_read(&lfile, (void*)&(*pbl0fn->pbm)->rsa_public,
				sizeof(struct asymmetrickey), &RSize)) {
		pbl0fn->printf("key read fail(%d)\r\n", RSize);
		goto errexitl;
	}
	/*===========================================================
	 * boot key certify
	 * ========================================================*/
#if 1
	if (!pbl0fn->keycheck(iv, option & 1 << DECRYPT, (option >> VERIFY) & 0x3)) {
		goto errexitl;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->rsa_public,
				(U32*)&(*pbl0fn->pbm)->rsa_public,
				iv, sizeof(struct asymmetrickey));
	}

	U32 ehash = (option >> VERIFY) & 0x3;
	if (ehash) {
		U32 bhash[32 / 4];
		pbl0fn->mbedtls_sha256((*pbl0fn->pbm)->rsa_public.rsapublicbootkey,
			256, (unsigned char *)bhash, 0);
		if (ehash == 1)
			ehash--;
		const U32 *pbootkeyhash =
			&(*(*pbl0fn->pbootkeyhash))[ehash][0];
		if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) != 0) {
			pbl0fn->printf("boot key verify fail.\r\n");
			goto errexitl;
		}
	}
#endif

	if (FR_OK != pbl0fn->f_read(&lfile, (void*)&(*pbl0fn->pbm)->image,
				(*pbl0fn->pbm)->bi.LoadSize, &RSize)) {
		pbl0fn->printf("boot read fail(%d)\r\n", RSize);
		goto errexitl;
	}
	/*===========================================================
	 * boot image decrypt if requared.
	 * ========================================================*/
	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((U32*)(*pbl0fn->pbm)->image,
				(U32*)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}

	Ret = CTRUE;

errexitl:
	f_close(&lfile);

	return Ret;
}

//------------------------------------------------------------------------------
CBOOL SDMMCFSBOOT(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL result = CFALSE;
	struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];

	if (CTRUE == pbl0fn->NX_SDMMC_Open(pSDXCBootStatus, option)) {
		if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
			volatile U32 tempcount = 0x100000;
			pbl0fn->printf("FIFO Reset!!!\n");
			pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;	/* Reset the FIFO. */
			/* Wait until the FIFO reset is completed. */
			while ((pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST) && tempcount--)
				pSDXCReg->CTRL;
		}

		result = pbl0fn->FSBoot(pSDXCBootStatus, option);
	}
	pbl0fn->NX_SDMMC_Close(pSDXCBootStatus);

	return result;
}

//------------------------------------------------------------------------------
U32 iSDXCFSBOOT(U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL	result = CFALSE;
	SDXCBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
	pSDXCBootStatus = &SDXCBootStatus;

	pSDXCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

	if (pSDXCBootStatus->SDPort >= 3) {
		pSDXCBootStatus->SDPort = 0;
		pSDXCBootStatus->bHighSpeed = CTRUE;
	} else
		pSDXCBootStatus->bHighSpeed = CFALSE;

	pbl0fn->printf("SD FS:%d\r\n", pSDXCBootStatus->SDPort);

	pbl0fn->NX_SDPADSetALT(pSDXCBootStatus->SDPort);

	/*--------------------------------------------------------------------*/
	/* Normal SD(eSD)/MMC ver 4.2 boot */

	pbl0fn->NX_SDMMC_Init(pSDXCBootStatus);
	result = pbl0fn->SDMMCFSBOOT(pSDXCBootStatus, option);
	pbl0fn->NX_SDMMC_Terminate(pSDXCBootStatus);

	pbl0fn->NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);

	return result;
}
