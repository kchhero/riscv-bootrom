#ifndef __NX_CHIP_SFR_H__
#define __NX_CHIP_SFR_H__

#include "nx_swallow.h"

typedef struct _sfr_info {
	unsigned int SYSCON[1];
	unsigned int VIP[1];
	unsigned int SPI[3];
	unsigned int C2DL[1];
	unsigned int PLL[2];
	unsigned int SCALER[1];
	unsigned int CODA[1];
	unsigned int CMU_SYS[1];
	unsigned int RSP[1];
	unsigned int QSPI[5];
	unsigned int SYSREG_SYS[1];
	unsigned int CRYPTO[1];
	unsigned int DDRC[1];
	unsigned int DMA[2];
	unsigned int HOVER[1];
	unsigned int UART[2];
	unsigned int USART[4];
	unsigned int PWM[3];
	unsigned int USB[1];
	unsigned int TIMER[2];
	unsigned int OSDMMC[2];
	unsigned int AXIM_DMA[2];
	unsigned int I2C[12];
	unsigned int TRIKBOX[1];
	unsigned int AXIM_CPUSFR[1];
	unsigned int AXIM_CPUMEM[1];
	unsigned int AXIM_VIP[1];
	unsigned int AXIM_USB[1];
	unsigned int AXIM_HOVER[1];
	unsigned int AXIM_SCALER[1];
	unsigned int AXIM_CODA[1];
	unsigned int AXIM_OSDMMC[2];
	unsigned int ADC[1];
	unsigned int WDT[1];
	unsigned int DMA_DATA_BUS_CFG[1];
	unsigned int GPIO[8];
	unsigned int VIP_DATA_BUS_CFG[1];
	unsigned int CODA_DATA_BUS_CFG[1];
	unsigned int MAIN_BUS_CFG[1];
	unsigned int CFG_BUS_CFG[1];
} _SFR_INFO;

extern struct _SFR_INFO SFR_INFO;

