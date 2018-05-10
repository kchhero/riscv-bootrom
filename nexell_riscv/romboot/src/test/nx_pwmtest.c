#include <nx_swallow.h>
#include <nx_swallow_printf.h>
#include <nx_swallow_platform.h>
#include "nx_pwmtest.h"
#include <nx_clock.h>
#include <riscv_csr.h>

#define PWM_MODULE_NUM 1

//static void InterruptHandler_PWM_T0 (void *pArgument);
//static const unsigned int INTERRUPT_COUNT    = 3;
//static unsigned int		NX_PWM_GetInterruptNumber( unsigned int ModuleIndex );
//static struct NX_INTC_RegisterSet * __g_pRegister[1] = { 0, };
//static void Stop_Timer( unsigned int ModuleNum, unsigned int SubModuleIndex );
//static void    NX_INTC_SetPriorityMask( unsigned int ModuleIndex, unsigned int CpuNum, unsigned char Priority );
//static unsigned int		NX_PWM_GetT0InterruptNumber( unsigned int ModuleIndex );
//static int TestDone[2];
static void Run_Timer( unsigned int ModuleIndex, unsigned int TestNum );
void	NX_PWM_SetT0InterruptEnable( unsigned int ModuleIndex, int Enable );
//void	NX_INTC_SetInterruptEnable( int IntNum, int Enable );
//int 	NX_INT_RegisterHandler( unsigned int intNum, void (*pHandler)(void *), void *pArgument ) ;

/* typedef enum */
/* { */
/*     NX_INTC_TARGET_CPU0     = 0x01  , */
/*     NX_INTC_TARGET_CPU1     = 0x02  , */
/*     NX_INTC_TARGET_CPU2     = 0x04  , */
/*     NX_INTC_TARGET_CPU3     = 0x08  , */
/*     NX_INTC_TARGET_CPU4     = 0x10  , */
/*     NX_INTC_TARGET_CPU5     = 0x20  , */
/*     NX_INTC_TARGET_CPU6     = 0x40  , */
/*     NX_INTC_TARGET_CPU7     = 0x80    */
/* } NX_INTC_TARGETID; */

static unsigned int PRESCALEVALUE	[4] = {1,1,1,1};
//static unsigned int TEST_CNT     	[4] = {5,10,200,1600};
//static unsigned int TEST_CMP     	[4] = {2,2,2,2};

typedef struct tagHandlerList 
{
    struct tagHandlerList *next;		// next pointer
    void (*pHandler)( void * );			// handler
    void *pArgument;					// argument for handler
} HANDLERLIST, *PHANDLERLIST;

/* #define NX_INTC_MAX_HANDLERS 256 */
/* #define NX_INTC_NUM_OF_INT  (256 + 32) */

/* static int         s_HandlerCount=0; */
/* static HANDLERLIST s_HandlerPool[NX_INTC_MAX_HANDLERS]; */
/* static PHANDLERLIST s_HandlerPoolFree = 0; */
/* static PHANDLERLIST	s_pHandlerList[NX_INTC_NUM_OF_INT]; // static variables are automatically initialized by '0' */

NX_PWM_RegisterSet * const pgPWMReg[2] =
    {
        (NX_PWM_RegisterSet *)PHY_BASEADDR_PWM0_MODULE,
        (NX_PWM_RegisterSet *)PHY_BASEADDR_PWM1_MODULE,
    };

/* NX_INTC_RegisterSet * const pgINTCReg[1] = */
/* { */
/*     (NX_INTC_RegisterSet *)0x220C0000, */
/* }; */

void NX_CLOCK_SetClockGroupEnable ( __nx_cpuif_nx01000_CMUTOP_clock_info__ *Clock, int Enable )
{
    /* NX_ASSERT( 0 != Clock                        ); */
    /* NX_ASSERT( 0 != Clock->clock_enable.baseaddr ); */

    __nx_cpuif_symbol__ clock_disable;
    clock_disable.baseaddr = Clock->clock_enable.baseaddr;
    clock_disable.offset   =(Clock->clock_enable.offset   + 0x10); // clear register offset
    clock_disable.startbit = Clock->clock_enable.startbit;
    clock_disable.bitwidth = Clock->clock_enable.bitwidth;

    if( Enable == 1 ) { nx_cpuif_reg_write_one_notread( Clock->clock_enable, 1); }
    else              { nx_cpuif_reg_write_one_notread(       clock_disable, 1); }
		
}

