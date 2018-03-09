/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: bl0
 * File		: libarm.c
 * Description	:
 * Author	: Hans
 * History	: 2014.08.20 Hans create
 * 		2017.10.09 Hans modify to use operator
 */
#include <nx_type.h>
#include "nx_antares.h"


void delay(U32 us)
{
#ifdef NXP4330
	register volatile unsigned int   count, temp;

	for (count = 0; count < us; count++) {
		temp ^= count;
	}
#else
	register volatile unsigned int   count, temp;

	for (count = 0; count < us; count++) {
		temp ^= count;
	}
#endif
}
int __udivmodsi4(int, int, int);
signed int __aeabi_idiv(signed int num, signed int den)
{
	signed int minus = 0;
	signed int v;

	if (num < 0) {
		num = -num;
		minus = 1;
	}
	if (den < 0) {
		den = -den;
		minus ^= 1;
	}

	v = __udivmodsi4(num, den, 0);
	if (minus)
		v = -v;

	return v;
}
