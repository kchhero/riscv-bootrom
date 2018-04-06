#ifndef NX_SYSCON_H
#define NX_SYSCON_H

typedef struct syscon_cpuif {
    unsigned int Reserved:28;
    unsigned int bootmode:1;                //BOOTMODE	"0: SDMMC 1: SPI"
    unsigned int clr_reset_reason:1;        //1:clear RESET_REASON
    unsigned int reset_reason:1;            //1:reset by WDT
    unsigned int not_use_wdt:1;             //0: use WDT reset, 1: not use WDT reset
} SYSCON_CPUIF;
/*
RUN_COUNTER Register 0				
Address : 4(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN0_MAXCOUNT		0x40
RUN_COUNTER Register 1				
Address : 8(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN1_MAXCOUNT		0x40
RUN_COUNTER Register 2				
Address : C(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN2_MAXCOUNT		0x40
RUN_COUNTER Register 3				
Address : 10(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN3_MAXCOUNT		0x40
RUN_COUNTER Register 4				
Address : 14(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN4_MAXCOUNT		0x40
RUN_COUNTER Register 5				
Address : 18(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN5_MAXCOUNT		0x40
RUN_COUNTER Register 6				
Address : 1C(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN6_MAXCOUNT		0x40
RUN_COUNTER Register 7				
Address : 20(SYSCON_CPUIF)				
[31:16]	R	Reserved	Reserved	0
[15:0]	R/W	RUN7_MAXCOUNT		0x40
*/

#endif	/* NX_SYSCON_H */
