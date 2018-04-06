#include <nx_pll.h>
#include <nx_clock.h>
#include <nx_cpuif_regmap.h>
#include <nx_chip_sfr.h>

#ifdef QEMU_RISCV
#include <nx_qemu_platform.h>
#else
#include <nx_swallow_platform.h>
#endif

unsigned int *__PLL_CPUIF_BASEADDR[NUMBER_OF_PLL_CPUIF_MODULE];

/* static struct NX_CMU_RegisterSet *const pCMUCPU = (struct NX_CMU_RegisterSet *)PHY_BASEADDR_CMU_CPU_MODULE; */
/* static struct NX_CMU_RegisterSet *const pCMUSYS = (struct NX_CMU_RegisterSet *)PHY_BASEADDR_CMU_SYS_MODULE; */

/* static struct NX_PLL_RegisterSet *const pPLLCPU = (struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPU_MODULE; */
/* static struct NX_PLL_RegisterSet *const pPLL0 =   (struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL0_MODULE; */
extern _SFR_INFO SFR_INFO;

struct nx_pll_info {
	unsigned short m;
	unsigned short s;
} const pllpms[] =
{
	{125, 3},	/* 24M * 125 / 3 / 2^3 = 125MHz */
	{250, 2},	/* 24M * 250 / 3 / 2^2 = 500MHz */
	{200, 1},	/* 24M * 200 / 3 / 2^1 = 800MHz */
	{250, 1}	/* 24M * 250 / 3 / 2^1 = 1000MHz */
};
struct nx_cpuclkdiv_info {
	unsigned char axi;
	unsigned char atclk;
	unsigned char cntclk;
	unsigned char tsclk;
	unsigned char dbgapb;
	unsigned char apb;
} const cpuclkdiv[] = {
	{
		1,	/* axi: 125 / 1 = 125MHz */
		1,	/* atclk: 125 / 1 = 125MHz */
		1,	/* cntclk: 125 / 1 = 125MHz */
		1,	/* tsclk: 125 / 1 = 125MHz */
		2,	/* dbgapb: 125 / 2 = 62.5MHz */
		2	/* apb: 125 / 2 = 62.5MHz */
	}, {
		1,	/* axi: 500 / 2 = 250MHz */
		3,	/* atclk: 500 / 4 = 125MHz */
		3,	/* cntclk: 500 / 4 = 125MHz */
		3,	/* tsclk: 500 / 4 = 125MHz */
		4,	/* dbgapb: 500 / 5 = 100MHz */
		4	/* apb: 500 / 5 = 100MHz */
	}, {
		1,	/* axi: 800 / 2 = 400MHz */
		3,	/* atclk: 800 / 4 = 200MHz */
		3,	/* cntclk: 800 / 4 = 200MHz */
		3,	/* tsclk: 800 / 4 = 200MHz */
		7,	/* dbgapb: 800 / 8 = 100MHz */
		7	/* apb: 800 / 8 = 100MHz */
	}
};

/* struct NX_PLL_RegisterSet *const gppll[5] = { */
/* 	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPUIF0_MODULE, */
/* 	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPUIF1_MODULE, */
/* 	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPUIF2_MODULE, */
/* 	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPUIF3_MODULE, */
/* 	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPUIF4_MODULE, */
/* }; */
/* #define PLL_P	0 */
/* #define PLL_M	16 */
/* #define PLL_S	0 */
/* #define PLL_K	16 */

/* #define __g_OSC_KHz	24000 */
/* unsigned int NX_PLL_GetFrequency(unsigned int PllNumber) */
/* { */
/* 	struct NX_PLL_RegisterSet *const ppll = gppll[PllNumber]; */
/* 	unsigned int regvalue, regvalue1, nP, nM, nS, nK; */
/* 	unsigned int temp = 0; */

/* 	regvalue = ppll->CFG1; */
/* 	regvalue1 = ppll->CFG2; */
/* 	nP = (regvalue >> PLL_P) & 0x3F; */
/* 	nM = (regvalue >> PLL_M) & 0x3FF; */
/* 	nS = (regvalue1 >> PLL_S) & 0xFF; */
/* 	nK = (regvalue1 >> PLL_K) & 0xFFFF; */

/* 	if (nK) */
/* 		temp = ((((nK * 1000) / 65536) * __g_OSC_KHz) / nP) >> nS; */

