#ifndef __NX_SPI_H__
#define __NX_SPI_H__

struct  NX_SPI_RegisterSet
{
	volatile unsigned int CTRLR0;            // 0x00 : control
	volatile unsigned int CTRLR1;            // 0x04 : control for only master
	volatile unsigned int SSIENR;            // 0x08 : enable
	volatile unsigned int MWCR;              // 0x0C : control for microwave serial protocol
	volatile unsigned int SER;               // 0x10 : slave enable for only master
	volatile unsigned int BAUDR;             // 0x14 : baud rate select
	volatile unsigned int TXFTLR;            // 0x18 : transmit fifo threshold level
	volatile unsigned int RXFTLR;            // 0x1C : receive fifo threshold level
	volatile unsigned int TXFLR;             // 0x20 : transmit fifo level
	volatile unsigned int RXFLR;             // 0x24 : receive fifo level
	volatile unsigned int SR;                // 0x28 : status
	volatile unsigned int IMR;               // 0x2C : interrupt mask
	volatile unsigned int ISR;               // 0x30 : interrupt status
	volatile unsigned int RISR;              // 0x34 : raw interrupt status
	volatile unsigned int TXOICR;            // 0x38 : transmit fifo overflow interrupt clear
	volatile unsigned int RXOICR;            // 0x3C : receive fifo overflow interrupt clear
	volatile unsigned int RXUICR;            // 0x40 : receive fifo underflow interrupt clear
	volatile unsigned int MSTICR;            // 0x44 : multi-master interrupt clear
	volatile unsigned int ICR;               // 0x48 : interrupt clear
	volatile unsigned int DMACR;             // 0x4C : dma control
	volatile unsigned int DMATDLR;           // 0x50 : dma transmit data level
	volatile unsigned int DMARDLR;           // 0x54 : dma receive data level
	volatile unsigned int IDR;               // 0x58 : idendification
	volatile unsigned int SSI_VERSION_ID;    // 0x5C : coreKit version ID
	volatile unsigned int DR;                // 0x60 : data
	volatile unsigned int _Rev0[35];         // 0x64 : reserved
	volatile unsigned int RX_SAMPLE_DLY;     // 0xF0 : rx sample delay
	volatile unsigned int SPI_CTRLR0;        // 0xF4 : spi control
	volatile unsigned int TXD_DRIVE_EDGE;    // 0xF8 : transmit drive edge
	volatile unsigned int _Rev1;             // 0xFC : reserved
};

#endif //__NX_SPI_H__
