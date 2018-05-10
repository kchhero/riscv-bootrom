// See LICENSE for license details.
#include <nx_swallow.h>
#include <nx_smp.h>

.section .text.start, "ax", @progbits
.globl _start
_start:
	smp_pause(s1, s2)        
        li sp, (SRAM_ADDR + SRAM_SIZE - VECTOR_SIZE)
//        csrr a0, mhartid
//        bnez a0, stop_and_go

	li a0,2        //boot mode, XIP=1, SDCARD=2

        call romboot

        //mv s3, a0
        li s3, 0
        addi s3, a0, 0
        
      	smp_resume(s1, s2) //        ;; li a0, 0 //SRAM_ADDR //        ;; jr a0
	jr s3

stop_and_go:
        j stop_and_go

.section .text.hang, "ax", @progbits
.globl _hang
_hang:
        csrr a0, mhartid

1:
        wfi
        j 1b
