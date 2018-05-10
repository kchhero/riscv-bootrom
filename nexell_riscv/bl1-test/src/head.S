// See LICENSE for license details.
#include <nx_swallow.h>

.section .text.start, "ax", @progbits
.globl bl1start
bl1start:
        call bl1main

        li s3, 0
        addi s3, a0, 0

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
