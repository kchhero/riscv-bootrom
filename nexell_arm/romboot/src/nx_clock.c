
#include <nx_chip_sfr.h>
#include <nx_cmu.h>
#include <nx_pll.h>


static struct NX_CMU_RegisterSet *const pCMUCPU =
	(struct NX_CMU_RegisterSet *)PHY_BASEADDR_CMU_CPU_MODULE;
static struct NX_CMU_RegisterSet *const pCMUSYS =
	(struct NX_CMU_RegisterSet *)PHY_BASEADDR_CMU_SYS_MODULE;
static struct NX_CMU_RegisterSet *const pCMUSRC =
	(struct NX_CMU_RegisterSet *)PHY_BASEADDR_CMU_SRC_MODULE;

static struct NX_PLL_RegisterSet *const pPLLCPU =
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPU_MODULE;
static struct NX_PLL_RegisterSet *const pPLL0 =
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL0_MODULE;

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

struct NX_PLL_RegisterSet *const gppll[5] = {
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_CPU_MODULE,
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL0_MODULE,
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL1_MODULE,
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_DDR0_MODULE,
	(struct NX_PLL_RegisterSet *)PHY_BASEADDR_PLL_DDR1_MODULE
};
#define PLL_P	0
#define PLL_M	16
#define PLL_S	0
#define PLL_K	16

#define __g_OSC_KHz	24000
U32 NX_PLL_GetFrequency(U32 PllNumber)
{
	struct NX_PLL_RegisterSet *const ppll = gppll[PllNumber];
	U32 regvalue, regvalue1, nP, nM, nS, nK;
	U32 temp = 0;

	regvalue = ppll->CFG1;
	regvalue1 = ppll->CFG2;
	nP = (regvalue >> PLL_P) & 0x3F;
	nM = (regvalue >> PLL_M) & 0x3FF;
	nS = (regvalue1 >> PLL_S) & 0xFF;
	nK = (regvalue1 >> PLL_K) & 0xFFFF;

	if (nK)
		temp = ((((nK * 1000) / 65536) * __g_OSC_KHz) / nP) >> nS;

	temp = ((((nM * __g_OSC_KHz) / nP) >> nS) * 1000) + temp;
	return temp;
}

void setcpuclock(unsigned int clklvl)
{

	pCMUCPU->CMUBLK[0].DIV[5] = 0x1 & 0xFFFF;	/* cpu dbgapb div 2 */
	pCMUCPU->CMUBLK[0].DIV[6] = 0x1 & 0xFFFF;	/* cpu apb div 2 */

	while (5000 < pPLLCPU->DBG0)
		;
	pPLLCPU->CTRL |= (1 << 3);	/* bypass off, use pll value */

	if (clklvl != 0) {
		pCMUCPU->CMUBLK[0].DIV[1] = cpuclkdiv[clklvl].axi - 1;		/* cpu axi */
		pCMUCPU->CMUBLK[0].DIV[2] = cpuclkdiv[clklvl].atclk - 1;	/* cpu atclk */
		pCMUCPU->CMUBLK[0].DIV[3] = cpuclkdiv[clklvl].cntclk - 1;	/* cpu cntclk */
		pCMUCPU->CMUBLK[0].DIV[4] = cpuclkdiv[clklvl].tsclk - 1;	/* cpu tsclk */
		pCMUCPU->CMUBLK[0].DIV[5] = cpuclkdiv[clklvl].dbgapb - 1;	/* cpu dbgapb */
		pCMUCPU->CMUBLK[0].DIV[6] = cpuclkdiv[clklvl].apb - 1;		/* cpu apb */

		pPLLCPU->CFG1 =
			pllpms[clklvl].m << 16 |	/* M */
					3 << 0;		/* P */
		pPLLCPU->CFG2 =
					0 << 16 |	/* K */
			pllpms[clklvl].s <<  0;		/* S */

		pPLLCPU->CTRL |= 1 << 1;	/* dirtyflag */
		pPLLCPU->CTRL |= 1 << 0;	/* run change */

		while (((pPLLCPU->CTRL >> 8) & 0x1F) != 1)
			;
	}
}

struct nx_sysclkdiv_info {
	unsigned char axi;
	unsigned char apb;
	unsigned char hsif_axi;
	unsigned char hsif_apb;
	unsigned char cmu_apb;
	unsigned char smc_axi;
};
void setsystemclock(unsigned int clklvl)
{
	if (clklvl != 0) {
		pCMUSRC->CMUSRC.SSRCOFF[0] = 0xFFFFFFDC;	/* clock off all device */
		pCMUSRC->CMUSRC.SSRCOFF[1] = 0xFFFF7FFF;
		pCMUSRC->CMUSRC.SSRCOFF[2] = 0xFFFFFFFF;
		pCMUSRC->CMUSRC.SSRCOFF[3] = 0xFFFFFFFF;

		pCMUSYS->CMUBLK[0].DIV[0] = 0x2 & 0xFFFF;	/* sys axi div 3 */
		pCMUSYS->CMUBLK[0].DIV[1] = 0x1 & 0xFFFF;	/* sys apb div 2 */
		pCMUSYS->CMUBLK[1].DIV[0] = 0x3 & 0xFFFF;	/* sys hsif axi div 4 */
		pCMUSYS->CMUBLK[1].DIV[1] = 0x1 & 0xFFFF;	/* sys hsif apb div 2 */
		pCMUSYS->CMUBLK[5].DIV[0] = 0x7 & 0xFFFF;	/* sys cmu apb div 8 */
		pCMUSYS->CMUBLK[47].DIV[0] = 0x4 & 0xFFFF;	/* sys smc axi div 5 */
	}

	while (5000 < pPLL0->DBG0)
		;
	pPLL0->CTRL |= (1 << 3);	/* bypass off, use pll value */

	if (clklvl != 0) {
		pPLL0->CFG1 =
			pllpms[clklvl].m << 16 |	/* M */
					3 <<  0;	/* P */
		pPLL0->CFG2 =
					0 << 16 |	/* K */
			pllpms[clklvl].s <<  0;		/* S */

		pPLL0->CTRL |= 1 << 1;	/* dirtyflag */
		pPLL0->CTRL |= 1 << 0;	/* run change */

		while (((pPLL0->CTRL >> 8) & 0x1F) != 1)
			;
	}
}

void setdeviceclock(const struct cmu_device_clk *pdclk, int num, int enb)
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
