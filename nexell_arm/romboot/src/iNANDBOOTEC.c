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
// File		: iNANDBOOT.c
// Description	: Bootloader for NAND flash memory with H/W BCH error correction.
// Author	: hans
// History	:
// 	2017-10-09	Hans	all change for new nand control ip
//------------------------------------------------------------------------------

//#undef NX_DEBUG

#include "nx_antares.h"
#include "nx_antares_bootoption.h"
#include "nx_antares_bootheader.h"
#include <nx_type.h>
#include "printf.h"

#ifdef NXP4330
#include <nx_chip.h>
#include <nx_mcus.h>
#endif

#ifdef NXP3220
#include <nx_chip_iomux.h>
#include <nx_chip_sfr.h>
#include <nx_cmu.h>
#include <nx_nand.h>
#endif

#include <nx_gpio.h>
#include "libarm.h"
#include "libplat.h"
#include "crypto.h"
#include "fnptr.h"
#include "iNANDBOOTEC.h"

#define ROW_START			(0)

//------------------------------------------------------------------------------
// NAND Command
#define NAND_CMD_READ_1ST		(0x00)
#define NAND_CMD_READ_2ND		(0x30)
#define NAND_CMD_RESET			(0xFF)


#ifdef NXP3220
#define MAXIMAGESIZE 0x10000

struct NX_NANDC_RegisterSet * const pnc =
	(struct NX_NANDC_RegisterSet *)PHY_BASEADDR_NANDC_MODULE;

const union nxpad nandpad[14] = {
	{PI_NANDC_SD_0_},
	{PI_NANDC_SD_1_},
	{PI_NANDC_SD_2_},
	{PI_NANDC_SD_3_},
	{PI_NANDC_SD_4_},
	{PI_NANDC_SD_5_},
	{PI_NANDC_SD_6_},
	{PI_NANDC_SD_7_},
	{PI_NANDC_nOE},
	{PI_NANDC_nWE},
	{PI_NANDC_nCS_0_},
	{PI_NANDC_ALE},
	{PI_NANDC_CLE},
	{PI_NANDC_RnB}
};

/* pll1 100MHz, divider bypass */
const struct cmu_device_clk nandclk[2] = {
	{0x6e00, 22 + 32, 0, NX_CLKSRC_PLL1, 1},// axi
	{0x0400, 1, 13, NX_CLKSRC_PLL1, 1}	// apb
};

void nandcontroller_init(struct nandbootinfo *pni, unsigned int bootoption)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	unsigned int rvalue;
	unsigned char pagesize, pagecnt;
	const unsigned char ecn[2] = {24, 60};
	const unsigned char eccsize[2] = {39, 105};
