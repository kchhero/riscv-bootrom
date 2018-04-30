#include <nx_cpuif_regmap.h>
#include <nx_chip_sfr.h>

typedef enum
{
	ePLL0_CLK,
	ePLL1_CLK,
	eOSCCLK_IN,
} NX_CLOCKSOURCE_DEFINES ;

#define __NX_CPUIF_NX01000_CMUTOP_CLOCK_INFO_MAX_CLK_SRC__ 16

enum {
    INTNUM_OF_PWM_0_MODULE_INT0               = 62   // drone_soc.u_CORE.PWM_0.IRQ__INT0
    ,INTNUM_OF_PWM_0_MODULE_INT1               = 63   // drone_soc.u_CORE.PWM_0.IRQ__INT1
    ,INTNUM_OF_PWM_0_MODULE_INT2               = 64   // drone_soc.u_CORE.PWM_0.IRQ__INT2
    ,INTNUM_OF_PWM_0_MODULE_INT3               = 65   // drone_soc.u_CORE.PWM_0.IRQ__INT3
    ,INTNUM_OF_PWM_1_MODULE_INT0               = 66   // drone_soc.u_CORE.PWM_1.IRQ__INT0
    ,INTNUM_OF_PWM_1_MODULE_INT1               = 67   // drone_soc.u_CORE.PWM_1.IRQ__INT1
    ,INTNUM_OF_PWM_1_MODULE_INT2               = 68   // drone_soc.u_CORE.PWM_1.IRQ__INT2
    ,INTNUM_OF_PWM_1_MODULE_INT3               = 69   // drone_soc.u_CORE.PWM_1.IRQ__INT3
    ,INTNUM_OF_PWM_2_MODULE_INT0               = 70   // drone_soc.u_CORE.PWM_2.IRQ__INT0
    ,INTNUM_OF_PWM_2_MODULE_INT1               = 71   // drone_soc.u_CORE.PWM_2.IRQ__INT1
    ,INTNUM_OF_PWM_2_MODULE_INT2               = 72   // drone_soc.u_CORE.PWM_2.IRQ__INT2
    ,INTNUM_OF_PWM_2_MODULE_INT3               = 73   // drone_soc.u_CORE.PWM_2.IRQ__INT3
    ,INTNUM_OF_TIMER_0_MODULE_INT0             = 74   // drone_soc.u_CORE.TIMER_0.IRQ__INT0
    ,INTNUM_OF_TIMER_0_MODULE_INT1             = 75   // drone_soc.u_CORE.TIMER_0.IRQ__INT1
    ,INTNUM_OF_TIMER_0_MODULE_INT2             = 76   // drone_soc.u_CORE.TIMER_0.IRQ__INT2
    ,INTNUM_OF_TIMER_0_MODULE_INT3             = 77   // drone_soc.u_CORE.TIMER_0.IRQ__INT3
    ,INTNUM_OF_TIMER_1_MODULE_INT0             = 78   // drone_soc.u_CORE.TIMER_1.IRQ__INT0
    ,INTNUM_OF_TIMER_1_MODULE_INT1             = 79   // drone_soc.u_CORE.TIMER_1.IRQ__INT1
    ,INTNUM_OF_TIMER_1_MODULE_INT2             = 80   // drone_soc.u_CORE.TIMER_1.IRQ__INT2
    ,INTNUM_OF_TIMER_1_MODULE_INT3             = 81   // drone_soc.u_CORE.TIMER_1.IRQ__INT3
};

enum
{
	PWM_TCFG0_PRESCALE		= 0,
	PWM_TCFG0_DEADZONE		= 16
};
enum
{
	PWM_TCFG1_MUX0			= 0
};

enum
{
	PWM_TCON_DEADZONEENB	= 4,
	PWM_TCON_AOTORELOAD		= 3,
	PWM_TCON_OUTINVERTER	= 2,
	PWM_TCON_MANUALUPDATE	= 1,
	PWM_TCON_STARTSTOP		= 0
};
enum
{
	PWM_DIV_TCLK	= 5,
	PWM_DIV_16		= 4,
	PWM_DIV_8		= 3,
	PWM_DIV_4		= 2,
	PWM_DIV_2		= 1,
	PWM_DIV_1		= 0
};

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

