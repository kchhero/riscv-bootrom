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

#include "nx_type.h"
#include <nx_gpio.h>
#include "printf.h"
#include <nx_ecid.h>
#include <nx_alive.h>
//#include <nx_tieoff.h>
#include "aes.h"
#include "iNANDBOOTEC.h"
#include "iUSBBOOT.h"
#include "iSDHCBOOT.h"
#include "xyz_modem.h"
#ifdef NXP3220
#include <nx_sss.h>
#endif
#include "crypto.h"
#include <mbedtls/rsa.h>
//#ifdef NXP4330
#include <mbedtls/sha256.h>
//#endif
#include "diskio.h"
#include "fatfs.h"
#include "libplat.h"


#ifndef __FNPTR_H__
#define __FNPTR_H__

struct nx_bl0_fn {
	struct nx_bootmm *const (*pbm);
	/*=================================================*/
	/* library                                         */
	/*=================================================*/
	void (*printchar)(char **, int);
	int (*prints)(char **, const char *, int, int);
	int (*printi)(char **, int, int, int, int, int, int);
	int (*print)(char **, const char *, va_list);
	int (*printf)(const char *, ...);

	void (*buildinfo)(void);
	CBOOL (*DebugInit)(void);

	/* string */
	void (*udelay)(unsigned int);
	void *(*memcpy)(void *dest, const void *src, size_t n);
	void *(*memset)(void *str, int c, size_t n);
	int (*memcmp)(const void* s1, const void* s2, size_t n);
	/* math */
	void (*__div0)(void);
	unsigned int (*__udivmodsi4)(unsigned int num, unsigned int den);
	unsigned int (*__aeabi_uidiv)(unsigned int num, unsigned int den);
	signed int (*__aeabi_idiv)(signed int num, signed int den);
#ifdef NXP3220
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
#endif

	/*=================================================*/
	/* crypto                                          */
	/*=================================================*/

#ifndef BL1SIMUL
#ifdef NXP3220
	void (*gethash)(struct NX_HRDMADESC *, int, unsigned int *, int);
	void (*getkeyhash)(unsigned int *phash);
	void (*getimagehash)(unsigned int *phash);
	void (*aesdecrypt)(struct NX_BRTDMADESC *, int, unsigned int *);
#endif
	int (*headercheck)(unsigned int iv[], int dec);
	int (*keycheck)(unsigned int iv[], int dec, int verify);
	void (*imagedecrypt)(unsigned int iv[]);

	/*=================================================*/
	/* authentication                                  */
	/*=================================================*/
	void *(*nxalloc)(size_t n, size_t size);
	void *(*mbedtls_calloc)(size_t n, size_t size);
	void (*nxfree)(void *);
	void (*mbedtls_free)(void *);
	int (*mbedtls_rsa_check_pubkey)(const mbedtls_rsa_context *);
	int (*get_hash)(unsigned char *, unsigned int, unsigned char *,
			unsigned int, unsigned char []);
	void (*mgf_mask)(unsigned char *dst, size_t dlen, unsigned char *src,
			size_t slen, mbedtls_sha256_context *ctx);
	int (*mbedtls_rsa_public)(mbedtls_rsa_context *, const unsigned char *,
			unsigned char *);
	int (*mbedtls_rsa_rsassa_pss_verify_ext)(mbedtls_rsa_context *,
			unsigned int, const unsigned char *,
			int, const unsigned char *);
	int (*verify_image)(unsigned char [], void **, unsigned char []);
	int (*authenticate_image)(unsigned char *, unsigned char *,
			unsigned char *, int);
	/*=================================================*/
	/* nand boot                                       */
	/*=================================================*/
#ifdef NXP3220
	void (*nandcontroller_init)(struct nandbootinfo *, unsigned int);
	void (*nandcontroller_deinit)(void);
	int (*nandreset)(void);
	int (*nandread)(struct nandbootinfo *pni, void *pdata, int size);
#endif
	int (*iNANDBOOTEC)(unsigned int option);
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
	/* gpt */
	const unsigned int (*gpt_crc_table)[];
	unsigned int (*crc32_no_comp)(unsigned int crc,
			const unsigned char *buf, unsigned int len);
	unsigned int (*crc32)(unsigned int crc,
			const unsigned char *p, unsigned int len);
	int (*is_gpt_valid)(unsigned char *psector);
	unsigned int (*get_first_lba)(unsigned char *psector);
	unsigned int (*get_first_empty_lba)(unsigned char *psector);
	unsigned int (*is_gpt_part)(unsigned char *mbr);

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
	int (*eMMCBootRead)(SDXCBOOTSTATUS *, unsigned int);
	int (*eMMCBoot_Normal)(SDXCBOOTSTATUS *, unsigned int);
	int (*eMMCBoot_Alt)(SDXCBOOTSTATUS *, unsigned int);
	int (*SDMMCBOOT)(SDXCBOOTSTATUS *, unsigned int);
	void (*NX_SDPADSetALT)(unsigned int);
	void (*NX_SDPADSetGPIO)(unsigned int);
	unsigned int (*iSDXCBOOT)(unsigned int);
	int (*ieMMCBOOT)(unsigned int);
	/*=================================================*/
	/* sdfs boot                                       */
	/*=================================================*/
	FRESULT (*move_window)(FATFS *fs, U32 sector);
	U32 (*get_cluster)(FATFS *fs, U32 clst);
	U32 (*clust2sect)(FATFS *fs, U32 clst);
	FRESULT (*dir_seek)(DIR *dj, U16 idx);
	FRESULT (*dir_next)(DIR *dj, CBOOL streach);
	FRESULT (*dir_find)(DIR *dj);
	FRESULT (*create_name)(DIR *dj, const char **path);
	FRESULT (*follow_path)(DIR *dj, const char *path);
	U8 (*check_fs)(FATFS *fs, U32 sect);
	FRESULT (*f_mount)(const char **path, FATFS *rfs, U8 chk_wp);
	FRESULT (*validate)(FATFS *fs, U16 id);
	FRESULT (*f_open)(FIL *fp, const char *path, U8 mode, FATFS *pfs);
	FRESULT (*f_read)(FIL *fp, void *buff, U32 btr, U32 *br);
	FRESULT (*f_close)(FIL *fp);

