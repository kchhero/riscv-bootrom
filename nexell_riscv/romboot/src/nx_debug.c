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

#include <nx_uart.h>
#include <nx_debug.h>
#include <nx_lib.h>
#include <nx_bootheader.h>
#include <nx_chip_iomux.h>
#include <nx_gpio.h>
#include <nx_swallow.h>
#include <nx_clock.h>

#define NX_CLKSRC_UART 1
#define SOURCE_DIVID	(4UL)
#define BAUD_RATE	(115200)

static struct NX_UART_RegisterSet * const pUART =
    (struct NX_UART_RegisterSet *)PHY_BASEADDR_UART0_MODULE;

//------------------------------------------------------------------------------
int DebugInit(void)
{
    static const union nxpad uartpad = {PADINDEX_OF_UART0_TXD};

    setpad(&uartpad, 1, 1);
    //    nxSetDeviceClock(dbguart, 2, 1);
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

    return 1;
}

void DebugPutch(char ch)
{
    const unsigned int TX_FIFO_FULL = 1 << 1;
    while (!(pUART->USR & TX_FIFO_FULL))
        ;
    pUART->RBR_DLL_THR = (unsigned int)ch;
}

int DebugIsBusy(void)
{
    const unsigned char UART_TX_EMPTY = 1 << 2;
    const unsigned char UART_SR_BUSY = 1 << 0;
    unsigned int usr = pUART->USR;
    if ((usr & UART_SR_BUSY) || !(usr & UART_TX_EMPTY))
        return 1;
    return 0;
}
