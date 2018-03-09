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
==============================================================================*/
#include <nx_antares.h>
#include "nx_antares_bootoption.h"
#include "nx_antares_bootheader.h"
#include <nx_type.h>
#include "printf.h"


#include <nx_gpio.h>

#ifdef NXP4330
#include <nx_chip.h>
#include <nx_clkgen.h>
#include <nx_ssp.h>
#endif

#ifdef NXP3220
#include <nx_chip_sfr.h>
#include <nx_chip_iomux.h>
#include <nx_cmu.h>
#include <nx_spi.h>
#include "crypto.h"
#endif

#include "libarm.h"
#include "libplat.h"
#include "fnptr.h"

/*----------------------------------------------------------------------------*/
#define SER_READ	0x03	/* Read Data from Memory Array */

//------------------------------------------------------------------------------
extern struct NX_GPIO_RegisterSet (* const pGPIOReg)[1];
#ifdef NXP4330
struct NX_SSP_RegisterSet * const pgSPI[3] =
{
	(struct NX_SSP_RegisterSet (*const))PHY_BASEADDR_SSP0_MODULE,
	(struct NX_SSP_RegisterSet (*const))PHY_BASEADDR_SSP1_MODULE,
	(struct NX_SSP_RegisterSet (*const))PHY_BASEADDR_SSP2_MODULE
};
#endif
#ifdef NXP3220
struct NX_SPI_RegisterSet * const pgSPI[3] =
{
	(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI0_MODULE,
	(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI1_MODULE,
	(struct NX_SPI_RegisterSet (*const))PHY_BASEADDR_SPI2_MODULE
};

const union nxpad spipad[3][6] = {
	{
		{PI_SPI0_SS},
		{PI_SPI0_SCLK},
		{PI_SPI0_MO_IO0},
		{PI_SPI0_MI_IO1},
		{PI_SPI0_IO2},
		{PI_SPI0_IO3}
	},
	{
		{PI_SPI1_SS},
		{PI_SPI1_SCLK},
		{PI_SPI1_MISO},
		{PI_SPI1_MOSI},
		{PI_SPI1_IO2},
		{PI_SPI1_IO3}
	},
	{
		{PI_SPI2_SS},
		{PI_SPI2_SCLK},
		{PI_SPI2_MISO},
		{PI_SPI2_MOSI},
		{PI_SPI2_IO2},
		{PI_SPI2_IO3}
	},
};
const struct cmu_device_clk spiclk[3][2] = {
	{
		{0x3600, 26, 0, NX_CLKSRC_PLL1, 1},		// apb
		{0x3800, 27, 0, NX_CLKSRC_PLL1, 1}
	}, {
		{0x3600, 26, 1, NX_CLKSRC_PLL1, 1},
		{0x3800, 27, 1, NX_CLKSRC_PLL1, 1}
	}, {
		{0x3600, 26, 2, NX_CLKSRC_PLL1, 1},
		{0x3800, 27, 2, NX_CLKSRC_PLL1, 1}
	},
};
#endif

void SPIPortInit(unsigned int SPIPort)
{
#ifdef NXP4330
	if (SPIPort == 0) {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_C];
		pGPIOxReg->GPIOx_ALTFN[1] =
			(pGPIOxReg->GPIOx_ALTFN[1] & ~0xFC000000) |
			0x54000000; // spi 0 GPIO C[29, 30, 31] ALT 1

		pGPIOxReg->GPIOx_SLEW &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV0 |= (1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV1 |= (1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLSEL |= (1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLENB &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);

		pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_D];
		pGPIOxReg->GPIOx_ALTFN[0] =
			(pGPIOxReg->GPIOx_ALTFN[0] & ~0x00000003) |
			0x00000001; // spi 0 GPIO D[0] ALT 1

		pGPIOxReg->GPIOx_SLEW &= ~(1 << 0);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_DRV0 |= (1 << 0);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_DRV1 |= (1 << 0);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_PULLSEL |= (1 << 0);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_PULLENB &= ~(1 << 0);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |= (1 << 0);
	} else if (SPIPort == 1) {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_E];

		pGPIOxReg->GPIOx_ALTFN[0] =
			(pGPIOxReg->GPIOx_ALTFN[0] & ~0xF0000000) |
			0xA0000000; // spi 1 GPIO E[14, 15] ALT 2
		pGPIOxReg->GPIOx_ALTFN[1] =
			(pGPIOxReg->GPIOx_ALTFN[1] & ~0x000000F0) |
			0x000000A0; // spi 1 GPIO E[18, 19] ALT 2

		pGPIOxReg->GPIOx_SLEW &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV0 |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV1 |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLSEL |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLENB &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
	} else {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_C];
		pGPIOxReg->GPIOx_ALTFN[0] =
			(pGPIOxReg->GPIOx_ALTFN[0] & ~0x03F30000) |
			0x02A80000; // spi 2 GPIO C[9, 10, 11, 12] ALT 2

		pGPIOxReg->GPIOx_SLEW &= ~(1 << 12 | 1 << 11 | 1 << 10 |
				1 << 9); // txd, rxd, clk, frm
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV0 |= (1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV1 |= (1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLSEL |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLENB &=
			~(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
	}
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setpad(spipad[SPIPort], 6, 1);
#endif
}
void SPIPortDeinit(unsigned int SPIPort)
{
#ifdef NXP4330
	if (SPIPort == 0) {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_C];

		pGPIOxReg->GPIOx_OUT |= 0x40000000;
		pGPIOxReg->GPIOx_OUTENB &= ~0x40000000;
		pGPIOxReg->GPIOx_ALTFN[1] &=
			~0xFC000000; // spi 0 GPIO C[29, 30, 31] ==> ALT 0

		pGPIOxReg->GPIOx_SLEW |= (1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV0 &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV1 &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLSEL &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLENB &= ~(1 << 31 | 1 << 30 | 1 << 29);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 31 | 1 << 30 | 1 << 29);

		pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_D];
		pGPIOxReg->GPIOx_ALTFN[0] &=
			~0x00000003; // spi 0 GPIO D[0] ==> ALT 0

		pGPIOxReg->GPIOx_SLEW |= (1 << 0);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_DRV0 &= ~(1 << 0);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_DRV1 &= ~(1 << 0);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_PULLSEL &= ~(1 << 0);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |= (1 << 0);
		pGPIOxReg->GPIOx_PULLENB &= ~(1 << 0);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |= (1 << 0);
	} else if (SPIPort == 1) {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_E];

		pGPIOxReg->GPIOx_OUT |= 0x00008000;
		pGPIOxReg->GPIOx_OUTENB &= ~0x00008000;
		pGPIOxReg->GPIOx_ALTFN[0] &=
			~0xF0000000; // spi 1 GPIO E[14, 15] ==> ALT 0
		pGPIOxReg->GPIOx_ALTFN[1] &=
			~0x000000F0; // spi 1 GPIO E[18, 19] ==> ALT 0

		pGPIOxReg->GPIOx_SLEW |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV0 &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV1 &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLSEL &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLENB &=
			~(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 19 | 1 << 18 | 1 << 15 | 1 << 14);
	} else {
		register struct NX_GPIO_RegisterSet *pGPIOxReg =
			(struct NX_GPIO_RegisterSet *)&pGPIOReg[GPIO_GROUP_C];

		pGPIOxReg->GPIOx_OUT |= 0x00000400;
		pGPIOxReg->GPIOx_OUTENB &= ~0x00000400;
		/* spi 2 GPIO C[9, 10, 11, 12] ==> ALT 0 */
		pGPIOxReg->GPIOx_ALTFN[0] &= ~0x03FC0000;

		pGPIOxReg->GPIOx_SLEW |= (1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_SLEW_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV0 &=
			~(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV0_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV1 &=
			~(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_DRV1_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLSEL &=
			~(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLSEL_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLENB &=
			~(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
		pGPIOxReg->GPIOx_PULLENB_DISABLE_DEFAULT |=
			(1 << 12 | 1 << 11 | 1 << 10 | 1 << 9);
	}
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setpad(spipad[SPIPort], 6, 1);
#endif
}
void SPIInit(unsigned int option)
{
#ifdef NXP4330
	option = option;
#endif
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int port = (option >> 5) & 0x3;
	if (port == 3)
		port = 0;
	struct NX_SPI_RegisterSet *const pSPI = pgSPI[port];
	/* -----------------------------------------------------------------------------
	 * gpio init
	 * ---------------------------------------------------------------------------*/
	pbl0fn->SPIPortInit(port);
	/* -----------------------------------------------------------------------------
	 * clock, reset init
	 * ---------------------------------------------------------------------------*/
	pbl0fn->setdeviceclock(&spiclk[port][0], 2, 1);
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
		3 <<  8 |       // transfer mode. 0:trx, 1:tx, 2:rx, 3:eeprom read mode
		0 <<  7 |       // serial clock prlarity. 0:low inactive, 1:high
		0 <<  6 |       // serial clock phase. 0:middle, 1:start
		0 <<  4 |       // frame format. 0:motorola, 1:texas, 2:Microwire
		0 <<  0;        // Data Frame Size. When MAX XFER == 16 or 32, 4~16bit
	pSPI->CTRLR0 = regval;
	regval = pSPI->CTRLR0;

	// slave enable, there is only one slave for each controller
	pSPI->SER = 1 << 0;

	regval = 6;		// 16.6666MHz
	if (option & 1 << 4)    // 0: 16MHz, 1: 1MHz
		regval = 100;
	pSPI->BAUDR = regval;   // baudrate. even only. in / baud => out, 0: out disable.
#endif
}

void SPIDeinit(unsigned int option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int port = (option >> 5) & 0x3;
	if (port == 3)
		port = 0;
#ifdef NXP3220
	struct NX_SPI_RegisterSet *const pSPI = pgSPI[port];
	pSPI->SER = 0 << 0;     // cs off
	pSPI->SSIENR = 0;       // clock disable also.
	pbl0fn->setdeviceclock(&spiclk[port][0], 2, 0);
#endif
	pbl0fn->SPIPortDeinit(port);
}

void read_flash(int port, unsigned char *pmem, unsigned int flashaddr,
		                int rsize, int addrstep)
{
#ifdef NXP3220
	struct NX_SPI_RegisterSet *const pSPI = pgSPI[port];
	unsigned int i = 0;

	pSPI->CTRLR1 = rsize - 1;
	pSPI->SSIENR = 1;

	pSPI->DR = SER_READ;

	while (addrstep--)
		pSPI->DR = (flashaddr >> addrstep * 8) & 0xFF;

	while (i < (unsigned int)rsize) {
		while (!(pSPI->SR & 1 << 3))    // wait until receive fifo not empty
			;
		pmem[i] = pSPI->DR;
		i++;
	}
	pSPI->SSIENR = 0;       // clock disable also.
#endif
#ifdef NXP4330
	port = port;
	pmem = pmem;
	flashaddr = flashaddr;
	rsize = rsize;
	addrstep = addrstep;
	return;
#endif
}

unsigned int iSPIBOOT(unsigned int option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int ret = 1, rsize, port;
	unsigned int flashaddr = 0;
	unsigned char *plp;

	port = (option >> SELSPIPORT) & 0x3;
	if (port == 3)
		port = 0;

	pbl0fn->SPIInit(option);
/* -----------------------------------------------------------------------------
 * main code
 * ---------------------------------------------------------------------------*/

	int addrstep = 2;
	if (option & 1 << 3)
		addrstep++;
	pbl0fn->printf("%d step address\n", addrstep);

/* -----------------------------------------------------------------------------
 * read header
 * ---------------------------------------------------------------------------*/

	pbl0fn->printf("read boot info\n");

	struct nx_bootinfo *pbi = (struct nx_bootinfo *)0xFFFF0000;
	plp = (unsigned char *)&(*pbl0fn->pbm)->bi;
	rsize = sizeof(struct nx_bootinfo);
	pbl0fn->read_flash(port, plp, flashaddr, rsize, addrstep);
	flashaddr += rsize;

	/*===========================================================
	 * boot header decryption
	 * ========================================================*/
	unsigned int iv[4];
	iv[0] = (*pbl0fn->iv)[0];
	iv[1] = (*pbl0fn->iv)[1];
	iv[2] = (*pbl0fn->iv)[2];
	iv[3] = (*pbl0fn->iv)[3];
#if 1
	if (!pbl0fn->headercheck(iv, option & 1 << DECRYPT)) {
		ret = 0;
		goto error;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((unsigned int *)&(*pbl0fn->pbm)->bi,
				(unsigned int *)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));
	}

	if (pbi->signature != HEADER_ID) {
		pbl0fn->printf("cannot read boot image(%08X)\r\n", pbi->signature);
		ret = 0;
		goto error;
	}
#endif

/* -----------------------------------------------------------------------------
 * read key
 * ---------------------------------------------------------------------------*/
	pbl0fn->printf("read key\n");
	plp = (unsigned char *)&(*pbl0fn->pbm)->rsa_public;
	rsize = sizeof(struct asymmetrickey);
	pbl0fn->read_flash(port, plp, flashaddr, rsize, addrstep);
	flashaddr += rsize;

	/*===========================================================
	 * boot key certify
	 * ========================================================*/
#if 1
	if (!pbl0fn->keycheck(iv, option & 1 << DECRYPT, (option >> VERIFY) & 0x3)) {
		ret = 0;
		goto error;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				(unsigned int *)&(*pbl0fn->pbm)->rsa_public,
				iv, sizeof(struct asymmetrickey));
	}

	unsigned int ehash = (option >> VERIFY) & 0x3;
	if (ehash) {
		unsigned int bhash[32 / 4];
		pbl0fn->mbedtls_sha256((*pbl0fn->pbm)->rsa_public.rsapublicbootkey,
				256, (unsigned char *)bhash, 0);
		if (ehash == 1)
			ehash--;
		const unsigned int *pbootkeyhash =
			&(*(*pbl0fn->pbootkeyhash))[ehash][0];
		if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) != 0) {
			pbl0fn->printf("boot key verify fail.\r\n");
			goto error;
		}
	}
#endif
/* -----------------------------------------------------------------------------
 * read boot image
 * ---------------------------------------------------------------------------*/

	rsize = pbi->LoadSize;
	plp = (unsigned char *)(*pbl0fn->pbm)->image;
	pbl0fn->read_flash(port, plp, flashaddr, rsize, addrstep);

	/*===========================================================
	 * boot image decrypt if requared.
	 * ========================================================*/
	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((unsigned int *)(*pbl0fn->pbm)->image,
				(unsigned int *)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}

error:
	pbl0fn->SPIDeinit(option);

	return ret;
}
