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
 * File		: uartboot.c
 * Description	: uartboot used y-modem
 * Author	: Hans
 * History	: 2018.01.12 First Implementation
 */

#include <xyz_modem.h>
#include "debug.h"
#include "libplat.h"
#include "printf.h"
#include "nx_antares_bootheader.h"
#include "nx_antares_bootoption.h"
#ifdef NXP3220
#include <nx_chip_sfr.h>
#include <nx_chip_iomux.h>
#include <nx_cmu.h>
#endif
#include "libarm.h"
#include "crypto.h"
#include <nx_uart.h>
#include "fnptr.h"


#ifdef NXP4330
char getchar(void);
void putchar(char);
int isrxreceived(void);
#endif

#ifdef NXP4330
const U32 UARTBASEADDR[] =
{
	PHY_BASEADDR_UART0_MODULE,
	PHY_BASEADDR_pl01115_Uart_modem_MODULE,
	PHY_BASEADDR_UART1_MODULE,
	PHY_BASEADDR_pl01115_Uart_nodma0_MODULE,
	PHY_BASEADDR_pl01115_Uart_nodma1_MODULE
};
const U32 UARTCLKGENADDR[] =
{
	PHY_BASEADDR_CLKGEN22_MODULE,
	PHY_BASEADDR_CLKGEN24_MODULE,
	PHY_BASEADDR_CLKGEN23_MODULE,
	PHY_BASEADDR_CLKGEN25_MODULE,
	PHY_BASEADDR_CLKGEN26_MODULE
};
const U8 RESETNUM[] =
{
	RESETINDEX_OF_UART0_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_modem_MODULE_nUARTRST,
	RESETINDEX_OF_UART1_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_nodma0_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_nodma1_MODULE_nUARTRST
};
const U32 GPIOALTNUM[] =
{
	PADINDEX_OF_UART0_UARTRXD,
	PADINDEX_OF_UART0_UARTTXD,
	PADINDEX_OF_pl01115_Uart_modem_UARTRXD,
	PADINDEX_OF_pl01115_Uart_modem_UARTTXD,
	PADINDEX_OF_UART1_UARTRXD,
	PADINDEX_OF_UART1_UARTTXD,
	PADINDEX_OF_pl01115_Uart_nodma0_UARTRXD,
	PADINDEX_OF_pl01115_Uart_nodma0_UARTTXD,
	PADINDEX_OF_pl01115_Uart_nodma1_UARTRXD,
	PADINDEX_OF_pl01115_Uart_nodma1_UARTTXD
};
#endif

#ifdef NXP3220
struct NX_UART_RegisterSet * const pgUART[7] =
{
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART0_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART1_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART2_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART3_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART4_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART5_MODULE,
	(struct NX_UART_RegisterSet *)PHY_BASEADDR_UART6_MODULE
};
const union nxpad uartpad[7][2] = {
	{{PI_UART0_TXD}, {PI_UART0_RXD_SMC}},
	{{PI_UART1_TXD}, {PI_UART1_RXD_SMC}},
	{{PI_UART2_TXD}, {PI_UART2_RXD_SMC}},
	{{PI_UART3_TXD}, {PI_UART3_RXD_SMC}},
	{{PI_UART4_TXD}, {PI_UART4_RXD_SMC}},
	{{PI_UART5_TXD}, {PI_UART5_RXD_SMC}},
	{{PI_UART6_TXD}, {PI_UART6_RXD_SMC}}			
};
const struct cmu_device_clk uartclk[7][2] = {
	{	// port 0
		{0x1C00, 13, 0, NX_CLKSRC_PLL0, 5},	// core
		{0x1E00, 14, 0, NX_CLKSRC_PLL0, 10}	// apb
	}, {	// port 1
		{0x1C00, 13, 1, NX_CLKSRC_PLL0, 5},
		{0x1E00, 14, 1, NX_CLKSRC_PLL0, 10}
	}, {	// port 2
		{0x1C00, 13, 2, NX_CLKSRC_PLL0, 5},
		{0x1E00, 14, 2, NX_CLKSRC_PLL0, 10}
	}, {	// port 3
		{0x1C00, 13, 3, NX_CLKSRC_PLL0, 5},
		{0x1E00, 14, 3, NX_CLKSRC_PLL0, 10}
	}, {	// port 4
		{0x1C00, 13, 4, NX_CLKSRC_PLL0, 5},
		{0x1E00, 14, 4, NX_CLKSRC_PLL0, 10}
	}, {	// port 5
		{0x1C00, 13, 5, NX_CLKSRC_PLL0, 5},
		{0x1E00, 14, 5, NX_CLKSRC_PLL0, 10}
	}, {	// port 6
		{0x1C00, 13, 6, NX_CLKSRC_PLL0, 5}, 
		{0x1E00, 14, 6, NX_CLKSRC_PLL0, 10}
	}
};

