/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module	: sign
 * File		: rsa_verify_pss.c
 * Description	:
 * Author	: Hans
 * History	: 2016.06.06 First implementation
 */

/*
 *  RSASSA-PSS/SHA-256 signature verification program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#include "nx_type.h"
#include "fnptr.h"
#include "mbedtls/config.h"

#include "mbedtls/platform.h"

#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"

#include <debug.h>
#include <nx_antares_bootheader.h>
#include "libarm.h"
#include "libplat.h"
#include "printf.h"

#define RSA_KEY_SIZE			2048
#define MD_HASH_SIZE			256
#define MBEDTLS_MD_MAX_SIZE		(MD_HASH_SIZE/8)

#define MAX_MEM_ALLOC_SIZE		0x2000
#define MAX_MEM_ALLOC_MASK		(MAX_MEM_ALLOC_SIZE - 1)

//char mempool[MAX_MEM_ALLOC_SIZE];	/* currently 0x1840 */
//size_t memallocpool;


static inline unsigned int makealign(unsigned int size)
{
	int i = 0;

	while (size >> i)
		i++;

	if (size & ((1ul << (i - 1)) - 1))
		return 1ul << i;
	else
		return 1ul << (i - 1);
}

void *memalloc(size_t *lastusedptr, unsigned int count, unsigned int alignment)
{
	size_t newptr, lastptr = *lastusedptr;
	size_t alignedsize = makealign(alignment);
	size_t alignmask = ~(alignedsize -1);

	newptr = (lastptr + alignedsize - 1) & alignmask;
	lastptr = newptr + (count * alignment);	/* update last used */
	*lastusedptr = lastptr;

	return (void *)newptr;
}
void *nxalloc(size_t n, size_t size)
{
	size_t memallocpool = (size_t)GetMempoolPtr();
	void *ptr = memalloc(&memallocpool, n, size);
	SetMempoolPtr((unsigned int)memallocpool);

	if (((unsigned int)ptr & MAX_MEM_ALLOC_MASK) + n * size >=
			MAX_MEM_ALLOC_SIZE)
		printf("mem alloc pool is empty !!!!!!!!!!!!\n"
				"Please increase pool size.\n");
	return ptr;
}

void * mbedtls_calloc(size_t n, size_t size)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	return pbl0fn->nxalloc(n, size);
}
void nxfree(void *ptr)
{
	ptr = ptr;
	return;
}
void mbedtls_free(void *ptr)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->nxfree(ptr);
}

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx)
{
	if (!ctx->N.p || !ctx->E.p)
		return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);

	if ((ctx->N.p[0] & 1) == 0 || (ctx->E.p[0] & 1) == 0)
		return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);

	if (mbedtls_mpi_bitlen(&ctx->N) < 128 ||
			mbedtls_mpi_bitlen(&ctx->N) > MBEDTLS_MPI_MAX_BITS)
		return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);

	if (mbedtls_mpi_bitlen(&ctx->E) < 2 ||
			mbedtls_mpi_cmp_mpi(&ctx->E, &ctx->N) >= 0)
		return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);

	return (0);
}

int get_hash(
		unsigned char *imageptr0, unsigned int size0,
		unsigned char *imageptr1, unsigned int size1,
		unsigned char hash[])
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret = 0;
	mbedtls_sha256_context ctx;

	pbl0fn->mbedtls_sha256_init(&ctx);
	pbl0fn->mbedtls_sha256_starts(&ctx, 0);
	pbl0fn->mbedtls_sha256_update(&ctx, imageptr0, size0);
	pbl0fn->mbedtls_sha256_update(&ctx, imageptr1, size1);
	pbl0fn->mbedtls_sha256_finish(&ctx, hash);

	return (ret);
}

void mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
		size_t slen, mbedtls_sha256_context *ctx)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	unsigned char mask[MBEDTLS_MD_MAX_SIZE];
	unsigned char counter[4];
	unsigned char *p;
	unsigned int hlen;
	size_t i, use_len;

	pbl0fn->memset(mask, 0, MBEDTLS_MD_MAX_SIZE);
	pbl0fn->memset(counter, 0, 4);

	hlen = MD_HASH_SIZE/8;

	/* Generate and apply dbMask */
	p = dst;

	while (dlen > 0) {
		use_len = hlen;
		if (dlen < hlen)
			use_len = dlen;

		pbl0fn->mbedtls_sha256_starts(ctx, 0);
		pbl0fn->mbedtls_sha256_update(ctx, src, slen);
		pbl0fn->mbedtls_sha256_update(ctx, counter, 4);
		pbl0fn->mbedtls_sha256_finish(ctx, mask);

		for (i = 0; i < use_len; ++i)
			*p++ ^= mask[i];

		counter[3]++;

		dlen -= use_len;
	}
}

/*
 * Do an RSA public key operation
 */
int mbedtls_rsa_public(
		mbedtls_rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output)
{
	int ret = 0;
	size_t olen;
	mbedtls_mpi T;

	mbedtls_mpi_init(&T);

	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&T, input, ctx->len));

	if (mbedtls_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
		ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
		goto cleanup;
	}

	olen = ctx->len;
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(
				&T, &T, &ctx->E, &ctx->N, &ctx->RN));
	MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&T, output, olen));

