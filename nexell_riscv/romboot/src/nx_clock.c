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

#ifdef DEBUG
    _dprintf("<<bootrom>>nxSetClockInit: Clock Init Done\n");
#endif
}
