#ifndef __NX_PLATFORM__H__
#define __NX_PLATFORM__H__

#ifdef QEMU_RISCV
#include "nx_qemu_platform.h"
#else
#include "nx_swallow_platform.h"
#endif

//CPU core num
#define RISCV_NUM_CORES 2

#define SRAM_ADDR 0xFFFF0000
#define SRAM_SIZE 0x00010000

#endif //__NX_PLATFORM__H__
