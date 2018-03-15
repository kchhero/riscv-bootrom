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
 * File		: fnptr.h
 * Description	: all pointer
 * Author	: Hans
 * History	: 2018.01.14 First Implementation
 */

#include "include/nx_type.h"
#include "include/nx_gpio.h"
#ifndef QEMU_RISCV
#include "printf.h"
#else
#include "kprintf.h"
#endif
#include "include/nx_ecid.h"
#include "include/nx_alive.h"

#include "iUSBBOOT.h"
#include "iSDHCBOOT.h"

#include "include/nx_sss.h"

#include "diskio.h"

#include "libplat.h"


#ifndef __FNPTR_H__
#define __FNPTR_H__

typedef struct{
	struct nx_bootmm *const (*pbm);
	/*=================================================*/
	/* library                                         */
	/*=================================================*/
#ifndef QEMU_RISCV
	void (*printchar)(char **, int);
	int (*prints)(char **, const char *, int, int);
	int (*printi)(char **, int, int, int, int, int, int);
	int (*print)(char **, const char *, va_list);
	int (*_dprintf)(const char *, ...);
#else
        void (*kputs)(const char *);
        void (*_dprintf)(const char *, ...);
#endif
  //	void (*buildinfo)(void);
	CBOOL (*DebugInit)(void);

	/* string */
	void (*udelay)(unsigned int);
	void *(*nx_memcpy)(void *dest, const void *src, size_t n);
	void *(*nx_memset)(void *str, int c, size_t n);
	int (*nx_memcmp)(const void* s1, const void* s2, size_t n);
	/* math */
	void (*__div0)(void);

	/* pad */
	void (*GPIOSetAltFunction)(const struct nxpadi *pad, CBOOL setalt);
	void (*GPIOSetDrvSt)(const struct nxpadi *pad, NX_GPIO_DRVSTRENGTH str);
	void (*GPIOSetPullup)(const struct nxpadi *pad, NX_GPIO_PULL pull);
	void (*GPIOReleasePAD)(const struct nxpadi *pad);
	void (*setpad)(const union nxpad *const ppad, int num, int enable);
	/* clock */
  	void (*setcpuclock)(unsigned int clklvl);
  	void (*setsystemclock)(unsigned int clklvl);
  	void (*setdeviceclock)(const struct cmu_device_clk *pdclk, int num, int enb);

	/*=================================================*/
	/* usb boot                                        */
	/*=================================================*/
	const unsigned char (*gs_DevDescFS)[];
	const unsigned char (*gs_DevDescHS)[];
	const unsigned char (*gs_ConfigDescFS)[];
	const unsigned char (*gs_ConfigDescHS)[];
	void (*nx_usb_write_in_fifo)(unsigned int, unsigned char *, int);
	void (*nx_usb_read_out_fifo)(unsigned int, unsigned char *, int);
	void (*nx_usb_ep0_int_hndlr)(USBBOOTSTATUS *);
	void (*nx_usb_transfer_ep0)(USBBOOTSTATUS *);
	void (*nx_usb_int_bulkin)(USBBOOTSTATUS *);
	void (*nx_usb_int_bulkout)(USBBOOTSTATUS *, unsigned int, unsigned int);
	void (*nx_usb_reset)(USBBOOTSTATUS *);
	int (*nx_usb_set_init)(USBBOOTSTATUS *);
	void (*nx_usb_pkt_receive)(USBBOOTSTATUS *, unsigned int);
	void (*nx_usb_transfer)(USBBOOTSTATUS *);
	void (*nx_udc_int_hndlr)(USBBOOTSTATUS *, unsigned int);
	int (*iUSBBOOT)(unsigned int);
	/*=================================================*/
	/* sdmmc boot                                      */
	/*=================================================*/

	int (*NX_SDMMC_SetClock)(SDXCBOOTSTATUS *, int, unsigned int);
	unsigned int (*NX_SDMMC_SendCommandInternal)(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
	unsigned int (*NX_SDMMC_SendStatus)(SDXCBOOTSTATUS *);
	unsigned int (*NX_SDMMC_SendCommand)(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
	unsigned int (*NX_SDMMC_SendAppCommand)(SDXCBOOTSTATUS *, NX_SDMMC_COMMAND *);
	int (*NX_SDMMC_IdentifyCard)(SDXCBOOTSTATUS *);
	int (*NX_SDMMC_SelectCard)(SDXCBOOTSTATUS *);
	int (*NX_SDMMC_SetCardDetectPullUp)(SDXCBOOTSTATUS *, int);
	int (*NX_SDMMC_SetBusWidth)(SDXCBOOTSTATUS *, unsigned int);
	int (*NX_SDMMC_SetBlockLength)(SDXCBOOTSTATUS *, unsigned int);
	int (*NX_SDMMC_Init)(SDXCBOOTSTATUS *);
	int (*NX_SDMMC_Terminate)(SDXCBOOTSTATUS *);
	int (*NX_SDMMC_Open)(SDXCBOOTSTATUS *, unsigned int);
	int (*NX_SDMMC_Close)(SDXCBOOTSTATUS *);
	int (*NX_SDMMC_ReadSectorData)(SDXCBOOTSTATUS *, unsigned int, unsigned int *);
	int (*NX_SDMMC_ReadSectors)(SDXCBOOTSTATUS *, unsigned int, unsigned int, unsigned int *);
	int (*NX_SDMMC_ReadBootSector)(SDXCBOOTSTATUS *, unsigned int, unsigned int *);
	int (*SDMMCBOOT)(SDXCBOOTSTATUS *, unsigned int);
	void (*NX_SDPADSetALT)(unsigned int);
	void (*NX_SDPADSetGPIO)(unsigned int);
	unsigned int (*iSDXCBOOT)(unsigned int);

        /*=================================================*/
	const unsigned int (*iv)[];
	const unsigned int (*(*pbootkeyhash))[4][8];
	unsigned int (*iROMBOOT)(unsigned int);
} NXBL0FN;

extern const NXBL0FN bl0fn;
  
void SetICache(int enable);
unsigned int GetBootOption(void);
void SetBootOption(unsigned int);
unsigned int *GetMempoolPtr(void);
void SetMempoolPtr(unsigned int mempoolptr);
unsigned int *GetStringPtr(void);
void SetStringPtr(unsigned int *StringPtr);
void Setbl0fnPtr(const NXBL0FN *);
NXBL0FN *Getbl0fnPtr(void);
unsigned int GetPMCnt(void);
//; void asmudelay(unsigned int utime);

#endif /* __FNPTR_H__ */
