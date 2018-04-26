#include <nx_cpuif_regmap.h>
#include <nx_chip_sfr.h>

typedef enum
{
	ePLL0_CLK,
	ePLL1_CLK,
	eOSCCLK_IN,
} NX_CLOCKSOURCE_DEFINES ;

#define __NX_CPUIF_NX01000_CMUTOP_CLOCK_INFO_MAX_CLK_SRC__ 16

typedef struct __nx_cpuif_nx01000_CMUTOP_clock_info_struct__ {
	int                         is_first_div           ;
	const __nx_cpuif_symbol__	group_clock_source     ;
	const __nx_cpuif_symbol__	group_all_divider_reset;
	const __nx_cpuif_symbol__	group_gate_mode        ;
	const __nx_cpuif_symbol__	divider_reset;
	const __nx_cpuif_symbol__	dynamic_divider_value;
	const __nx_cpuif_symbol__	dynamic_divider_stop;
	const __nx_cpuif_symbol__	dynamic_divider_busy_status;
	const __nx_cpuif_symbol__	clock_enable;
	double *clock_src_freq[__NX_CPUIF_NX01000_CMUTOP_CLOCK_INFO_MAX_CLK_SRC__];
	const NX_CLOCKSOURCE_DEFINES clock_src_def[__NX_CPUIF_NX01000_CMUTOP_CLOCK_INFO_MAX_CLK_SRC__];
	unsigned int    num_of_clock_src  ;
	unsigned int    num_of_clock_dst  ;
	void   *parent  ;
	void   *unused  ;
	double cur_freq ;
	void   *cmu_src  ;
	const __nx_cpuif_symbol__	clkdivstop;
	const __nx_cpuif_symbol__	srcoff;
	const __nx_cpuif_symbol__	srcoff_clr;
} __nx_cpuif_nx01000_CMUTOP_clock_info__;

typedef struct __nx_clockcontrol_SYS {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CLK400;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CLK133;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CLK50;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CLK40;
} _nx_clockcontrol_SYS;

