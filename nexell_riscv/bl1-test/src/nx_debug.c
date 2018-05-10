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
#include <nx_chip_iomux.h>
#include <nx_swallow.h>
#include <nx_gpio.h>

#define NX_CLKSRC_UART 1
#define SOURCE_DIVID	(4UL)
#define BAUD_RATE	(115200)

static struct NX_UART_RegisterSet * const pUART =
    (struct NX_UART_RegisterSet *)PHY_BASEADDR_UART0_MODULE;

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
