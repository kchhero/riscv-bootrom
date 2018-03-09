//------------------------------------------------------------------------------
/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: bl0
 * File		: debug.c
 * Description	:
 * Author	: Hans
 * History	: 2017.10.09 Hans Create
 *
 */

#include <nx_type.h>
#include <nx_uart.h>
#include "libarm.h"
#include "libplat.h"
#include "nx_antares_bootoption.h"

#ifdef NXP4330
#include <nx_chip.h>
#include <nx_tieoff.h>
#include <nx_clkgen.h>
#endif

#ifdef NXP3220
#include <nx_chip_sfr.h>
#include <nx_chip_iomux.h>
#include <nx_cmu.h>
#endif

#define NX_CLKSRC_UART 1
#define SOURCE_DIVID	(4UL)
#define BAUD_RATE	(115200)
//#define BAUD_RATE	(19200)

#ifdef NXP4330
static struct NX_TIEOFF_RegisterSet * const pReg_Tieoff =
	(struct NX_TIEOFF_RegisterSet *)PHY_BASEADDR_TIEOFF_MODULE;
static struct NX_UART_RegisterSet * pReg_Uart;
#endif

#ifdef NXP3220
static struct NX_UART_RegisterSet * const pUART =
		(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART2_MODULE;
const struct cmu_device_clk dbguart[2] = {
	{	// core
		0x1C00, 13, 2, NX_CLKSRC_PLL1, 0
	}, {	// apb
		0x1E00, 14, 2, NX_CLKSRC_PLL1, 0
	}
};
#endif
//------------------------------------------------------------------------------

CBOOL DebugInit(void)
{
	if ((GetBootOption() & 1 << NOBOOTMSG) &&
			(GetBootOption() & 1 << EXNOBOOTMSG_SAVE))
		return CFALSE;
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	if ((GetBootOption() & 1 << NOBOOTMSG) ||
			(GetBootOption() & 1 << EXNOBOOTMSG_SAVE)) {
		unsigned int *pCMU_SYS_APB_SRST = (unsigned int *)0x27010234;
		*pCMU_SYS_APB_SRST = 0x1 << 3;
		pbl0fn->memset((void*)PHY_BASEADDR_CAN1_MODULE_RAM, 0, 8192);
		return CTRUE;
	}
#endif

#ifdef NXP4330
	U32 SOURCE_CLOCK = 147456000;
	SOURCE_CLOCK = NX_CLKPWR_GetPLLFrequency(NX_CLKSRC_UART);
	pReg_Uart = (struct NX_UART_RegisterSet *)
		PHY_BASEADDR_pl01115_Uart_nodma0_MODULE;
	struct NX_CLKGEN_RegisterSet * const pReg_UartClkGen =
		(struct NX_CLKGEN_RegisterSet * const)PHY_BASEADDR_CLKGEN25_MODULE;

	GPIOSetAltFunction(PADINDEX_OF_pl01115_Uart_nodma0_UARTRXD);
	GPIOSetAltFunction(PADINDEX_OF_pl01115_Uart_nodma0_UARTTXD);

	pReg_Tieoff->TIEOFFREG[((TIEOFFINDEX_OF_UART3_USESMC) & 0xFFFF) >> 5] &=
				(~(1 << ((TIEOFFINDEX_OF_UART3_USESMC) & 0x1F)));
	pReg_Tieoff->TIEOFFREG[((TIEOFFINDEX_OF_UART3_SMCTXENB) & 0xFFFF) >> 5] &=
				(~(1 << ((TIEOFFINDEX_OF_UART3_SMCTXENB) & 0x1F)));
	pReg_Tieoff->TIEOFFREG[((TIEOFFINDEX_OF_UART3_SMCRXENB) & 0xFFFF) >> 5] &=
				(~(1 << ((TIEOFFINDEX_OF_UART3_SMCRXENB) & 0x1F)));

	ResetCon(RESETINDEX_OF_pl01115_Uart_nodma0_MODULE_nUARTRST, CTRUE);// reset on
	ResetCon(RESETINDEX_OF_pl01115_Uart_nodma0_MODULE_nUARTRST, CFALSE); // reset negate

	pReg_UartClkGen->CLKENB = (1 << 3);	// PCLKMODE : always, Clock Gen Disable
	pReg_UartClkGen->CLKGEN[0] = ((SOURCE_DIVID - 1) << 5) |
					(NX_CLKSRC_UART << 2);
	//--------------------------------------------------------------------------
	pReg_Uart->LCR_H = 0x0070;	// 8 bit, none parity, stop 1, normal mode
	pReg_Uart->CR = 0x0300;		// rx, tx enable
	pReg_Uart->IBRD = (U16)((SOURCE_CLOCK / SOURCE_DIVID) /
				((BAUD_RATE / 1) * 16)); // IBRD = 8, 115200bps
	pReg_Uart->FBRD = (U16)(((((SOURCE_CLOCK / SOURCE_DIVID) %
				((BAUD_RATE / 1) * 16)) + 32) * 64) /
				((BAUD_RATE / 1) * 16)); // FBRD = 0,

	// PCLKMODE : always, Clock Gen Enable
	pReg_UartClkGen->CLKENB = (1 << 3) | (1 << 2);

	pReg_Uart->CR 	= 0x0301;
#endif
#ifdef NXP3220
	static const union nxpad uartpad = {PI_UART2_TXD};

	pbl0fn->setpad(&uartpad, 1, 1);
	pbl0fn->setdeviceclock(dbguart, 2, 1);
	//------------------------------------------------------------------------------
	// Uart Initialize
	//------------------------------------------------------------------------------
/* TODO */
	const unsigned char baudrate[3] = {54 - 1, 0, 16 - 1};	/* need check */

	pUART->DLH_IER = 0x0;		/* disable int */

	pUART->LCR = 1 << 7;		/* divisor latch access enable */
	pUART->RBR_DLL_THR = baudrate[0];
	pUART->DLH_IER = baudrate[1];
	pUART->DLF = baudrate[2];	/* 6 bits */
	pUART->LCR = 0 << 7;		/* divisor latch access disable */

	pUART->FCR_IIR =
		0 << 6 |	/* rx trig level. 0:1ch, 1:1/4, 2:1/2, 3:full */
		0 << 4 |	/* tx trig level. 0:empty, 1:2ch, 2:1/4, 3:1/2 */
		0 << 3 |	/* no dma mode */
		1 << 2 |	/* tx fifo reset */
		1 << 1 |	/* rx fifo reset */
		1 << 0;		/* fifo enable */

	pUART->LCR =
		0 << 7 |	/* no divisor latch access */
		0 << 6 |	/* no break */
		0 << 5 |	/* stick parity disable */
		0 << 4 |	/* 0: odd, 1: even parity */
		0 << 3 |	/* disable parity */
		0 << 2 |	/* 0: 1 stop bit, 1: 1.5 or 2 stop bits */
		3 << 0;		/* 0: 5bit, 1: 6bit, 2: 7bit, 3: 8bit */
#endif

	//--------------------------------------------------------------------------

	return CTRUE;
}

#ifdef NXP4330
void putchar(S8 ch)
{
	const U32 TX_FIFO_FULL = 1 << 5;
	while (pReg_Uart->FR & TX_FIFO_FULL)	{ ; }
	pReg_Uart->DR = (U32)ch;
}
#endif

//#define SIMUL
void DebugPutch(S8 ch)
{
#ifdef NXP4330
	const U32 TX_FIFO_FULL = 1 << 5;
	while (pReg_Uart->FR & TX_FIFO_FULL)	{ ; }
	pReg_Uart->DR = (U32)ch;
#endif
#ifdef NXP3220
#ifdef BL1SIMUL
	unsigned int *psimout = (unsigned int *)PHY_BASEADDR_DUMMY_MODULE;
	*psimout = ch;
#else
	const U32 TX_FIFO_FULL = 1 << 1;
	while (!(pUART->USR & TX_FIFO_FULL))
		;
	pUART->RBR_DLL_THR = (unsigned int)ch;
#endif
#endif
}

CBOOL	DebugIsBusy(void)
{
#ifdef NXP4330
	const U16 UART_TX_BUSY = 1 << 3;
	return (CBOOL)(pReg_Uart->FR & UART_TX_BUSY);
#endif
#ifdef NXP3220
	const U8 UART_TX_EMPTY = 1 << 2;
	const U8 UART_SR_BUSY = 1 << 0;
	unsigned int usr = pUART->USR;
	if ((usr & UART_SR_BUSY) || !(usr & UART_TX_EMPTY))
		return 1;
	return 0;
#endif
}

#ifdef NXP4330
int isrxreceived(void)
{
	const U16 RX_FIFO_EMPTY = 1 << 4;
	return !(pReg_Uart->FR & RX_FIFO_EMPTY);
}

S8 getchar(void)
{
	const U16 RX_FIFO_EMPTY = 1 << 4;
	while (pReg_Uart->FR & RX_FIFO_EMPTY);
	return (S8)pReg_Uart->DR;
}

S8	DebugGetch(void)
{
	const U16 RX_FIFO_EMPTY = 1 << 4;
	while (pReg_Uart->FR & RX_FIFO_EMPTY);
	return (S8)pReg_Uart->DR;
}
#endif