typedef struct __nx_clockcontrol_CPU {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_CPU;

typedef struct __nx_clockcontrol_SYS_BUS {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_SYS_BUS;

typedef struct __nx_clockcontrol_AXISRAM {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
} _nx_clockcontrol_AXISRAM;

typedef struct __nx_clockcontrol_VIP {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ PADOUT0;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ PADOUT1;
} _nx_clockcontrol_VIP;

typedef struct __nx_clockcontrol_SPI {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_SPI;

typedef struct __nx_clockcontrol_C2DL {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AHB;
} _nx_clockcontrol_C2DL;

typedef struct __nx_clockcontrol_SCALER {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_SCALER;

typedef struct __nx_clockcontrol_CODA {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_CODA;

typedef struct __nx_clockcontrol_RSP {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_RSP;

typedef struct __nx_clockcontrol_QSPI {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_QSPI;

typedef struct __nx_clockcontrol_SYSREG_SYS {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_SYSREG_SYS;

typedef struct __nx_clockcontrol_CRYPTO {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_CRYPTO;

typedef struct __nx_clockcontrol_DDRC {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ DDRC;
} _nx_clockcontrol_DDRC;

typedef struct __nx_clockcontrol_DMA {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_DMA;

typedef struct __nx_clockcontrol_HOVER {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AXI;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ HOVER_SYS;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ HOVER_CAM;
} _nx_clockcontrol_HOVER;

typedef struct __nx_clockcontrol_UART {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_UART;

typedef struct __nx_clockcontrol_USART {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
} _nx_clockcontrol_USART;

typedef struct __nx_clockcontrol_PWM {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK0;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK1;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK2;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK3;
} _nx_clockcontrol_PWM;

typedef struct __nx_clockcontrol_USB {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AHB;
} _nx_clockcontrol_USB;

typedef struct __nx_clockcontrol_TIMER {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK0;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK1;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK2;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK3;
} _nx_clockcontrol_TIMER;

typedef struct __nx_clockcontrol_OSDMMC {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ CORE;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ AHB;
} _nx_clockcontrol_OSDMMC;

typedef struct __nx_clockcontrol_I2C {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_I2C;

typedef struct __nx_clockcontrol_TRIKBOX {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_TRIKBOX;

typedef struct __nx_clockcontrol_ADC {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_ADC;

typedef struct __nx_clockcontrol_WDT {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ POR;
} _nx_clockcontrol_WDT;

typedef struct __nx_clockcontrol_GPIO {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_GPIO;

typedef struct __nx_cmu_info {
	_nx_clockcontrol_SYS                            SYS[1];
	_nx_clockcontrol_CPU                            CPU[1];
	_nx_clockcontrol_SYS_BUS                        SYS_BUS[1];
	_nx_clockcontrol_AXISRAM                        AXISRAM[1];
	_nx_clockcontrol_VIP                            VIP[1];
	_nx_clockcontrol_SPI                            SPI[3];
	_nx_clockcontrol_C2DL                           C2DL[1];
	_nx_clockcontrol_SCALER                         SCALER[1];
	_nx_clockcontrol_CODA                           CODA[1];
	_nx_clockcontrol_RSP                            RSP[1];
	_nx_clockcontrol_QSPI                           QSPI[5];
	_nx_clockcontrol_SYSREG_SYS                     SYSREG_SYS[1];
	_nx_clockcontrol_CRYPTO                         CRYPTO[1];
	_nx_clockcontrol_DDRC                           DDRC[1];
	_nx_clockcontrol_DMA                            DMA[2];
	_nx_clockcontrol_HOVER                          HOVER[1];
	_nx_clockcontrol_UART                           UART[2];
	_nx_clockcontrol_USART                          USART[4];
	_nx_clockcontrol_PWM                            PWM[3];
	_nx_clockcontrol_USB                            USB[1];
	_nx_clockcontrol_TIMER                          TIMER[2];
	_nx_clockcontrol_OSDMMC                         OSDMMC[2];
	_nx_clockcontrol_I2C                            I2C[12];
	_nx_clockcontrol_TRIKBOX                        TRIKBOX[1];
	_nx_clockcontrol_ADC                            ADC[1];
	_nx_clockcontrol_WDT                            WDT[1];
	_nx_clockcontrol_GPIO                           GPIO[8];

	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_DIVSTOP[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_SRCOFF[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_SRCOFF_CLR[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___OSCCLK_SEL;
	
} _nx_cmu_info;

_nx_cmu_info CMU_INFO =
{
	/* PWM */
	{
		/* PWM[0] */
		{
			/* APB */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x218, 12,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].APB
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK0 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x210, 27,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK1 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x210, 28,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK2 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x210, 29,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK3 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x210, 30,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
		}, /* PWM[0] */
		/* PWM[1] */
		{
			/* APB */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x218, 13,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].APB
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK0 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x210, 31,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK1 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x214, 0,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK2 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x214, 1,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
			/* TCLK3 */
			{
				 /* is_first_div ([31] 1 : gmuxed ) */ 0
				,/* group_clock_source              */ { 0,0,0,0 } 
				,/* group_all_divider_reset         */ { 0,0,0,0 }  
				,/* group_gate_mode                 */ { 0,0,0,0 } // not supported 
				,/* divider_reset                   */ {                    0, 0x0, 0,  0 } // not supported
				,/* dynamic_divider_value           */ { (unsigned int*)0x20030000, 0x260, 0, 16 } 
				,/* dynamic_divider_stop            */ { (unsigned int*)0x20030000, 0x280,16,  1 }
				,/* dynamic_divider_busy_status     */ { (unsigned int*)0x20030000, 0x260,18,  1 }
				,/* clock_enable                    */ { (unsigned int*)0x20030000, 0x214, 2,  1 }
				,/* clock_sources                   */ { 0}
				,/* clock_defines                   */ { 0}
				,/* num_of_clock_src                */ 0
				,/* num_of_clock_dst                */ 0
				,/* *parent                         */ &CMU_INFO.SYS[0].AXI
				,/* *unused                         */ 0
				,/* cur_freq                        */ 0
				,/* *cmu_src                        */ 0
				,/* clkdivstop                      */ { (unsigned int*)0x20030000, 0x20, 0,  1 }
				,/* srcoff                          */ { (unsigned int*)0x20030000, 0x40, 0,  1 }
				,/* srcoff_clr                      */ { (unsigned int*)0x20030000, 0x50, 0,  1 }
			},				
		}, /* PWM[1] */
        },
};

struct NX_PWM_RegisterSet
{
	volatile unsigned int T0CFG0		;	// 0x00		// R/W	// Timer Configuration Register 0 that configures the two 8-bit Prescaler and DeadZone Length
	volatile unsigned int T0CFG1		;	// 0x04		// R/W	// Timer Configuration Register 1 that controls 5 MUX and DMA Mode Select Bit
	volatile unsigned int T0CON      ;	// 0x08		// R/W	// Timer Control Register
	volatile unsigned int T0CNTB     ;	// 0x0C		// R/W	// Timer 0 Count Buffer Register
	volatile unsigned int T0CMPB     ;	// 0x10		// R/W	// Timer 0 Compare Buffer Register
	volatile unsigned int T0CNTO     ;	// 0x14		// R	// Timer 0 Count Observation Register
	volatile unsigned int T0INT_CSTAT;	// 0x18		// R/W	// Timer Interrupt Control and Status Register
	volatile unsigned int __reserved0[57];	// 0x1C~0xFF
	volatile unsigned int T1CFG0		;	// 0x100	// R/W	// Timer Configuration Register 0 that configures the two 8-bit Prescaler and DeadZone Length
	volatile unsigned int T1CFG1		;	// 0x104	// R/W	// Timer Configuration Register 1 that controls 5 MUX and DMA Mode Select Bit
	volatile unsigned int T1CON      ;	// 0x108	// R/W	// Timer Control Register
 	volatile unsigned int T1CNTB     ;	// 0x10C	// R/W	// Timer 1 Count Buffer Register
	volatile unsigned int T1CMPB     ;	// 0x110	// R/W	// Timer 1 Compare Buffer Register
	volatile unsigned int T1CNTO     ;	// 0x114	// R	// Timer 1 Count Observation Register
	volatile unsigned int T1INT_CSTAT;	// 0x118	// R/W	// Timer Interrupt Control and Status Register
	volatile unsigned int __reserved1[57];	// 0x1C~0xFF
	volatile unsigned int T2CFG0		;	// 0x200	// R/W	// Timer Configuration Register 0 that configures the two 8-bit Prescaler and DeadZo
};
