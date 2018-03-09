/*
 *	FIPS-197 compliant AES implementation
 *
 *	Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *	SPDX-License-Identifier: Apache-2.0
 *
 *	Licensed under the Apache License, Version 2.0 (the "License"); you may
 *	not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *	http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *	WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 *
 *	This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 *	The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *	http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *	http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "aes.h"
#include "libplat.h"

#define mbedtls_printf printf


/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n) {
	volatile unsigned char *p = (unsigned char*)v; while (n--) *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (little endian)
 */
#ifndef GET_UINT32_LE
#define GET_UINT32_LE(n,b,i)				\
{							\
	(n) =	  ((uint32_t)(b)[(i) + 0] <<  0)	\
		| ((uint32_t)(b)[(i) + 1] <<  8)	\
		| ((uint32_t)(b)[(i) + 2] << 16)	\
		| ((uint32_t)(b)[(i) + 3] << 24);	\
}
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n,b,i)					\
{								\
	(b)[(i)	+ 0] = (unsigned char)(((n) >>  0) & 0xFF);	\
	(b)[(i) + 1] = (unsigned char)(((n) >>	8) & 0xFF);	\
	(b)[(i) + 2] = (unsigned char)(((n) >> 16) & 0xFF);	\
	(b)[(i) + 3] = (unsigned char)(((n) >> 24) & 0xFF);	\
}
#endif


/*
 * Forward S-box
 */