/* const _SFR_INFO SFR_INFO = { */
/*     { PHY_BASEADDR_SYS_CON0_MODULE, }, ///\*SYSCON[0] *\/ (0x20000000), }, */
/*     { PHY_BASEADDR_VIP0_MODULE, },     ///\*VIP[0] *\/ (0x20400000),    }, */
/*     { PHY_BASEADDR_SPI0_MODULE,        ///\*SPI[0] *\/ (0x20800000), */
/*       PHY_BASEADDR_SPI1_MODULE,        ///\*SPI[1] *\/ (0x20810000), */
/*       PHY_BASEADDR_SPI2_MODULE,        ///\*SPI[2] *\/ (0x20820000), */
/*     }, */
/*     { PHY_BASEADDR_C2DL0_MODULE, },    ///\*C2DL[0] *\/ (0x20C00000),   }, */
/*     { PHY_BASEADDR_PLL0_MODULE,        ///\*PLL[0] *\/ (0x20010000), */
/*       PHY_BASEADDR_PLL1_MODULE,        ///\*PLL[1] *\/ (0x20020000), */
/*     }, */
/*     { PHY_BASEADDR_SCALER0_MODULE, },  ///\*SCALER[0] *\/ (0x20410000), }, */
/*     { PHY_BASEADDR_CODA0_MODULE, },    ///\*CODA[0] *\/ (0x20420000),   }, */
/*     { PHY_BASEADDR_CMU_SYS_MODULE, },  ///\*CMU_SYS[0] *\/ (0x20030000),}, */
/*     { PHY_BASEADDR_RSP0_MODULE, },     ///\*RSP[0] *\/ (0x20430000),    }, */
/*     { PHY_BASEADDR_QSPI0_MODULE,       ///\*QSPI[0] *\/ (0x20830000), */
/*       PHY_BASEADDR_QSPI1_MODULE,       ///\*QSPI[1] *\/ (0x20840000), */
/*       PHY_BASEADDR_QSPI2_MODULE,       ///\*QSPI[2] *\/ (0x20850000), */
/*       PHY_BASEADDR_QSPI3_MODULE,       ///\*QSPI[3] *\/ (0x20860000), */
/*       PHY_BASEADDR_QSPI4_MODULE,       ///\*QSPI[4] *\/ (0x20870000), */
/*     }, */
/*     { PHY_BASEADDR_SYSREG_SYS0_MODULE, },  ///\*SYSREG_SYS[0] *\/ (0x20040000), }, */
/*     { PHY_BASEADDR_CRYPTO0_MODULE, },  ///\*CRYPTO[0] *\/ (0x20440000), }, */
/*     { PHY_BASEADDR_DDRC0_MODULE, },    ///\*DDRC[0] *\/ (0x20050000),   }, */
/*     { PHY_BASEADDR_DMA0_MODULE,        ///\*DMA[0] *\/ (0x20450000), */
/*       PHY_BASEADDR_DMA1_MODULE,        ///\*DMA[1] *\/ (0x20460000), */
/*     }, */
/*     { PHY_BASEADDR_HOVER0_DMA_MODULE, }, ///\*HOVER[0] *\/ (0x20480000),  }, */
/*     { PHY_BASEADDR_UART0_MODULE,       ///\*UART[0] *\/ (0x20880000), */
/*       PHY_BASEADDR_UART1_MODULE,       ///\*UART[1] *\/ (0x20890000), */
/*     }, */
/*     { PHY_BASEADDR_USART0_MODULE,      ///\*USART[0] *\/ (0x208A0000), */
/*       PHY_BASEADDR_USART1_MODULE,      ///\*USART[1] *\/ (0x208B0000), */
/*       PHY_BASEADDR_USART2_MODULE,      ///\*USART[2] *\/ (0x208C0000), */
/*       PHY_BASEADDR_USART3_MODULE,      ///\*USART[3] *\/ (0x208D0000), */
/*     }, */
/*     { PHY_BASEADDR_PWM0_MODULE,        ///\*PWM[0] *\/ (0x208E0000), */
/*       PHY_BASEADDR_PWM1_MODULE,        ///\*PWM[1] *\/ (0x208F0000), */
/*       PHY_BASEADDR_PWM2_MODULE,        ///\*PWM[2] *\/ (0x20900000), */
/*     }, */
/*     { PHY_BASEADDR_USB0_MODULE, },     ///\*USB[0] *\/ (0x20D00000),    }, */
/*     { PHY_BASEADDR_TIMER0_MODULE,      ///\*TIMER[0] *\/ (0x20910000), */
/*       PHY_BASEADDR_TIMER1_MODULE,      ///\*TIMER[1] *\/ (0x20920000), */
/*     }, */
/*     { PHY_BASEADDR_SDMMC0_MODULE,      ///\*OSDMMC[0] *\/ (0x20930000), */
/*       PHY_BASEADDR_SDMMC1_MODULE,      ///\*OSDMMC[1] *\/ (0x20940000), */
/*     }, */
/*     { PHY_BASEADDR_AXIM_DMA0_MODULE,   ///\*AXIM_DMA[0] *\/ (0x20200000), */
/*       PHY_BASEADDR_AXIM_DMA1_MODULE,   ///\*AXIM_DMA[1] *\/ (0x20210000), */
/*     }, */
/*     { PHY_BASEADDR_I2C0_MODULE,        ///\*I2C[0] *\/ (0x20600000), */
/*       PHY_BASEADDR_I2C1_MODULE,        ///\*I2C[1] *\/ (0x20610000), */
/*       PHY_BASEADDR_I2C2_MODULE,        ///\*I2C[2] *\/ (0x20620000), */
/*       PHY_BASEADDR_I2C3_MODULE,        ///\*I2C[3] *\/ (0x20630000), */
/*       PHY_BASEADDR_I2C4_MODULE,        ///\*I2C[4] *\/ (0x20640000), */
/*       PHY_BASEADDR_I2C5_MODULE,        ///\*I2C[5] *\/ (0x20650000), */
/*       PHY_BASEADDR_I2C6_MODULE,        ///\*I2C[6] *\/ (0x20660000), */
/*       PHY_BASEADDR_I2C7_MODULE,        ///\*I2C[7] *\/ (0x20670000), */
/*       PHY_BASEADDR_I2C8_MODULE,        ///\*I2C[8] *\/ (0x20680000), */
/*       PHY_BASEADDR_I2C9_MODULE,        ///\*I2C[9] *\/ (0x20690000), */
/*       PHY_BASEADDR_I2C10_MODULE,       ///\*I2C[10] *\/ (0x206A0000), */
/*       PHY_BASEADDR_I2C11_MODULE,       ///\*I2C[11] *\/ (0x206B0000), */
/*     }, */
/*     { PHY_BASEADDR_TRIKBOX0, },        ///\*TRIKBOX[0] *\/ (0x20A00000), }, */
/*     { PHY_BASEADDR_AXIM_CPU_SFR0_MODULE, }, ///\*AXIM_CPUSFR[0] *\/ (0x20220000),    }, */
/*     { PHY_BASEADDR_AXIM_CPU_MEM0_MODULE, }, ///\*AXIM_CPUMEM[0] *\/ (0x20230000),    }, */
/*     { PHY_BASEADDR_AXIM_VIP0_MODULE, },     ///\*AXIM_VIP[0] *\/ (0x20240000),    }, */
/*     { PHY_BASEADDR_AXIM_USB0_MODULE, },     ///\*AXIM_USB[0] *\/ (0x20250000),    }, */
/*     { PHY_BASEADDR_AXIM_HOVER0_MODULE, },   ///\*AXIM_HOVER[0] *\/ (0x20260000),  }, */
/*     { PHY_BASEADDR_AXIM_SCALER0_MODULE, },  ///\*AXIM_SCALER[0] *\/ (0x20270000), }, */
/*     { PHY_BASEADDR_AXIM_CODA0_MODULE, },    ///\*AXIM_CODA[0] *\/ (0x20280000),   }, */
/*     { PHY_BASEADDR_AXIM_SDMMC0_MODULE,      ///\*AXIM_OSDMMC[0] *\/ (0x20290000), */
/*       PHY_BASEADDR_AXIM_SDMMC1_MODULE,      ///\*AXIM_OSDMMC[1] *\/ (0x202A0000), */
/*     }, */
/*     { PHY_BASEADDR_ADC0_MODULE, },     ///\*ADC[0] *\/ (0x206C0000),    }, */
/*     { PHY_BASEADDR_WDT0_MODULE, },     // /\*WDT[0] *\/ (0x206D0000),    }, */
/*     { PHY_BASEADDR_DMA_DATA_BUS_CFG0_MODULE, }, ///\*DMA_DATA_BUS_CFG[0] *\/ (0x20300000),    }, */
/*     { PHY_BASEADDR_GPIO0_MODULE,       ///\*GPIO[0] *\/ (0x20700000), */
/*       PHY_BASEADDR_GPIO1_MODULE,       ///\*GPIO[1] *\/ (0x20710000), */
/*       PHY_BASEADDR_GPIO2_MODULE,       ///\*GPIO[2] *\/ (0x20720000), */
/*       PHY_BASEADDR_GPIO3_MODULE,       ///\*GPIO[3] *\/ (0x20730000), */
/*       PHY_BASEADDR_GPIO4_MODULE,       ///\*GPIO[4] *\/ (0x20740000), */
/*       PHY_BASEADDR_GPIO5_MODULE,       ///\*GPIO[5] *\/ (0x20750000), */
/*       PHY_BASEADDR_GPIO6_MODULE,       ///\*GPIO[6] *\/ (0x20760000), */
/*       PHY_BASEADDR_GPIO7_MODULE,       ///\*GPIO[7] *\/ (0x20770000), */
/*     }, */
/*     { PHY_BASEADDR_VIP_DATA_BUS_CFG0_MODULE, },   ///\*VIP_DATA_BUS_CFG[0] *\/ (0x20310000),    }, */
/*     { PHY_BASEADDR_CODA0_DATA_BUS_CFG_MODULE, },  ///\*CODA_DATA_BUS_CFG[0] *\/ (0x20320000),   }, */
/*     { PHY_BASEADDR_MAIN_BUS_CFG0_MODULE, },       ///\*MAIN_BUS_CFG[0] *\/ (0x20330000),        }, */
/*     { PHY_BASEADDR_CFG_BUS_CFG0_MODULE, },        ///\*CFG_BUS_CFG[0] *\/ (0x20340000),         }, */
/* }; */

#endif //__NX_CHIP_SFR_H__
