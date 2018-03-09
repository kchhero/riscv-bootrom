////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or fitness
// for a particular puporse.
//
// Module	: bl0
// File		: CRYPTO.c
// Description	:
// Author	: Hans
// History	: 2013.01.10 First implementation
//		2017.10.09 Hans add RSA Authentication
//		2018.02.16 Hans all new code for using SSS
////////////////////////////////////////////////////////////////////////////////
#include <nx_antares.h>
#include <nx_type.h>
#include "nx_antares_bootheader.h"
#include "nx_antares_bootoption.h"
#include "libarm.h"
#include "fnptr.h"

#ifdef NXP3220
#include <nx_chip_sfr.h>
#include <nx_sss.h>


static struct NX_SSS_RegisterSet *pSSS =
	(struct NX_SSS_RegisterSet *)PHY_BASEADDR_SSS_MODULE_SSS;

static struct NX_SKM_RegisterSet *pSKM =
	(struct NX_SKM_RegisterSet *)PHY_BASEADDR_SSS_MODULE_HOST_KEYMAN;

void gethash(struct NX_HRDMADESC *pdesc, int descsize,
		unsigned int *phash, int tsize)
{
	/* ---------------------------------------------------------
	 *      hash control
	 *---------------------------------------------------------*/
	pSSS->hash.BYTE_SWAP = 0;
	pSSS->hash.FIFO_MODE = 1 << 0;

	pSSS->hash.PRE_MSG_SIZE_EXT_HIGH = 0;
	pSSS->hash.PRE_MSG_SIZE_EXT_LOW = 0;
	pSSS->hash.PRE_MSG_SIZE_HIGH = 0;
	pSSS->hash.PRE_MSG_SIZE_LOW = 0;

	pSSS->hash.MSG_SIZE_EXT_HIGH = 0;
	pSSS->hash.MSG_SIZE_EXT_LOW = 0;
	pSSS->hash.MSG_SIZE_HIGH = 0;
	pSSS->hash.MSG_SIZE_LOW = tsize;

	pSSS->hash.CONTROL[0] = ENG_SHA256_HASH | START_BIT;


	/* ---------------------------------------------------------
	 *      set and start dma for aes
	 *---------------------------------------------------------*/
	pSSS->feeder.FIFOCTRL = HASH_ALONE;
	pSSS->feeder.DDMACTRL =
		1 << 3 |	// cipher table swap
		0 << 1;		// no hash table swap
	pSSS->feeder.BDRDMASH = 0;
	pSSS->feeder.BDRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  5 |	// ARCACHE
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap

	int i;
	for (i = 0; i < descsize; i++) {
		pSSS->feeder.BDRDMAS = (unsigned int)&pdesc[i];
		pSSS->feeder.BDRDMAL =
			sizeof(struct NX_HRDMADESC);	// descriptor size
		/* ---------------------------------------------------------
		 *      wait dma done
		 *---------------------------------------------------------*/
		while (!(pSSS->feeder.INTPEND & (1 << 16)));	// check BDDMA_DONE
		pSSS->feeder.INTPEND = 1 << 16;			// clear pending
	}

	U32 fctrl = pSSS->feeder.FIFOCTRL;
	if (((fctrl & 3) == 1) || ((fctrl & 3) == 2)) {
		while (!(pSSS->hash.STATUS & 1 << 6));
		pSSS->hash.STATUS = 1 << 6;
	}

	for (i = 0; i < (256 / 32); i++)
		phash[i] = pSSS->hash.RESULT[i];
}

void getkeyhash(unsigned int *phash)
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	struct NX_HRDMADESC desc;
	int size = (unsigned int)
		(sizeof((*pbl0fn->pbm)->rsa_public.rsapublicbootkey));
	// make table
	desc.HRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap
	desc.HRDMASH = 0;	// start addr high
	// start addr low
	desc.HRDMAS = (unsigned int)&(*pbl0fn->pbm)->rsa_public;
	desc.HRDMAL = size;

	pbl0fn->gethash(&desc, 1, phash, size);
}

