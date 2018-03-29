#ifndef __NX_CLOCK_H__
#define __NX_CLOCK_H__

#include "nx_cmu.h"

void setcpuclock(unsigned int clklvl);
void setsystemclock(unsigned int clklvl);
void setdeviceclock(const struct cmu_device_clk *pdclk, int num, int enb);

#endif //__NX_CLOCK_H__