void pwm_test_init(void)
{
    _dprintf("<<bootrom>> pll set start\n");
    
    nxSetClockInit();

    csr_write(mie, 0x800);
    csr_write(mtvec, 0x20948888);

    plic_test();
    
    _dprintf("<<bootrom>> pwm test start\n");
    _dprintf("1 mip = 0x%x\n",csr_read(mip));
    _dprintf("1 mie = 0x%x\n",csr_read(mie));        
    _dprintf("1 medeleg = 0x%x\n",csr_read(medeleg));
    _dprintf("1 mideleg = 0x%x\n",csr_read(mideleg));        
    _dprintf("1 mstatus = 0x%x\n",csr_read(mstatus));

    register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0];
    
    // clk reset enable
    /* NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].APB, 1 ); */
	
    /* NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK0, 1 ); */
    //    NX_CLOCK_SetClockSourceDivisor( &CMU_INFO.PWM[i].TCLK0, 200, 0 );
    //static const float TCLK1 = 499.92f;
    /* NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK1, 1 ); */
	
    /* //static const float TCLK2 = 9.9984f; */
    /* NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK2, 1 ); */
	
    /* //static const float TCLK3 = 1.249801f; */
    /* NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK3, 1 ); */

    _dprintf("PWM setClockGroupEnable Done\n");
    // Set BaseAddress, PAD enable
    //    __g_ModuleVariables[0].pRegister = (struct NX_PWM_RegisterSet *)PHY_BASEADDR_PWM0_MODULE;

    // Interrupt RegisterHandler
    NX_PWM_SetT0InterruptEnable( 0, 1 );
    /* NX_PWM_SetT1InterruptEnable( moduleNum, 1 ); */
    /* NX_PWM_SetT2InterruptEnable( moduleNum, 1 ); */
    /* NX_PWM_SetT3InterruptEnable( moduleNum, 1 ); */

    /* NX_INTC_SetInterruptEnable( NX_PWM_GetInterruptNumber(0), 1 ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetInterruptNumber(0), InterruptHandler_PWM_T0, (void*)0 ); */

    /* NX_INTC_SetInterruptEnable( i, 1 ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT1InterruptNumber(moduleNum), InterruptHandler_PWM_T1, (void*)moduleNum ); */

    /* NX_INT_Enable( NX_PWM_GetT2InterruptNumber(moduleNum) ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT2InterruptNumber(moduleNum), InterruptHandler_PWM_T2, (void*)moduleNum ); */

    /* NX_INT_Enable( NX_PWM_GetT3InterruptNumber(moduleNum) ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT3InterruptNumber(moduleNum), InterruptHandler_PWM_T3, (void*)moduleNum ); */

    _dprintf("2 mip = 0x%x\n",csr_read(mip));
    _dprintf("2 mie = 0x%x\n",csr_read(mie));        
    _dprintf("2 medeleg = 0x%x\n",csr_read(medeleg));
    _dprintf("2 mideleg = 0x%x\n",csr_read(mideleg));        
    _dprintf("2 mstatus = 0x%x\n",csr_read(mstatus));
    
    Run_Timer(0, 1);

    while(1) {
        _dprintf("mip = 0x%x\n",csr_read(mip));
        _dprintf("mie = 0x%x\n",csr_read(mie));
        _dprintf("medeleg = 0x%x\n",csr_read(medeleg));
        _dprintf("mideleg = 0x%x\n",csr_read(mideleg));        
        _dprintf("mstatus = 0x%x\n",csr_read(mstatus));
        
    }
}

/* int 	NX_INT_RegisterHandler( unsigned int intNum, void (*pHandler)(void *), void *pArgument )  */
/* { */
/* 	PHANDLERLIST pCurList = s_pHandlerList[intNum];		 */
/* 	PHANDLERLIST pPreList = 0;		 */

/*         _dprintf("INT RegisterHandler Start \n"); */
/* 	/\* NX_ASSERT( NX_INTC_NUM_OF_INT > intNum ); *\/ */
/* 	/\* NX_ASSERT( 0 != pHandler ); *\/ */
/* 	/\* NX_ASSERT( CPU_MODE_IRQ != NX_ARM_GetCurMode() ); *\/ */

