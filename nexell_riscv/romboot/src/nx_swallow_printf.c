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

#include <nx_bootheader.h>
#include <nx_swallow_printf.h>
#include <nx_debug.h>

#ifndef SOC_SIM

#define nx_putchar(c) DebugPutch(c)

void printchar(char **str, int c)
{
	if (str) {
		**str = c;
		++(*str);
	} else{
            (void)nx_putchar((char)c);
        }
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

int prints(char **out, const char *string, int width, int pad)
{
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
			printchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printchar(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printchar(out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 12
int printi(char **out, int i, int b, int sg, int width, int pad,
		  int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, pad);
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
			printchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, pad);
}

int print(char **out, const char *format, va_list args)
{
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
				pc += prints(out, s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd') {
				pc += printi(out, va_arg(args, int), 10, 1,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'x') {
				pc += printi(out, va_arg(args, int), 16, 0,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'X') {
				pc += printi(out, va_arg(args, int), 16, 0,
					     width, pad, 'A');
				continue;
			}
			if (*format == 'u') {
				pc += printi(out, va_arg(args, int), 10, 0,
					     width, pad, 'a');
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then
				 * pushed on the stack */
				scr[0] = (char)va_arg(args, int);
				scr[1] = '\0';
				pc += prints(out, scr, width, pad);
				continue;
			}
		} else {
		out:
			printchar(out, *format);
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
    va_list args;
    va_start(args, format);
    return print(0, format, args);
}

#else //SOC_SIM
static void kputc(char c)
{
    volatile char *reg = (char*)PHY_BASEADDR_DUMMY_MODULE;
    *reg = (char)c;
}
//static inline void _kputs(const char *s)
static void _kputs(const char *s)
{
    char c;
    for (; (c = *s) != '\0'; s++)
	kputc(c);
}

/* void kputs(const char *s) */
/* { */
/*     volatile char *reg = (char*)PHY_BASEADDR_DUMMY_MODULE; */
/*     char c; */
/*     for (; (c = *s) != '\0'; s++) */
/*        *reg = c; */
/* } */

void _dprintf(const char *fmt, ...)
{
	va_list vl;
	int is_format, is_long, is_char;
	char c;

	va_start(vl, fmt);
	is_format = 0;
	is_long = 0;
	is_char = 0;
	while ((c = *fmt++) != '\0') {
		if (is_format) {
			switch (c) {
			case 'l':
				is_long = 1;
				continue;
			case 'h':
				is_char = 1;
				continue;
			case 'x': {
				unsigned long n;
				long i;
				if (is_long) {
					n = va_arg(vl, unsigned long);
					i = (sizeof(unsigned long) << 3) - 4;
				} else {
					n = va_arg(vl, unsigned int);
					i = is_char ? 4 : (sizeof(unsigned int) << 3) - 4;
				}
				for (; i >= 0; i -= 4) {
					long d;
					d = (n >> i) & 0xF;
					kputc(d < 10 ? '0' + d : 'a' + d - 10);
				}
				break;
			}
			case 's':
				_kputs(va_arg(vl, const char *));
				break;
			case 'c':
				kputc(va_arg(vl, int));
				break;
			}
			is_format = 0;
			is_long = 0;
			is_char = 0;
		} else if (c == '%') {
			is_format = 1;
		} else {
			kputc(c);
		}
	}
	va_end(vl);
}

#endif //SOC_SIM
