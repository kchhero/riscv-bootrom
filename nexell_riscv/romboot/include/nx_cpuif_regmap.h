#ifndef __NX_CPUIF_REGMAP_H__
#define __NX_CPUIF_REGMAP_H__

#include "nx_chip_sfr.h"

typedef struct __nx_cpuif_symbol_struct__ {
    unsigned int *baseaddr;
    unsigned int offset;
    unsigned int startbit;
    unsigned int bitwidth;
}__nx_cpuif_symbol__ ;

#define CPUIF_END (__nx_cpuif_symbol__){0,0,0,0}
#define nx_cpuif_regmap_MAX_LIST 128

// modified with Prototype/nx_base/nx_clockcontrol.c
#define CMU_INFO_DEF__SYS_0___CLK400__group_clock_source	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x200, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK400__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x260, 0, 32 }
#define CMU_INFO_DEF__SYS_0___AXI__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x264, 0, 32 }
#define CMU_INFO_DEF__SYS_0___APB__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x268, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK133__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x26c, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK50__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x270, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK40__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x274, 0, 32 }

#define CMU_INFO_DEF__CPU_0___CORE__group_clock_source		(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x400, 0, 32 }
#define CMU_INFO_DEF__CPU_0___CORE__dynamic_divider_value	(__nx_cpuif_symbol__){ &SFR_INFO.CMU_SYS[0], 0x460, 0, 32 }

#define DDRC_REG_0		(__nx_cpuif_symbol__){ &SFR_INFO.DDRC[0], 0x0, 0, 32 }
#define DDRC_REG_4		(__nx_cpuif_symbol__){ &SFR_INFO.DDRC[0], 0x4, 0, 32 }
#define DDRC_REG_8		(__nx_cpuif_symbol__){ &SFR_INFO.DDRC[0], 0x8, 0, 32 }

void nx_cpuif_reg_write_one(__nx_cpuif_symbol__ symbol, unsigned int  regval);
unsigned int nx_cpuif_reg_read_one ( __nx_cpuif_symbol__ symbol, unsigned int * regval );

#endif //__NX_CPUIF_REGMAP_H__
