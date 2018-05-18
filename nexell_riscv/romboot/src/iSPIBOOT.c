/*==============================================================================
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or fitness
// for a particular puporse.
//
// Module	: bl0
// File		: iSPIBOOT.c
// Description	:
// Author	: Hans
// History	:
//		Hans: 2014.01.12 First implementation
//		Hans: 2016.08.16 modify for nxp5430
//		Hans: 2018.05.10 modify for drone
==============================================================================*/
#include <nx_swallow.h>
#include <nx_bootheader.h>

#if defined(QEMU_RISCV) || defined(SOC_SIM)
#include <nx_qemu_sim_printf.h>
#else
#include <nx_swallow_printf.h>
#endif

#include <nx_gpio.h>

#include <nx_chip_sfr.h>
#include <nx_chip_iomux.h>
#include <nx_spi.h>
#include <nx_clock.h>
#include <nx_cmu.h>
#include <nx_lib.h>

#define HEADER_ID                               \
                ((((unsigned int)'N')<< 0) |    \
                 (((unsigned int)'S')<< 8) |    \
                 (((unsigned int)'I')<<16) |    \
                 (((unsigned int)'H')<<24))
/*----------------------------------------------------------------------------*/
#define SER_READ	0x03	/* Read Data from Memory Array */

//------------------------------------------------------------------------------
extern struct NX_GPIO_RegisterSet (* const pGPIOReg)[1];

const union nxpad spipad[1][4] = {
	{
		{PADINDEX_OF_GPIOC_GPIO_15_},	//PADINDEX_OF_SPI1_SS
		{PADINDEX_OF_SPI1_SCLK},
		{PADINDEX_OF_SPI1_MOSI},
		{PADINDEX_OF_SPI1_MISO}
	}
};

void SPIPortInit(void)
{
	GPIOSetOutput((struct nxpadi *)&spipad[0], 1);	// output high
	GPIOSetIO((struct nxpadi *)&spipad[0][0], 0);	// 0: output, 1: input
	setpad(spipad[0], 4, 1);
}

void SPIPortDeinit(void)
{
	setpad(spipad[0], 4, 0);
	GPIOSetIO((struct nxpadi *)&spipad[0][0], 1);	// 0: output, 1: input
}

void SPIInit(void)
{
	struct NX_SPI_RegisterSet *const pSPI =
		(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI1_MODULE;
	/* -----------------------------------------------------------------------------
	 * gpio init
	 * ---------------------------------------------------------------------------*/
	SPIPortInit();

	/* -----------------------------------------------------------------------------
	 * spi init
	 * ---------------------------------------------------------------------------*/
	pSPI->SSIENR = 0;       // spi disable

	pSPI->IMR = 0x3F;       // all int disable;

	unsigned int regval =
		0 << 24 |       // slave select toggle enable: for eeprom, always low
		0 << 21 |       // 0:1bit, 1:2bit, 2:4bit, 3,8bit
		7 << 16 |       // (x + 1) bit frame: 7 ==> 8 bit frame
		0 << 12 |       // Microwire frame format control word
		0 << 11 |       // internal loopback - test mode only
		0 << 10 |       // slave output enable
		0 <<  8 |       // transfer mode. 0:trx, 1:tx, 2:rx, 3:eeprom read mode
		0 <<  7 |       // serial clock prlarity. 0:low inactive, 1:high
		0 <<  6 |       // serial clock phase. 0:middle, 1:start
		0 <<  4 |       // frame format. 0:motorola, 1:texas, 2:Microwire
		0 <<  0;        // Data Frame Size. When MAX XFER == 16 or 32, 4~16bit
	pSPI->CTRLR0 = regval;
	regval = pSPI->CTRLR0;

	// slave enable, there is only one slave for each controller
	pSPI->SER = 1 << 0;

	regval = 6;		// 200.0000MHz / 6 = 33.333MHz
	pSPI->BAUDR = regval;   // baudrate. even only. in / baud => out, 0: out disable.
}

void SPIDeinit(void)
{
	struct NX_SPI_RegisterSet *const pSPI =
		(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI1_MODULE;
	pSPI->SER = 0 << 0;     // cs off
	pSPI->SSIENR = 0;       // clock disable also.
	SPIPortDeinit();
}

void read_flash(unsigned char *pmem, unsigned int flashaddr, int rsize)
{
	struct NX_SPI_RegisterSet *const pSPI =
		(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI1_MODULE;
	int i = 0;
	int addrstep = 3;

	GPIOSetOutput((struct nxpadi *)&spipad[0][0], 0);	// output low

	pSPI->SSIENR = 1;
	pSPI->DR = SER_READ;

	while (addrstep--)
		pSPI->DR = (flashaddr >> addrstep * 8) & 0xFF;

	while (!(pSPI->SR & 1 << 2));	// tx fifo empty
	while (pSPI->SR & 1 << 0);	// ssi busy
	while (pSPI->SR & 1 << 3)	// rx fifo not empty
		pSPI->DR;

	pSPI->DR = 0xA9;
	pSPI->DR = 0xA9;
	pSPI->DR = 0xA9;
	pSPI->DR = 0xA9;

	while (i < rsize) {
		pSPI->DR = 0xA9;
		while (!(pSPI->SR & 1 << 3))    // wait until receive fifo not empty
			;
		pmem[i] = pSPI->DR;
		i++;
	}

	while (!(pSPI->SR & 1 << 2));	// tx fifo empty
	while (pSPI->SR & 1 << 0);	// ssi busy
	while (pSPI->SR & 1 << 3)	// rx fifo not empty
		pSPI->DR;

	pSPI->SSIENR = 0;       // clock disable also.
	GPIOSetOutput((struct nxpadi *)&spipad[0][0], 1);	// output high
}

unsigned int iSPIBOOT(void)
{
	int ret = 1, rsize;
	unsigned int flashaddr = 0;
	unsigned char *plp;
	struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_OF_PBM;
#ifdef DEBUG
	_dprintf("spi init\n");
#endif
	SPIInit();
/* -----------------------------------------------------------------------------
 * read header
 * ---------------------------------------------------------------------------*/
	struct nx_bootinfo *pbi = (struct nx_bootinfo *)&pbm->bi;
	plp = (unsigned char *)pbi;
	rsize = sizeof(struct nx_bootinfo);
#ifdef DEBUG
	_dprintf("header read\n");
#endif
	read_flash(plp, flashaddr, rsize);
	flashaddr += rsize;

	if (pbi->signature != HEADER_ID) {
#ifdef DEBUG
		_dprintf("not boot image\n");
#endif
		goto error;
	}
/* -----------------------------------------------------------------------------
 * read boot image
 * ---------------------------------------------------------------------------*/

	rsize = pbi->LoadSize;
#ifdef DEBUG        
	_dprintf("load size:%d\n", rsize);
#endif
	plp = (unsigned char *)pbm->image;

	read_flash(plp, flashaddr, rsize);
#ifdef DEBUG
	_dprintf("boot image load done.\n");
#endif

error:
	SPIDeinit();

	return ret;
}
