// See LICENSE for license details.

#ifndef _NEXELL_PLATFORM_H
#define _NEXELL_PLATFORM_H

#include "nx_const.h"
#include "nx_uart.h"

/****************************************************************************
 * Platform definitions
 *****************************************************************************/

// CPU info
#define GLOBAL_INT_SIZE 15
#define GLOBAL_INT_MAX_PRIORITY 7

// Memory map
#define DEBUG_CTRL_ADDR _AC(0x0,UL)
#define DEBUG_CTRL_SIZE _AC(0x1000,UL)  //#define DEBUG_CTRL_SIZE _AC(0x1000,UL)

#define ERROR_MEM_ADDR _AC(0x3000,UL)
#define ERROR_MEM_SIZE _AC(0x1000,UL)

#define MASKROM_MEM_ADDR _AC(0x1000,UL)
#define MASKROM_MEM_SIZE _AC(0x10000,UL)

#define CLINT_CTRL_ADDR _AC(0x2000000,UL)
#define CLINT_CTRL_SIZE _AC(0x10000,UL)

#define PLIC_CTRL_ADDR _AC(0xC000000,UL)
#define PLIC_CTRL_SIZE _AC(0x4000000,UL)

#define GPIO_CTRL_ADDR _AC(0x20700000,UL) //#define GPIO_CTRL_ADDR _AC(0x54002000,UL)
#define GPIO_CTRL_SIZE _AC(0x1000,UL)

#define UART_CTRL_ADDR _AC(0x208A0000,UL)  //#define UART_CTRL_ADDR _AC(0x54000000,UL)
#define UART_CTRL_SIZE _AC(0x1000,UL)


// Interrupt numbers
#define UART_INT_BASE 1
#define SPI_INT_BASE 2
#define GPIO_INT_BASE 3
#define AXI_PCIE_HOST_1_00_A_INT_BASE 7

// Helper functions
#define _REG64(p, i) (*(volatile uint64_t *)((p) + (i)))
#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))
#define _REG16(p, i) (*(volatile uint16_t *)((p) + (i)))
// Bulk set bits in `reg` to either 0 or 1.
// E.g. SET_BITS(MY_REG, 0x00000007, 0) would generate MY_REG &= ~0x7
// E.g. SET_BITS(MY_REG, 0x00000007, 1) would generate MY_REG |= 0x7
#define SET_BITS(reg, mask, value) if ((value) == 0) { (reg) &= ~(mask); } else { (reg) |= (mask); }
#define CLINT_REG(offset) _REG32(CLINT_CTRL_ADDR, offset)
#define DEBUG_REG(offset) _REG32(DEBUG_CTRL_ADDR, offset)
#define ERROR_REG(offset) _REG32(ERROR_CTRL_ADDR, offset)
#define GPIO_REG(offset) _REG32(GPIO_CTRL_ADDR, offset)
#define MASKROM_REG(offset) _REG32(MASKROM_CTRL_ADDR, offset)
#define MEMORY_REG(offset) _REG32(MEMORY_CTRL_ADDR, offset)
#define PLIC_REG(offset) _REG32(PLIC_CTRL_ADDR, offset)
#define SPI_REG(offset) _REG32(SPI_CTRL_ADDR, offset)
#define UART_REG(offset) _REG32(UART_CTRL_ADDR, offset)
#define CLINT_REG64(offset) _REG64(CLINT_CTRL_ADDR, offset)
#define DEBUG_REG64(offset) _REG64(DEBUG_CTRL_ADDR, offset)
#define ERROR_REG64(offset) _REG64(ERROR_CTRL_ADDR, offset)
#define GPIO_REG64(offset) _REG64(GPIO_CTRL_ADDR, offset)
#define MASKROM_REG64(offset) _REG64(MASKROM_CTRL_ADDR, offset)
#define MEMORY_REG64(offset) _REG64(MEMORY_CTRL_ADDR, offset)
#define PLIC_REG64(offset) _REG64(PLIC_CTRL_ADDR, offset)
#define UART_REG64(offset) _REG64(UART_CTRL_ADDR, offset)


#ifdef DEBUG
	#define NX_ASSERT(expr)    \
	{                          \
		if (!(expr))       \
                {                  \
			NX_TRACE((TEXT("%s(%d) : %s %s (%s)\n"),	\
				TEXT(__FILE__), __LINE__, __FUNCTION__, TEXT("ASSERT"), TEXT(#expr)));	\
			NX_DEBUG_BREAK();				\
		}	\
	}
#else
	#define NX_ASSERT(expr)
#endif // NX_DEBUG

#endif /* _NEXELL_PLATFORM_H */
