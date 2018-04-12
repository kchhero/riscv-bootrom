#include <nx_cpuif_regmap.h>
#include <nx_swallow_platform.h>
#ifdef QEMU_RISCV
#include <nx_qemu_printf.h>
#else
#include <nx_swallow_printf.h>
#endif

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

    reg_addr     = (unsigned int*)(*symbol.baseaddr + symbol.offset);
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

#ifdef DEBUG
    _dprintf("\n[DEBUG]------------------------ ");
    _dprintf("\n[DEBUG] reg_addr        = 0x%x", reg_addr     );
    _dprintf("\n[DEBUG] reg_startbit    = 0x%x", reg_startbit );
    _dprintf("\n[DEBUG] reg_bitwidth    = 0x%x", reg_bitwidth );
    _dprintf("\n[DEBUG] masked_writeval = 0x%x", masked_writeval );
    _dprintf("\n[DEBUG] reg_writeval    = 0x%x", reg_writeval );
    _dprintf("\n[DEBUG] reg_mask        = 0x%x", reg_mask     );
    _dprintf("\n[DEBUG]------------------------");
#endif

    reg_val = reg_val & (~reg_mask);
    reg_val = reg_val | reg_writeval;

    WriteIODW(reg, reg_val);
#ifdef DEBUG
    _dprintf("\n[DEBUG] reg_val = 0x%x(%d)", reg_val, reg_val );
    _dprintf("\n[DEBUG] ===============================================");
    _dprintf("\n");
#endif
}

unsigned int nx_cpuif_reg_read_one(__nx_cpuif_symbol__ symbol, unsigned int * regval )
{
    unsigned int * reg_addr;
    unsigned int reg_val;
    volatile unsigned int * reg;

    unsigned int reg_readval;
    unsigned int reg_mask;
    unsigned int reg_startbit;
    unsigned int reg_bitwidth;

    reg_addr     = (unsigned int*)(*symbol.baseaddr + symbol.offset);
    reg_startbit = symbol.startbit;
    reg_bitwidth = symbol.bitwidth;

    reg = (unsigned int*) reg_addr;
    reg_val  = ReadIODW(reg);
#ifdef DEBUG
    _dprintf("\n[DEBUG] reg_val = 0x%x(%d), reg_addr = 0x%x", reg_val, reg_val, reg_addr);
#endif

    reg_mask = reg_bitwidth < 32 ? ((1<<(reg_bitwidth))-1) << reg_startbit : ~0;

    reg_readval = (reg_val & reg_mask) >> reg_startbit;
    reg_val = reg_readval;

    if( regval != NULL ) {
        *regval = reg_val;
    }

    //NX_ASSERT( reg_bitwidth != 0 );

#ifdef DEBUG
    _dprintf("\n[DEBUG]------------------------ ");
    _dprintf("\n[DEBUG] reg_addr     = 0x%x", reg_addr     );
    _dprintf("\n[DEBUG] reg_startbit = 0x%x", reg_startbit );
    _dprintf("\n[DEBUG] reg_bitwidth = 0x%x", reg_bitwidth );
    _dprintf("\n[DEBUG] reg_mask     = 0x%x", reg_mask     );
    _dprintf("\n[DEBUG] reg_readval  = 0x%x", reg_readval  );
    _dprintf("\n[DEBUG] reg_val      = 0x%x(%d)", reg_val, reg_val);
    _dprintf("\n[DEBUG]------------------------");
#endif
    return reg_readval;
}
