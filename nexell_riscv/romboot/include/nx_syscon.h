#ifndef NX_SYSCON_H
#define NX_SYSCON_H

typedef struct syscon_cpuif {
    unsigned int Reserved:28;
    unsigned int bootmode:1;                //BOOTMODE	"0: SDMMC 1: SPI"
    unsigned int clr_reset_reason:1;        //1:clear RESET_REASON
    unsigned int reset_reason:1;            //1:reset by WDT
    unsigned int not_use_wdt:1;             //0: use WDT reset, 1: not use WDT reset
} SYSCON_CPUIF;

#endif	/* NX_SYSCON_H */