#define UART_SRC_CLK    100000000
//#define UART_BAUDRATE   115200
/* 100MHz / 16 / 115200 = 54.2534722222
 * divisor low = 54,
 * fractional divisor = 0.253472222 * 64 = 16
 * real baudrate = 100MHz / 16 / (54 + 16 / 64) = 115207.37327
 * error rate = 115207.37327 / 115200 = 1.000064 = 0.0064%
 *
 * 100MHz / 16 / 921600 = 6.7816840277777
 * divisor low = 6
 * fractional divisor = 0.781684027777 * 64 = 50.027777
 * real baudrate = 100MHz / 16 / (6 + 50 / 64) = 921658.986175
 * error rate = 921658.986175 / 921600 = 1.000064 = 0.0064%
 */
const unsigned char baudrate[2][2] = {
	{54,	/* divisor low */
	16},	/* fractional divisor */
	{6,	/* divisor low */
	50}	/* fractional divisor */
};
#endif
void uartboot_init(unsigned int option)
{
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int port = (option >> 5) & 0x7;
	int speed = (option >> 3) & 0x1;
	struct NX_UART_RegisterSet *const pUART = pgUART[port];

	pbl0fn->setpad(uartpad[port], 2, 1);
	pbl0fn->setdeviceclock(&uartclk[port][0], 2, 1);

	pUART->DLH_IER = 0x0;           /* disable int */

	pUART->LCR = 1 << 7;            /* divisor latch access enable */
	pUART->RBR_DLL_THR = baudrate[speed][0];
	pUART->DLH_IER = 0;
	pUART->DLF = baudrate[speed][1];           /* 6 bits */
	pUART->LCR = 0 << 7;            /* divisor latch access disable */

	pUART->FCR_IIR =
		0 << 6 |        /* rx trig level. 0:1ch, 1:1/4, 2:1/2, 3:full */
		0 << 4 |        /* tx trig level. 0:empty, 1:2ch, 2:1/4, 3:1/2 */
		0 << 3 |        /* no dma mode */
		1 << 2 |        /* tx fifo reset */
		1 << 1 |        /* rx fifo reset */
		1 << 0;         /* fifo enable */

	pUART->LCR =
		0 << 7 |        /* no divisor latch access */
		0 << 6 |        /* no break */
		0 << 5 |        /* stick parity disable */
		0 << 4 |        /* 0: odd, 1: even parity */
		0 << 3 |        /* disable parity */
		0 << 2 |        /* 0: 1 stop bit, 1: 1.5 or 2 stop bits */
		3 << 0;         /* 0: 5bit, 1: 6bit, 2: 7bit, 3: 8bit */
#endif
#ifdef NXP4330
	option = option;
#endif
}
void uartboot_deinit(unsigned int option)
{
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	int port = (option >> 5) & 0x7;
//	struct NX_UART_RegisterSet *const pUART = pgUART[port];

	pbl0fn->setpad(uartpad[port], 2, 0);
	pbl0fn->setdeviceclock(&uartclk[port][0], 2, 0);
#endif
#ifdef NXP4330
	option = option;
#endif
}
int isrxready(int port)
{
#ifdef NXP3220
	struct NX_UART_RegisterSet *pUART = pgUART[port];
	if (pUART->USR & 1 << 3)        /* rx fifo empty check(rx not empty) */
		return 1;
	return 0;
#endif
#ifdef NXP4330
	port = port;
	return isrxreceived();
#endif
}
char getch(int port)
{
#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	struct NX_UART_RegisterSet *pUART = pgUART[port];
	while (pbl0fn->isrxready(port))
		;
	return pUART->RBR_DLL_THR;
#endif
#ifdef NXP4330
	while (isrxready(port))
		;
	return getchar();
#endif
}
void putch(int port, char ch)
{
#ifdef NXP3220
	struct NX_UART_RegisterSet *pUART = pgUART[port];
	while (!(pUART->USR & 1 << 1))  /* tx fifo full check(tx not full) */
		;
	pUART->RBR_DLL_THR = (unsigned int)ch;
#endif
#ifdef NXP4330
	port = port;
	putchar(ch);
#endif
}
/* External Function */
int iUARTBOOT(unsigned int option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	unsigned int port = (option >> UARTPORT) & 0x7;
	connection_info_t info;
	xyzsts xyz;
	int err = 0, res, ret = 0;

	pbl0fn->uartboot_init(option);

	info.mode = xyzModem_ymodem;
	info.chan = port;

	do {
		while (pbl0fn->isrxready(port))
			pbl0fn->getch(port);

		res = pbl0fn->xyzModem_stream_open(&info, &xyz, &err);
	} while (res);

	/*===========================================================
	 * header receive
	 * ========================================================*/
	res = pbl0fn->xyzModem_stream_read(&xyz,
			(char *)&(*pbl0fn->pbm)->bi,
			sizeof(struct nx_bootinfo), &err);

	unsigned int iv[4];
	iv[0] = (*pbl0fn->iv)[0];
	iv[1] = (*pbl0fn->iv)[1];
	iv[2] = (*pbl0fn->iv)[2];
	iv[3] = (*pbl0fn->iv)[3];
#if 1
	if (!pbl0fn->headercheck(iv, option & 1 << DECRYPT)) {
		return 0;
	}
#else
	unsigned int pdd[4];	// partial decrypt data
#if 1
	if (option & 1 << DECRYPT) {
		unsigned int *pti = (unsigned int *)(*pbl0fn->pbm);
		iv[0] = pti[12]; 
		iv[1] = pti[13]; 
		iv[2] = pti[14]; 
		iv[3] = pti[15]; 
		pdd[0] = pti[16];	// save data
		pdd[1] = pti[17];
		pdd[2] = pti[18];
		pdd[3] = pti[19];
		pbl0fn->Decrypt(pdd, pdd, iv, 16);	// partial Decrypt
	}
#else
	unsigned int *pti = (unsigned int *)(*pbl0fn->pbm);
	if (option & 1 << DECRYPT) {
		iv[0] = (*pbl0fn->iv)[0];
		iv[1] = (*pbl0fn->iv)[1];
		iv[2] = (*pbl0fn->iv)[2];
		iv[3] = (*pbl0fn->iv)[3];
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->bi,
				(U32*)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));
	}
	pdd[0] = pti[16];	// save data
