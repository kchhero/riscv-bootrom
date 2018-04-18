#ifndef __NX_PLL_H__
#define __NX_PLL_H__

#include "nx_cpuif_regmap.h"
//Mhz
#define NX_CLK_OSC_IN            25
#define NX_CLK_PLL0_FREQ        400
#define NX_CLK_PLL1_FREQ        400

struct NX_PLL_RegisterSet {
	volatile unsigned int    CTRL;	/* 0x000 */
	volatile unsigned int    DBG0;	/* 0x004 */
	volatile unsigned int    _Rev0[2];	/* 0x008 -- 0x00C */
	volatile unsigned int    CNT[4];	/* 0x010 -- 0x01C */
	volatile unsigned int    CFG1;	/* 0x020 */
	volatile unsigned int    _Rev1[3];	/* 0x024 -- 0x02C */
	volatile unsigned int    CFG2;	/* 0x030 */
	volatile unsigned int    _Rev2[3];	/* 0x034 -- 0x03C */
	volatile unsigned int    LOCKINT;	/* 0x040 */
};
struct __nx_cpuif_PLL_CPUIFregmap_struct__ { 
	const __nx_cpuif_symbol__ CLKOFF; 
	const __nx_cpuif_symbol__ OSCCLK_MUXSEL; 
	const __nx_cpuif_symbol__ DIRTYFLAG; 
	const __nx_cpuif_symbol__ CLKSEL_DONE; 
	const __nx_cpuif_symbol__ CLKEN_DEBUG; 
	const __nx_cpuif_symbol__ CurSt; 
	const __nx_cpuif_symbol__ LOCK; 
	const __nx_cpuif_symbol__ RUNDONE; 
	const __nx_cpuif_symbol__ IDLE; 
	const __nx_cpuif_symbol__ UPDATECONFIG_DIRECTLY; 
	const __nx_cpuif_symbol__ DIRTYFLAG_CLR; 
	const __nx_cpuif_symbol__ RUN_CHANGE; 
	const __nx_cpuif_symbol__ StCount; 
	const __nx_cpuif_symbol__ RUN0_MAXCOUNT; 
	const __nx_cpuif_symbol__ RUN1_MAXCOUNT; 
	const __nx_cpuif_symbol__ RUN2_MAXCOUNT; 
	const __nx_cpuif_symbol__ RUN3_MAXCOUNT; 
	const __nx_cpuif_symbol__ CPUIF_PD; 
	const __nx_cpuif_symbol__ LFR; 
	const __nx_cpuif_symbol__ FMUX; 
	const __nx_cpuif_symbol__ REFDIV; 
	const __nx_cpuif_symbol__ FBDIV; 
	const __nx_cpuif_symbol__ VCS; 
	const __nx_cpuif_symbol__ PMUX; 
	const __nx_cpuif_symbol__ GV; 
	const __nx_cpuif_symbol__ RUNDONE_INTDISABLE; 
	const __nx_cpuif_symbol__ RUNDONE_INTENB; 
	const __nx_cpuif_symbol__ RUNDONE_INTPEND; 
	const __nx_cpuif_symbol__ LOCK_INTDISABLE; 
	const __nx_cpuif_symbol__ LOCK_INTENB; 
	const __nx_cpuif_symbol__ LOCK_INTPEND; 
// prototype group 
}; // __nx_cpuif_PLL_CPUIFregmap__ ;

#undef  NUMBER_OF_PLL_MODULE        
#define NUMBER_OF_PLL_MODULE        5

#define NUMBER_OF_PLL_CPUIF_MODULE  NUMBER_OF_PLL_MODULE

#define PLL_LOCK_COUNT       0x200

#define PLL_MUX_OSCCLK   0 
#define PLL_MUX_PLL_FOUT 1 

#define TYPE_PLL2555X 0
#define TYPE_PLL2651X 1

#define INDEX_PLL_0    0
#define INDEX_PLL_1    1
#define INDEX_PLL_CPU  2
#define INDEX_PLL_DDR0 3
#define INDEX_PLL_DDR1 4

#define PHY_BASEADDR_PLL2_MODULE PHY_BASEADDR_PLL_CPU_MODULE
#define PHY_BASEADDR_PLL3_MODULE PHY_BASEADDR_PLL_DDR0_MODULE
#define PHY_BASEADDR_PLL4_MODULE PHY_BASEADDR_PLL_DDR1_MODULE

#define PHY_BASEADDR_PLL_CPUIF0_MODULE PHY_BASEADDR_PLL0_MODULE
#define PHY_BASEADDR_PLL_CPUIF1_MODULE PHY_BASEADDR_PLL1_MODULE
#define PHY_BASEADDR_PLL_CPUIF2_MODULE PHY_BASEADDR_PLL2_MODULE
#define PHY_BASEADDR_PLL_CPUIF3_MODULE PHY_BASEADDR_PLL3_MODULE
#define PHY_BASEADDR_PLL_CPUIF4_MODULE PHY_BASEADDR_PLL4_MODULE

#define TYPE_PLL_0    TYPE_PLL2555X
#define TYPE_PLL_1    TYPE_PLL2651X
#define TYPE_PLL_CPU  TYPE_PLL2555X
#define TYPE_PLL_DDR0 TYPE_PLL2555X
#define TYPE_PLL_DDR1 TYPE_PLL2651X

//extern	unsigned int *__PLL_CPUIF_BASEADDR[NUMBER_OF_PLL_CPUIF_MODULE];

#endif /* __NX_PLL_H__ */
