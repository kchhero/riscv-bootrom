/*
 *  RSASSA-PSS/SHA-256 signature creation program
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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_snprintf   snprintf
#define mbedtls_printf     printf
#endif

#if !defined(MBEDTLS_MD_C) || !defined(MBEDTLS_ENTROPY_C) ||  \
    !defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_SHA256_C) ||        \
    !defined(MBEDTLS_PK_PARSE_C) || !defined(MBEDTLS_FS_IO) ||    \
    !defined(MBEDTLS_CTR_DRBG_C)
int main( void )
{
	mbedtls_printf("MBEDTLS_MD_C and/or MBEDTLS_ENTROPY_C and/or "
			"MBEDTLS_RSA_C and/or MBEDTLS_SHA256_C and/or "
			"MBEDTLS_PK_PARSE_C and/or MBEDTLS_FS_IO and/or "
			"MBEDTLS_CTR_DRBG_C not defined.\n");
	return( 0 );
}
#else

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/md.h"
#include "mbedtls/x509.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *f;
	int ret = 1;
	mbedtls_pk_context bpk;
	mbedtls_pk_context upk;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	unsigned char hash[32];
	unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
	char filename[512];
	const char *pers = "nexell trust sign";
	size_t olen = 0;

	mbedtls_entropy_init(&entropy);
	mbedtls_pk_init(&bpk);
	mbedtls_pk_init(&upk);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	if (argc != 4) {
		mbedtls_printf("usage: rsa_sign_pss <boot key_file> <user key_file> <filename>\n");

#if defined(_WIN32)
		mbedtls_printf("\n");
#endif
		goto exit;
	}

	mbedtls_printf("\n  . Seeding the random number generator...");
	fflush(stdout);

	if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg,
					mbedtls_entropy_func,
					&entropy,
					(const unsigned char *) pers,
					strlen(pers))) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf("\n  . Reading private key from '%s'", argv[1]);
	fflush(stdout);

/* -----------------------------------------------------------------------------
 * boot key open
 -----------------------------------------------------------------------------*/
	if ((ret = mbedtls_pk_parse_keyfile(&bpk, argv[1], "")) != 0) {
		ret = 1;
		mbedtls_printf(" failed\n  ! Could not read key from '%s'\n", argv[1]);
		mbedtls_printf("  ! mbedtls_pk_parse_public_keyfile returned %d\n\n", ret);
		goto exit;
	}

	if (!mbedtls_pk_can_do(&bpk, MBEDTLS_PK_RSA)) {
		ret = 1;
		mbedtls_printf( " failed\n  ! Key is not an RSA key\n" );
		goto exit;
	}

/* -----------------------------------------------------------------------------
 * user key open
 -----------------------------------------------------------------------------*/
	mbedtls_printf("\n  . Reading private key from '%s'", argv[2]);
	fflush(stdout);
	if ((ret = mbedtls_pk_parse_keyfile(&upk, argv[2], "")) != 0) {
		ret = 1;
		mbedtls_printf(" failed\n  ! Could not read key from '%s'\n", argv[2]);
		mbedtls_printf("  ! mbedtls_pk_parse_public_keyfile returned %d\n\n", ret);
		goto exit;
	}

	if (!mbedtls_pk_can_do(&upk, MBEDTLS_PK_RSA)) {
		ret = 1;
		mbedtls_printf( " failed\n  ! Key is not an RSA key\n" );
		goto exit;
	}

	mbedtls_rsa_set_padding(mbedtls_pk_rsa(bpk), MBEDTLS_RSA_PKCS_V21,
			MBEDTLS_MD_SHA256);

/* -----------------------------------------------------------------------------
 * result raw file create
 -----------------------------------------------------------------------------*/
	memset(filename, 0, 512);
	mbedtls_snprintf(filename, 512, "%s.raw", argv[3]);
	if ((f = fopen(filename, "wb+")) == NULL) {
		ret = 1;
		mbedtls_printf(" failed\n ! Could not create %s\n\n", filename);
		goto exit;
	}
