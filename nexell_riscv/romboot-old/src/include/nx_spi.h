#ifndef __NX_SPI_H__
#define __NX_SPI_H__

struct  NX_SPI_RegisterSet
{
	volatile U32 CTRLR0;            // 0x00 : control
	volatile U32 CTRLR1;            // 0x04 : control for only master
	volatile U32 SSIENR;            // 0x08 : enable
	volatile U32 MWCR;              // 0x0C : control for microwave serial protocol
	volatile U32 SER;               // 0x10 : slave enable for only master
	volatile U32 BAUDR;             // 0x14 : baud rate select
	volatile U32 TXFTLR;            // 0x18 : transmit fifo threshold level
	volatile U32 RXFTLR;            // 0x1C : receive fifo threshold level
	volatile U32 TXFLR;             // 0x20 : transmit fifo level
	volatile U32 RXFLR;             // 0x24 : receive fifo level
	volatile U32 SR;                // 0x28 : status
	volatile U32 IMR;               // 0x2C : interrupt mask
	volatile U32 ISR;               // 0x30 : interrupt status
	volatile U32 RISR;              // 0x34 : raw interrupt status
	volatile U32 TXOICR;            // 0x38 : transmit fifo overflow interrupt clear
	volatile U32 RXOICR;            // 0x3C : receive fifo overflow interrupt clear
	volatile U32 RXUICR;            // 0x40 : receive fifo underflow interrupt clear
	volatile U32 MSTICR;            // 0x44 : multi-master interrupt clear
	volatile U32 ICR;               // 0x48 : interrupt clear
	volatile U32 DMACR;             // 0x4C : dma control
	volatile U32 DMATDLR;           // 0x50 : dma transmit data level
	volatile U32 DMARDLR;           // 0x54 : dma receive data level
	volatile U32 IDR;               // 0x58 : idendification
	volatile U32 SSI_VERSION_ID;    // 0x5C : coreKit version ID
	volatile U32 DR;                // 0x60 : data
	volatile U32 _Rev0[35];         // 0x64 : reserved
	volatile U32 RX_SAMPLE_DLY;     // 0xF0 : rx sample delay
	volatile U32 SPI_CTRLR0;        // 0xF4 : spi control
	volatile U32 TXD_DRIVE_EDGE;    // 0xF8 : transmit drive edge
	volatile U32 _Rev1;             // 0xFC : reserved
};

#endif //__NX_SPI_H__