//	const unsigned char eccconf[2] = {4, 7};
	const unsigned char bchmode[2] = {NX_NANDC_BCH_512_24, NX_NANDC_BCH_1024_60};

	pni->address_step = 1 - ((bootoption >> 3) & 1);

	if (bootoption  & 1 << 4) {	// 1024
		pagesize = 0;		// small block(512B)
		pni->datasize = 512 - (13 * 24 + (8 - 1)) / 8 - 1;
		pagecnt = 1;
	} else {
		pagesize = 1;		// large block(1024B ~)
		pni->datasize = 1024 - (14 * 60 + (8 - 1)) / 8 - 1;
		// 2K, 4K, 8K, 16K
		pagecnt = (unsigned char)(2 << ((bootoption >> (3 + 2)) & 0x3));
	}
	pbl0fn->printf("data size: %d\n", pni->datasize);
	pni->pagesize = (512 << pagesize) * pagecnt;
	pbl0fn->printf("pagesize:%x\n", pni->pagesize);
	pni->sectorsize = 512 << pagesize;
	pbl0fn->printf("sectorsize:%x\n", pni->sectorsize);
	pni->eccsize = eccsize[pagesize];
	pbl0fn->printf("eccsize:%x\n", pni->eccsize);
	pni->pagecnt = pagecnt;
	pni->remainbufsize = 0;
	pni->zeroimage_page = 0;
	pni->copyimage_page = 0;
	pni->sector_ptr = 0;
	pni->state = NAND_PAGE_READ;
	pni->imageblockcnt = 0;
	pni->nextretry = 0;

	pbl0fn->printf("addr step:%d, pagecnt:%xK\n",
			pni->address_step, pagecnt);

	pbl0fn->setdeviceclock(nandclk, 2, 1);
	pbl0fn->setpad(nandpad, 14, 1);

	rvalue = pnc->CTRL;
	rvalue =
		(0x7 << 4) |	// chip select no select
		0x1 << 0;	// chip select enable
	pnc->CTRL = rvalue ;

	pnc->DDR_CTRL &= ~((1 << 1) |	// ddr clock disable
			(1 << 0));	// set sdr mode
	pnc->STATUS =
		1 << 28 |	// clr rdy
		1 << 27 |	// clr dma busy
		1 << 26 |	// clr bch busy
		1 << 25 |	// clr nf if busy
		1 << 24 |	// clr nf busy
		1 <<  6 |	// clr dma pending
		1 <<  5 |	// unmask dma int
		1 <<  4 |	// dma enable
		1 <<  2 |	// clr rdy pending
		1 <<  1 |	// unmask rdy int
		1 <<  0; 	// rdy enable
	pnc->CMD_TIME =
		50 << 24 |	// RHW
		 4 << 16 |	// SETUP
		13 <<  8 |	// WIDTH
		 3 <<  0;	// HOLD
	pnc->DAT_TIME =
		30 << 24 |	// RHW
		 3 << 16 |	// SETUP
		13 <<  8 |	// WIDTH
		 5 <<  0;	// HOLD
	pnc->DDR_TIME =
		2 << 28 |	// tCAD
		3 << 24 |	// tLAST
		0 << 16 |	// RD_DLY
		0 <<  8 |	// DQ_DLY
		2 <<  0;	// CLK_PERIOD
	pnc->DMA_CTRL = (0 <<  0);	// CPU_MODE

	pnc->CTRL =
		1 << 16 |	// sram power on
		0 <<  4 |	// ncs 0 enable
		1 <<  0;	// cs enable

	pnc->BCH_MODE = bchmode[pagesize];
	pnc->BCH_MODE &= ~(1 << 4);	// decoding mode
	pnc->DMA_ADDR = (unsigned int)pni->buf;	// data & xxx & ecc

	unsigned int dsize;
	dsize = ((512 << pagesize) * 1 - 1) << 0;	// total size;
	// dma size (subpage size * cnt)
	dsize |= (((13 + pagesize) * ecn[pagesize] + (8 - 1)) / 8 - 1) << 16;
	pnc->DMA_SIZE = dsize;
	pnc->DMA_SUBP = (1 - 1) << 16;	// dma subpage number
	pnc->DMA_SUBP |= (512 << pagesize) - 1;	// dma subpage size
}

void nandcontroller_deinit(void)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setdeviceclock(nandclk, 2, 0);
	pbl0fn->setpad(nandpad, 14, 0);
}

int nandreset(void)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int timeout = 0x100000;

	pnc->STATUS |= 1 << 2;		// clear rnb pending
	pnc->CMD = NAND_CMD_RESET;	// nand reset
	while (!(pnc->STATUS & 0x1 << 2) && --timeout);	// wait rnb high
	pnc->STATUS |= 0x1 << 2;	// clear rnb int pending

	if (timeout == 0) {
		pbl0fn->printf("nand is not response\n");
		return 0;
	}
	return 1;
}

int nandread(struct nandbootinfo *pni, void *pdata, int size)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int i;
	int noerr = 1, timeout;