/* -----------------------------------------------------------------------------
 * boot.pub write to raw file
 -----------------------------------------------------------------------------*/

	mbedtls_rsa_context *ctx = (mbedtls_rsa_context *)bpk.pk_ctx;
	if (fwrite(ctx->N.p, 1, ctx->N.n * 8, f) != ctx->N.n * 8) {
		mbedtls_printf("failed\n  ! fwrite failed\n\n");
		goto exit;
	}
	int pi;
	mbedtls_printf("\nboot.pub\n");
	for (pi = 0; pi < ctx->N.n; pi++) {
		if (pi % 4 == 0)
			printf("\n");
		printf("%016lx ", ctx->N.p[pi]);
	}
	printf("\n\n");
	printf("E:%lx\n\n", ctx->E.p[0]);

/* -----------------------------------------------------------------------------
 * boot.pub generating hash
 -----------------------------------------------------------------------------*/
	unsigned int boothash[8];
	mbedtls_sha256((const unsigned char *)ctx->N.p, 256,
			(unsigned char *)boothash, 0);

	/*
	 * Write the hash into <filename>.hash
	 */
	memset(filename, 0, 512);
	mbedtls_snprintf(filename, 512, "%s.hash", argv[1]);
	printf("generating rsa public boot key hash file(%s)\n", filename);

	FILE *hf;
	if ((hf = fopen(filename, "wb+")) == NULL) {
		ret = 1;
		mbedtls_printf(" failed\n  ! Could not create %s\n\n", filename);
		goto exit;
	}

	if (fwrite(boothash, 1, sizeof(boothash), hf) != sizeof(boothash)) {
		mbedtls_printf("failed\n  ! fwrite failed\n\n");
		goto exit;
	}
	{
		unsigned int pi;
		unsigned long *ulbuf = (unsigned long *)boothash;

		printf("rsa public boot key hash\n");
		for (pi = 0; pi < sizeof(boothash) >> 3; pi++) {
			if (pi % 4 == 0)
				printf("\n");
			printf("%016lx ", (unsigned long)ulbuf[pi]);
		}
		printf("\n\n");
	}
	fclose(hf);

/* -----------------------------------------------------------------------------
 * user.pub write to raw file
 -----------------------------------------------------------------------------*/
	ctx = (mbedtls_rsa_context *)upk.pk_ctx;
	mbedtls_printf("user.pub\n");
	if (fwrite(ctx->N.p, 1, ctx->N.n * 8, f) != ctx->N.n * 8) {
		mbedtls_printf("failed\n  ! fwrite failed\n\n");
		goto exit;
	}
	for (pi = 0; pi < ctx->N.n; pi++) {
		if (pi % 4 == 0)
			printf("\n");
		printf("%016lx ", ctx->N.p[pi]);
	}
	printf("\n\n");
	printf("E:%lx\n\n", ctx->E.p[0]);
/* -----------------------------------------------------------------------------
 * bl1 open
 -----------------------------------------------------------------------------*/

	FILE *tf;
	if ((tf = fopen(argv[3], "rb")) == NULL) {
		ret = 1;
		mbedtls_printf(" failed\n ! Could not open %s\n\n", filename);
		goto exit;
	}
	fseek(tf, 0, SEEK_END);
	int fsize = ftell(tf);
	fseek(tf, 0, SEEK_SET);
	printf("%s size is %d\n", argv[3], fsize);
	if (fsize % 16)
		fsize = ((fsize + 15) >> 4) << 4;
	printf("%s resize is %d\n", argv[3], fsize);
	void *imgbuf = malloc(fsize);
	memset(imgbuf, 0, fsize);
	int rsize = fread(imgbuf, 1, fsize, tf);
	printf("%s read size is %d\n", argv[3], rsize);
	fclose(tf);

	fwrite(imgbuf, 1, fsize, f);
	fclose(f);
	free(imgbuf);
	
