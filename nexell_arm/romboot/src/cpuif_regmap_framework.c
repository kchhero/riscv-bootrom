
#include <nx_type.h>
#include <nx_prototype.h>

#include "cpuif_regmap_framework.h"

#define CAST_TO_PTR (unsigned int*)

#ifdef __NX_CPUIF_REG_FUNC_BODY__
#else
	#define __NX_CPUIF_REG_FUNC_BODY__

void nx_cpuif_reg_write_one_notread(
		_nx_cpuif_sym_ symbol,
		unsigned int  regval)
{
	unsigned int * reg_addr;
	unsigned int reg_val;
	volatile unsigned int * reg;

	unsigned int reg_writeval;
	unsigned int reg_mask;
	unsigned int reg_startbit;
	unsigned int reg_bitwidth;
	unsigned int masked_writeval;

	reg_addr     = CAST_TO_PTR(*symbol.baseaddr + symbol.offset);
	reg_startbit = symbol.startbit;
	reg_bitwidth = symbol.bitwidth;

	reg_writeval = regval << reg_startbit;

	reg = (unsigned int*) reg_addr;

	reg_val = 0;

	reg_mask = reg_bitwidth < 32 ?
		((1 << reg_bitwidth) - 1) << reg_startbit : ~0;

	masked_writeval = (reg_bitwidth < 32) ?
		regval & ((1 << reg_bitwidth) - 1) : regval ;
	reg_writeval = masked_writeval << reg_startbit;

#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG]------------------------ ");
	nx_cpuif_debug_print("\n[DEBUG] reg_addr        = 0x%x", reg_addr);
	nx_cpuif_debug_print("\n[DEBUG] reg_startbit    = 0x%x", reg_startbit);
	nx_cpuif_debug_print("\n[DEBUG] reg_bitwidth    = 0x%x", reg_bitwidth);
	nx_cpuif_debug_print("\n[DEBUG] masked_writeval = 0x%x", masked_writeval);
	nx_cpuif_debug_print("\n[DEBUG] reg_writeval    = 0x%x", reg_writeval);
	nx_cpuif_debug_print("\n[DEBUG] reg_mask        = 0x%x", reg_mask);
	nx_cpuif_debug_print("\n[DEBUG]------------------------");
#endif

	reg_val = reg_val & (~reg_mask);
	reg_val = reg_val | reg_writeval;

	WriteIODW(reg, reg_val);
#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG] reg_val = 0x%x(%d)", reg_val, reg_val);
	nx_cpuif_debug_print("\n[DEBUG] ===============================================");
	nx_cpuif_debug_print("\n");
#endif

}

void nx_cpuif_reg_write_one(_nx_cpuif_sym_ symbol, unsigned int  regval)
{
	unsigned int * reg_addr;
	unsigned int reg_val;
	volatile unsigned int * reg;

	unsigned int reg_writeval;
	unsigned int reg_mask;
	unsigned int reg_startbit;
	unsigned int reg_bitwidth;
	unsigned int masked_writeval;

	reg_addr     = CAST_TO_PTR(*symbol.baseaddr + symbol.offset);
	reg_startbit = symbol.startbit;
	reg_bitwidth = symbol.bitwidth;

	reg_writeval = regval << reg_startbit;


	reg = (unsigned int*) reg_addr;

	reg_val  = ReadIODW(reg);

	reg_mask = reg_bitwidth < 32 ?
			((1 << reg_bitwidth) - 1) << reg_startbit : ~0;

	masked_writeval = (reg_bitwidth < 32) ?
				regval & ((1 << reg_bitwidth) - 1) : regval ;
	reg_writeval = masked_writeval << reg_startbit;

#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG]------------------------ ");
	nx_cpuif_debug_print("\n[DEBUG] reg_addr        = 0x%x", reg_addr);
	nx_cpuif_debug_print("\n[DEBUG] reg_startbit    = 0x%x", reg_startbit);
	nx_cpuif_debug_print("\n[DEBUG] reg_bitwidth    = 0x%x", reg_bitwidth);
	nx_cpuif_debug_print("\n[DEBUG] masked_writeval = 0x%x", masked_writeval);
	nx_cpuif_debug_print("\n[DEBUG] reg_writeval    = 0x%x", reg_writeval);
	nx_cpuif_debug_print("\n[DEBUG] reg_mask        = 0x%x", reg_mask);
	nx_cpuif_debug_print("\n[DEBUG]------------------------");
#endif

	reg_val = reg_val & (~reg_mask);
	reg_val = reg_val | reg_writeval;

	WriteIODW(reg, reg_val);
#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG] reg_val = 0x%x(%d)", reg_val, reg_val);
	nx_cpuif_debug_print("\n[DEBUG] ===============================================");
	nx_cpuif_debug_print("\n");
#endif

}

unsigned int nx_cpuif_reg_read_one (
		_nx_cpuif_sym_ symbol,
		unsigned int * regval)
{
	unsigned int * reg_addr;
	unsigned int reg_val;
	volatile unsigned int * reg;

	unsigned int reg_readval;
	unsigned int reg_mask;
	unsigned int reg_startbit;
	unsigned int reg_bitwidth;

	reg_addr     = CAST_TO_PTR(*symbol.baseaddr + symbol.offset);
	reg_startbit = symbol.startbit;
	reg_bitwidth = symbol.bitwidth;

	reg = (unsigned int *)reg_addr;
	reg_val  = ReadIODW(reg);
#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG] reg_val = 0x%x(%d), reg_addr = 0x%x",
				reg_val, reg_val, reg_addr);
#endif

	reg_mask = reg_bitwidth < 32 ? ((1 << reg_bitwidth) - 1) << reg_startbit : ~0;

	reg_readval = (reg_val & reg_mask) >> reg_startbit;
	reg_val = reg_readval;

	if (regval != CNULL)
		*regval = reg_val;

#ifdef CPUIF_REGMAP_DEBUG
	nx_cpuif_debug_print("\n[DEBUG]------------------------ ");
	nx_cpuif_debug_print("\n[DEBUG] reg_addr     = 0x%x", reg_addr);
	nx_cpuif_debug_print("\n[DEBUG] reg_startbit = 0x%x", reg_startbit);
	nx_cpuif_debug_print("\n[DEBUG] reg_bitwidth = 0x%x", reg_bitwidth);
	nx_cpuif_debug_print("\n[DEBUG] reg_mask     = 0x%x", reg_mask);
	nx_cpuif_debug_print("\n[DEBUG] reg_readval  = 0x%x", reg_readval);
	nx_cpuif_debug_print("\n[DEBUG] reg_val      = 0x%x(%d)", reg_val, reg_val);
	nx_cpuif_debug_print("\n[DEBUG]------------------------");
#endif

	return reg_readval;
}

#endif // #ifdef __NX_CPUIF_REG_FUNC_BODY__
