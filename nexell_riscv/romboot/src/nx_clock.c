#include <nx_debug.h>
#include <nx_pll.h>
#include <nx_clock.h>
#include <nx_cpuif_regmap.h>
#include <nx_chip_sfr.h>

#ifdef QEMU_RISCV
#include <nx_qemu_platform.h>
#else
#include <nx_swallow_printf.h>
#include <nx_swallow_platform.h>
#endif

#define NULL 0

void nxSetClockInit(void)
{
    //    struct __nx_cpuif_PLL_CPUIFregmap_struct__ _PLL_CPUIFregmap[2];
    /* __nx_cpuif_symbol__ oscclkMuxsel[2] = { */
    /*     {(unsigned int*)&SFR_INFO.PLL[0], 0x0, 3, 1}, */
    /*     {(unsigned int*)&SFR_INFO.PLL[1], 0x0, 3, 1}, */
    /* }; */

    /* NX_PLL_SetBaseAddress(INDEX_PLL_0, SFR_INFO.PLL[0]); */
    /* NX_PLL_SetBaseAddress(INDEX_PLL_1, SFR_INFO.PLL[1]); */


    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__group_clock_source     , 1  ); // PLL[0]


    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__dynamic_divider_value	, 1-1); // div 1
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___AXI__dynamic_divider_value	, 2-1); // div 2
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___APB__dynamic_divider_value	, 4-1); // div 4
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK133__dynamic_divider_value	, 3-1); // div 3
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK50__dynamic_divider_value	, 8-1); // div 8
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK40__dynamic_divider_value	,10-1); // div 10

    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__group_clock_source       , 2  ); // PLL[1]
    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__dynamic_divider_value    , 2-1); // div 2

    nx_cpuif_reg_write_one(PLL_INFO_0_CPUIF, PLL_MUX_PLL_FOUT);
    nx_cpuif_reg_write_one(PLL_INFO_1_CPUIF, PLL_MUX_PLL_FOUT);


    /* NX_PLL_SetOSCMUX(INDEX_PLL_0, PLL_MUX_PLL_FOUT); */
    /* NX_PLL_SetOSCMUX(INDEX_PLL_1, PLL_MUX_PLL_FOUT); */
    //            __asm__ __volatile__ ("dmb");

    /* //DDR1 Clock Init */
    /* nx_cpuif_reg_write_one(	DDRC_REG_4   , 0x88   ); // address : bank, row, column */
    /* //nx_cpuif_reg_write_one(	DDRC_REG_8   , 0xC0C06   ); // reset default */
    /* nx_cpuif_reg_write_one(	DDRC_REG_8   , 0x0B270   ); // for 200MHz operation */
    /* nx_cpuif_reg_write_one(	DDRC_REG_0   , 0x1    ); */

    /* while(0 == nx_cpuif_reg_read_one(DDRC_REG_0, NULL) ); */
}