typedef struct __nx_clockcontrol_SYSREG_SYS {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_SYSREG_SYS;

typedef struct __nx_clockcontrol_PWM {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK0;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK1;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK2;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK3;
} _nx_clockcontrol_PWM;

typedef struct __nx_clockcontrol_TIMER {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK0;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK1;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK2;
	__nx_cpuif_nx01000_CMUTOP_clock_info__ TCLK3;
} _nx_clockcontrol_TIMER;

typedef struct __nx_clockcontrol_TRIKBOX {
	__nx_cpuif_nx01000_CMUTOP_clock_info__ APB;
} _nx_clockcontrol_TRIKBOX;

typedef struct __nx_cmu_info {
	_nx_clockcontrol_SYS                            SYS[1];
	_nx_clockcontrol_CPU                            CPU[1];
	_nx_clockcontrol_SYS_BUS                        SYS_BUS[1];
	_nx_clockcontrol_AXISRAM                        AXISRAM[1];
	_nx_clockcontrol_SYSREG_SYS                     SYSREG_SYS[1];
	_nx_clockcontrol_PWM                            PWM[3];
	_nx_clockcontrol_TIMER                          TIMER[2];
	_nx_clockcontrol_TRIKBOX                        TRIKBOX[1];

	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_DIVSTOP[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_SRCOFF[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___CLKGRP_SRCOFF_CLR[4];
	const __nx_cpuif_symbol__	CMU_SYS_0___OSCCLK_SEL;
	
} _nx_cmu_info;

/* _nx_cmu_info CMU_INFO = */
/* { */
/* 	/\* PWM *\/ */
/* 	{ */
/* 		/\* PWM[0] *\/ */
/* 		{ */
/* 			/\* APB *\/ */
/* 			{ */
/* 				 /\* is_first_div ([31] 1 : gmuxed ) *\/ 0 */
/* 				,/\* group_clock_source              *\/ { 0,0,0,0 }  */
/* 				,/\* group_all_divider_reset         *\/ { 0,0,0,0 }   */
/* 				,/\* group_gate_mode                 *\/ { 0,0,0,0 } // not supported  */
/* 				,/\* divider_reset                   *\/ {                    0, 0x0, 0,  0 } // not supported */
/* 				,/\* dynamic_divider_value           *\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  */
/* 				,/\* dynamic_divider_stop            *\/ { (unsigned int*)0x20030000, 0x280,16,  1 } */
/* 				,/\* dynamic_divider_busy_status     *\/ { (unsigned int*)0x20030000, 0x260,18,  1 } */
/* 				,/\* clock_enable                    *\/ { (unsigned int*)0x20030000, 0x218, 12,  1 } */
/* 				,/\* clock_sources                   *\/ { 0} */
/* 				,/\* clock_defines                   *\/ { 0} */
/* 				,/\* num_of_clock_src                *\/ 0 */
/* 				,/\* num_of_clock_dst                *\/ 0 */
/* 				,/\* *parent                         *\/ &CMU_INFO.SYS[0].APB */
/* 				,/\* *unused                         *\/ 0 */
/* 				,/\* cur_freq                        *\/ 0 */
/* 				,/\* *cmu_src                        *\/ 0 */
/* 				,/\* clkdivstop                      *\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } */
/* 				,/\* srcoff                          *\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } */
/* 				,/\* srcoff_clr                      *\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } */
/* 			},				 */
/* 			/\* TCLK0 *\/ */
/* 			{ */
/* 				 /\* is_first_div ([31] 1 : gmuxed ) *\/ 0 */
/* 				,/\* group_clock_source              *\/ { 0,0,0,0 }  */
/* 				,/\* group_all_divider_reset         *\/ { 0,0,0,0 }   */
/* 				,/\* group_gate_mode                 *\/ { 0,0,0,0 } // not supported  */
/* 				,/\* divider_reset                   *\/ {                    0, 0x0, 0,  0 } // not supported */
/* 				,/\* dynamic_divider_value           *\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  */
/* 				,/\* dynamic_divider_stop            *\/ { (unsigned int*)0x20030000, 0x280,16,  1 } */
/* 				,/\* dynamic_divider_busy_status     *\/ { (unsigned int*)0x20030000, 0x260,18,  1 } */
/* 				,/\* clock_enable                    *\/ { (unsigned int*)0x20030000, 0x210, 27,  1 } */
/* 				,/\* clock_sources                   *\/ { 0} */
/* 				,/\* clock_defines                   *\/ { 0} */
/* 				,/\* num_of_clock_src                *\/ 0 */
/* 				,/\* num_of_clock_dst                *\/ 0 */
/* 				,/\* *parent                         *\/ &CMU_INFO.SYS[0].AXI */
/* 				,/\* *unused                         *\/ 0 */
/* 				,/\* cur_freq                        *\/ 0 */
/* 				,/\* *cmu_src                        *\/ 0 */
/* 				,/\* clkdivstop                      *\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } */
/* 				,/\* srcoff                          *\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } */
/* 				,/\* srcoff_clr                      *\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } */
/* 			},				 */
/* 			/\* TCLK1 *\/ */
/* 			{ */
/* 				 /\* is_first_div ([31] 1 : gmuxed ) *\/ 0 */
/* 				,/\* group_clock_source              *\/ { 0,0,0,0 }  */
/* 				,/\* group_all_divider_reset         *\/ { 0,0,0,0 }   */
/* 				,/\* group_gate_mode                 *\/ { 0,0,0,0 } // not supported  */
/* 				,/\* divider_reset                   *\/ {                    0, 0x0, 0,  0 } // not supported */
/* 				,/\* dynamic_divider_value           *\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  */
/* 				,/\* dynamic_divider_stop            *\/ { (unsigned int*)0x20030000, 0x280,16,  1 } */
/* 				,/\* dynamic_divider_busy_status     *\/ { (unsigned int*)0x20030000, 0x260,18,  1 } */
/* 				,/\* clock_enable                    *\/ { (unsigned int*)0x20030000, 0x210, 28,  1 } */
/* 				,/\* clock_sources                   *\/ { 0} */
/* 				,/\* clock_defines                   *\/ { 0} */
/* 				,/\* num_of_clock_src                *\/ 0 */
/* 				,/\* num_of_clock_dst                *\/ 0 */
/* 				,/\* *parent                         *\/ &CMU_INFO.SYS[0].AXI */
/* 				,/\* *unused                         *\/ 0 */
/* 				,/\* cur_freq                        *\/ 0 */
/* 				,/\* *cmu_src                        *\/ 0 */
/* 				,/\* clkdivstop                      *\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } */
/* 				,/\* srcoff                          *\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } */
/* 				,/\* srcoff_clr                      *\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } */
/* 			},				 */
/* 			/\* TCLK2 *\/ */
/* 			{ */
/* 				 /\* is_first_div ([31] 1 : gmuxed ) *\/ 0 */
/* 				,/\* group_clock_source              *\/ { 0,0,0,0 }  */
/* 				,/\* group_all_divider_reset         *\/ { 0,0,0,0 }   */
/* 				,/\* group_gate_mode                 *\/ { 0,0,0,0 } // not supported  */
/* 				,/\* divider_reset                   *\/ {                    0, 0x0, 0,  0 } // not supported */
/* 				,/\* dynamic_divider_value           *\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  */
/* 				,/\* dynamic_divider_stop            *\/ { (unsigned int*)0x20030000, 0x280,16,  1 } */
/* 				,/\* dynamic_divider_busy_status     *\/ { (unsigned int*)0x20030000, 0x260,18,  1 } */
/* 				,/\* clock_enable                    *\/ { (unsigned int*)0x20030000, 0x210, 29,  1 } */
/* 				,/\* clock_sources                   *\/ { 0} */
/* 				,/\* clock_defines                   *\/ { 0} */
/* 				,/\* num_of_clock_src                *\/ 0 */
/* 				,/\* num_of_clock_dst                *\/ 0 */
/* 				,/\* *parent                         *\/ &CMU_INFO.SYS[0].AXI */
/* 				,/\* *unused                         *\/ 0 */
/* 				,/\* cur_freq                        *\/ 0 */
/* 				,/\* *cmu_src                        *\/ 0 */
/* 				,/\* clkdivstop                      *\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } */
/* 				,/\* srcoff                          *\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } */
/* 				,/\* srcoff_clr                      *\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } */
/* 			},				 */
/* 			/\* TCLK3 *\/ */
/* 			{ */
/* 				 /\* is_first_div ([31] 1 : gmuxed ) *\/ 0 */
/* 				,/\* group_clock_source              *\/ { 0,0,0,0 }  */
/* 				,/\* group_all_divider_reset         *\/ { 0,0,0,0 }   */
/* 				,/\* group_gate_mode                 *\/ { 0,0,0,0 } // not supported  */
/* 				,/\* divider_reset                   *\/ {                    0, 0x0, 0,  0 } // not supported */
/* 				,/\* dynamic_divider_value           *\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  */
/* 				,/\* dynamic_divider_stop            *\/ { (unsigned int*)0x20030000, 0x280,16,  1 } */
/* 				,/\* dynamic_divider_busy_status     *\/ { (unsigned int*)0x20030000, 0x260,18,  1 } */
/* 				,/\* clock_enable                    *\/ { (unsigned int*)0x20030000, 0x210, 30,  1 } */
/* 				,/\* clock_sources                   *\/ { 0} */
/* 				,/\* clock_defines                   *\/ { 0} */
/* 				,/\* num_of_clock_src                *\/ 0 */
/* 				,/\* num_of_clock_dst                *\/ 0 */
/* 				,/\* *parent                         *\/ &CMU_INFO.SYS[0].AXI */
/* 				,/\* *unused                         *\/ 0 */
/* 				,/\* cur_freq                        *\/ 0 */
/* 				,/\* *cmu_src                        *\/ 0 */
/* 				,/\* clkdivstop                      *\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } */
/* 				,/\* srcoff                          *\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } */
/* 				,/\* srcoff_clr                      *\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } */
/* 			},				 */
/* 		}, /\* PWM[0] *\/ */
/* 		/\* /\\* PWM[1] *\\/ *\/ */
/* 		/\* { *\/ */
/* 		/\* 	/\\* APB *\\/ *\/ */
/* 		/\* 	{ *\/ */
/* 		/\* 		 /\\* is_first_div ([31] 1 : gmuxed ) *\\/ 0 *\/ */
/* 		/\* 		,/\\* group_clock_source              *\\/ { 0,0,0,0 }  *\/ */
/* 		/\* 		,/\\* group_all_divider_reset         *\\/ { 0,0,0,0 }   *\/ */
/* 		/\* 		,/\\* group_gate_mode                 *\\/ { 0,0,0,0 } // not supported  *\/ */
/* 		/\* 		,/\\* divider_reset                   *\\/ {                    0, 0x0, 0,  0 } // not supported *\/ */
/* 		/\* 		,/\\* dynamic_divider_value           *\\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  *\/ */
/* 		/\* 		,/\\* dynamic_divider_stop            *\\/ { (unsigned int*)0x20030000, 0x280,16,  1 } *\/ */
/* 		/\* 		,/\\* dynamic_divider_busy_status     *\\/ { (unsigned int*)0x20030000, 0x260,18,  1 } *\/ */
/* 		/\* 		,/\\* clock_enable                    *\\/ { (unsigned int*)0x20030000, 0x218, 13,  1 } *\/ */
/* 		/\* 		,/\\* clock_sources                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* clock_defines                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* num_of_clock_src                *\\/ 0 *\/ */
/* 		/\* 		,/\\* num_of_clock_dst                *\\/ 0 *\/ */
/* 		/\* 		,/\\* *parent                         *\\/ &CMU_INFO.SYS[0].APB *\/ */
/* 		/\* 		,/\\* *unused                         *\\/ 0 *\/ */
/* 		/\* 		,/\\* cur_freq                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* *cmu_src                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* clkdivstop                      *\\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff                          *\\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff_clr                      *\\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } *\/ */
/* 		/\* 	},				 *\/ */
/* 		/\* 	/\\* TCLK0 *\\/ *\/ */
/* 		/\* 	{ *\/ */
/* 		/\* 		 /\\* is_first_div ([31] 1 : gmuxed ) *\\/ 0 *\/ */
/* 		/\* 		,/\\* group_clock_source              *\\/ { 0,0,0,0 }  *\/ */
/* 		/\* 		,/\\* group_all_divider_reset         *\\/ { 0,0,0,0 }   *\/ */
/* 		/\* 		,/\\* group_gate_mode                 *\\/ { 0,0,0,0 } // not supported  *\/ */
/* 		/\* 		,/\\* divider_reset                   *\\/ {                    0, 0x0, 0,  0 } // not supported *\/ */
/* 		/\* 		,/\\* dynamic_divider_value           *\\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  *\/ */
/* 		/\* 		,/\\* dynamic_divider_stop            *\\/ { (unsigned int*)0x20030000, 0x280,16,  1 } *\/ */
/* 		/\* 		,/\\* dynamic_divider_busy_status     *\\/ { (unsigned int*)0x20030000, 0x260,18,  1 } *\/ */
/* 		/\* 		,/\\* clock_enable                    *\\/ { (unsigned int*)0x20030000, 0x210, 31,  1 } *\/ */
/* 		/\* 		,/\\* clock_sources                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* clock_defines                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* num_of_clock_src                *\\/ 0 *\/ */
/* 		/\* 		,/\\* num_of_clock_dst                *\\/ 0 *\/ */
/* 		/\* 		,/\\* *parent                         *\\/ &CMU_INFO.SYS[0].AXI *\/ */
/* 		/\* 		,/\\* *unused                         *\\/ 0 *\/ */
/* 		/\* 		,/\\* cur_freq                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* *cmu_src                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* clkdivstop                      *\\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff                          *\\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff_clr                      *\\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } *\/ */
/* 		/\* 	},				 *\/ */
/* 		/\* 	/\\* TCLK1 *\\/ *\/ */
/* 		/\* 	{ *\/ */
/* 		/\* 		 /\\* is_first_div ([31] 1 : gmuxed ) *\\/ 0 *\/ */
/* 		/\* 		,/\\* group_clock_source              *\\/ { 0,0,0,0 }  *\/ */
/* 		/\* 		,/\\* group_all_divider_reset         *\\/ { 0,0,0,0 }   *\/ */
/* 		/\* 		,/\\* group_gate_mode                 *\\/ { 0,0,0,0 } // not supported  *\/ */
/* 		/\* 		,/\\* divider_reset                   *\\/ {                    0, 0x0, 0,  0 } // not supported *\/ */
/* 		/\* 		,/\\* dynamic_divider_value           *\\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  *\/ */
/* 		/\* 		,/\\* dynamic_divider_stop            *\\/ { (unsigned int*)0x20030000, 0x280,16,  1 } *\/ */
/* 		/\* 		,/\\* dynamic_divider_busy_status     *\\/ { (unsigned int*)0x20030000, 0x260,18,  1 } *\/ */
/* 		/\* 		,/\\* clock_enable                    *\\/ { (unsigned int*)0x20030000, 0x214, 0,  1 } *\/ */
/* 		/\* 		,/\\* clock_sources                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* clock_defines                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* num_of_clock_src                *\\/ 0 *\/ */
/* 		/\* 		,/\\* num_of_clock_dst                *\\/ 0 *\/ */
/* 		/\* 		,/\\* *parent                         *\\/ &CMU_INFO.SYS[0].AXI *\/ */
/* 		/\* 		,/\\* *unused                         *\\/ 0 *\/ */
/* 		/\* 		,/\\* cur_freq                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* *cmu_src                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* clkdivstop                      *\\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff                          *\\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff_clr                      *\\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } *\/ */
/* 		/\* 	},				 *\/ */
/* 		/\* 	/\\* TCLK2 *\\/ *\/ */
/* 		/\* 	{ *\/ */
/* 		/\* 		 /\\* is_first_div ([31] 1 : gmuxed ) *\\/ 0 *\/ */
/* 		/\* 		,/\\* group_clock_source              *\\/ { 0,0,0,0 }  *\/ */
/* 		/\* 		,/\\* group_all_divider_reset         *\\/ { 0,0,0,0 }   *\/ */
/* 		/\* 		,/\\* group_gate_mode                 *\\/ { 0,0,0,0 } // not supported  *\/ */
/* 		/\* 		,/\\* divider_reset                   *\\/ {                    0, 0x0, 0,  0 } // not supported *\/ */
/* 		/\* 		,/\\* dynamic_divider_value           *\\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  *\/ */
/* 		/\* 		,/\\* dynamic_divider_stop            *\\/ { (unsigned int*)0x20030000, 0x280,16,  1 } *\/ */
/* 		/\* 		,/\\* dynamic_divider_busy_status     *\\/ { (unsigned int*)0x20030000, 0x260,18,  1 } *\/ */
/* 		/\* 		,/\\* clock_enable                    *\\/ { (unsigned int*)0x20030000, 0x214, 1,  1 } *\/ */
/* 		/\* 		,/\\* clock_sources                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* clock_defines                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* num_of_clock_src                *\\/ 0 *\/ */
/* 		/\* 		,/\\* num_of_clock_dst                *\\/ 0 *\/ */
/* 		/\* 		,/\\* *parent                         *\\/ &CMU_INFO.SYS[0].AXI *\/ */
/* 		/\* 		,/\\* *unused                         *\\/ 0 *\/ */
/* 		/\* 		,/\\* cur_freq                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* *cmu_src                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* clkdivstop                      *\\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff                          *\\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff_clr                      *\\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } *\/ */
/* 		/\* 	},				 *\/ */
/* 		/\* 	/\\* TCLK3 *\\/ *\/ */
/* 		/\* 	{ *\/ */
/* 		/\* 		 /\\* is_first_div ([31] 1 : gmuxed ) *\\/ 0 *\/ */
/* 		/\* 		,/\\* group_clock_source              *\\/ { 0,0,0,0 }  *\/ */
/* 		/\* 		,/\\* group_all_divider_reset         *\\/ { 0,0,0,0 }   *\/ */
/* 		/\* 		,/\\* group_gate_mode                 *\\/ { 0,0,0,0 } // not supported  *\/ */
/* 		/\* 		,/\\* divider_reset                   *\\/ {                    0, 0x0, 0,  0 } // not supported *\/ */
/* 		/\* 		,/\\* dynamic_divider_value           *\\/ { (unsigned int*)0x20030000, 0x260, 0, 16 }  *\/ */
/* 		/\* 		,/\\* dynamic_divider_stop            *\\/ { (unsigned int*)0x20030000, 0x280,16,  1 } *\/ */
/* 		/\* 		,/\\* dynamic_divider_busy_status     *\\/ { (unsigned int*)0x20030000, 0x260,18,  1 } *\/ */
/* 		/\* 		,/\\* clock_enable                    *\\/ { (unsigned int*)0x20030000, 0x214, 2,  1 } *\/ */
/* 		/\* 		,/\\* clock_sources                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* clock_defines                   *\\/ { 0} *\/ */
/* 		/\* 		,/\\* num_of_clock_src                *\\/ 0 *\/ */
/* 		/\* 		,/\\* num_of_clock_dst                *\\/ 0 *\/ */
/* 		/\* 		,/\\* *parent                         *\\/ &CMU_INFO.SYS[0].AXI *\/ */
/* 		/\* 		,/\\* *unused                         *\\/ 0 *\/ */
/* 		/\* 		,/\\* cur_freq                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* *cmu_src                        *\\/ 0 *\/ */
/* 		/\* 		,/\\* clkdivstop                      *\\/ { (unsigned int*)0x20030000, 0x20, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff                          *\\/ { (unsigned int*)0x20030000, 0x40, 0,  1 } *\/ */
/* 		/\* 		,/\\* srcoff_clr                      *\\/ { (unsigned int*)0x20030000, 0x50, 0,  1 } *\/ */
/* 		/\* 	},				 *\/ */
/* 		/\* }, /\\* PWM[1] *\\/ *\/ */
/*         }, */
/* }; */

typedef struct _NX_PWM_RegisterSet
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
}NX_PWM_RegisterSet;

/* typedef struct _NX_INTC_RegisterSet */
/* { */
/*     volatile unsigned char     _Reserved0[0x01000-0x00000];    //    : 0x00000 ~ 0x01000 : Reserved */
/*     // Distributor Register Map */
/*     volatile unsigned int    GICD_CTRL;                      // RW : 0x01000 : Distributor Control Register */
/*     volatile unsigned int    GICD_TYPER;                     // RO : 0x01004 : Interrupt Controller Type Register */
/*     volatile unsigned int    GICD_IIDR;                      // RO : 0x01008 : Distributor Implementer Idenfification Register */
/*     volatile unsigned char     _Reserved1[0x01080-0x0100C];    //    : 0x0100C ~ 0x0107C : Reserved */
/*     volatile unsigned int    GICD_IGROUPn_SPI[32];           // RW : 0x01080 ~ 0x010FC : Interrupt Group Register for SPI */
/*     volatile unsigned int    GICD_ISENABLERn_SPI[32];        // RW : 0x01100 ~ 0x0117C : Interrupt Set Enable Register for SPI */
/*     volatile unsigned int    GICD_ISCENABLER_SPI[32];        // RW : 0x01180 ~ 0x011FC : Interrupt Clear-Enable Register for SPI */
/*     volatile unsigned int    GICD_ISPENDRn_SPI[32];          // RW : 0x01200 ~ 0x0127C : Interrupt Set-Pending Register for SPI */
/*     volatile unsigned int    GICD_ICPENDRn_SPI[32];          // RW : 0x01280 ~ 0x012FC : Interrupt Clear-Pending Register for SPI */
/*     volatile unsigned int    GICD_ISACTIVERn_SPI[32];        // RW : 0x01300 ~ 0x0137C : Interrupt Set-Active Register */
/*     volatile unsigned char     _Reserved2[0x01400-0x01380];    //      0x01380 ~ 0x013FC : Reserved */
/*     volatile unsigned char     GICD_IPRIORITYRn_SPI[1020];      // RW : 0x01420 ~ 0x017F8 : Interrupt Priority Level Register for SPI */
/*     volatile unsigned int    _Reserved3;                     //      0x017FC : Reserved */
/*     volatile unsigned char     GICD_ITARGETSRn_SPI[1020];       // RW : 0x0181F ~ 0x01BFB : Interrupt Processor Target Register for SPI */
/*     volatile unsigned int    _Reserved4;                     //      0x01BFC : Reversed */
/*     volatile unsigned int    GICD_ICFGRn_SPI[64];            // RW : 0x01C08 ~ 0x01CFC : Interrupt Configuration Register for SPI */
/*     volatile unsigned int    GICD_PPISR;                     // RO : 0x01D00 :           PPI Interrupt Status Register */
/*     volatile unsigned int    GICD_SPISRn[31];                // RO : 0x01D04 ~ 0x01D7C : SPI Interrupt Status Register */
/*     volatile unsigned char     _Reserved5[0x01F00-0x01D80];    //      0x01D80 ~ 0x01EFC : Reserved */
/*     volatile unsigned int    GICD_SGIR;                      // WO : 0x01F00 : Software Generated Interrupt Register */
/*     volatile unsigned char     _Reserved6[0x01F10-0x01F04];    //    : 0x01F04 ~ 0x01F0C : Reserved */
/*     volatile unsigned int    GICD_CPENDSGIRn[4];             // RW : 0x01F10 ~ 0x01F1C : SGI Clear-Pending Register */
/*     volatile unsigned int    GICD_SPENDSGIRn[4];             // RW : 0x01F20 ~ 0x01F2C : SGI Set-Pending Register */
/*     volatile unsigned char     _Reserved7[0x01FD0-0x01F30];    //      0x01F30 ~ 0x01FCC : Reserved */
/*     volatile unsigned int    GICD_PIDR4;                     // RO : 0x01FD0 : Peripheral ID 4 Register   */
/*     volatile unsigned int    GICD_PIDR5;                     // RO : 0x01FD4 : Peripheral ID 5 Register   */
/*     volatile unsigned int    GICD_PIDR6;                     // RO : 0x01FD8 : Peripheral ID 6 Register   */
/*     volatile unsigned int    GICD_PIDR7;                     // RO : 0x01FDC : Peripheral ID 7 Register   */
/*     volatile unsigned int    GICD_PIDR0;                     // RO : 0x01FE0 : Peripheral ID 0 Register   */
/*     volatile unsigned int    GICD_PIDR1;                     // RO : 0x01FE4 : Peripheral ID 1 Register   */
/*     volatile unsigned int    GICD_PIDR2;                     // RO : 0x01FE8 : Peripheral ID 2 Register   */
/*     volatile unsigned int    GICD_PIDR3;                     // RO : 0x01FEC : Peripheral ID 3 Register   */
/*     volatile unsigned int    GICD_CIDR0;                     // RO : 0x01FF0 : Component ID 0 Register   */
/*     volatile unsigned int    GICD_CIDR1;                     // RO : 0x01FF4 : Component ID 1 Register   */
/*     volatile unsigned int    GICD_CIDR2;                     // RO : 0x01FF8 : Component ID 2 Register   */
/*     volatile unsigned int    GICD_CIDR3;                     // RO : 0x01FFC : Component ID 3 Register   */
/*     // Cpu Interface Register Map */
/*     volatile unsigned int    GICC_CTRL;                      // RW : 0x02000 : CPU Interface Control Register */
/*     volatile unsigned int    GICC_PMR;                       // RW : 0x02004 : Interrupt Priority Mask Register */
/*     volatile unsigned int    GICC_BPR;                       // RW : 0x02008 : Binary Point Register */
/*     volatile unsigned int    GICC_IAR;                       // RO : 0x0200C : Interrupt Acknowledge Register */
/*     volatile unsigned int    GICC_EOIR;                      // WO : 0x02010 : End of Interrupt Register */
/*     volatile unsigned int    GICC_RPR;                       // RO : 0x02014 : Running Priority Register */
/*     volatile unsigned int    GICC_HPPIR;                     // RO : 0x02018 : Highest Pending Interrupt Register */
/*     volatile unsigned int    GICC_ABPR;                      // RW : 0x0201C : Aliased Binary Point Register */
/*     volatile unsigned int    GICC_AIAR;                      // RO : 0x02020 : Aliased Interrupt Acknowledge Regsiter */
/*     volatile unsigned int    GICC_AEOIR;                     // WO : 0x02024 : Aliased End of Interrupt Register */
/*     volatile unsigned int    GICC_AHPPIR;                    // RO : 0x02028 : Aliased Highest Priority Pending Interrupt Register */
/*     volatile unsigned char     _Reserved8[0x020D0-0x0202C];    //      0x020CC ~ 0x0202C : Reserved */
/*     volatile unsigned int    GICC_APR0;                      // RW : 0x020D0 : Active Priority Register */
/*     volatile unsigned char     _Reserved9[0x020E0-0x020D4];    //      0x020D4 ~ 0x020DC : Reserved */
/*     volatile unsigned int    GICC_NSAPR0;                    // RW : 0x020E0 : Non-Secure Active Prioirty Register */
/*     volatile unsigned char     _Reserved10[0x020FC-0x020E4];    //      0x020E4 ~ 0x020F8 : Reserved */
/*     volatile unsigned int    GICC_IIDR;                      // RO : 0x020FC : CPU Interface Implementer Identification Register */
/*     volatile unsigned char     _Reserved11[0x03000-0x02100];   //      0x021FC ~ 0x02FFC : Reserved */
/*     volatile unsigned int    GICC_DIR;                       // WO : 0x03000 : Deactive Interrupt Register */
/*     volatile unsigned char     _Reserved12[0x04000-0x03004];   //      0x03004 ~ 0x03FFC : Reserved */
                
/*     // GIC Virtual Interface Control Register */
/*     volatile unsigned int    GICH_HCR;                       // RW : 0x04000 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR;                       // RO : 0x04004 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR;                      // RW : 0x04008 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved13;                    //      0x0400C : Reserved  */
/*     volatile unsigned int    GICH_MISR;                      // RO : 0x04010 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved14[0x04020-0x04014];   //      0x04014 ~ 0x0401C : Reserved */
/*     volatile unsigned int    GICH_EISR0;                     // RO : 0x04020 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved15[0x04030-0x04024];   //      0x04024 ~ 0x0402C : Reserved */
/*     volatile unsigned int    GICH_ELSR0;                     // RO : 0x04030 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved16[0x040F0-0x04034];   //      0x04034 ~ 0x040EC : Reserved */
/*     volatile unsigned int    GICH_APR0;                      // RW : 0x040F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved17[0x04100-0x040F4];   //      0x040F4 ~ 0x040FC : Reserved */
/*     volatile unsigned int    GICH_LR0;                       // RW : 0x04100 : List Register 0 */
/*     volatile unsigned int    GICH_LR1;                       // RW : 0x04104 : List Register 1 */
/*     volatile unsigned int    GICH_LR2;                       // RW : 0x04108 : List Register 2 */
/*     volatile unsigned int    GICH_LR3;                       // RW : 0x0410C : List Register 3 */
/*     volatile unsigned char     _Reserved19[0x05000-0x04110];   //      0x04110 ~ 0x04FFC : Reserved */
        
/*     // GIC Virtual Interface Control Register Processor 0 */
/*     volatile unsigned int    GICH_HCR_P0;                    // RW : 0x05000 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P0;                    // RO : 0x05004 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P0;                   // RW : 0x05008 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved20;                    //      0x0500C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P0;                   // RO : 0x05010 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved21[0x05020-0x05014];   //      0x05014 ~ 0x0501C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P0;                  // RO : 0x05020 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved22[0x05030-0x05024];   //      0x05024 ~ 0x0502C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P0;                  // RO : 0x05030 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved23[0x050F0-0x05034];   //      0x05034 ~ 0x050EC : Reserved */
/*     volatile unsigned int    GICH_APR0_P0;                   // RW : 0x050F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved24[0x05100-0x050F4];   //      0x050F4 ~ 0x050FC : Reserved */
/*     volatile unsigned int    GICH_LR0_P0;                    // RW : 0x05100 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P0;                    // RW : 0x05104 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P0;                    // RW : 0x05108 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P0;                    // RW : 0x0510C : List Register 3 */
/*     volatile unsigned char     _Reserved25[0x05200-0x05110];   //      0x05110 ~ 0x051FC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 1 */
/*     volatile unsigned int    GICH_HCR_P1;                    // RW : 0x05200 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P1;                    // RO : 0x05204 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P1;                   // RW : 0x05208 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved26;                    //      0x0520C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P1;                   // RO : 0x05210 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved27[0x05220-0x05214];   //      0x05214 ~ 0x0521C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P1;                  // RO : 0x05220 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved28[0x05230-0x05224];   //      0x05224 ~ 0x0522C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P1;                  // RO : 0x05230 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved29[0x052F0-0x05234];   //      0x05234 ~ 0x052EC : Reserved */
/*     volatile unsigned int    GICH_APR0_P1;                   // RW : 0x052F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved30[0x05300-0x052F4];   //      0x052F4 ~ 0x052FC : Reserved */
/*     volatile unsigned int    GICH_LR0_P1;                    // RW : 0x05300 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P1;                    // RW : 0x05304 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P1;                    // RW : 0x05308 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P1;                    // RW : 0x0530C : List Register 3 */
/*     volatile unsigned char     _Reserved31[0x05400-0x05310];   //      0x05310 ~ 0x053FC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 2 */
/*     volatile unsigned int    GICH_HCR_P2;                    // RW : 0x05400 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P2;                    // RO : 0x05404 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P2;                   // RW : 0x05408 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved32;                    //      0x0540C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P2;                   // RO : 0x05410 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved33[0x05420-0x05414];   //      0x05414 ~ 0x0541C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P2;                  // RO : 0x05420 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved34[0x05430-0x05424];   //      0x05424 ~ 0x0542C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P2;                  // RO : 0x05430 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved35[0x054F0-0x05434];   //      0x05434 ~ 0x054EC : Reserved */
/*     volatile unsigned int    GICH_APR0_P2;                   // RW : 0x054F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved36[0x05500-0x054F4];   //      0x054F4 ~ 0x054FC : Reserved */
/*     volatile unsigned int    GICH_LR0_P2;                    // RW : 0x05500 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P2;                    // RW : 0x05504 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P2;                    // RW : 0x05508 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P2;                    // RW : 0x0550C : List Register 3 */
/*     volatile unsigned char     _Reserved37[0x05600-0x05510];   //      0x05510 ~ 0x055FC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 3 */
/*     volatile unsigned int    GICH_HCR_P3;                    // RW : 0x05600 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P3;                    // RO : 0x05604 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P3;                   // RW : 0x05608 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved38;                    //      0x0560C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P3;                   // RO : 0x05610 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved39[0x05620-0x05614];   //      0x05614 ~ 0x0561C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P3;                  // RO : 0x05620 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved40[0x05630-0x05624];   //      0x05624 ~ 0x0562C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P3;                  // RO : 0x05630 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved41[0x056F0-0x05634];   //      0x05634 ~ 0x056EC : Reserved */
/*     volatile unsigned int    GICH_APR0_P3;                   // RW : 0x056F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved42[0x05700-0x056F4];   //      0x056F4 ~ 0x056FC : Reserved */
/*     volatile unsigned int    GICH_LR0_P3;                    // RW : 0x05700 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P3;                    // RW : 0x05704 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P3;                    // RW : 0x05708 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P3;                    // RW : 0x0570C : List Register 3 */
/*     volatile unsigned char     _Reserved43[0x05800-0x05710];   //      0x05710 ~ 0x057FC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 4 */
/*     volatile unsigned int    GICH_HCR_P4;                    // RW : 0x05800 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P4;                    // RO : 0x05804 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P4;                   // RW : 0x05808 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved44;                    //      0x0580C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P4;                   // RO : 0x05810 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved45[0x05820-0x05814];   //      0x05814 ~ 0x0581C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P4;                  // RO : 0x05820 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved46[0x05830-0x05824];   //      0x05824 ~ 0x0582C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P4;                  // RO : 0x05830 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved47[0x058F0-0x05834];   //      0x05834 ~ 0x058EC : Reserved */
/*     volatile unsigned int    GICH_APR0_P4;                   // RW : 0x058F0 : Active Priority Register */
/*     volatile unsigned char     _Reserved48[0x05900-0x058F4];   //      0x058F4 ~ 0x058FC : Reserved */
/*     volatile unsigned int    GICH_LR0_P4;                    // RW : 0x05900 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P4;                    // RW : 0x05904 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P4;                    // RW : 0x05908 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P4;                    // RW : 0x0590C : List Register 3 */
/*     volatile unsigned char     _Reserved49[0x05A00-0x05910];   //      0x05910 ~ 0x059FC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 5 */
/*     volatile unsigned int    GICH_HCR_P5;                    // RW : 0x05A00 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P5;                    // RO : 0x05A04 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P5;                   // RW : 0x05A08 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved50;                    //      0x05A0C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P5;                   // RO : 0x05A10 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved51[0x05A20-0x05A14];   //      0x05A14 ~ 0x05A1C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P5;                  // RO : 0x05A20 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved52[0x05A30-0x05A24];   //      0x05A24 ~ 0x05A2C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P5;                  // RO : 0x05A30 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved53[0x05AF0-0x05A34];   //      0x05A34 ~ 0x05AEC : Reserved */
/*     volatile unsigned int    GICH_APR0_P5;                   // RW : 0x05AF0 : Active Priority Register */
/*     volatile unsigned char     _Reserved54[0x05B00-0x05AF4];   //      0x05AF4 ~ 0x05AFC : Reserved */
/*     volatile unsigned int    GICH_LR0_P5;                    // RW : 0x05B00 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P5;                    // RW : 0x05B04 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P5;                    // RW : 0x05B08 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P5;                    // RW : 0x05B0C : List Register 3 */
/*     volatile unsigned char     _Reserved55[0x05C00-0x05B10];   //      0x05B10 ~ 0x05BFC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 6 */
/*     volatile unsigned int    GICH_HCR_P6;                    // RW : 0x05C00 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P6;                    // RO : 0x05C04 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P6;                   // RW : 0x05C08 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved56;                    //      0x05C0C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P6;                   // RO : 0x05C10 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved57[0x05C20-0x05C14];   //      0x05C14 ~ 0x05C1C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P6;                  // RO : 0x05C20 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved58[0x05C30-0x05C24];   //      0x05C24 ~ 0x05C2C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P6;                  // RO : 0x05C30 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved59[0x05CF0-0x05C34];   //      0x05C34 ~ 0x05CEC : Reserved */
/*     volatile unsigned int    GICH_APR0_P6;                   // RW : 0x05CF0 : Active Priority Register */
/*     volatile unsigned char     _Reserved60[0x05D00-0x05CF4];   //      0x05CF4 ~ 0x05CFC : Reserved */
/*     volatile unsigned int    GICH_LR0_P6;                    // RW : 0x05D00 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P6;                    // RW : 0x05D04 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P6;                    // RW : 0x05D08 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P6;                    // RW : 0x05D0C : List Register 3 */
/*     volatile unsigned char     _Reserved61[0x05E00-0x05D10];   //      0x05D10 ~ 0x05DFC : Reserved */

/*     // GIC Virtual Interface Control Register Processor 7 */
/*     volatile unsigned int    GICH_HCR_P7;                    // RW : 0x05E00 : Hypervisor Control Register */
/*     volatile unsigned int    GICH_VTR_P7;                    // RO : 0x05E04 : VGIC Type Register, GICH_VTR on page 3-13 */
/*     volatile unsigned int    GICH_VMCR_P7;                   // RW : 0x05E08 : Virtual Machine Control Register */
/*     volatile unsigned int    _Reserved62;                    //      0x05E0C : Reserved  */
/*     volatile unsigned int    GICH_MISR_P7;                   // RO : 0x05E10 : Maintenance Interrupt Status Register */
/*     volatile unsigned char     _Reserved63[0x05E20-0x05E14];   //      0x05E14 ~ 0x05E1C : Reserved */
/*     volatile unsigned int    GICH_EISR0_P7;                  // RO : 0x05E20 : End of Interrupt Status Register */
/*     volatile unsigned char     _Reserved64[0x05E30-0x05E24];   //      0x05E24 ~ 0x05E2C : Reserved */
/*     volatile unsigned int    GICH_ELSR0_P7;                  // RO : 0x05E30 : Empty List register Status Register */
/*     volatile unsigned char     _Reserved65[0x05EF0-0x05E34];   //      0x05E34 ~ 0x05EEC : Reserved */
/*     volatile unsigned int    GICH_APR0_P7;                   // RW : 0x05EF0 : Active Priority Register */
/*     volatile unsigned char     _Reserved66[0x05F00-0x05EF4];   //      0x05EF4 ~ 0x05EFC : Reserved */
/*     volatile unsigned int    GICH_LR0_P7;                    // RW : 0x05F00 : List Register 0 */
/*     volatile unsigned int    GICH_LR1_P7;                    // RW : 0x05F04 : List Register 1 */
/*     volatile unsigned int    GICH_LR2_P7;                    // RW : 0x05F08 : List Register 2 */
/*     volatile unsigned int    GICH_LR3_P7;                    // RW : 0x05F0C : List Register 3 */
/*     volatile unsigned char     _Reserved67[0x06000-0x05F10];   //      0x05F10 ~ 0x05FFC : Reserved */

/*     // GIC Virtual CPU Interface Register */
/*     volatile unsigned int    GICV_CTLR;                      // RW : 0x06000 : Virtual Machine Control Register */
/*     volatile unsigned int    GICV_PMR;                       // RW : 0x06004 : VM Priority Mask Register */
/*     volatile unsigned int    GICV_BPR;                       // RW : 0x06008 : VM Binary Point Register */
/*     volatile unsigned int    GICV_IAR;                       // RO : 0x0600C : VM Interrupt Acknowledge Register */
/*     volatile unsigned int    GICV_EOIR;                      // WO : 0x06010 : VM End of Interrupt Register */
/*     volatile unsigned int    GICV_RPR;                       // RO : 0x06014 : VM Running Priority Register */
/*     volatile unsigned int    GICV_HPPIR;                     // RO : 0x06018 : VM Highest Priority Pending Interrupt Registerb */
/*     volatile unsigned int    GICV_ABPR;                      // RW : 0x0601C : VM Aliased Binary Point Register */
/*     volatile unsigned int    GICV_AIAR;                      // RO : 0x06020 : VM Aliased Interrupt Acknowledge Register */
/*     volatile unsigned int    GICV_AEOIR;                     // WO : 0x06024 : VM Aliased End of Interrupt Register */
/*     volatile unsigned int    GICV_AHPPIR;                    // RO : 0x06028 : VM Aliased Highest Priority Pending Interrupt Registerb */
/*     volatile unsigned char     _Reserved68[0x060D0-0x0602C];   //      0x0602C ~ 0x060CC : Reserved */
/*     volatile unsigned int    GICV_APR0;                      // RW : 0x060D0 : VM Active Priority Register */
/*     volatile unsigned char     _Reserved69[0x060FC-0x060D4];   //      0x060D4 ~ 0x060F8 : Reserved */
/*     volatile unsigned int    GICV_IIDR;                      // RO : 0x060FC : VM CPU Interface Identification Register, GICV_IIDR on page 3-15 */
/*     volatile unsigned char     _Reserved70[0x07000-0x06100];   //      0x06100 ~ 0x06FF8 : Reserved */
/*     volatile unsigned int    GICV_DIR;                       // WO : 0x07000 : VM Deactivate Interrupt Register */
/* }NX_INTC_RegisterSet; */