void getimagehash(unsigned int *phash)
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	struct NX_HRDMADESC desc[2];
	int ksize = (unsigned int)
		(sizeof((*pbl0fn->pbm)->rsa_public.rsapublicbootkey) +
		sizeof((*pbl0fn->pbm)->rsa_public.rsapublicuserkey));
	// make table
	desc[0].HRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap
	desc[0].HRDMASH = 0;	// start addr high
	// start addr low
	desc[0].HRDMAS = (unsigned int)&(*pbl0fn->pbm)->rsa_public;
	desc[0].HRDMAL = ksize;

	int isize = (unsigned int)sizeof((*pbl0fn->pbm)->image);

	// make table
	desc[1].HRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap
	desc[1].HRDMASH = 0;	// start addr high
	// start addr low
	desc[1].HRDMAS = (unsigned int)&(*pbl0fn->pbm)->image;
	desc[1].HRDMAL = isize;

	pbl0fn->gethash(desc, 2, phash, ksize + isize);
}

void aesdecrypt(struct NX_BRTDMADESC *pdesc, int descsize, unsigned int *iv)
{
	/* ---------------------------------------------------------
	 *      aes control
	 *---------------------------------------------------------*/
        pSSS->aes.CONTROL = AES_ENC | AES_CBC | AES_FIFO | AES_128 | AES_NOSWAP;
        pSSS->aes.SIZELOW = pdesc->BTDMAL;
        pSSS->aes.SIZEHIGH = 0;
#if 0
        pSSS->aes.KEYDATA[4] = aeskey[0];
        pSSS->aes.KEYDATA[5] = aeskey[1];
        pSSS->aes.KEYDATA[6] = aeskey[2];
        pSSS->aes.KEYDATA[7] = aeskey[3];
#else
	pSKM->CTRL = 1;	// open key manager path
	pSKM->MODE = 0;	// 0: aes key 0, 1: aes key 1, 2: iv0, 3: iv1
        pSSS->aes.KEYDATA[4] = 0xFFFFFFFF;
        pSSS->aes.KEYDATA[5] = 0xFFFFFFFF;
        pSSS->aes.KEYDATA[6] = 0xFFFFFFFF;
        pSSS->aes.KEYDATA[7] = 0xFFFFFFFF;
	pSKM->CTRL = 0;	// close path
#endif
        pSSS->aes.IVDATA[0] = iv[0];
        pSSS->aes.IVDATA[1] = iv[1];
        pSSS->aes.IVDATA[2] = iv[2];
        pSSS->aes.IVDATA[3] = iv[3];	
	/* ---------------------------------------------------------
	 *      set and start dma for aes
	 *---------------------------------------------------------*/
	pSSS->feeder.FIFOCTRL = AESSEL | HASH_BC_OUT;
	pSSS->feeder.DDMACTRL =
		1 << 3 |	// cipher table swap
		0 << 1;		// no hash table swap
	pSSS->feeder.BDRDMASH = 0;
	pSSS->feeder.BDRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  5 |	// ARCACHE
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap

	int i;
	for (i = 0; i < descsize; i++) {
		pSSS->feeder.BDRDMAS = (unsigned int)&pdesc[i];
		pSSS->feeder.BDRDMAL =
			sizeof(struct NX_BRTDMADESC);	// descriptor size

		/* ---------------------------------------------------------
		 *      wait dma done
		 *---------------------------------------------------------*/
		while (!(pSSS->feeder.INTPEND & (1 << 16)));	// check BDDMA_DONE
		pSSS->feeder.INTPEND = 1 << 16;			// clear pending
	}
	pSSS->aes.STATUS = AES_MAC_DONE;

	U32 fctrl = pSSS->feeder.FIFOCTRL;
	if (((fctrl & 3) == 1) || ((fctrl & 3) == 2)) {
		while (!(pSSS->hash.STATUS & 1 << 6));
		pSSS->hash.STATUS = 1 << 6;
	}

	iv[0] = pSSS->aes.IVDATA[0];
	iv[1] = pSSS->aes.IVDATA[1];
	iv[2] = pSSS->aes.IVDATA[2];
	iv[3] = pSSS->aes.IVDATA[3];
}

