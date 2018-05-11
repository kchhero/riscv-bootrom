// See LICENSE for license details.
#include <nx_swallow.h>

.section .text.start, "ax", @progbits
.globl bl1start
bl1start:
        call bl1main

stop_and_go:
        j stop_and_go

.section .text.hang, "ax", @progbits
.globl _hang
_hang:
        csrr a0, mhartid