#endif
#endif


	/*===========================================================
	 * key receive
	 * ========================================================*/
	res = pbl0fn->xyzModem_stream_read(&xyz,
			(char *)&(*pbl0fn->pbm)->rsa_public,
			sizeof(struct asymmetrickey), &err);

	/*===========================================================
	 * image receive
	 * ========================================================*/
#if 1
	res = pbl0fn->xyzModem_stream_read(&xyz,
			(char *)(*pbl0fn->pbm)->image,
			(*pbl0fn->pbm)->bi.LoadSize, &err);
#else
	res = pbl0fn->xyzModem_stream_read(&xyz,
			(char *)(*pbl0fn->pbm)->image,
			pdd[0] + 1, &err);

	if (res != (int)pdd[0])
		ret = pbl0fn->xyzModem_error(err);
#endif

	pbl0fn->xyzModem_stream_close(&err);
	pbl0fn->xyzModem_stream_terminate(&xyz, 0, pbl0fn->xyzModem_getc);

	if (ret < 0) {
		pbl0fn->printf("receive data broken.\r\n");
		return 0;
	}
	pbl0fn->printf("\r\nreceive done %d.\r\n", res);

#if 0
	int i;
	unsigned char *buf = &(*pbl0fn->pbm)->bi;
	printf("org:\n");
	for (i = 0; i < 512; i++) {
		if (i % 16 == 0)
			printf("\r\n");
		printf("%02x ", buf[i]);
	}
	printf("\r\n");
#endif

	/*===========================================================
	 * boot header check
	 * ========================================================*/
#if 0
	if (option & 1 << DECRYPT) {
		iv[0] = (*pbl0fn->iv)[0];
		iv[1] = (*pbl0fn->iv)[1];
		iv[2] = (*pbl0fn->iv)[2];
		iv[3] = (*pbl0fn->iv)[3];
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->bi,
				(U32*)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));
	}
#endif
#if 0
//	unsigned char *buf = &(*pbl0fn->pbm)->bi;
	printf("dec:\n");
	for (i = 0; i < 512; i++) {
		if (i % 16 == 0)
			printf("\r\n");
		printf("%02x ", buf[i]);
	}
	printf("\r\n");
#endif

#if 0
	if ((*pbl0fn->pbm)->bi.signature != HEADER_ID) {
		pbl0fn->printf("invalid boot image(%08x).\r\n",
				(*pbl0fn->pbm)->bi.signature);
		return 0;
	}
#endif
	/*===========================================================
	 * boot key certify
	 * ========================================================*/
#if 1
	if (!pbl0fn->keycheck(iv, option & 1 << DECRYPT, (option >> VERIFY) & 0x3)) {
		return 0;
	}
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->rsa_public,
				(U32*)&(*pbl0fn->pbm)->rsa_public,
				iv, sizeof(struct asymmetrickey));
	}

	U32 ehash = (option >> VERIFY) & 0x3;
	if (ehash) {
		U32 bhash[32 / 4];
		pbl0fn->mbedtls_sha256((*pbl0fn->pbm)->rsa_public.rsapublicbootkey,
			256, (unsigned char *)bhash, 0);
		if (ehash == 1)
			ehash--;
		const U32 *pbootkeyhash =
			&(*(*pbl0fn->pbootkeyhash))[ehash][0];
		if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) != 0) {
			pbl0fn->printf("boot key verify fail.\r\n");
			return 0;
		}
	}
#endif

	/*===========================================================
	 * boot image decrypt if requared.
	 * ========================================================*/
	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((U32*)(*pbl0fn->pbm)->image,
				(U32*)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}

	pbl0fn->uartboot_deinit(option);

	if (ret < 0)
		return 0;

	return 1;
}