/* //	NX_ARM_EnterCriticalSection(); */
	
/* 	while( 0 != pCurList )  */
/* 	{ */
/* 		pPreList = pCurList; */
/* 		pCurList = pCurList->next; */
/* 	} */
	
/* 	if( s_HandlerPoolFree ) */
/* 	{ */
/* 		pCurList =  s_HandlerPoolFree; */
/* 		s_HandlerPoolFree = s_HandlerPoolFree->next; */
/* 	} */
/* 	else */
/* 	{ */
/* 		pCurList =  s_HandlerPool + s_HandlerCount++; ;//(PHANDLERLIST)malloc(sizeof(HANDLERLIST)); */
/*                 //		NX_ASSERT( sizeof(s_HandlerPool)/sizeof(s_HandlerPool[0]) >=s_HandlerCount ); */
/* 	} */
/*         //	NX_ASSERT( 0 != pCurList ); */
/* 	pCurList->next 		= 0; */
/* 	pCurList->pHandler 	= pHandler; */
/* 	pCurList->pArgument = pArgument; */

/* 	if( 0 == pPreList )		s_pHandlerList[intNum] = pCurList; */
/* 	else				pPreList->next = pCurList; */

/* //	NX_ARM_LeaveCriticalSection(); */
/* 	return 1; */
/* } */

static void Run_Timer( unsigned int ModuleIndex, unsigned int TestNum )
{
    unsigned int regvalue;
    unsigned int GoldenT0CNT, GoldenT0CMP;
    unsigned int DEADZONELENGTH = 0;
    register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0];

    _dprintf("Run Timer Start\n");
    _dprintf("pPWMReg = 0x%x\n",pPWMReg);
    /* _dprintf("T0CFG0 = 0x%x\n",pPWMReg->T0CFG0); */
    /* _dprintf("T0CON = 0x%x\n",pPWMReg->T0CON); */
    /* _dprintf("T0CNTB = 0x%x\n",pPWMReg->T0CNTB); */
    /* _dprintf("T0CNTO = 0x%x\n",pPWMReg->T0CNTO); */
    /* _dprintf("T0INT_CSTAT = 0x%x\n",pPWMReg->T0INT_CSTAT); */

    // pwm setting
    // TCFG0 - prescaler / deadzone
    regvalue = (DEADZONELENGTH<<PWM_TCFG0_DEADZONE) | (PRESCALEVALUE[0]<<PWM_TCFG0_PRESCALE);
    //WriteIODW(pPWMReg->T0CFG0, regvalue);
    pPWMReg->T0CFG0 = regvalue;
    _dprintf("T0CFG0 = 0x%x\n",regvalue);
    /* regvalue = (PWM_DIV_1<<PWM_TCFG1_MUX0); */
    /* WriteIODW(pPWMReg->T0CFG1, regvalue); */

    // TCON - deadzone en[4] / auto reload[3] / output inverter[2] / manual update[1] / start/stop[0]
    regvalue =  (0x0<<PWM_TCON_DEADZONEENB) | (0x1<<PWM_TCON_AOTORELOAD) | (0x0<<PWM_TCON_OUTINVERTER) |
        (0x0<<PWM_TCON_MANUALUPDATE) | (0x0<<PWM_TCON_STARTSTOP);
    //NX_CONSOLE_Printf("regvalue : 0x%x, SubModuleIndex : %d\n", regvalue, SubModuleIndex);
    pPWMReg->T0CON = regvalue;
    _dprintf("T0CON = 0x%x\n",regvalue);
    
    // TCNTB, TCMPB
    //    WriteIODW(pPWMReg->T0CNTB, TEST_CNT[0]);
    regvalue = 100;
    pPWMReg->T0CNTB = regvalue;
    //    pPWMReg->T0CMPB, TEST_CMP[0]);
    _dprintf("T0CNTB = 0x%x\n",regvalue);
    
    // TCON - deadzone en[4] / auto reload[3] / output inverter[2] / manual update[1] / start/stop[0]
    regvalue =  (0x0<<PWM_TCON_DEADZONEENB) | (0x1<<PWM_TCON_AOTORELOAD) | (0x0<<PWM_TCON_OUTINVERTER) |
        (0x1<<PWM_TCON_MANUALUPDATE) | (0x0<<PWM_TCON_STARTSTOP);
    //NX_CONSOLE_Printf("regvalue : 0x%x, SubModuleIndex : %d\n", regvalue, SubModuleIndex);
    pPWMReg->T0CON = regvalue;
    _dprintf("T0CON = 0x%x\n",regvalue);
    
    // TCON - deadzone en[4] / auto reload[3] / output inverter[2] / manual update[1] / start/stop[0]
    regvalue =  (0x0<<PWM_TCON_DEADZONEENB) | (0x1<<PWM_TCON_AOTORELOAD) | (0x0<<PWM_TCON_OUTINVERTER) |
        (0x0<<PWM_TCON_MANUALUPDATE) | (0x1<<PWM_TCON_STARTSTOP);
    pPWMReg->T0CON = regvalue;
    _dprintf("T0CON = 0x%x\n",regvalue);
}