	DSTATUS (*disk_initialize)(U8 drv);
	DSTATUS (*disk_status)(U8 drv);
	DRESULT (*disk_read)(U8 drv, U8 *buff, U32 sector, U8 count, U32 *diskhandle);

	int (*FSBoot)(SDXCBOOTSTATUS *, unsigned int);
	int (*SDMMCFSBOOT)(SDXCBOOTSTATUS *, unsigned int);
	unsigned int (*iSDXCFSBOOT)(unsigned int);
	/*=================================================*/
	/* spi boot                                        */
	/*=================================================*/
	void (*SPIPortInit)(unsigned int SPIPort);
	void (*SPIPortDeinit)(unsigned int SPIPort);
	void (*SPIInit)(unsigned int option);
	void (*SPIDeinit)(unsigned int option);
	void (*read_flash)(int, unsigned char *, unsigned int, int, int);
	unsigned int (*iSPIBOOT)(unsigned int option);

	/*=================================================*/
	/* uart boot                                        */
	/*=================================================*/
	void (*uartboot_init)(unsigned int option);
	void (*uartboot_deinit)(unsigned int option);
	char (*getch)(int port);
	void (*putch)(int port, char ch);
	int (*isrxready)(int port);
	const unsigned short (*crc16_tab)[];
	unsigned short (*cyg_crc16)(unsigned char *buf, int len);
	int (*CYGACC_COMM_IF_GETC_TIMEOUT)(int port, char *c);
	void (*CYGACC_COMM_IF_PUTC)(int port, char y);
	bool (*parse_num)(char *s, unsigned long *val, char **es, char *delim);
	int (*xyzModem_getc)(int port);
	void (*xyzModem_flush)(int port);
	int (*xyzModem_get_hdr)(xyzsts *xyz);
	int (*xyzModem_stream_open)(connection_info_t *info, xyzsts *xyz, int *err);
	int (*xyzModem_stream_read)(xyzsts *xyz, char *buf, int size, int *err);
	void (*xyzModem_stream_close)(int *err);
	void (*xyzModem_stream_terminate)(xyzsts *xyz, bool abort, int (*getc)(int));
	int (*xyzModem_error)(int err);
	int (*iUARTBOOT)(unsigned int option);

	/*=================================================*/
	const unsigned int (*iv)[];
	const unsigned int (*(*pbootkeyhash))[4][8];
	unsigned int (*iROMBOOT)(unsigned int);

	/*=================================================*/
	/* aes cbc                                         */
	/*=================================================*/
#ifdef NXP4330
	const unsigned int (*bootkey)[];
	void (*mbedtls_aes_init)(mbedtls_aes_context *ctx);
	void (*mbedtls_aes_free)(mbedtls_aes_context *ctx);
	int (*mbedtls_aes_setkey_enc)(mbedtls_aes_context *ctx,
			const unsigned char *key);
	int (*mbedtls_aes_setkey_dec)(mbedtls_aes_context *ctx,
			const unsigned char *key);
	int (*mbedtls_internal_aes_decrypt)(mbedtls_aes_context *ctx,
		const unsigned char input[16],
		unsigned char output[16]);
	int (*mbedtls_aes_decrypt_cbc)(mbedtls_aes_context *ctx, size_t length,
			unsigned char iv[16],
			const unsigned char *input, unsigned char *output);
#endif
	/*=================================================*/
	/* sha256                                          */
	/*=================================================*/
	void (*mbedtls_sha256_init)(mbedtls_sha256_context *ctx);
	void (*mbedtls_sha256_free)(mbedtls_sha256_context *ctx);
	void (*mbedtls_sha256_starts)(mbedtls_sha256_context *ctx, int is224);
	void (*mbedtls_sha256_process)(mbedtls_sha256_context *,
			const unsigned char [64]);
	void (*mbedtls_sha256_update)(mbedtls_sha256_context *,
			const unsigned char *, size_t);
	void (*mbedtls_sha256_finish)(mbedtls_sha256_context *, unsigned char [32]);
	void (*mbedtls_sha256)( const unsigned char *input, size_t ilen,
		     unsigned char output[32], int is224);

	unsigned int *(*Decrypt)(unsigned int *src, unsigned int *dst,
			unsigned int iv[], unsigned int size);
#endif
};

extern const struct nx_bl0_fn bl0fn;
#endif /* __FNPTR_H__ */
