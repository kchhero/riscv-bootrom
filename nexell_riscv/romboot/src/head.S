// See LICENSE for license details.
#ifdef QEMU_RISCV
#include <qemu_platform.h>
#else
#include "include/platform.h"        
#endif        
#include <smp.h>

	.section .text.init
	.option norvc
	.globl _prog_start
_prog_start:
	smp_pause(s1, s2)
	li sp, (SRAM_ADDR + 0x7fff)

        li a0,77        //boot mode, temporary bootoption value=77

        call iROMBOOT
        
	smp_resume(s1, s2)
	csrr a0, mhartid
//	la a1, dtb
	li s1, SRAM_ADDR
	jr s1
        
	.section .rodata
dtb:
        nop
        //.incbin DEVICE_TREE