static const unsigned char FSb[256] =
{
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
	0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
	0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
	0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
	0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
	0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
	0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
	0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
	0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
	0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
	0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
	0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
	0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
	0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
	0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
	0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
	0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/*
 * Reverse S-box
 */
static const unsigned char RSb[256] =
{
	0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
	0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
	0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
	0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
	0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
	0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
	0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
	0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
	0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
	0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
	0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
	0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
	0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
	0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
	0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
	0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
	0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/*
 * Reverse tables
 */
#define RT \
	\
V(50,A7,F4,51), V(53,65,41,7E), V(C3,A4,17,1A), V(96,5E,27,3A), \
V(CB,6B,AB,3B), V(F1,45,9D,1F), V(AB,58,FA,AC), V(93,03,E3,4B), \
V(55,FA,30,20), V(F6,6D,76,AD), V(91,76,CC,88), V(25,4C,02,F5), \
V(FC,D7,E5,4F), V(D7,CB,2A,C5), V(80,44,35,26), V(8F,A3,62,B5), \
V(49,5A,B1,DE), V(67,1B,BA,25), V(98,0E,EA,45), V(E1,C0,FE,5D), \
V(02,75,2F,C3), V(12,F0,4C,81), V(A3,97,46,8D), V(C6,F9,D3,6B), \
V(E7,5F,8F,03), V(95,9C,92,15), V(EB,7A,6D,BF), V(DA,59,52,95), \
V(2D,83,BE,D4), V(D3,21,74,58), V(29,69,E0,49), V(44,C8,C9,8E), \
V(6A,89,C2,75), V(78,79,8E,F4), V(6B,3E,58,99), V(DD,71,B9,27), \
V(B6,4F,E1,BE), V(17,AD,88,F0), V(66,AC,20,C9), V(B4,3A,CE,7D), \
V(18,4A,DF,63), V(82,31,1A,E5), V(60,33,51,97), V(45,7F,53,62), \
V(E0,77,64,B1), V(84,AE,6B,BB), V(1C,A0,81,FE), V(94,2B,08,F9), \
V(58,68,48,70), V(19,FD,45,8F), V(87,6C,DE,94), V(B7,F8,7B,52), \
V(23,D3,73,AB), V(E2,02,4B,72), V(57,8F,1F,E3), V(2A,AB,55,66), \
V(07,28,EB,B2), V(03,C2,B5,2F), V(9A,7B,C5,86), V(A5,08,37,D3), \
V(F2,87,28,30), V(B2,A5,BF,23), V(BA,6A,03,02), V(5C,82,16,ED), \
V(2B,1C,CF,8A), V(92,B4,79,A7), V(F0,F2,07,F3), V(A1,E2,69,4E), \
V(CD,F4,DA,65), V(D5,BE,05,06), V(1F,62,34,D1), V(8A,FE,A6,C4), \
V(9D,53,2E,34), V(A0,55,F3,A2), V(32,E1,8A,05), V(75,EB,F6,A4), \
V(39,EC,83,0B), V(AA,EF,60,40), V(06,9F,71,5E), V(51,10,6E,BD), \
V(F9,8A,21,3E), V(3D,06,DD,96), V(AE,05,3E,DD), V(46,BD,E6,4D), \
V(B5,8D,54,91), V(05,5D,C4,71), V(6F,D4,06,04), V(FF,15,50,60), \
V(24,FB,98,19), V(97,E9,BD,D6), V(CC,43,40,89), V(77,9E,D9,67), \
V(BD,42,E8,B0), V(88,8B,89,07), V(38,5B,19,E7), V(DB,EE,C8,79), \
V(47,0A,7C,A1), V(E9,0F,42,7C), V(C9,1E,84,F8), V(00,00,00,00), \
V(83,86,80,09), V(48,ED,2B,32), V(AC,70,11,1E), V(4E,72,5A,6C), \
V(FB,FF,0E,FD), V(56,38,85,0F), V(1E,D5,AE,3D), V(27,39,2D,36), \
V(64,D9,0F,0A), V(21,A6,5C,68), V(D1,54,5B,9B), V(3A,2E,36,24), \
V(B1,67,0A,0C), V(0F,E7,57,93), V(D2,96,EE,B4), V(9E,91,9B,1B), \
V(4F,C5,C0,80), V(A2,20,DC,61), V(69,4B,77,5A), V(16,1A,12,1C), \
V(0A,BA,93,E2), V(E5,2A,A0,C0), V(43,E0,22,3C), V(1D,17,1B,12), \
V(0B,0D,09,0E), V(AD,C7,8B,F2), V(B9,A8,B6,2D), V(C8,A9,1E,14), \
V(85,19,F1,57), V(4C,07,75,AF), V(BB,DD,99,EE), V(FD,60,7F,A3), \
V(9F,26,01,F7), V(BC,F5,72,5C), V(C5,3B,66,44), V(34,7E,FB,5B), \
V(76,29,43,8B), V(DC,C6,23,CB), V(68,FC,ED,B6), V(63,F1,E4,B8), \
V(CA,DC,31,D7), V(10,85,63,42), V(40,22,97,13), V(20,11,C6,84), \
V(7D,24,4A,85), V(F8,3D,BB,D2), V(11,32,F9,AE), V(6D,A1,29,C7), \
V(4B,2F,9E,1D), V(F3,30,B2,DC), V(EC,52,86,0D), V(D0,E3,C1,77), \
V(6C,16,B3,2B), V(99,B9,70,A9), V(FA,48,94,11), V(22,64,E9,47), \
V(C4,8C,FC,A8), V(1A,3F,F0,A0), V(D8,2C,7D,56), V(EF,90,33,22), \
V(C7,4E,49,87), V(C1,D1,38,D9), V(FE,A2,CA,8C), V(36,0B,D4,98), \
V(CF,81,F5,A6), V(28,DE,7A,A5), V(26,8E,B7,DA), V(A4,BF,AD,3F), \
V(E4,9D,3A,2C), V(0D,92,78,50), V(9B,CC,5F,6A), V(62,46,7E,54), \
V(C2,13,8D,F6), V(E8,B8,D8,90), V(5E,F7,39,2E), V(F5,AF,C3,82), \
V(BE,80,5D,9F), V(7C,93,D0,69), V(A9,2D,D5,6F), V(B3,12,25,CF), \
V(3B,99,AC,C8), V(A7,7D,18,10), V(6E,63,9C,E8), V(7B,BB,3B,DB), \
V(09,78,26,CD), V(F4,18,59,6E), V(01,B7,9A,EC), V(A8,9A,4F,83), \
V(65,6E,95,E6), V(7E,E6,FF,AA), V(08,CF,BC,21), V(E6,E8,15,EF), \
V(D9,9B,E7,BA), V(CE,36,6F,4A), V(D4,09,9F,EA), V(D6,7C,B0,29), \
V(AF,B2,A4,31), V(31,23,3F,2A), V(30,94,A5,C6), V(C0,66,A2,35), \
V(37,BC,4E,74), V(A6,CA,82,FC), V(B0,D0,90,E0), V(15,D8,A7,33), \
V(4A,98,04,F1), V(F7,DA,EC,41), V(0E,50,CD,7F), V(2F,F6,91,17), \
V(8D,D6,4D,76), V(4D,B0,EF,43), V(54,4D,AA,CC), V(DF,04,96,E4), \
V(E3,B5,D1,9E), V(1B,88,6A,4C), V(B8,1F,2C,C1), V(7F,51,65,46), \
V(04,EA,5E,9D), V(5D,35,8C,01), V(73,74,87,FA), V(2E,41,0B,FB), \
V(5A,1D,67,B3), V(52,D2,DB,92), V(33,56,10,E9), V(13,47,D6,6D), \
V(8C,61,D7,9A), V(7A,0C,A1,37), V(8E,14,F8,59), V(89,3C,13,EB), \
V(EE,27,A9,CE), V(35,C9,61,B7), V(ED,E5,1C,E1), V(3C,B1,47,7A), \
V(59,DF,D2,9C), V(3F,73,F2,55), V(79,CE,14,18), V(BF,37,C7,73), \
V(EA,CD,F7,53), V(5B,AA,FD,5F), V(14,6F,3D,DF), V(86,DB,44,78), \
V(81,F3,AF,CA), V(3E,C4,68,B9), V(2C,34,24,38), V(5F,40,A3,C2), \
V(72,C3,1D,16), V(0C,25,E2,BC), V(8B,49,3C,28), V(41,95,0D,FF), \
V(71,01,A8,39), V(DE,B3,0C,08), V(9C,E4,B4,D8), V(90,C1,56,64), \
V(61,84,CB,7B), V(70,B6,32,D5), V(74,5C,6C,48), V(42,57,B8,D0)

#define V(a,b,c,d) 0x##a##b##c##d
static const uint32_t RT0[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
static const uint32_t RT1[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
static const uint32_t RT2[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
static const uint32_t RT3[256] = { RT };
#undef V

#undef RT

/*
 * Round constants
 */
static const uint32_t RCON[10] =
{
	0x00000001, 0x00000002, 0x00000004, 0x00000008,
	0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x0000001B, 0x00000036
};


void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
	memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
	if (ctx == NULL)
		return;

	mbedtls_zeroize(ctx, sizeof (mbedtls_aes_context));
}
/*
 *  * AES key schedule (encryption)
 *   */
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key)
{
	unsigned int i;
	uint32_t *RK;

	ctx->nr = 10;
	ctx->rk = RK = ctx->buf;

	for (i = 0; i < (128 >> 5); i++) {
		GET_UINT32_LE(RK[i], key, i << 2);
	}

	for (i = 0; i < 10; i++, RK += 4) {
		RK[4] = RK[0] ^ RCON[i] ^
			((uint32_t)FSb[(RK[3] >>  8) & 0xFF] <<  0) ^
			((uint32_t)FSb[(RK[3] >> 16) & 0xFF] <<  8) ^
			((uint32_t)FSb[(RK[3] >> 24) & 0xFF] << 16) ^
			((uint32_t)FSb[(RK[3] >>  0) & 0xFF] << 24);

		RK[5] = RK[1] ^ RK[4];
		RK[6] = RK[2] ^ RK[5];
		RK[7] = RK[3] ^ RK[6];
	}

	return (0);
}
/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key)
{
	int i, j, ret;
	mbedtls_aes_context cty;
	uint32_t *RK;
	uint32_t *SK;

	mbedtls_aes_init (&cty);

	ctx->rk = RK = ctx->buf;

	/* Also checks keybits */
	if ((ret = mbedtls_aes_setkey_enc(&cty, key)) != 0)
		goto exit;

	ctx->nr = cty.nr;


	SK = cty.rk + cty.nr * 4;

	*RK++ = *SK++;
	*RK++ = *SK++;
	*RK++ = *SK++;
	*RK++ = *SK++;

	for (i = ctx->nr - 1, SK -= 8; i > 0; i--, SK -= 8) {
		for (j = 0; j < 4; j++, SK++) {
			*RK++ = RT0[FSb[(*SK >>  0) & 0xFF]] ^
				RT1[FSb[(*SK >>  8) & 0xFF]] ^
				RT2[FSb[(*SK >> 16) & 0xFF]] ^
				RT3[FSb[(*SK >> 24) & 0xFF]];
		}
	}

	*RK++ = *SK++;
	*RK++ = *SK++;
	*RK++ = *SK++;
	*RK++ = *SK++;

exit:
	mbedtls_aes_free(&cty);

	return (ret);
}

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)		\
{							\
	X0 = *RK++ ^	RT0[(Y0	>>  0) & 0xFF] ^	\
			RT1[(Y3 >>  8) & 0xFF] ^	\
			RT2[(Y2 >> 16) & 0xFF] ^	\
			RT3[(Y1 >> 24) & 0xFF];		\
							\
	X1 = *RK++ ^	RT0[(Y1	>>  0) & 0xFF] ^	\
			RT1[(Y0 >>  8) & 0xFF] ^	\
			RT2[(Y3 >> 16) & 0xFF] ^	\
			RT3[(Y2 >> 24) & 0xFF];		\
							\
	X2 = *RK++ ^	RT0[(Y2	>>  0) & 0xFF] ^	\
			RT1[(Y1 >>  8) & 0xFF] ^	\
			RT2[(Y0 >> 16) & 0xFF] ^	\
			RT3[(Y3 >> 24) & 0xFF];		\
							\
	X3 = *RK++ ^	RT0[(Y3	>>  0) & 0xFF] ^	\
			RT1[(Y2 >>  8) & 0xFF] ^	\
			RT2[(Y1 >> 16) & 0xFF] ^	\
			RT3[(Y0 >> 24) & 0xFF];		\
}

/*
 * AES-ECB block decryption
 */
int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx,
		const unsigned char input[16],
		unsigned char output[16])
{
	int i;
	uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

	RK = ctx->rk;

	GET_UINT32_LE(X0, input,  0); X0 ^= *RK++;
	GET_UINT32_LE(X1, input,  4); X1 ^= *RK++;
	GET_UINT32_LE(X2, input,  8); X2 ^= *RK++;
	GET_UINT32_LE(X3, input, 12); X3 ^= *RK++;

	for (i = (ctx->nr >> 1) - 1; i > 0; i--) {
		AES_RROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);
		AES_RROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3);
	}

	AES_RROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);

	X0 = *RK++ ^ \
		((uint32_t)RSb[(Y0 >>  0) & 0xFF] <<  0) ^
		((uint32_t)RSb[(Y3 >>  8) & 0xFF] <<  8) ^
		((uint32_t)RSb[(Y2 >> 16) & 0xFF] << 16) ^
		((uint32_t)RSb[(Y1 >> 24) & 0xFF] << 24);

	X1 = *RK++ ^ \
		 ((uint32_t)RSb[(Y1 >>  0) & 0xFF] <<  0) ^
		 ((uint32_t)RSb[(Y0 >>  8) & 0xFF] <<  8) ^
		 ((uint32_t)RSb[(Y3 >> 16) & 0xFF] << 16) ^
		 ((uint32_t)RSb[(Y2 >> 24) & 0xFF] << 24);

	X2 = *RK++ ^ \
		 ((uint32_t)RSb[(Y2 >>  0) & 0xFF] <<  0) ^
		 ((uint32_t)RSb[(Y1 >>  8) & 0xFF] <<  8) ^
		 ((uint32_t)RSb[(Y0 >> 16) & 0xFF] << 16) ^
		 ((uint32_t)RSb[(Y3 >> 24) & 0xFF] << 24);

	X3 = *RK++ ^ \
		 ((uint32_t)RSb[(Y3 >>  0) & 0xFF] <<  0) ^
		 ((uint32_t)RSb[(Y2 >>  8) & 0xFF] <<  8) ^
		 ((uint32_t)RSb[(Y1 >> 16) & 0xFF] << 16) ^
		 ((uint32_t)RSb[(Y0 >> 24) & 0xFF] << 24);

	PUT_UINT32_LE(X0, output,  0);
	PUT_UINT32_LE(X1, output,  4);
	PUT_UINT32_LE(X2, output,  8);
	PUT_UINT32_LE(X3, output, 12);

	return (0);
}

