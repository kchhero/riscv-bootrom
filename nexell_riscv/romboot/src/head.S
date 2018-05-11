// See LICENSE for license details.
#include <nx_swallow.h>
#include <nx_smp.h>

.section .text.start, "ax", @progbits
.globl _start
_start:
	smp_pause(s1, s2)        
        li sp, (SRAM_ADDR + SRAM_SIZE - VECTOR_SIZE)

        call romboot

        li s3, 0
        addi s3, a0, 0
        
        //smp_resume(s1, s2) //        ;; li a0, 0 //SRAM_ADDR //        ;; jr a0
        fence.i

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
