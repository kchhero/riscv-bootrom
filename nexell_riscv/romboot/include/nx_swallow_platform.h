#ifndef __NX_CHIP_SFR__H__
#define __NX_CHIP_SFR__H__

#include "nx_const.h"

#ifdef	__cplusplus
extern "C"
{
#endif

#define NULL 0

#define CLINT_CTRL_ADDR _AC(0x2000000,UL)
#define CLINT_CTRL_SIZE _AC(0x10000,UL)

// Helper functions
#define _REG64(p, i) (*(volatile uint64_t *)((p) + (i)))
#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))
#define _REG16(p, i) (*(volatile uint16_t *)((p) + (i)))

#define SET_BITS(reg, mask, value) if ((value) == 0) { (reg) &= ~(mask); } else { (reg) |= (mask); }
#define CLINT_REG(offset) _REG32(CLINT_CTRL_ADDR, offset)
#define CLINT_REG64(offset) _REG64(CLINT_CTRL_ADDR, offset)
#define DEBUG_REG64(offset) _REG64(DEBUG_CTRL_ADDR, offset)
#define ERROR_REG64(offset) _REG64(ERROR_CTRL_ADDR, offset)


#define ReadIODW(Addr) (*(volatile unsigned int*)(Addr))
#define ReadIOW(Addr) (*(volatile unsigned short*)(Addr))
#define ReadIOB(Addr) (*(volatile unsigned char*)Addr)
#define WriteIODW(Addr,Data) (*(volatile unsigned int*)Addr)=((unsigned int)Data)
#define WriteIOW(Addr,Data) (*(volatile unsigned short*)Addr)=((unsigned short)Data)
#define WriteIOB(Addr,Data) (*(volatile unsigned char*)Addr)=((unsigned char)Data)

//--------------------------------------------------------------------------


#ifdef	__cplusplus
}
#endif

#endif
