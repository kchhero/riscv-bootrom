// See LICENSE for license details.
#ifndef _KPRINTF_H
#define _KPRINTF_H

#define PHY_BASEADDR_DUMMY_MODULE (0x20A00000)
#define MAX_MESSAGE_LEN (128)
extern void _kputs(const char *s);
extern void kputc(char c);

#include "nx_swallow.h"
#include "nx_uart.h"
#include <stdint.h>

#define REG32(p, i)	((p)[(i) >> 2])

#define _CONCAT3(A, B, C) A ## B ## C

extern void kputs(const char *);
extern void _dprintf(const char *, ...);


#define dprintf(s, ...)	_dprintf((s), ##__VA_ARGS__)
#define dputs(s)	kputs((s))

#endif /* _KPRINTF_H */