/* static void    NX_INTC_SetPriorityMask( unsigned int ModuleIndex, unsigned int CpuNum, unsigned char Priority ) */
/* { */
/*     register NX_INTC_RegisterSet * const pINTCReg = pgINTCReg[0]; */
    
/*     /\*     NX_ASSERT( NUMBER_OF_INTC_MODULE > ModuleIndex ); *\/ */
/*     /\* NX_ASSERT( __g_pRegister[ModuleIndex] != 0 ); *\/ */

/*     _dprintf("INTC SetPriorityMask \n"); */
/*     CpuNum = CpuNum; */

/*     WriteIODW( pINTCReg->GICC_PMR, Priority ); */
/*     //WriteIODW( &pRegister->GICC_PMR, (Priority<<4) ); */
/* } */

/* void	NX_INTC_SetInterruptEnable( int IntNum, int Enable ) */
/* { */
/*     register NX_INTC_RegisterSet * const pINTCReg = pgINTCReg[0]; */
/*     register unsigned int RegSel, RegBit, RegVal; */

/*     _dprintf("INTC SetInterruptEnable, pINTCReg = 0x%x\n",pINTCReg); */
/*     _dprintf("INTC SetInterruptEnable, IntNum = 0x%x\n",IntNum); */
/*     // @Todo sei - decide Target ID */
/*     if(15<IntNum){ */
/*         WriteIOB( pINTCReg->GICD_ITARGETSRn_SPI[IntNum], NX_INTC_TARGET_CPU0 ); */
/*     } */
/*     _dprintf("INTC SetInterruptEnable 1\n"); */
/*     NX_INTC_SetPriorityMask( 0, 0, 0xff ); */
/*     //NX_INTC_SetPriority( 0, IntNum, 1 ); */

/*     // @Todo sei - decide GIC module index, IntNum, RegBit; */
/*     //    pRegister = __g_pRegister[0]; */

/*     RegSel = IntNum >> 5; */
/*     RegBit = IntNum & 0x1F; */

/*     //RegVal = ~(pRegister->GICD_ISCENABLER_SPI[RegSel]) & pRegister->GICD_ISENABLERn_SPI[RegSel]; */
/*     //RegVal = pRegister->GICD_ISENABLERn_SPI[RegSel]; */

/*     _dprintf("INTC SetInterruptEnable 2\n"); */
/*     if( Enable ) */
/*     { */
/*         RegVal = ((unsigned int)(0x1 << RegBit)); */
/*     	//WriteIODW( &pRegister->GICD_ISCENABLER_SPI[RegSel], ~RegVal ); */
/*         _dprintf("INTC SetInterruptEnable 3\n"); */
/*     	WriteIODW( pINTCReg->GICD_ISENABLERn_SPI[RegSel], RegVal  ); */
/*     } */
/*     else */
/*     { */
/*         RegVal = ((unsigned int)(0x1 << RegBit)); */
/*     	WriteIODW( pINTCReg->GICD_ISCENABLER_SPI[RegSel], RegVal ); */
/*     } */
/*     _dprintf("INTC SetInterruptEnable Done \n"); */
/* } */

/* static void Stop_Timer( unsigned int ModuleNum, unsigned int SubModuleIndex ) */
/* { */
/*     register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0]; */
        
