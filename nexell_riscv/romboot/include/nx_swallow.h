#ifndef NX_SWALLOW_H
#define NX_SWALLOW_H

#ifdef QEMU_RISCV
#include "nx_qemu_platform.h"
#else
#include "nx_swallow_platform.h"
#endif

//--------------------------------------------------------------------------    
// RISC-V
//--------------------------------------------------------------------------
   
#if __riscv_xlen == 32
  #define MCAUSE_INT         0x80000000UL
  #define MCAUSE_CAUSE       0x7FFFFFFFUL
#else
   #define MCAUSE_INT         0x8000000000000000UL
   #define MCAUSE_CAUSE       0x7FFFFFFFFFFFFFFFUL
#endif

//CPU core num
#define RISCV_NUM_CORES 2

#ifdef QEMU_RISCV
#define SRAM_ADDR (0xFFFF0000)
#define SRAM_SIZE (0x10000) //64KB
#else
#define SRAM_ADDR _AC(0x40000000,UL)
#define SRAM_SIZE _AC(0x8000,UL) //32KB
#endif
#define DRAM_ADDR _AC(0x80000000,UL)
#define DRAM_SIZE _AC(0x40000000,uL) //64KB

#define USBD_VID		0x2375
#define USBD_PID		0x0322

#define BASEADDR_SRAM		SRAM_ADDR
#define INTERNAL_SRAM_SIZE	SRAM_SIZE
#define BASEADDR_DRAM		DRAM_ADDR
#define INTERNAL_DRAM_SIZE	DRAM_SIZE

#define BASEADDR_OF_PBM		BASEADDR_SRAM

#define SECONDBOOT_FSIZENCRC	(64*1024)
#define SECONDBOOT_FSIZE	(SECONDBOOT_FSIZENCRC-(128/8))
#define SECONDBOOT_STACK	0x2000

//--------------------------------------------------------------------------
//  Base Address
//--------------------------------------------------------------------------
#define PHY_BASEADDR_SYS_CON0_MODULE        (0x20000000)
#define PHY_BASEADDR_PLL0_MODULE            (0x20010000)
#define PHY_BASEADDR_PLL1_MODULE            (0x20020000)
#define PHY_BASEADDR_CMU_SYS_MODULE         (0x20030000)
#define PHY_BASEADDR_SYSREG_SYS0_MODULE     (0x20040000)
#define PHY_BASEADDR_DDRC0_MODULE           (0x20050000)

#define PHY_BASEADDR_AXIM_DMA0_MODULE       (0x20200000)
#define PHY_BASEADDR_AXIM_DMA1_MODULE       (0x20210000)

#define PHY_BASEADDR_AXIM_CPU_SFR0_MODULE   (0x20220000)
#define PHY_BASEADDR_AXIM_CPU_MEM0_MODULE   (0x20230000)
#define PHY_BASEADDR_AXIM_VIP0_MODULE       (0x20240000)
#define PHY_BASEADDR_AXIM_USB0_MODULE       (0x20250000)
#define PHY_BASEADDR_AXIM_HOVER0_MODULE     (0x20260000)
#define PHY_BASEADDR_AXIM_SCALER0_MODULE    (0x20270000)
#define PHY_BASEADDR_AXIM_CODA0_MODULE      (0x20280000)
#define PHY_BASEADDR_AXIM_SDMMC0_MODULE     (0x20290000)
#define PHY_BASEADDR_AXIM_SDMMC1_MODULE     (0x202A0000)

#define PHY_BASEADDR_DMA_DATA_BUS_CFG0_MODULE (0x20300000)
#define PHY_BASEADDR_VIP_DATA_BUS_CFG0_MODULE (0x20310000)

#define PHY_BASEADDR_CODA0_DATA_BUS_CFG_MODULE (0x20320000)
#define PHY_BASEADDR_MAIN_BUS_CFG0_MODULE      (0x20330000)
#define PHY_BASEADDR_CFG_BUS_CFG0_MODULE       (0x20340000)

#define PHY_BASEADDR_VIP0_MODULE            (0x20400000)
#define PHY_BASEADDR_SCALER0_MODULE         (0x20410000)
#define PHY_BASEADDR_CODA0_MODULE           (0x20420000)
#define PHY_BASEADDR_RSP0_MODULE            (0x20430000)
#define PHY_BASEADDR_CRYPTO0_MODULE         (0x20440000)
#define PHY_BASEADDR_DMA0_MODULE            (0x20450000)
#define PHY_BASEADDR_DMA1_MODULE            (0x20460000)
#define PHY_BASEADDR_HOVER0_HOVER_MODULE    (0x20470000)
#define PHY_BASEADDR_HOVER0_DMA_MODULE      (0x20480000)

