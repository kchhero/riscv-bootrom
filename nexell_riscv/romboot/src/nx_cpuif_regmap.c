#include <nx_cpuif_regmap.h>
#include <nx_swallow_platform.h>

void nx_cpuif_reg_write_one(__nx_cpuif_symbol__ symbol, unsigned int  regval)
{
    unsigned int *reg_addr;
    unsigned int reg_val;
    volatile unsigned int * reg;

    unsigned int reg_writeval;
    unsigned int reg_mask;
    unsigned int reg_startbit;
    unsigned int reg_bitwidth;
    unsigned int masked_writeval;

    reg_addr     = (unsigned int*)(*symbol.baseaddr +  symbol.offset);
    reg_startbit = symbol.startbit;
    reg_bitwidth = symbol.bitwidth;

    reg_writeval = regval << reg_startbit;


    reg = (unsigned int*) reg_addr;

    if( reg_bitwidth == 32 ) {
        reg_val  = 0;
    } else {
        reg_val  = ReadIODW(reg);
    }

    reg_mask = reg_bitwidth < 32 ? ((1<<(reg_bitwidth))-1) << reg_startbit : ~0;

    masked_writeval = (reg_bitwidth < 32) ? regval & ((1<<(reg_bitwidth))-1) : regval ;
    reg_writeval = masked_writeval << reg_startbit;

#ifdef CPUIF_REGMAP_DEBUG
    nx_cpuif_debug_print("\n[DEBUG]------------------------ ");
    nx_cpuif_debug_print("\n[DEBUG] reg_addr        = 0x%x", reg_addr     );
    nx_cpuif_debug_print("\n[DEBUG] reg_startbit    = 0x%x", reg_startbit );
    nx_cpuif_debug_print("\n[DEBUG] reg_bitwidth    = 0x%x", reg_bitwidth );
    nx_cpuif_debug_print("\n[DEBUG] masked_writeval = 0x%x", masked_writeval );
    nx_cpuif_debug_print("\n[DEBUG] reg_writeval    = 0x%x", reg_writeval );
    nx_cpuif_debug_print("\n[DEBUG] reg_mask        = 0x%x", reg_mask     );
    nx_cpuif_debug_print("\n[DEBUG]------------------------");
#endif

    reg_val = reg_val & (~reg_mask);
    reg_val = reg_val | reg_writeval;

    WriteIODW(reg, reg_val);
#ifdef CPUIF_REGMAP_DEBUG
    nx_cpuif_debug_print("\n[DEBUG] reg_val = 0x%x(%d)", reg_val, reg_val );
    nx_cpuif_debug_print("\n[DEBUG] ===============================================");
    nx_cpuif_debug_print("\n");
#endif

}
