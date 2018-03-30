/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module	: uart
 * File		: nx_uart.h
 * Description	: uart register struct
 * Author	: Hans
 * History	: 2018.01.06 First draft
 */
#ifndef __NX_UART_H__
#define __NX_UART_H__

#include "nx_type.h"

struct NX_UART_RegisterSet
{
	volatile U32    RBR_DLL_THR;    /* 0x00 receive buf, divisor latch, tx holding */
	volatile U32    DLH_IER;        /* 0x04 divisor latch, int enb */
	volatile U32    FCR_IIR;        /* 0x08 fifo control, int id */
	volatile U32    LCR;            /* 0x0C line control */
	volatile U32    MCR;            /* 0x10 modem control */
	volatile U32    LSR;            /* 0x14 line status */
	volatile U32    MSR;            /* 0x18 modem status */
	volatile U32    SCR;            /* 0x1C scratch pad */
	volatile U32    LPDLL;          /* 0x20 low power divisor latch low */
	volatile U32    LPDLH;          /* 0x24 low power divisor latch high */
	volatile U32    _Rev0[2];       /* 0x28 */
	volatile U32    SRBR[8];        /* 0x30 shadow reg low */
	volatile U32    STHR[8];        /* 0x50 shadow reg high */
	volatile U32    FAR;            /* 0x70 fifo access for test */
	volatile U32    TFR;            /* 0x74 tx fifo read for test */
	volatile U32    RFW;            /* 0x78 rx fifo write for test */
	volatile U32    USR;            /* 0x7C status */
	volatile U32    TFL;            /* 0x80 tx fifo level */
	volatile U32    RFL;            /* 0x84 rx fifo level */
	volatile U32    SRR;            /* 0x88 software reset */
	volatile U32    SRTS;           /* 0x8C shadow req to send */
	volatile U32    SBCR;           /* 0x90 shadow brk ctrl */
	volatile U32    SDMAM;          /* 0x94 shadow dma mode */
	volatile U32    SFE;            /* 0x98 shadow fifo enable */
	volatile U32    SRT;            /* 0x9C shadow RCVR trigger */
	volatile U32    STET;           /* 0xA0 shadow TX Empty Trigger */
	volatile U32    HTX;            /* 0xA4 Halt TX */
	volatile U32    DMASA;          /* 0xA8 dma software ack */
	volatile U32    TCR;            /* 0xAC tx ctrl */
	volatile U32    DEN;            /* 0xB0 driver output enable */
	volatile U32    REN;            /* 0xB4 receiver output enable */
	volatile U32    DET;            /* 0xB8 driver output enable timing */
	volatile U32    TAT;            /* 0xBC turnaround timing */
	volatile U32    DLF;            /* 0xC0 divisor latch fraction */
	volatile U32    RAR;            /* 0xC4 rx address */
	volatile U32    TAR;            /* 0xC8 tx address */
	volatile U32    LCR_EXT;        /* 0xCC line extended control */
	volatile U32    _Rev1[9];       /* 0xD0 */
	volatile U32    CPR;            /* 0xF4 component parameter */
	volatile U32    UCV;            /* 0xF8 uart component version */
	volatile U32    CTR;            /* 0xFC component type */
};

#endif // __NX_UART_H__
