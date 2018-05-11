////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Nexell Co., Ltd All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	:
// File		: iROMBOOT.c
// Description	:
// Author	: Hans
// History	:
//		2014-01-12	Hans create
//		2017-10-09	Hans modified for NXP3220
//		2018-01-15	Hans sim done
////////////////////////////////////////////////////////////////////////////////
#include <nx_swallow.h>
#include <nx_qemu_sim_printf.h>

#ifndef __ASSEMBLY__
#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw " #csr ", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})
#endif

int bl1main()
{
    //    volatile unsigned int* pCLINT0Reg = (unsigned int*)(0x02000000);
    volatile unsigned int* pCLINT1Reg = (unsigned int*)(0x02000004);
    _dprintf("BL1-TEST Good2\n");
    *pCLINT1Reg = 0x1;


   
    while(1);
    return ;
}
