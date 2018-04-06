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

#ifdef QEMU_RISCV
/* Register offsets */
#define UART_REG_TXFIFO         0x00
#define UART_REG_RXFIFO         0x04
#define UART_REG_TXCTRL         0x08
#define UART_REG_RXCTRL         0x0c
#define UART_REG_IE             0x10
#define UART_REG_IP             0x14
#define UART_REG_DIV            0x18

/* TXCTRL register */
#define UART_TXEN               0x1
#define UART_TXNSTOP            0x2
#define UART_TXWM(x)            (((x) & 0xffff) << 16)

/* RXCTRL register */
#define UART_RXEN               0x1
#define UART_RXWM(x)            (((x) & 0xffff) << 16)

/* IP register */
#define UART_IP_TXWM            0x1
#define UART_IP_RXWM            0x2

#else
struct NX_UART_RegisterSet
{
	volatile unsigned int    RBR_DLL_THR;    /* 0x00 receive buf, divisor latch, tx holding */
	volatile unsigned int    DLH_IER;        /* 0x04 divisor latch, int enb */
	volatile unsigned int    FCR_IIR;        /* 0x08 fifo control, int id */
	volatile unsigned int    LCR;            /* 0x0C line control */
	volatile unsigned int    MCR;            /* 0x10 modem control */
	volatile unsigned int    LSR;            /* 0x14 line status */
	volatile unsigned int    MSR;            /* 0x18 modem status */
	volatile unsigned int    SCR;            /* 0x1C scratch pad */
	volatile unsigned int    LPDLL;          /* 0x20 low power divisor latch low */
	volatile unsigned int    LPDLH;          /* 0x24 low power divisor latch high */
	volatile unsigned int    _Rev0[2];       /* 0x28 */
	volatile unsigned int    SRBR[8];        /* 0x30 shadow reg low */
	volatile unsigned int    STHR[8];        /* 0x50 shadow reg high */
	volatile unsigned int    FAR;            /* 0x70 fifo access for test */
	volatile unsigned int    TFR;            /* 0x74 tx fifo read for test */
	volatile unsigned int    RFW;            /* 0x78 rx fifo write for test */
	volatile unsigned int    USR;            /* 0x7C status */
	volatile unsigned int    TFL;            /* 0x80 tx fifo level */
	volatile unsigned int    RFL;            /* 0x84 rx fifo level */
	volatile unsigned int    SRR;            /* 0x88 software reset */
	volatile unsigned int    SRTS;           /* 0x8C shadow req to send */
	volatile unsigned int    SBCR;           /* 0x90 shadow brk ctrl */
	volatile unsigned int    SDMAM;          /* 0x94 shadow dma mode */
	volatile unsigned int    SFE;            /* 0x98 shadow fifo enable */
	volatile unsigned int    SRT;            /* 0x9C shadow RCVR trigger */
	volatile unsigned int    STET;           /* 0xA0 shadow TX Empty Trigger */
	volatile unsigned int    HTX;            /* 0xA4 Halt TX */
	volatile unsigned int    DMASA;          /* 0xA8 dma software ack */
	volatile unsigned int    TCR;            /* 0xAC tx ctrl */
	volatile unsigned int    DEN;            /* 0xB0 driver output enable */
	volatile unsigned int    REN;            /* 0xB4 receiver output enable */
	volatile unsigned int    DET;            /* 0xB8 driver output enable timing */
	volatile unsigned int    TAT;            /* 0xBC turnaround timing */
	volatile unsigned int    DLF;            /* 0xC0 divisor latch fraction */
	volatile unsigned int    RAR;            /* 0xC4 rx address */
	volatile unsigned int    TAR;            /* 0xC8 tx address */
	volatile unsigned int    LCR_EXT;        /* 0xCC line extended control */
	volatile unsigned int    _Rev1[9];       /* 0xD0 */
	volatile unsigned int    CPR;            /* 0xF4 component parameter */
	volatile unsigned int    UCV;            /* 0xF8 uart component version */
	volatile unsigned int    CTR;            /* 0xFC component type */
};
#endif // QEMU_RISCV
#endif // __NX_UART_H__
