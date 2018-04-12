// See LICENSE for license details.
#include <nx_swallow.h>
#include <nx_smp.h>

	.section .text.init
	.option norvc
	.globl _prog_start
_prog_start:
	smp_pause(s1, s2)

	li sp, (DRAM_ADDR + (DRAM_SIZE-32768)) //stack size 32KB

        li a0,22        //boot mode, temporary bootoption value=22

        call romboot
        
	smp_resume(s1, s2)
	csrr a0, mhartid
//	la a1, dtb     //dtb?
	li s1, DRAM_ADDR
	jr s1
        
	.section .rodata
dtb:
        nop
        //.incbin DEVICE_TREE
