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
#include <nx_bootheader.h>
#include <nx_type.h>

#include <nx_cmu.h>

#include <nx_clock.h>

#include <iSDHCBOOT.h>

#ifdef QEMU_RISCV
#include <kprintf.h>
#include <qemu_platform.h>
#else
#include <printf.h>
#include <platform.h>
#endif


const struct cmu_device_clk sssclk[2] = {
	{0x200, 0, 11, NX_CLKSRC_PLL0, 5},	// axi
	{0x200, 0, 6, NX_CLKSRC_PLL0, 10}	// apb
};

//	73FC7B44B996F9990261A01C9CB93C8F
const unsigned int iv[4] = {
//	0x73FC7B44, 0xB996F999, 0x0261A01C, 0x9CB93C8F
	0x447BFC73, 0x99F996B9, 0x1CA06102, 0x8F3CB99C
};

struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_SRAM;
//------------------------------------------------------------------------------
unsigned int iROMBOOT(unsigned int OrgBootOption)
{
    //    unsigned int option = 1 | OrgBootOption & 0xFF;
    const U32 option = OrgBootOption - 20;
    /* if (asdf >= 1) { */
    /*     _dprintf("<<bootrom>> kkkk = 0x%x\n", kkkk);         */
    _dprintf("<<bootrom>> option %s = 0x%x\n", __func__, option); 
    /*     _dprintf("<<bootrom>> %s asdf = 0x%x\n",__func__, asdf); */
    /* _dprintf("<<bootrom>> %s asdf = 0x%x\n",__func__, bootoptionss); */
    /* } */
    

    /* if (option & (1 << EXNOBOOTMSG)) */
    /*     option = (option & ~(1 << EXNOBOOTMSG)) | 1 << EXNOBOOTMSG_SAVE; */

    /* // check low boot config written and valid */
    /* if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) { */
    /*     soption = eBootOption; */
    /*     eBootOption &= 0x7FF;	// mask VALIDFIELD & BOOTCFGUSE, HALT, SPEED */
    /*     //		option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY; */
    /*     option |= eBootOption; */
    /* } else { */
    /*     // check high boot config and valid */
    /*     eBootOption >>= 13; */
    /*     if ((eBootOption & 0x3 << VALIDFIELD) == 0x2 << VALIDFIELD) { */
    /*         soption = eBootOption; */
    /*         eBootOption &= 0x7FF; */
    /*         //			option &= 1 << DECRYPT | 1 << EXNOBOOTMSG_SAVE | 3 << VERIFY; */
    /*         option |= eBootOption; */
    /*     } else { */
    /*         // any efuse value does not written. */
    /*         // ext rstcfg has no next port information. */
    /*         // default: port 1 */
    /*         option |= (0x1 + 1) << NEXTTRYPORT;	// 0: no next try */
    /*     } */
    /* } */

    //todo check!!!
    //	SetBootOption(option);

    //	SetMempoolPtr(PHY_BASEADDR_CAN0_MODULE_RAM);
    //	SetStringPtr((unsigned int *)PHY_BASEADDR_CAN1_MODULE_RAM);
    //	Setbl0fnPtr(&bl0fn);
#ifndef QEMU_RISCV
    U32 speedup = 0;
    /* if ((soption & 1 << SPEEDUP) || (OrgBootOption & 1 << ICACHE)) */
        speedup = 1;
    /* else if (soption & 1 << SPEEDUP) */
    /*     speedup = 2; */
	
    setcpuclock(speedup);
    setsystemclock(speedup);

	// external usb boot is top priority, always first checked.
	if ((OrgBootOption & 0x7 << BOOTMODE) == (USBBOOT << BOOTMODE)) {
		_dprintf("force usb boot\r\n");
		goto lastboot;
	}

	do {
		CBOOL Result = CFALSE;
//--------------------------------------------------------------------------
// iROMBOOT mode : option [2:0]
//--------------------------------------------------------------------------
		switch ((option >> BOOTMODE) & 0x7) {
		default:
			_dprintf("no support boot mode(%x)\r\n", option);
		case USBBOOT:
			Result = iUSBBOOT(option);
			break;
		case SDBOOT:	// iSDHCBOOT (SD/MMC/eSD/eMMC)
			Result = iSDXCBOOT(option);
			break;
		}

		if (Result)
			break;

		unsigned int next_port = (option >> NEXTTRYPORT) & 0x3;
		if (0 == next_port)
			goto lastboot;

		option &= ~0x67UL;	// mask boot mode, portnumber for SDBOOT
		option |= next_port << PORTNUMBER;

		_dprintf("update boot\r\n");

		Result = iSDXCBOOT(option);

		if (Result)
			break;

lastboot:
		iUSBBOOT(option);
	} while (0);


	int ret = 1;
	if (ret) {
		struct nx_bootinfo *pbi = (struct nx_bootinfo *)BASEADDR_SRAM;
		_dprintf("Launch to 0x%X\r\n", pbi->StartAddr);
//		void (*plaunch)(void) = (void (*)(void))pbi->StartAddr;
//		plaunch();
		return pbi->StartAddr;
	} else
		goto lastboot;
#else
    {
        U32 result;
        struct nx_bootinfo *pbi = (struct nx_bootinfo *)BASEADDR_SRAM;
        //    _dprintf("<<bootrom>> %s option = 0x%x\n",__func__, option);
        if (option) {
            _dprintf("-----------------\n");            
            result = iSDXCBOOT(1);//option);

        }
        else
            _dprintf("asdfasdfasdfasdfasdf\n");
        
        //	while (1);
        __asm__ __volatile__ ("fence.i" : : : "memory");
        //        _dprintf("Launch to 0x%x\r\n", pbi->StartAddr);
        //		void (*plaunch)(void) = (void (*)(void))pbi->StartAddr;
        //		plaunch();
        //        return pbi->StartAddr;
        return 18;
    }
#endif
}
