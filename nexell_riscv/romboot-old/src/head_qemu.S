// See LICENSE for license details.
#ifdef QEMU_RISCV
#include "include/qemu_platform.h"
#else
#include "include/platform.h"        
#endif        
#include <smp.h>
#include "include/common.h"

#define NONSMP_HART 1
        
  .section .text.init
  .option norvc
  .globl _prog_start
_prog_start:
//  smp_pause(s1, s2)
        li sp, (PAYLOAD_DEST + 0x7fff)
        csrr a0, mhartid
        ld t0,24(t0)
        
  call iROMBOOT
//  smp_resume(s1, s2)
//  csrr a0, mhartid
//  la a1, dtb
  li s1, 0xFFFF0000//PAYLOAD_DEST
//  li s1, (PAYLOAD_DEST 0x600)
  jr s1

  .section .rodata
dtb:
        nop
        //.incbin DEVICE_TREE