//	pbl0fn->printf("\n%s start - size:%d\n", __func__, size);
	while (size > 0 && noerr) {
		switch (pni->state) {
		case NAND_PAGE_READ:
			pbl0fn->printf("%s read page start\n", __func__);
			unsigned int nadr;
			if (pni->nextretry == 1) {
				nadr = pni->copyimage_page;
				pni->imageblockcnt++;
			} else {
				nadr = pni->zeroimage_page;
				pni->imageblockcnt = 0;
			}
			pbl0fn->printf("nand addr:%x\n", nadr);
			pnc->CMD = NAND_CMD_READ_1ST;        // read cmd start

			pnc->ADR = 0x00;        // col addr low is always 0
			if (pni->pagesize)
				pnc->ADR = 0x00;        // col addr high is always 0

			pnc->ADR = (nadr >>  0) & 0xFF;
			pnc->ADR = (nadr >>  8) & 0xFF;
			if (pni->address_step)
				pnc->ADR = (nadr >> 16) & 0xFF;

			pnc->STATUS |= 1 << 2;  // clear rnb pending
			if (pni->pagesize)
				pnc->CMD = NAND_CMD_READ_2ND;
			timeout = 0x1000000;
			while (!(pnc->STATUS & 1 << 2) && --timeout);   // wait rnb int
			if (timeout == 0) {
				noerr = 0;
				pbl0fn->printf("read cmd rnb timeout\n");
				break;
			}
			pnc->STATUS |= 1 << 2;  // clear rnb pending
//			pbl0fn->printf("%s read page done\n", __func__);

		case NAND_SECTOR_READ:
			if (pni->nextretry)
				i = pni->sector_ptr % pni->pagecnt;
			else
				i = 0;
			do {
				pbl0fn->printf("read sector start 0x%x\n",
						pni->sector_ptr);
				pnc->RAND = 0x576A;     // reset random seed
				pnc->STATUS |= 1 << 6;  // clear dma done pending
				pnc->DMA_CTRL |= 1 << 0;// nand read dma start
				timeout = 0x10000;
				while (!(pnc->STATUS & 1 << 6) && --timeout);   // wait dma int
				if (timeout == 0) {
					pbl0fn->printf("read dma timeout\n");
					noerr = 0;
					break;
				}
				pnc->STATUS |= 1 << 6;  // clear dma done pending

				while (pnc->STATUS & 1 << 25);  // NFIF check wait for idle
				pnc->DMA_CTRL &= ~(1 << 0);     // cpu mode
			} while (i--);

//			pbl0fn->printf("error correction\n");
			pnc->SRAM_SUBPAGE = 0;
			int errcnt = pnc->ERR_INFO;
			pbl0fn->printf("error count:%x\n", errcnt);
			if (errcnt == 0x3F) {
				pbl0fn->printf("error correction fail page:%d sector:%d\n",
						pni->copyimage_page,
						pni->sector_ptr % pni->sectorsize);
//                              if (pni->imageblockcnt >= 63)     // 64KB x 64 = 4MB (0x400000)
				if (pni->imageblockcnt >= pni->checkimagecnt) {     // for test
					pbl0fn->printf("all copy image is broken (%d)\n",
							pni->imageblockcnt);
					noerr = 0;
					break;
				}
				if ((pni->nextretry == 1) ||
						(pni->copyimage_page != pni->zeroimage_page))
					pni->copyimage_page +=
						MAXIMAGESIZE / pni->pagesize;
				else
					pni->copyimage_page = pni->zeroimage_page +
						MAXIMAGESIZE / pni->pagesize;
				pni->state = NAND_PAGE_READ;
				pni->nextretry = 1;
				break;
			} else if (errcnt == 0) {
				pbl0fn->printf("no data error\n");
			}
			else {
				pbl0fn->printf("err cnt:%d\n", errcnt);
			}
			pni->nextretry = 0;
			if ((errcnt < 0x3f) && errcnt > 0) {
				int elp;
				int offset = (pni->sectorsize * 2 -
						pni->datasize - pni->eccsize) * 8;
				unsigned char *data = pni->buf;
				for (i = 0; i < errcnt; i++) {
					elp = pnc->SRAM[i] & 0x3FFF;
					elp -= offset;
					if (pni->sectorsize * 8 > elp) {     // only data area
						data[elp >> 3] ^=
							1 << (7 - (elp & 0x7));
					}
				}
				pbl0fn->printf("%d error corrected\n", errcnt);
			}
			pni->sector_ptr++;
			pni->state = BUF_COPY;
			pni->remainbufsize = pni->datasize;
			pbl0fn->printf("read sector done\n");
		case BUF_COPY:
			pbl0fn->printf("read buf copy start:%d\n",
					pni->remainbufsize);
			int cpysize = size > pni->remainbufsize ?
				pni->remainbufsize : size;
			unsigned char *psrc = &pni->buf[pni->datasize -
				pni->remainbufsize];
			unsigned char *pdst = pdata;
			for (i = 0; i < cpysize; i++)
				pdst[i] = psrc[i];
			pni->remainbufsize -= cpysize;
			pdata += cpysize;       // memory ptr
			size -= cpysize;        // real data addr
			if (pni->remainbufsize == 0) {
				pbl0fn->printf("no remain data(%x:%x:%x:%d)\n",
						pni->zeroimage_page, pni->copyimage_page,
						pni->sector_ptr, size);
				if ((pni->sector_ptr % pni->pagecnt) == 0) {
					pbl0fn->printf("no nand buf, need page read\n");
					pni->state = NAND_PAGE_READ;
					pni->zeroimage_page++;
					pni->copyimage_page = pni->zeroimage_page;
					pni->nextretry = 0;
					break;
				}
				pni->state = NAND_SECTOR_READ;
				break;
			}
//			pbl0fn->printf("read buf copy done:%d\n", size);
		default:
			break;
		}
//		pbl0fn->printf("all data read done\n\n");
	}
