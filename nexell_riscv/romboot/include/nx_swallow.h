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

//;==================================================================
//; Porting defines
//;==================================================================
//todo
#define NX_CLKSRCPLL0_FREQ	(550000000UL)
#define NX_CLKSRCPLL1_FREQ	(147456000UL)

#define USBD_VID		0x2375
#define USBD_PID		0x0322

#ifdef QEMU_RISCV
#define BASEADDR_SRAM		(0xFFFF0000)
#else
#define BASEADDR_SRAM		(0xFFFF0000)
#endif

#define INTERNAL_SRAM_SIZE	(64*1024)
#define SECONDBOOT_FSIZENCRC	(64*1024)
#define SECONDBOOT_FSIZE	(SECONDBOOT_FSIZENCRC-(128/8))
#define SECONDBOOT_STACK	0x2000

#endif	/* NX_ETACARINAE_H */
