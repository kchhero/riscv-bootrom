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

#include "include/nx_type.h"
#include "include/nx_uart.h"

#include "libplat.h"
#include "include/nx_swallow_bootoption.h"

#include "include/nx_chip_sfr.h"
#include "include/nx_chip_iomux.h"
#include "include/nx_cmu.h"
#include "fnptr.h"

#define NX_CLKSRC_UART 1
#define SOURCE_DIVID	(4UL)
#define BAUD_RATE	(115200)
//#define BAUD_RATE	(19200)
SUKERTEST   test;

static struct NX_UART_RegisterSet * const pUART =
  (struct NX_UART_RegisterSet *)0x10013000;//PHY_BASEADDR_UART2_MODULE;
const struct cmu_device_clk dbguart[2] = {
	{	// core
		0x1C00, 13, 2, NX_CLKSRC_PLL1, 0
	}, {	// apb
		0x1E00, 14, 2, NX_CLKSRC_PLL1, 0
	}
};
//------------------------------------------------------------------------------

CBOOL DebugInit(void)
{
	/* if ((GetBootOption() & 1 << NOBOOTMSG) && */
	/* 		(GetBootOption() & 1 << EXNOBOOTMSG_SAVE)) */
	/* 	return CFALSE; */

  //	NXBL0FN *pbl0fn = Getbl0fnPtr();
	/* if ((GetBootOption() & 1 << NOBOOTMSG) || */
	/* 		(GetBootOption() & 1 << EXNOBOOTMSG_SAVE)) { */
	/* 	unsigned int *pCMU_SYS_APB_SRST = (unsigned int *)0x27010234; */
	/* 	*pCMU_SYS_APB_SRST = 0x1 << 3; */
	/* 	pbl0fn->nx_memset((void*)PHY_BASEADDR_CAN1_MODULE_RAM, 0, 8192); */
	/* 	return CTRUE; */
	/* } */

  //	static const union nxpad uartpad = {PI_UART2_TXD};

        //	pbl0fn->setpad(&uartpad, 1, 1);
        //	pbl0fn->setdeviceclock(dbguart, 2, 1);
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

	//--------------------------------------------------------------------------

	return CTRUE;
}

//#define SIMUL
void DebugPutch(S8 ch)
{
#ifdef BL1SIMUL
	unsigned int *psimout = (unsigned int *)PHY_BASEADDR_DUMMY_MODULE;
	*psimout = ch;
#else
	const U32 TX_FIFO_FULL = 1 << 1;
	while (!(pUART->USR & TX_FIFO_FULL))
		;
	pUART->RBR_DLL_THR = (unsigned int)ch;
#endif
}

CBOOL	DebugIsBusy(void)
{
	const U8 UART_TX_EMPTY = 1 << 2;
	const U8 UART_SR_BUSY = 1 << 0;
	unsigned int usr = pUART->USR;
	if ((usr & UART_SR_BUSY) || !(usr & UART_TX_EMPTY))
		return 1;
	return 0;
}

