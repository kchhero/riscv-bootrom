#ifndef __NX_CHIP_SFR_H__
#define __NX_CHIP_SFR_H__

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

extern _SFR_INFO SFR_INFO;

#endif //__NX_CHIP_SFR_H__
