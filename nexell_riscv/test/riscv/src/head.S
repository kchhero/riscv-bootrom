// See LICENSE for license details.
#include <platform.h>
#include <smp.h>

#define NONSMP_HART 1
        
  .section .text.init
  .option norvc
  .globl _prog_start
_prog_start:
//  smp_pause(s1, s2)
        li sp, (SRAM_ADDR + 0x7fff)
        csrr a0, mhartid
        ld t0,24(t0)
        
  call testMain
//  smp_resume(s1, s2)
//  csrr a0, mhartid
//  la a1, dtb
  li s1, SRAM_ADDR//PAYLOAD_DEST
//  li s1, (PAYLOAD_DEST 0x600)
  jr s1

  .section .rodata
dtb:
        nop
        //.incbin DEVICE_TREE
