#ifndef __SYS_HEADER_H__
#define __SYS_HEADER_H__

#include "cfgBootDefine.h"

#include <nx_pyrope.h>
#include <nx_type.h>
//#include <nx_debug2.h>
#include <nx_chip.h>
#include <nx_rstcon.h>

#include <nx_clkpwr.h>
#include <nx_ecid.h>
#include <nx_gpio.h>
#include <nx_alive.h>
#include <nx_rtc.h>
#include <nx_tieoff.h>
#include <nx_intc.h>
#include <nx_clkgen.h>
#include <nx_ssp.h>
#include <nx_uart.h>
#include <nx_wdt.h>

#include "secondboot.h"
#include "printf.h"
#include "debug.h"
#include "type.h"

//------------------------------------------------------------------------------
//  Set DEBUG Macro
//------------------------------------------------------------------------------

#if defined(SYSLOG_ON)
#define SYSMSG printf
#else
#define SYSMSG(x, ...)
#endif

// Memory debug message
#if defined(SYSLOG_ON)
#define MEMMSG  printf
#else
#define MEMMSG(x, ...)
#endif

// UserDebug Message
#if 0
#define DBGOUT  printf
#else
#define DBGOUT(x, ...)
#endif

//------------------------------------------------------------------------------
//  Set global variables
//------------------------------------------------------------------------------

#if defined(__SET_GLOBAL_VARIABLES)

struct NX_SecondBootInfo *const pSBI =
    (struct NX_SecondBootInfo * const)BASEADDR_SRAM;
struct NX_SecondBootInfo *const pTBI =
    (struct NX_SecondBootInfo * const)BASEADDR_SRAM;
struct NX_GPIO_RegisterSet (*const pReg_GPIO)[1] =
    (struct NX_GPIO_RegisterSet (*const)[])PHY_BASEADDR_GPIOA_MODULE;
struct NX_ALIVE_RegisterSet *const pReg_Alive =
    (struct NX_ALIVE_RegisterSet * const)PHY_BASEADDR_ALIVE_MODULE;
struct NX_TIEOFF_RegisterSet *const pReg_Tieoff =
    (struct NX_TIEOFF_RegisterSet * const)PHY_BASEADDR_TIEOFF_MODULE;
struct NX_ECID_RegisterSet *const pReg_ECID =
    (struct NX_ECID_RegisterSet * const)PHY_BASEADDR_ECID_MODULE;
struct NX_CLKPWR_RegisterSet *const pReg_ClkPwr =
    (struct NX_CLKPWR_RegisterSet * const)PHY_BASEADDR_CLKPWR_MODULE;
struct NX_RSTCON_RegisterSet *const pReg_RstCon =
    (struct NX_RSTCON_RegisterSet * const)PHY_BASEADDR_RSTCON_MODULE;
struct NX_DREXSDRAM_RegisterSet *const pReg_Drex =
    (struct NX_DREXSDRAM_RegisterSet * const)PHY_BASEADDR_DREX_MODULE_CH0_APB;
struct NX_DDRPHY_RegisterSet *const pReg_DDRPHY =
    (struct NX_DDRPHY_RegisterSet * const)PHY_BASEADDR_DREX_MODULE_CH1_APB;
struct NX_RTC_RegisterSet *const pReg_RTC =
    (struct NX_RTC_RegisterSet * const)PHY_BASEADDR_RTC_MODULE;

struct NX_WDT_RegisterSet *const pReg_WDT =
    (struct NX_WDT_RegisterSet * const)PHY_BASEADDR_WDT_MODULE;

#else

extern struct NX_SecondBootInfo *const pSBI; // second boot info
extern struct NX_SecondBootInfo *const pTBI; // third boot info
extern struct NX_GPIO_RegisterSet (*const pReg_GPIO)[1];
extern struct NX_ALIVE_RegisterSet *const pReg_Alive;
extern struct NX_TIEOFF_RegisterSet *const pReg_Tieoff;
extern struct NX_ECID_RegisterSet *const pReg_ECID;
extern struct NX_CLKPWR_RegisterSet *const pReg_ClkPwr;
extern struct NX_RSTCON_RegisterSet *const pReg_RstCon;
extern struct NX_DREXSDRAM_RegisterSet *const pReg_Drex;
extern struct NX_DDRPHY_RegisterSet *const pReg_DDRPHY;
extern struct NX_RTC_RegisterSet *const pReg_RTC;
#endif

#endif //	__SYS_HEADER_H__