int Decrypt(unsigned int *dst, unsigned int *src, unsigned int *iv, int size)
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	/* ---------------------------------------------------------
	 *      generate dma descriptor table for aes data
	 *---------------------------------------------------------*/
	// make table
	struct NX_BRTDMADESC desc;
	struct NX_BRTDMADESC *pdesc = &desc;
	pdesc->BRDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap
	pdesc->BRDMASH = 0;	// start addr high
	pdesc->BRDMAS = (unsigned int)src;	// start addr low
	pdesc->BRDMAL = size;

	pdesc->BTDMAC =
		0 << 14 |	// burst length;
		0 <<  9 |	// ARUSER
		0 <<  2 |	// ARPROT
		0 <<  1;	// byte swap
	pdesc->BTDMASH = 0;
	pdesc->BTDMAS = (unsigned int)dst;
	pdesc->BTDMAL = size;	// encrypted data

	pbl0fn->aesdecrypt(pdesc, 1, iv);

	return 1;
}
#endif

int headercheck(unsigned int iv[], int dec)
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	if (dec)
		pbl0fn->Decrypt((unsigned int *)&(*pbl0fn->pbm)->bi,
				(unsigned int *)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));

	if ((*pbl0fn->pbm)->bi.signature != HEADER_ID) {
		pbl0fn->printf("invalid boot image(%08x).\r\n",
				(*pbl0fn->pbm)->bi.signature);
		return 0;
	}
	return 1;
}

int keycheck(unsigned int iv[], int dec, int verify)
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	if (dec)
		pbl0fn->Decrypt((unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				(unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				iv, sizeof(struct asymmetrickey));

	if (verify) {
		/* ---------------------------------------------------------
		 *      generate dma descriptor table for aes data
		 *---------------------------------------------------------*/
		unsigned int bhash[32 / 4];
#ifdef NXP3220
		struct NX_HRDMADESC desc;
		int size = (unsigned int)
			sizeof((*pbl0fn->pbm)->rsa_public.rsapublicbootkey);
		// make table
		desc.HRDMAC =
			0 << 14 |	// burst length;
			0 <<  9 |	// ARUSER
			0 <<  2 |	// ARPROT
			0 <<  1;	// byte swap
		desc.HRDMASH = 0;	// start addr high
		// start addr low
		desc.HRDMAS = (unsigned int)&(*pbl0fn->pbm)->rsa_public;
		desc.HRDMAL = size;

		pbl0fn->gethash(&desc, 1, bhash, size);
#endif
#ifdef NXP4330
		pbl0fn->mbedtls_sha256((unsigned char *)&(*pbl0fn->pbm)->rsa_public,
			sizeof((*pbl0fn->pbm)->rsa_public.rsapublicbootkey),
			(unsigned char *)bhash, 0);
#endif
		if (verify == 1)	// because efuse hash value position
			verify = 0;	// please see efuse hash address

		const unsigned int *pbootkeyhash =
			&(*(*pbl0fn->pbootkeyhash))[verify][0];
		if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) != 0) {
			pbl0fn->printf("boot key verify fail.\r\n");
			return 0;
		}
	}
	return 1;
}

void imagedecrypt(U32 iv[])
{
	struct nx_bl0_fn *pbl0fn = (struct nx_bl0_fn *)Getbl0fnPtr();
	pbl0fn->Decrypt((unsigned int *)(*pbl0fn->pbm)->image,
		(unsigned int *)(*pbl0fn->pbm)->image,
		iv, (*pbl0fn->pbm)->bi.LoadSize);
}