#define PHY_BASEADDR_I2C0_MODULE            (0x20600000)
#define PHY_BASEADDR_I2C1_MODULE            (0x20610000)
#define PHY_BASEADDR_I2C2_MODULE            (0x20620000)
#define PHY_BASEADDR_I2C3_MODULE            (0x20630000)
#define PHY_BASEADDR_I2C4_MODULE            (0x20640000)
#define PHY_BASEADDR_I2C5_MODULE            (0x20650000)
#define PHY_BASEADDR_I2C6_MODULE            (0x20660000)
#define PHY_BASEADDR_I2C7_MODULE            (0x20670000)
#define PHY_BASEADDR_I2C8_MODULE            (0x20680000)
#define PHY_BASEADDR_I2C9_MODULE            (0x20690000)
#define PHY_BASEADDR_I2C10_MODULE           (0x206A0000)
#define PHY_BASEADDR_I2C11_MODULE           (0x206B0000)

#define PHY_BASEADDR_ADC0_MODULE            (0x206C0000)
#define PHY_BASEADDR_WDT0_MODULE            (0x206D0000)

#define PHY_BASEADDR_GPIO0_MODULE           (0x20700000)
#define PHY_BASEADDR_GPIO1_MODULE           (0x20710000)
#define PHY_BASEADDR_GPIO2_MODULE           (0x20720000)
#define PHY_BASEADDR_GPIO3_MODULE           (0x20730000)
#define PHY_BASEADDR_GPIO4_MODULE           (0x20740000)
#define PHY_BASEADDR_GPIO5_MODULE           (0x20750000)
#define PHY_BASEADDR_GPIO6_MODULE           (0x20760000)
#define PHY_BASEADDR_GPIO7_MODULE           (0x20770000)

#define PHY_BASEADDR_SPI0_MODULE            (0x20800000)
#define PHY_BASEADDR_SPI1_MODULE            (0x20810000)
#define PHY_BASEADDR_SPI2_MODULE            (0x20820000)
#define PHY_BASEADDR_QSPI0_MODULE           (0x20830000)
#define PHY_BASEADDR_QSPI1_MODULE           (0x20840000)
#define PHY_BASEADDR_QSPI2_MODULE           (0x20850000)
#define PHY_BASEADDR_QSPI3_MODULE           (0x20860000)
#define PHY_BASEADDR_QSPI4_MODULE           (0x20870000)
#define PHY_BASEADDR_UART0_MODULE           (0x20880000)
#define PHY_BASEADDR_UART1_MODULE           (0x20890000)
#define PHY_BASEADDR_USART0_MODULE          (0x208A0000)
#define PHY_BASEADDR_USART1_MODULE          (0x208B0000)
#define PHY_BASEADDR_USART2_MODULE          (0x208C0000)
#define PHY_BASEADDR_USART3_MODULE          (0x208D0000)

#define PHY_BASEADDR_PWM0_MODULE            (0x208E0000)
#define PHY_BASEADDR_PWM1_MODULE            (0x208F0000)
#define PHY_BASEADDR_PWM2_MODULE            (0x20900000)
#define PHY_BASEADDR_TIMER0_MODULE          (0x20910000)
#define PHY_BASEADDR_TIMER1_MODULE          (0x20920000)
#define PHY_BASEADDR_SDMMC0_MODULE          (0x20930000)
#define PHY_BASEADDR_SDMMC1_MODULE          (0x20940000)

#define PHY_BASEADDR_TRIKBOX0               (0x20A00000)
#define PHY_BASEADDR_TRIKBOX1               (0x20A10000)
#define PHY_BASEADDR_TRIKBOX2               (0x20A20000)
#define PHY_BASEADDR_TRIKBOX3               (0x20A30000)
#define PHY_BASEADDR_TRIKBOX4               (0x20A40000)
#define PHY_BASEADDR_TRIKBOX5               (0x20A50000)
#define PHY_BASEADDR_TRIKBOX6               (0x20A60000)
#define PHY_BASEADDR_TRIKBOX7               (0x20A70000)
#define PHY_BASEADDR_TRIKBOX8               (0x20A80000)
#define PHY_BASEADDR_TRIKBOX9               (0x20A90000)
#define PHY_BASEADDR_TRIKBOX10              (0x20AA0000)
#define PHY_BASEADDR_TRIKBOX11              (0x20AB0000)
#define PHY_BASEADDR_TRIKBOX12              (0x20AC0000)
#define PHY_BASEADDR_TRIKBOX13              (0x20AD0000)
#define PHY_BASEADDR_TRIKBOX14              (0x20AE0000)
#define PHY_BASEADDR_TRIKBOX15              (0x20AF0000)

#define PHY_BASEADDR_C2DL0_MODULE           (0x20C00000)

#define PHY_BASEADDR_USB0_MODULE            (0x20D00000)
#define PHY_BASEADDR_USB1_MODULE            (0x20D10000)
#define PHY_BASEADDR_USB2_MODULE            (0x20D20000)
#define PHY_BASEADDR_USB3_MODULE            (0x20D30000)

//--------------------------------------------------------------------------
#define NUMBER_OF_GPIO_MODULE                               6

#endif	/* NX_ETACARINAE_H */