int mbedtls_aes_decrypt_cbc(mbedtls_aes_context *ctx,
		size_t length,
		unsigned char iv[16],
		const unsigned char *input,
		unsigned char *output)
{
	int i;
	unsigned char temp[16];

	if (length % 16)
		return (MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH);

	while (length > 0) {
		memcpy(temp, input, 16);
		mbedtls_internal_aes_decrypt(ctx, input, output);

		for (i = 0; i < 16; i++)
			output[i] = (unsigned char)(output[i] ^ iv[i]);

		memcpy(iv, temp, 16);

		input  += 16;
		output += 16;
		length -= 16;
	}

	return (0);
}

const unsigned int bootkey[4] = {
//	0x315AC370, 0x74ACD6E1, 0x2D7A6104, 0x51F8B88A
	0x70C35A31, 0xE1D6AC74, 0x04617A2D, 0x8AB8F851
};
unsigned int *Decrypt(unsigned int *src, unsigned int *dst,
		unsigned int iv[], unsigned int size)
{
	mbedtls_aes_context ctx;
	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_dec(&ctx, (const unsigned char *)bootkey);
	mbedtls_aes_decrypt_cbc(&ctx, size, (unsigned char *)iv,
			(const unsigned char *)src, (unsigned char *)dst);

	mbedtls_aes_free(&ctx);
	return iv;
}
#if 0
static int atok(const char *string, unsigned char *pkey)
{
	char ch;
	unsigned char key = 0, cnt = 0;

	while (ch = *string++) {
		key <<= 4;
		if (ch >= '0' && ch <= '9') {
			key |= ch - '0' +  0;
		} else if (ch >= 'a' && ch <= 'f') {
			key |= ch - 'a' + 10;
		} else if (ch >= 'A' && ch <= 'F') {
			key |= ch - 'A' + 10;
		}
		if ((cnt % 2) == 1) {
			int i;
			for (i = 63; i > 0; i--)
				pkey[i] = pkey[i - 1];
			pkey[0] = key;
			key = 0;
		}
		cnt++;
	}

#if 0
	int k;
	for (k = 31; k >= 0; k--)
		printf("%02x", pkey[k]);
	printf("\n");
#endif
	if (cnt % 32)	// 128, 192, 256
		return 0;

	return 1;
}
#endif