/*     if(SubModuleIndex==0x0) { */
/* 	WriteIODW(pPWMReg->T0CON, 0x0); */
/*     } else if(SubModuleIndex==0x1) { */
/*         WriteIODW(pPWMReg->T1CON, 0x0); */
/*     /\* } else if(SubModuleIndex==0x2) { *\/ */
/*     /\*     WriteIODW(&__g_ModuleVariables[ModuleNum].pRegister->T2CON, 0x0); *\/ */
/*     /\* } else { *\/ */
/*     /\*     WriteIODW(&__g_ModuleVariables[ModuleNum].pRegister->T3CON, 0x0); *\/ */
/*     } */
/*     //	TestDone[ModuleNum][SubModuleIndex] = 1; */
/* } */

/* static void InterruptHandler_PWM_T0 (void *pArgument) { */
/*     int pwm_idx = (int)pArgument; */
/*     const unsigned int	PEND_POS	=	5; */
/*     const unsigned int	PEND_MASK	=	0x1; */
/*     register unsigned int PendEnb       =       0; */
    
/*     register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0]; */
/*     static unsigned int interruptCount	[PWM_MODULE_NUM][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}; */
    
/*     interruptCount[pwm_idx][0]++; */
    
/*     if(interruptCount[pwm_idx][0]>=INTERRUPT_COUNT){ */
/*     	Stop_Timer( (unsigned int)pwm_idx, (unsigned int)0x0 ); */
/*     } */
    
/*     /\* NX_ASSERT( NUMBER_OF_PWM_MODULE > ModuleIndex ); *\/ */
/*     /\* NX_ASSERT( 0 != __g_ModuleVariables[ModuleIndex].pRegister ); *\/ */

/*     PendEnb	=	ReadIODW(pPWMReg->T0INT_CSTAT); */
/*     WriteIODW(pPWMReg->T0INT_CSTAT, (PEND_MASK << PEND_POS) | PendEnb ); */
/* } */

/* static unsigned int		NX_PWM_GetInterruptNumber( unsigned int ModuleIndex ) */
/* { */
/* 	const unsigned int	PWMInterruptNumber[PWM_MODULE_NUM] = */
/* 				{ */
/* 					INTNUM_OF_PWM_0_MODULE_INT0, */
/* 					INTNUM_OF_PWM_1_MODULE_INT0, */
/* 					INTNUM_OF_PWM_2_MODULE_INT0, */
/* 					INTNUM_OF_TIMER_0_MODULE_INT0, */
/* 					INTNUM_OF_TIMER_1_MODULE_INT0 */
/* 				}; */

/* 	return	PWMInterruptNumber[ModuleIndex]; */
/* } */

void	NX_PWM_SetT0InterruptEnable( unsigned int ModuleIndex, int Enable )
{
    const unsigned int	PEND_POS	=	0;
    const unsigned int	PEND_MASK	=	0x1 << PEND_POS;

    register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0];
    //	register struct NX_PWM_RegisterSet*	pRegister;
    register unsigned int	ReadValue;

    /* NX_ASSERT( NUMBER_OF_PWM_MODULE > ModuleIndex ); */
    /* NX_ASSERT( (0==Enable) || (1==Enable) ); */
    /* NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister ); */

    //	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;
    ReadValue	=	ReadIODW(pPWMReg->T0INT_CSTAT) & ~PEND_MASK;
    ReadValue	|=	(unsigned int)Enable << PEND_POS;

    pPWMReg->T0INT_CSTAT = ReadValue;
    _dprintf("SetT0InterruptEnable T0INT_CSTAT = 0x%x\n",ReadValue);
    //WriteIODW(pPWMReg->T0INT_CSTAT, ReadValue);
}

/* void	NX_PWM_SetInterruptEnable( unsigned int ModuleIndex, int Enable ) */
/* { */
/*     register NX_PWM_RegisterSet * const pPWMReg = pgPWMReg[0]; */
/*     const unsigned int	PEND_POS	=	0; */
/*     const unsigned int	PEND_MASK	=	0x1 << PEND_POS; */
/*     register unsigned int	ReadValue; */
        
        
/*     switch(ModuleIndex) */
/*         { */
/*         case 0 : ReadValue	=	ReadIODW(pPWMReg->T0INT_CSTAT) & ~PEND_MASK; */
/*             ReadValue	|=	(unsigned int)Enable << PEND_POS; */
/*             pPWMReg->T0INT_CSTAT = ReadValue; */
/*             //WriteIODW(pPWMReg->T0INT_CSTAT, ReadValue); */
/*             break; */
/*         case 1 : ReadValue	=	ReadIODW(pPWMReg->T1INT_CSTAT) & ~PEND_MASK; */
/*             ReadValue	|=	(unsigned int)Enable << PEND_POS; */
/*             pPWMReg->T1INT_CSTAT = ReadValue; */
/*             //WriteIODW(pPWMReg->T1INT_CSTAT, ReadValue); */
/*             break; */
/*         }                  */
/* } */