/* 	temp = ((((nM * __g_OSC_KHz) / nP) >> nS) * 1000) + temp; */
/* 	return temp; */
/* } */


struct nx_sysclkdiv_info {
	unsigned char axi;
	unsigned char apb;
	unsigned char hsif_axi;
	unsigned char hsif_apb;
	unsigned char cmu_apb;
	unsigned char smc_axi;
};

void nxSetDeviceClock(const CMU_DEVICE_CLK *pdclk, int num, int enb)
{
	int i;
	for (i = 0; i < num; i++) {
		int of = pdclk[i].offset / 0x200;
		int sgrp = pdclk[i].srcbit >> 5;
		int sbit = pdclk[i].srcbit & 0x1F;
		int egrp = pdclk[i].clkenbbit >> 5;
		int ebit = pdclk[i].clkenbbit & 0x1F;

		if (!enb) {
			pCMUSRC->CMUSRC.SSRCOFF[sgrp] = 1 << sbit;
			pCMUSRC->CMUBLK[of - 1].CRST[egrp] = 1 << ebit;
			return;
		}


		pCMUSRC->CMUBLK[of - 1].CLKMUXSEL = NX_CLKSRC_OSC;
		pCMUSRC->CMUBLK[of - 1].CCLKENB[egrp] = 1 << ebit;
		pCMUSRC->CMUSRC.CSRCOFF[sgrp] = 1 << sbit;
		pCMUSRC->CMUBLK[of - 1].DIV[0] = pdclk[i].div;
		pCMUSRC->CMUBLK[of - 1].CLKMUXSEL = pdclk[i].clksrc;

		pCMUSRC->CMUBLK[of - 1].SRST[egrp] = 1 << ebit;

		pCMUSRC->CMUBLK[of - 1].SCLKENB[egrp] = 1 << ebit;
	}
}


/* static unsigned int  NX_PLL_TYPE[NUMBER_OF_PLL_MODULE] = { */
/*     TYPE_PLL_0    */
/*     ,TYPE_PLL_1    */
/*     ,TYPE_PLL_CPU  */
/*     ,TYPE_PLL_DDR0 */
/*     ,TYPE_PLL_DDR1 */
/* }; */

void __PLL_CPUIF_SET_BASEADDR (unsigned int moduleindex, unsigned int *baseaddr) {
    __PLL_CPUIF_BASEADDR[moduleindex] = baseaddr;
} 

void NX_PLL_SetBaseAddress(unsigned int inst_index, unsigned int BaseAddress)
{
    //CHECK_MODULE_INDEX;
    __PLL_CPUIF_SET_BASEADDR (inst_index, BaseAddress);
}

struct __nx_cpuif_PLL_CPUIFregmap_struct__ g_PLL_CPUIFregmap[NUMBER_OF_PLL_CPUIF_MODULE];

void NX_PLL_SetOSCMUX( unsigned int inst_index, unsigned int MUXSEL )
{
    nx_cpuif_reg_write_one(g_PLL_CPUIFregmap[inst_index].OSCCLK_MUXSEL, MUXSEL);
}

void nxSetClockInit(void)
{
    //    unsigned int rm_blk_usb, rm_blk_mm;

    NX_PLL_SetBaseAddress(INDEX_PLL_0, SFR_INFO.PLL[0]);
    NX_PLL_SetBaseAddress(INDEX_PLL_1, SFR_INFO.PLL[1]);

    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__group_clock_source     , 1  ); // PLL[0]

    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__dynamic_divider_value	, 1-1); // div 1
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___AXI__dynamic_divider_value	, 2-1); // div 2
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___APB__dynamic_divider_value	, 4-1); // div 4
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK133__dynamic_divider_value	, 3-1); // div 3
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK50__dynamic_divider_value	, 8-1); // div 8
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK40__dynamic_divider_value	,10-1); // div 10

    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__group_clock_source       , 2  ); // PLL[1]
    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__dynamic_divider_value    , 2-1); // div 2

    //TODO
    /* while(1) { */
    /*     if (lock == 1) { */
            NX_PLL_SetOSCMUX(INDEX_PLL_0, PLL_MUX_PLL_FOUT);
            NX_PLL_SetOSCMUX(INDEX_PLL_1, PLL_MUX_PLL_FOUT);
            //            __asm__ __volatile__ ("dmb");
    /*         break; */
    /*     } */
                     
    /* } */

}
