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
/*     { PHY_BASEADDR_CMU_SYS_MODULE, },  ///\*CMU_SYS[0] *\/ (0x20030000),}, */
#define CMU_INFO_DEF__SYS_0___CLK400__group_clock_source	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x200>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK400__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x260>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___AXI__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x264>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___APB__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x268>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK133__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x26c>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK50__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x270>>2, 0, 32 }
#define CMU_INFO_DEF__SYS_0___CLK40__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x274>>2, 0, 32 }

#define CMU_INFO_DEF__CPU_0___CORE__group_clock_source		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x400>>2, 0, 32 }
#define CMU_INFO_DEF__CPU_0___CORE__dynamic_divider_value	(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_CMU_SYS_MODULE, 0x460>>2, 0, 32 }

#define PLL_INFO_0_CPUIF                         		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_PLL0_MODULE, 0x0, 3, 1 }
#define PLL_INFO_1_CPUIF                         		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_PLL1_MODULE, 0x0, 3, 1 }

#define DDRC_REG_0		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_DDRC0_MODULE, 0x0, 0, 32 }
#define DDRC_REG_4		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_DDRC0_MODULE, 0x4>>2, 0, 32 }
#define DDRC_REG_8		(__nx_cpuif_symbol__){(unsigned int*)PHY_BASEADDR_DDRC0_MODULE, 0x8>>2, 0, 32 }

#ifdef	__cplusplus
extern "C"
{
#endif

/* #ifdef __SOC__ */
/* void NX_CONSOLE_Printf(const char *FormatString, ...); */
/* #endif */

void nx_cpuif_reg_write_one(__nx_cpuif_symbol__ symbol, unsigned int  regval);
unsigned int nx_cpuif_reg_read_one ( __nx_cpuif_symbol__ symbol, unsigned int * regval );
void setSFRINFO(void);
    
#ifdef	__cplusplus
}
#endif



#endif //__NX_CPUIF_REGMAP_H__
