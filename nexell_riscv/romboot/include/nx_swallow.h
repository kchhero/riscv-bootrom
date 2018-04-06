#ifndef NX_SWALLOW_H
#define NX_SWALLOW_H

#ifdef QEMU_RISCV
#include "nx_qemu_platform.h"
#else
#include "nx_swallow_platform.h"
#endif

//CPU core num
#define RISCV_NUM_CORES 2

#ifdef QEMU_RISCV
#define SRAM_ADDR (0xFFFF0000)
#define SRAM_SIZE (0x10000) //64KB
#else
#define SRAM_ADDR (0x40000000)
#define SRAM_SIZE (0x8000) //32KB
#endif

#define USBD_VID		0x2375
#define USBD_PID		0x0322

#define BASEADDR_SRAM		SRAM_ADDR
#define INTERNAL_SRAM_SIZE	SRAM_SIZE
#define SECONDBOOT_FSIZENCRC	(64*1024)
#define SECONDBOOT_FSIZE	(SECONDBOOT_FSIZENCRC-(128/8))
#define SECONDBOOT_STACK	0x2000

#endif	/* NX_ETACARINAE_H */