#define PLIC_BASE		0x0C000000
#define ENABLE_BASE		0x2000
#define ENABLE_PER_HART		0x80

#define CONTEXT_BASE		0x200000
#define CONTEXT_PER_HART	0x1000
#define CONTEXT_THRESHOLD	0
#define CONTEXT_CLAIM		4

#define PRIORITY_BASE        0
#define PRIORITY_PER_ID        4

unsigned int *plic_priority(unsigned int *base, int hwirq)
{
    return base + PRIORITY_BASE + (hwirq * PRIORITY_PER_ID)/4;
}


unsigned int *plic_enable_vector(unsigned int *base, unsigned int hartid)
{
    return base + (hartid + ENABLE_BASE + hartid * ENABLE_PER_HART)/4;
}

unsigned int *plic_hart_claim(unsigned int *base, unsigned int hartid)
{
    return base + (CONTEXT_BASE + CONTEXT_PER_HART * hartid + CONTEXT_CLAIM)/4;
}

unsigned int plic_claim(unsigned int *base, int hartid)
{
    unsigned int *pclaim = plic_hart_claim(base, hartid);
    return *pclaim;
}

void plic_complete(unsigned int *base, int hartid, unsigned int claim)
{
    unsigned int *pclaim = plic_hart_claim(base, hartid);
    *pclaim = claim;
}

void plic_disable(unsigned int *base, int hartid, int hwirq)
{
    unsigned int *reg = plic_enable_vector(base, hartid) + (hwirq / 32);
    unsigned int mask = ~(1 << (hwirq % 32));

    unsigned int val = *reg;
    val = val & mask;
    *reg = val;
}

void plic_enable(unsigned int *base, int hartid, int hwirq)
{
    unsigned int *reg = plic_enable_vector(base, hartid) + (hwirq / 32);
    unsigned int bit = 1 << (hwirq % 32);

    unsigned int val = *reg;

    _dprintf("plic_enable val = 0x%x\n", val);
 
    val = val | bit;
    *reg = val;

    _dprintf("plic_enable reg = 0x%x\n", reg);
    _dprintf("plic_enable val = 0x%x\n", val);
    _dprintf("plic_enable regval = 0x%x\n", *reg);
}

void plic_irq_enable(unsigned int *base, int hartid, int hwirq)
{
    unsigned int *priority = plic_priority(base, hwirq);

    *priority = 1;
    _dprintf("hwirq = 0x%x\n", hwirq);
    _dprintf("priority = 0x%x\n", priority);
    _dprintf("priority value = 0x%x\n", *priority);        
    plic_enable(base, hartid, hwirq);
}

void plic_irq_disable(unsigned int *base, int hartid, int hwirq)
{
    unsigned int *priority = plic_priority(base, hwirq);

    *priority = 0;
    plic_disable(base, hartid, hwirq);
}

void plic_irq_eoi(unsigned int *base, int hartid, int hwirq)
{
    plic_complete(base, hartid, hwirq);
}

unsigned int *plic_hart_threshold(unsigned int *base, int hartid)
{
    return base + (CONTEXT_BASE + CONTEXT_PER_HART * hartid + CONTEXT_THRESHOLD)/4;
}

void plic_test(void)
{
    /* timer0 interrupt is 62 */
    _dprintf("<<bootrom>> plic_test start\n");
    unsigned int *threshold = plic_hart_threshold((unsigned int*)PLIC_BASE, 0);
    *threshold  = 0;

    _dprintf("threshold = 0x%x\n", threshold);
    _dprintf("threshold val = 0x%x\n", *threshold);

    plic_irq_enable((unsigned int*)PLIC_BASE, 0, 62);
    
    _dprintf("<<bootrom>> plic_test end\n");    
}
