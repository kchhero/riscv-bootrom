/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module          : bl0
 * File            : printf.c
 * Description     :
 * Author          : Hans
 * History         :
 */

#include "include/nx_type.h"
#include "include/nx_swallow_bootoption.h"
#include "printf.h"

#include "libplat.h"
#include "fnptr.h"

#define nx_putchar DebugPutch

void printchar(char **str, int c)
{
	if (str) {
		**str = c;
		++(*str);
	} else
		(void)nx_putchar((S8)c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

int prints(char **out, const char *string, int width, int pad)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for (; width > 0; --width) {
			pbl0fn->printchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		pbl0fn->printchar(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		pbl0fn->printchar(out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 12
int printi(char **out, int i, int b, int sg, int width, int pad,
		  int letbase)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return pbl0fn->prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u) {
		t = u % b;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			pbl0fn->printchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}

	return pc + pbl0fn->prints(out, s, width, pad);
}

int print(char **out, const char *format, va_list args)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's') {
				register char *s = va_arg(args, char *);
				pc += pbl0fn->prints(out, s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd') {
				pc += pbl0fn->printi(out, va_arg(args, int), 10, 1,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'x') {
				pc += pbl0fn->printi(out, va_arg(args, int), 16, 0,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'X') {
				pc += pbl0fn->printi(out, va_arg(args, int), 16, 0,
					     width, pad, 'A');
				continue;
			}
			if (*format == 'u') {
				pc += pbl0fn->printi(out, va_arg(args, int), 10, 0,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then
				 * pushed on the stack */
				scr[0] = (char)va_arg(args, int);
				scr[1] = '\0';
				pc += pbl0fn->prints(out, scr, width, pad);
				continue;
			}
		} else {
		out:
			pbl0fn->printchar(out, *format);
			++pc;
		}
	}
	if (out)
		**out = '\0';
	va_end(args);

	return pc;
}

int _dprintf(const char *format, ...)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	va_list args;
	if ((GetBootOption() & 1 << NOBOOTMSG) &&
			(GetBootOption() & 1 << EXNOBOOTMSG_SAVE))
		return 0;
		
	va_start(args, format);
	if (!(GetBootOption() & 1 << NOBOOTMSG) &&
			!(GetBootOption() & 1 << EXNOBOOTMSG_SAVE))
		return pbl0fn->print(0, format, args);

	/* NOBOOTMSG ^ EXNOBOOTMSG == 1 */
	char *stringptr = (char *)GetStringPtr();
	int cnt = pbl0fn->print(&stringptr, format, args);
	SetStringPtr(GetStringPtr() + cnt);
	return cnt;
}

#if 0
int sprintf(char *out, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	return print(&out, format, args);
}
#endif
