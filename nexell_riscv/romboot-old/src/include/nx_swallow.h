//;-------------------------------------------------------------------------------
//;
//; Copyright (C) 2009 Nexell Co., All Rights Reserved
//; Nexell Co. Proprietary & Confidential
//;
//; NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//; AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//; BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//; FOR A PARTICULAR PURPOSE.
//;
//; Module      :
//; File        :
//; Description : core define
//; Author      : Hans
//; History     :
//;     2014-01-12  Hans, for peridot
//;	2016-08-10  Hans, for etacarinae
//;	2017-10-09  Hans, for antares
//;-------------------------------------------------------------------------------

#ifndef NX_ETACARINAE_H
#define NX_ETACARINAE_H



#define PRIMARY_CPU 0
//;==================================================================
//; Porting defines
//;==================================================================
#define NX_CLKSRCPLL0_FREQ	(550000000UL)
#define NX_CLKSRCPLL1_FREQ	(147456000UL)
#define NX_CLKSRCPLL2_FREQ	( 96000000UL)
#define NX_CLKSRCPLL3_FREQ	(125000000UL)

#define USBD_VID		0x2375
#define USBD_PID		0x0322

#define GPIO_GROUP_A		0
#define GPIO_GROUP_B		1
#define GPIO_GROUP_C		2
#define GPIO_GROUP_D		3
#define GPIO_GROUP_E		4
#define GPIO_GROUP_F		5

#ifdef QEMU_RISCV
#define BASEADDR_SRAM		(0xFFFF0000)
#else
#define BASEADDR_SRAM		(0xFFFF0000)
#endif

#define INTERNAL_SRAM_SIZE	(64*1024)
#define SECONDBOOT_FSIZENCRC	(64*1024)
#define SECONDBOOT_FSIZE	(SECONDBOOT_FSIZENCRC-(128/8))
#define SECONDBOOT_STACK	0x2000

#define BASEADDR_NFMEM		(0x0C000000)
#define BASEADDR_DDRSDRAM	(0x40000000)

#endif	/* NX_ETACARINAE_H */