cleanup:

	mbedtls_mpi_free(&T);

	if (ret != 0)
		return (MBEDTLS_ERR_RSA_PUBLIC_FAILED + ret);

	return (0);
}
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify_ext(
		mbedtls_rsa_context *rctx,
		unsigned int hashlen, const unsigned char *hash,
		int expected_salt_len, const unsigned char *sig)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret;
	size_t siglen;
	unsigned char *p;
	unsigned char result[MBEDTLS_MD_MAX_SIZE];
	unsigned char zeros[8];
	unsigned int hlen;
	size_t slen, msb;
	mbedtls_sha256_context * hctx =
		pbl0fn->mbedtls_calloc(1, sizeof(mbedtls_sha256_context));
	unsigned char buf[MBEDTLS_MPI_MAX_SIZE];

	siglen = rctx->len;

	if (siglen < 16 || siglen > sizeof(buf))
		return (MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

	ret = pbl0fn->mbedtls_rsa_public(rctx, sig, buf);

	if (ret != 0)
		return (ret);

	p = buf;

	if (buf[siglen - 1] != 0xBC)
		return (MBEDTLS_ERR_RSA_INVALID_PADDING);

	hashlen = MD_HASH_SIZE/8;

	hlen = MD_HASH_SIZE/8;
	slen = siglen - hlen - 1; /* Currently length of salt + padding */

	memset(zeros, 0, 8);

	/* Note: EMSA-PSS verification is over the length of N - 1 bits */
	msb = mbedtls_mpi_bitlen(&rctx->N) - 1;

	/* Compensate for boundary condition when applying mask */
	if (msb % 8 == 0) {
		p++;
		siglen -= 1;
	}
	if (buf[0] >> (8 - siglen * 8 + msb))
		return (MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

	pbl0fn->mbedtls_sha256_init(hctx);

	pbl0fn->mgf_mask(p, siglen - hlen - 1, p + siglen - hlen - 1, hlen, hctx);

	buf[0] &= 0xFF >> (siglen * 8 - msb);

	while (p < buf + siglen && *p == 0)
		p++;

	if (p == buf + siglen || *p++ != 0x01) {
		pbl0fn->mbedtls_sha256_free(hctx);
		mbedtls_free(hctx);
		return (MBEDTLS_ERR_RSA_INVALID_PADDING);
	}

	/* Actual salt len */
	slen -= p - buf;

	if (expected_salt_len != MBEDTLS_RSA_SALT_LEN_ANY &&
			slen != (size_t) expected_salt_len) {
		pbl0fn->mbedtls_sha256_free(hctx);
		mbedtls_free(hctx);
		return (MBEDTLS_ERR_RSA_INVALID_PADDING);
	}

	/* Generate H = Hash( M' ) */
	pbl0fn->mbedtls_sha256_starts(hctx, 0);
	pbl0fn->mbedtls_sha256_update(hctx, zeros, 8);
	pbl0fn->mbedtls_sha256_update(hctx, hash, hashlen);
	pbl0fn->mbedtls_sha256_update(hctx, p, slen);
	pbl0fn->mbedtls_sha256_finish(hctx, result);

	pbl0fn->mbedtls_sha256_free(hctx);
	mbedtls_free(hctx);

	if (memcmp(p + slen, result, hlen) == 0)
		return (0);
	else
		return (MBEDTLS_ERR_RSA_VERIFY_FAILED);
}

int verify_image(unsigned char encrypted_md[],
		void **pubkeyN,
		unsigned char hash[])
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret = 1;
	mbedtls_mpi_uint **p = (mbedtls_mpi_uint **)pubkeyN;
	mbedtls_rsa_context rsa;
	mbedtls_mpi_uint E = 0x3;


	pbl0fn->memset(&rsa, 0, sizeof(mbedtls_rsa_context));

	rsa.ver = 0;
	rsa.len = RSA_KEY_SIZE / 8;

	rsa.E.s = 1;
	rsa.E.n = 1;
	rsa.E.p = &E;

	rsa.N.s = 1;
	rsa.N.n = RSA_KEY_SIZE / 8 / sizeof(mbedtls_mpi_uint);
	rsa.N.p = *p;

	if ((ret = pbl0fn->mbedtls_rsa_check_pubkey(&rsa)) != 0) {
		printf("rsa public key check fail %x\n", ret);
		goto exit;
	}

#if 0
	{
	int i;
	for (i = 0; i < rsa.N.n; i++) {
		if(i%4 == 0)
			printf("\n");
		printf("%08x ", rsa.N.p[i]);
	}
	printf("\n");
	printf("E.p:%x\n", rsa.E.p[0]);
	printf("rsa.len:%ld\n", rsa.len);
	printf("N.s:%d, E.s:%d, N.n:%ld, E.n:%ld\n",
			rsa.N.s, rsa.E.s, rsa.N.n, rsa.E.n);
	printf("N.p:%p, E.p:%p\n", (void*)rsa.N.p, (void*)rsa.E.p);
	}
#endif


	/*
	 * Compute the SHA-256 hash of the input file and
	 * verify the signature
	 */
	printf("\n  . Verifying the RSA/SHA-256 signature\r\n");

	if ((ret = pbl0fn->mbedtls_rsa_rsassa_pss_verify_ext(
			&rsa, MBEDTLS_MD_MAX_SIZE,
			hash, -1, encrypted_md)) != 0) {
		goto exit;
	}

	ret = 0;

exit:
	return (ret);
}

int authenticate_image(unsigned char *signbootheaderptr,
		unsigned char *pkeybootheaderptr,
		unsigned char *bootimageptr,
		int size)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret;
	void *pub = pkeybootheaderptr;
	unsigned char hash[32];

	void *mempool = (void*)GetMempoolPtr();
	pbl0fn->memset(mempool, 0, MAX_MEM_ALLOC_SIZE);

	pbl0fn->get_hash(pub, 512, bootimageptr, size, hash);

	if ((ret = pbl0fn->verify_image(signbootheaderptr, &pub, hash)) == 0)
		printf("\n  . OK (the signature is valid)\r\n\n");
	else
		printf("\n  . Oops! (image was modified(%d))\r\n\n", ret);

	return (!ret);
}
