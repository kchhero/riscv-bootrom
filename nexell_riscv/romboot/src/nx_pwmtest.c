#include <nx_swallow.h>
#include <nx_swallow_printf.h>
#include "nx_pwmtest.h"

#define PWM_MODULE_NUM 1

static void InterruptHandler_PWM_T0 (void *pArgument);
static const unsigned int INTERRUPT_COUNT    = 3;
static unsigned int interruptCount	[PWM_MODULE_NUM][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
static unsigned int		NX_PWM_GetInterruptNumber( unsigned int ModuleIndex );

static	struct
{
	struct NX_PWM_RegisterSet *pRegister;
} __g_ModuleVariables[2] = { {0, }, };

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
    _dprintf("<<bootrom>> pwm test start\n");

    __g_ModuleVariables[0].pRegister = (struct NX_PWM_RegisterSet *)PHY_BASEADDR_PWM0_MODULE;
        
    // clk reset enable
    NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].APB, 1 );
	
    NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK0, 1 );
    //    NX_CLOCK_SetClockSourceDivisor( &CMU_INFO.PWM[i].TCLK0, 200, 0 );
    //static const float TCLK1 = 499.92f;
    NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK1, 1 );
	
    //static const float TCLK2 = 9.9984f;
    NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK2, 1 );
	
    //static const float TCLK3 = 1.249801f;
    NX_CLOCK_SetClockGroupEnable  ( &CMU_INFO.PWM[0].TCLK3, 1 );


    // Set BaseAddress, PAD enable
    __g_ModuleVariables[0].pRegister = (struct NX_PWM_RegisterSet *)PHY_BASEADDR_PWM0_MODULE;

    // Interrupt RegisterHandler
    NX_PWM_SetInterruptEnable( 0, 1 );
    /* NX_PWM_SetT1InterruptEnable( moduleNum, 1 ); */
    /* NX_PWM_SetT2InterruptEnable( moduleNum, 1 ); */
    /* NX_PWM_SetT3InterruptEnable( moduleNum, 1 ); */

    NX_INTC_SetInterruptEnable( NX_PWM_GetT0InterruptNumber(moduleNum), 1 );
    NX_INT_RegisterHandler( NX_PWM_GetInterruptNumber(0), InterruptHandler_PWM_T0, (void*)0 );

    /* NX_INTC_SetInterruptEnable( i, 1 ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT1InterruptNumber(moduleNum), InterruptHandler_PWM_T1, (void*)moduleNum ); */

    /* NX_INT_Enable( NX_PWM_GetT2InterruptNumber(moduleNum) ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT2InterruptNumber(moduleNum), InterruptHandler_PWM_T2, (void*)moduleNum ); */

    /* NX_INT_Enable( NX_PWM_GetT3InterruptNumber(moduleNum) ); */
    /* NX_INT_RegisterHandler( NX_PWM_GetT3InterruptNumber(moduleNum), InterruptHandler_PWM_T3, (void*)moduleNum ); */

}

void	NX_INTC_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
    register struct NX_INTC_RegisterSet *pRegister;
    register U32 RegSel, RegBit, RegVal;

    // @Todo sei - decide Target ID
    if(15<IntNum)
	    NX_INTC_SetTargetID( 0, IntNum, NX_INTC_TARGET_CPU0 );
    NX_INTC_SetPriorityMask( 0, 0, 0xff );
    //NX_INTC_SetPriority( 0, IntNum, 1 );

    // @Todo sei - decide GIC module index, IntNum, RegBit;
    pRegister = __g_pRegister[0];

    RegSel = IntNum >> 5;
    RegBit = IntNum & 0x1F;

    //RegVal = ~(pRegister->GICD_ISCENABLER_SPI[RegSel]) & pRegister->GICD_ISENABLERn_SPI[RegSel];
    //RegVal = pRegister->GICD_ISENABLERn_SPI[RegSel];

    if( Enable )
    {
        RegVal = ((U32)(0x1 << RegBit));
    	//WriteIODW( &pRegister->GICD_ISCENABLER_SPI[RegSel], ~RegVal );
    	WriteIODW( &pRegister->GICD_ISENABLERn_SPI[RegSel], RegVal  );
    }
    else
    {
		//==================================
		//@note by choiyk 2013/12/27
		// when disable IRQ enable..
		// just use Clear-Enable Register,
		// this mean clearing "IRQ enable Config"
		//====================================
        RegVal = ((U32)(0x1 << RegBit));
    	WriteIODW( &pRegister->GICD_ISCENABLER_SPI[RegSel], RegVal );
    }

}

static void InterruptHandler_PWM_T0 (void *pArgument) {
    int pwm_idx = (int)pArgument;
    
    interruptCount[pwm_idx][0]++;
    
    if(interruptCount[pwm_idx][0]>=INTERRUPT_COUNT){
    	Stop_Timer( (unsigned int)pwm_idx, (unsigned int)0x0 );
    }
    NX_PWM_ClearT0InterruptPending( pwm_idx );
}

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

static unsigned int		NX_PWM_GetInterruptNumber( unsigned int ModuleIndex )
{
	const unsigned int	PWMInterruptNumber[PWM_MODULE_NUM] =
				{
					INTNUM_OF_PWM_0_MODULE_INT0,
					INTNUM_OF_PWM_1_MODULE_INT0,
					INTNUM_OF_PWM_2_MODULE_INT0,
					INTNUM_OF_TIMER_0_MODULE_INT0,
					INTNUM_OF_TIMER_1_MODULE_INT0
				};

	return	PWMInterruptNumber[ModuleIndex];
}

void	NX_PWM_SetInterruptEnable( unsigned int ModuleIndex, int Enable )
{
	const unsigned int	PEND_POS	=	0;
	const unsigned int	PEND_MASK	=	0x1 << PEND_POS;

	register struct NX_PWM_RegisterSet*	pRegister;
	register unsigned int	ReadValue;

        switch(ModuleIndex)
        {
        case 0 : pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;
                 ReadValue	=	ReadIODW(&pRegister->T0INT_CSTAT) & ~PEND_MASK;
                 ReadValue	|=	(unsigned int)Enable << PEND_POS;

                 WriteIODW(&pRegister->T0INT_CSTAT, ReadValue);
        case 1 : pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;
                 ReadValue	=	ReadIODW(&pRegister->T1INT_CSTAT) & ~PEND_MASK;
                 ReadValue	|=	(unsigned int)Enable << PEND_POS;

                 WriteIODW(&pRegister->T1INT_CSTAT, ReadValue);
        }                 
}