/* -----------------------------------------------------------------------------
 * boot image hash Generating (boot.pub, user.pub, bl1.bin)
 -----------------------------------------------------------------------------*/
	/*
	 * Compute the SHA-256 hash of the input file,
	 * then calculate the RSA signature of the hash.
	 */
	mbedtls_printf("\n  . Generating the RSA/SHA-256 signature");
	fflush(stdout);

	memset(filename, 0, 512);
	mbedtls_snprintf(filename, 512, "%s.raw", argv[3]);
	if ((ret = mbedtls_md_file(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
					filename, hash)) != 0) {
		mbedtls_printf(" failed\n  ! Could not open or read %s\n\n", argv[3]);
		goto exit;
	}
	{
		unsigned int pi;
		unsigned long *ulbuf = (unsigned long *)hash;

		printf("\n");
		for (pi = 0; pi < sizeof(hash) >> 3; pi++) {
			if (pi % 4 == 0)
				printf("\n");
			printf("%016lx ", (unsigned long)ulbuf[pi]);
		}
		printf("\n");
	}

/* -----------------------------------------------------------------------------
 * Write the hash into <filename>.hash
 -----------------------------------------------------------------------------*/
	mbedtls_snprintf(filename, 512, "%s.hash", argv[3]);

	if ((f = fopen(filename, "wb+")) == NULL) {
		ret = 1;
		mbedtls_printf(" failed\n  ! Could not create %s\n\n", filename);
		goto exit;
	}

	if (fwrite(hash, 1, sizeof(hash), f) != sizeof(hash)) {
		mbedtls_printf("failed\n  ! fwrite failed\n\n");
		goto exit;
	}

	fclose(f);

	mbedtls_printf("\n  . Done (created \"%s\")\n\n", filename);

/* -----------------------------------------------------------------------------
 * generate sig and Write signature
 -----------------------------------------------------------------------------*/
	if ((ret = mbedtls_pk_sign(&bpk, MBEDTLS_MD_SHA256, hash, 0, buf, &olen,
					mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_pk_sign returned %d\n\n", ret);
		goto exit;
	}

	{
		unsigned int pi;
		unsigned long *ulbuf = (unsigned long *)buf;

		for (pi = 0; pi < olen >> 3; pi++) {
			if (pi % 4 == 0)
				printf("\n");
			printf("%016lx ", (unsigned long)ulbuf[pi]);
		}
		printf("\n");
	}
	/*
	 * Write the signature into <filename>.sig
	 */
	mbedtls_snprintf(filename, 512, "%s.raw.sig", argv[3]);

	if ((f = fopen(filename, "wb+")) == NULL) {
		ret = 1;
		mbedtls_printf(" failed\n  ! Could not create %s\n\n", filename);
		goto exit;
	}

	if (fwrite(buf, 1, olen, f) != olen) {
		mbedtls_printf("failed\n  ! fwrite failed\n\n");
		goto exit;
	}

	fclose(f);

	mbedtls_printf("\n  . Done (created \"%s\")\n\n", filename);
#if 0
	do {
		unsigned int pi;
		mbedtls_pk_context *ppk = &bpk;
		mbedtls_rsa_context *ctx = (mbedtls_rsa_context *)ppk->pk_ctx;

	/*	printf("bpk:%p, ctx:%p, ctx->N:%p, ctx->N.n:%x, ctx->N.p:%p\n",
			&bpk, ctx, &ctx->N, ctx->N.n, ctx->N.p);
		*/
		/*
		 * Write the public key binary <filename>.pub
		 */
		mbedtls_snprintf(filename, 512, "%s.pub", argv[3] );

		if ((f = fopen(filename, "wb+")) == NULL) {
			ret = 1;
			mbedtls_printf(" failed\n  ! Could not create %s\n\n",
					filename);
			goto exit;
		}

		if (fwrite(ctx->N.p, 1, ctx->N.n * sizeof(ctx->N.p[0]), f) !=
				ctx->N.n * sizeof(ctx->N.p[0])) {
			mbedtls_printf("failed\n  ! fwrite failed\n\n");
			goto exit;
		}

		fclose(f);

		mbedtls_printf("\n  . Done (created \"%s\")\n\n", filename);
	} while (0);
#endif

exit:
	mbedtls_pk_free(&bpk);
	mbedtls_pk_free(&upk);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

#if defined(_WIN32)
	mbedtls_printf("  + Press Enter to exit this program.\n");
	fflush(stdout); getchar();
#endif

	return(ret);
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_RSA_C &&
          MBEDTLS_SHA256_C && MBEDTLS_PK_PARSE_C && MBEDTLS_FS_IO &&
          MBEDTLS_CTR_DRBG_C */
