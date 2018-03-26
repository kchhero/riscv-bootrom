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
 * File		: fnptr.c
 * Description	: all function pointer used in bl0
 * Author	: Hans
 * History	: 2018.01.12 First Implementation
 */
#include "fnptr.h"

extern struct nx_bootmm * const pbm;

	/*=================================================*/
	/* library                                         */
	/*=================================================*/
#ifndef QEMU_RISCV
extern void printchar(char **, int c);
extern int prints(char **, const char *, int, int);
extern int printi(char **, int, int, int, int, int, int);
extern int print(char **, const char *, va_list);
extern int _dprintf(const char *, ...);
#else
extern void kputs(const char *);
extern void _dprintf(const char *, ...);
#endif

//extern void buildinfo(void);
extern CBOOL DebugInit(void);

	/* string */
extern void udelay(unsigned int);
extern void *nx_memcpy(void *dest, const void *src, unsigned int n);
extern void *nx_memset(void *str, int c, unsigned int n);
extern int nx_memcmp(const void* s1, const void* s2, unsigned int n);
	/* math */
extern void __div0(void);

	/* pad */
extern void GPIOSetAltFunction(const struct nxpadi *pad, CBOOL setalt);
extern void GPIOSetDrvSt(const struct nxpadi *pad, NX_GPIO_DRVSTRENGTH str);
extern void GPIOSetPullup(const struct nxpadi *pad, NX_GPIO_PULL pull);
extern void GPIOReleasePAD(const struct nxpadi *pad);
extern void setpad(const union nxpad *const ppad, int num, int enable);
	/* clock */
extern void setcpuclock(unsigned int clklvl);
extern void setsystemclock(unsigned int clklvl);
extern void setdeviceclock(const struct cmu_device_clk *pdclk, int num, int enb);


	/*=================================================*/
	/* usb boot                                        */
	/*=================================================*/
extern const U8 gs_DevDescFS[];
extern const U8 gs_DevDescHS[];
extern const U8 gs_ConfigDescFS[];
extern const U8 gs_ConfigDescHS[];
extern void nx_usb_write_in_fifo(U32, U8 *, S32);
extern void nx_usb_read_out_fifo(U32, U8 *, S32);
extern void nx_usb_ep0_int_hndlr(USBBOOTSTATUS *);
extern void nx_usb_transfer_ep0(USBBOOTSTATUS *);
extern void nx_usb_int_bulkin(USBBOOTSTATUS *);
extern void nx_usb_int_bulkout(USBBOOTSTATUS *, U32, U32);
extern void nx_usb_reset(USBBOOTSTATUS *);
extern S32 nx_usb_set_init(USBBOOTSTATUS *);
extern void nx_usb_pkt_receive(USBBOOTSTATUS *, U32);
extern void nx_usb_transfer(USBBOOTSTATUS *);
extern void nx_udc_int_hndlr(USBBOOTSTATUS *, U32);
extern CBOOL iUSBBOOT(U32);
	/*=================================================*/
	/* sdmmc boot                                      */
	/*=================================================*/