//	pbl0fn->printf("done\n\n");
	return noerr;
}	
#endif

//------------------------------------------------------------------------------
int iNANDBOOTEC(unsigned int option)
{
#ifdef NXP4330
	int ret = 1;
	option = option;
#endif
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret = 1, timeout;
	struct nandbootinfo ni;
	struct nandbootinfo *pni = &ni;
	unsigned int nbuf[1024 / 4];
	pni->buf = (unsigned char *)nbuf;

	pbl0fn->nandcontroller_init(pni, option);


	timeout = 0x1000000;
	while (!(pnc->STATUS & 1 << 28) && --timeout);  // wait rnb status
	if (timeout == 0) {
		printf("nand is busy for a long time.\n");
		ret = 0;
		goto error;
	}

	if (pbl0fn->nandreset() == 0) {
		ret = 0;
		goto error;
	}

	/*===========================================================
	 * read boot header
	 * ========================================================*/
	ret = pbl0fn->nandread(pni, (void *)&(*pbl0fn->pbm)->bi,
			sizeof(struct nx_bootinfo));
	if (ret == 0) {
		pbl0fn->printf("nand boot header read failure.\n");
		goto error;
	}

	unsigned int iv[4];
	iv[0] = (*pbl0fn->iv)[0];
	iv[1] = (*pbl0fn->iv)[1];
	iv[2] = (*pbl0fn->iv)[2];
	iv[3] = (*pbl0fn->iv)[3];
#if 1
	if (!pbl0fn->headercheck(iv, option & 1 << DECRYPT)) {
		ret = 0;
		goto error;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((unsigned int *)(*pbl0fn->pbm),
				(unsigned int *)(*pbl0fn->pbm),
				iv, sizeof(struct nx_bootinfo));
	}

	if ((*pbl0fn->pbm)->bi.signature != HEADER_ID) {
		pbl0fn->printf("invalid boot image(%08x).\r\n",
				(*pbl0fn->pbm)->bi.signature);
		ret = 0;
		goto error;
	}
#endif
	/*===========================================================
	 * read boot key
	 * ========================================================*/
	struct nx_bootinfo *pbi = (struct nx_bootinfo *)&(*pbl0fn->pbm)->bi;
	pni->checkimagecnt = pbi->LoadSize >> 16;

	ret &= pbl0fn->nandread(pni, (void *)&(*pbl0fn->pbm)->rsa_public,
			sizeof(struct asymmetrickey));
	if (ret == 0) {
		pbl0fn->printf("cannot read rsa boot key\r\n");
		goto error;
	}
	/*===========================================================
	 * boot key certify
	 * ========================================================*/
#if 1
	if (!pbl0fn->keycheck(iv, option & 1 << DECRYPT, (option >> VERIFY) & 0x3)) {
		ret = 0;
		goto error;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				(unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				iv, sizeof(struct asymmetrickey));
	}
        unsigned int ehash = (option >> VERIFY) & 0x3;
	if (ehash) {
		unsigned int bhash[32 / 4];
		pbl0fn->mbedtls_sha256((*pbl0fn->pbm)->rsa_public.rsapublicbootkey,
				256, (unsigned char *)bhash, 0);
		if (ehash == 1)
			ehash--;
		const unsigned int *pbootkeyhash =
			&(*(*pbl0fn->pbootkeyhash))[ehash][0];
		if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) != 0) {
			pbl0fn->printf("boot key verify fail.\r\n");
			ret = 0;
			goto error;
		}
	}
#endif

	/*===========================================================
	 * read boot image
	 * ========================================================*/
	ret &= pbl0fn->nandread(pni, (void *)(*pbl0fn->pbm)->image, pbi->LoadSize);
	if (ret == 0) {
		pbl0fn->printf("cannot read boot image\r\n");
		goto error;
	}

	/*===========================================================
	 * boot image decrypt if requared.
	 * ========================================================*/
	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((unsigned int *)(*pbl0fn->pbm)->image,
				(unsigned int *)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}

error:
	pbl0fn->nandcontroller_deinit();

#endif
	return ret;
}
