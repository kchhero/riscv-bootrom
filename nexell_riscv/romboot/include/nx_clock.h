#ifndef __NX_CLOCK_H__
#define __NX_CLOCK_H__

#include "nx_chip_sfr.h"
#include "nx_pll.h"

//MHz
#define NX_CMU_CLK_CLK400    400   //divider 1
#define NX_CMU_CLK_AXI       200   //divider 2
#define NX_CMU_CLK_133       133   //divider 3
#define NX_CMU_CLK_APB       100   //divider 4
#define NX_CMU_CLK_50         50   //divider 8
#define NX_CMU_CLK_40         40   //divider 10

typedef struct cmu_devclk {
	unsigned short offset;
	unsigned char srcbit;
	unsigned char clkenbbit;
	unsigned char clksrc;
	unsigned char div;
} CMU_DEVICE_CLK;

void nxSetDeviceClock(const CMU_DEVICE_CLK *pdclk, int num, int enb);
void nxSetClockInit(void);
void __PLL_CPUIF_SET_BASEADDR (unsigned int moduleindex, unsigned int *baseaddr);
void NX_PLL_SetBaseAddress(unsigned int inst_index, unsigned int BaseAddress);
void NX_PLL_SetOSCMUX( unsigned int inst_index, unsigned int MUXSEL );

#endif //__NX_CLOCK_H__