extern CBOOL NX_SDMMC_SetClock(SDXCBOOTSTATUS *, CBOOL, U32);
extern U32 NX_SDMMC_SendCommandInternal(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
extern U32 NX_SDMMC_SendStatus(SDXCBOOTSTATUS *);
extern U32 NX_SDMMC_SendCommand(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
extern U32 NX_SDMMC_SendAppCommand(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
extern CBOOL NX_SDMMC_IdentifyCard(SDXCBOOTSTATUS *);
extern CBOOL NX_SDMMC_SelectCard(SDXCBOOTSTATUS *);
extern CBOOL NX_SDMMC_SetCardDetectPullUp(SDXCBOOTSTATUS *, CBOOL);
extern CBOOL NX_SDMMC_SetBusWidth(SDXCBOOTSTATUS *, U32);
extern CBOOL NX_SDMMC_SetBlockLength(SDXCBOOTSTATUS *, U32);
extern CBOOL NX_SDMMC_Init(SDXCBOOTSTATUS *);
extern CBOOL NX_SDMMC_Terminate(SDXCBOOTSTATUS *);
extern CBOOL NX_SDMMC_Open(SDXCBOOTSTATUS *, U32);
extern CBOOL NX_SDMMC_Close(SDXCBOOTSTATUS *);
extern CBOOL NX_SDMMC_ReadSectorData(SDXCBOOTSTATUS *, U32, U32 *);
extern CBOOL NX_SDMMC_ReadSectors(SDXCBOOTSTATUS *, U32, U32, U32 *);
extern CBOOL SDMMCBOOT(SDXCBOOTSTATUS *, U32);
extern void NX_SDPADSetALT(U32);
extern void NX_SDPADSetGPIO(U32);
extern U32 iSDXCBOOT(U32);

DSTATUS disk_initialize(U8 drv);
DSTATUS disk_status(U8 drv);
DRESULT disk_read(U8 drv, U8 *buff, U32 sector, U8 count, U32 *diskhandle);

extern CBOOL FSBoot(SDXCBOOTSTATUS *, U32);
extern CBOOL SDMMCFSBOOT(SDXCBOOTSTATUS *, U32);
extern U32 iSDXCFSBOOT(U32);

	/*=================================================*/
extern const unsigned int iv[];
extern const U32 (*pbootkeyhash)[4][8];
extern unsigned int iROMBOOT(unsigned int);


const NXBL0FN bl0fn = {
	&pbm,

	/*=================================================*/
	/* library                                         */
	/*=================================================*/
#ifndef QEMU_RISCV
	printchar,
	prints,
	printi,
	print,
	_dprintf,
#else
        kputs,
        _dprintf,
#endif

        //	buildinfo,
	DebugInit,

	/* string */
	udelay,
	nx_memcpy,
	nx_memset,
	nx_memcmp,

	/* math */
	__div0,

	/* pad */
	GPIOSetAltFunction,
	GPIOSetDrvSt,
	GPIOSetPullup,
	GPIOReleasePAD,
	setpad,

	/* clock */
        setcpuclock,
        setsystemclock,
        setdeviceclock,

	/*=================================================*/
	/* usb boot                                        */
	/*=================================================*/
	&gs_DevDescFS,
	&gs_DevDescHS,
	&gs_ConfigDescFS,
	&gs_ConfigDescHS,
	nx_usb_write_in_fifo,
	nx_usb_read_out_fifo,
	nx_usb_ep0_int_hndlr,
	nx_usb_transfer_ep0,
	nx_usb_int_bulkin,
	nx_usb_int_bulkout,
	nx_usb_reset,
	nx_usb_set_init,
	nx_usb_pkt_receive,
	nx_usb_transfer,
	nx_udc_int_hndlr,
	iUSBBOOT,
	/*=================================================*/
	/* sdmmc boot                                      */
	/*=================================================*/

	NX_SDMMC_SetClock,
	NX_SDMMC_SendCommandInternal,
	NX_SDMMC_SendStatus,
	NX_SDMMC_SendCommand,
	NX_SDMMC_SendAppCommand,
	NX_SDMMC_IdentifyCard,
	NX_SDMMC_SelectCard,
	NX_SDMMC_SetCardDetectPullUp,
	NX_SDMMC_SetBusWidth,
	NX_SDMMC_SetBlockLength,
	NX_SDMMC_Init,
	NX_SDMMC_Terminate,
	NX_SDMMC_Open,
	NX_SDMMC_Close,
	NX_SDMMC_ReadSectorData,
	NX_SDMMC_ReadSectors,
	SDMMCBOOT,
	NX_SDPADSetALT,
	NX_SDPADSetGPIO,
	iSDXCBOOT,

	/*=================================================*/
	&iv,
	&pbootkeyhash,
	iROMBOOT,
};

NXBL0FN *Getbl0fnPtr(void)
{
  return (NXBL0FN*)&bl0fn;
}

void delay(U32 us)
{
	register volatile unsigned int   count, temp;

	for (count = 0; count < us; count++) {
		temp ^= count;
	}
}
