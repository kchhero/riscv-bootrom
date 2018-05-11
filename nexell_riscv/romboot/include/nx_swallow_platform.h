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
