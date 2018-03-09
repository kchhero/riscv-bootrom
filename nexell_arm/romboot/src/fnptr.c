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
extern void printchar(char **, int c);
extern int prints(char **, const char *, int, int);
extern int printi(char **, int, int, int, int, int, int);
extern int print(char **, const char *, va_list);
extern int printf(const char *, ...);

extern void buildinfo(void);
extern CBOOL DebugInit(void);

	/* string */
extern void udelay(unsigned int);
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *str, int c, size_t n);
extern int memcmp(const void* s1, const void* s2, size_t n);
	/* math */
extern void __div0(void);
extern unsigned int __udivmodsi4(unsigned int num, unsigned int den);
extern unsigned int __aeabi_uidiv(unsigned int num, unsigned int den);
extern signed int __aeabi_idiv(signed int num, signed int den);
#ifdef NXP3220
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
#endif
	/*=================================================*/
	/* crypto                                          */
	/*=================================================*/

#ifndef BL1SIMUL
#ifdef NXP3220
extern void gethash(struct NX_HRDMADESC *, int, unsigned int *, int);
extern void getkeyhash(unsigned int *phash);
extern void getimagehash(unsigned int *phash);
extern void aesdecrypt(struct NX_BRTDMADESC *, int, unsigned int *);
#endif
extern int headercheck(unsigned int iv[], int dec);
extern int keycheck(unsigned int iv[], int dec, int verify);
extern void imagedecrypt(unsigned int iv[]);

	/*=================================================*/
	/* authentication                                  */
	/*=================================================*/
extern void *nxalloc(size_t n, size_t size);
extern void *mbedtls_calloc(size_t n, size_t size);
extern void nxfree(void *);
extern void mbedtls_free(void *);
extern int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *);
extern int get_hash(unsigned char *, unsigned int, unsigned char *,
		unsigned int, unsigned char []);
extern void mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
			size_t slen, mbedtls_sha256_context *ctx);
extern int mbedtls_rsa_public(mbedtls_rsa_context *, const unsigned char *,
			unsigned char *);
extern int mbedtls_rsa_rsassa_pss_verify_ext(mbedtls_rsa_context *, unsigned int,
		const unsigned char *, int , const unsigned char *);
extern int verify_image(unsigned char [], void **, unsigned char []);
extern int authenticate_image(unsigned char *, unsigned char *,
		unsigned char *, int);

	/*=================================================*/
	/* nand boot                                       */
	/*=================================================*/
#ifdef NXP3220
extern void nandcontroller_init(struct nandbootinfo *pni, unsigned int bootoption);
extern void nandcontroller_deinit(void);
extern int nandreset(void);
extern int nandread(struct nandbootinfo *pni, void *pdata, int size);
#endif
extern int iNANDBOOTEC(unsigned int option);
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
	/* gpt */
extern const unsigned int gpt_crc_table[];
extern unsigned int crc32_no_comp(unsigned int crc,
 		const unsigned char *buf, unsigned int len);
extern unsigned int crc32(unsigned int crc,
 		const unsigned char *p, unsigned int len);
extern int is_gpt_valid(unsigned char *psector);
extern unsigned int get_first_lba(unsigned char *psector);
extern unsigned int get_first_empty_lba(unsigned char *psector);
extern unsigned int is_gpt_part(unsigned char *mbr);

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
extern CBOOL NX_SDMMC_ReadBootSector(SDXCBOOTSTATUS *, U32, U32 *);
extern CBOOL eMMCBootRead(SDXCBOOTSTATUS *, unsigned int);
extern CBOOL eMMCBoot_Normal(SDXCBOOTSTATUS *, U32);
extern CBOOL eMMCBoot_Alt(SDXCBOOTSTATUS *, U32);
extern CBOOL SDMMCBOOT(SDXCBOOTSTATUS *, U32);
extern void NX_SDPADSetALT(U32);
extern void NX_SDPADSetGPIO(U32);
extern U32 iSDXCBOOT(U32);
extern CBOOL ieMMCBOOT(U32);
	/*=================================================*/
	/* sdfs boot                                       */
	/*=================================================*/
FRESULT move_window(FATFS *fs, U32 sector);
U32 get_cluster(FATFS *fs, U32 clst);
U32 clust2sect(FATFS *fs, U32 clst);
FRESULT dir_seek(DIR *dj, U16 idx);
FRESULT dir_next(DIR *dj, CBOOL streach);
FRESULT dir_find(DIR *dj);
FRESULT create_name(DIR *dj, const char **path);
FRESULT follow_path(DIR *dj, const char *path);
U8 check_fs(FATFS *fs, U32 sect);
FRESULT f_mount(const char **path, FATFS *rfs, U8 chk_wp);
FRESULT validate(FATFS *fs, U16 id);
FRESULT f_open(FIL *fp, const char *path, U8 mode, FATFS *pfs);
FRESULT f_read(FIL *fp, void *buff, U32 btr, U32 *br);
FRESULT f_close(FIL *fp);

DSTATUS disk_initialize(U8 drv);
DSTATUS disk_status(U8 drv);
DRESULT disk_read(U8 drv, U8 *buff, U32 sector, U8 count, U32 *diskhandle);

extern CBOOL FSBoot(SDXCBOOTSTATUS *, U32);
extern CBOOL SDMMCFSBOOT(SDXCBOOTSTATUS *, U32);
extern U32 iSDXCFSBOOT(U32);

	/*=================================================*/
	/* spi boot                                        */
	/*=================================================*/
extern void SPIPortInit(unsigned int SPIPort);
extern void SPIPortDeinit(unsigned int SPIPort);
extern void SPIInit(unsigned int option);
extern void SPIDeinit(unsigned int option);
extern void read_flash(int, unsigned char *, unsigned int, int, int);
extern unsigned int iSPIBOOT(unsigned int option);

	/*=================================================*/
	/* uart boot                                        */
	/*=================================================*/
extern void uartboot_init(unsigned int option);
extern void uartboot_deinit(unsigned int option);
extern char getch(int port);
extern void putch(int port, char ch);
extern int isrxready(int port);
extern const unsigned short crc16_tab[];
extern unsigned short cyg_crc16(unsigned char *buf, int len);
extern int CYGACC_COMM_IF_GETC_TIMEOUT(int port, char *c);
extern void CYGACC_COMM_IF_PUTC(int port, char y);
extern bool parse_num(char *s, unsigned long *val, char **es, char *delim);
extern int xyzModem_getc(int port);
extern void xyzModem_flush(int port);
extern int xyzModem_get_hdr(xyzsts *xyz);
extern int xyzModem_stream_open(connection_info_t *info, xyzsts *xyz, int *err);
extern int xyzModem_stream_read(xyzsts *xyz, char *buf, int size, int *err);
extern void xyzModem_stream_close(int *err);
extern void xyzModem_stream_terminate(xyzsts *xyz, bool abort, int (*getc)(int));
extern int xyzModem_error(int err);
extern int iUARTBOOT(unsigned int option);

	/*=================================================*/
extern const unsigned int iv[];
extern const U32 (*pbootkeyhash)[4][8];
extern unsigned int iROMBOOT(unsigned int);

	/*=================================================*/
	/* aes cbc                                         */
	/*=================================================*/
#ifdef NXP4330
extern const unsigned int bootkey[];
extern void mbedtls_aes_init(mbedtls_aes_context *);
extern void mbedtls_aes_free(mbedtls_aes_context *);
extern int mbedtls_aes_setkey_enc(mbedtls_aes_context *, const unsigned char *);
extern int mbedtls_aes_setkey_dec(mbedtls_aes_context *, const unsigned char *);
extern int mbedtls_internal_aes_decrypt(mbedtls_aes_context *,
		const unsigned char *,
		unsigned char *);
extern int mbedtls_aes_decrypt_cbc(mbedtls_aes_context *, size_t,
		unsigned char *,
		const unsigned char *, unsigned char *);
#endif
	/*=================================================*/
	/* sha256                                          */
	/*=================================================*/
extern void mbedtls_sha256_init(mbedtls_sha256_context *ctx);
extern void mbedtls_sha256_free(mbedtls_sha256_context *ctx);
extern void mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224);
extern void mbedtls_sha256_process(mbedtls_sha256_context *,
		const unsigned char [64]);
extern void mbedtls_sha256_update(mbedtls_sha256_context *,
		const unsigned char *, size_t ilen);
extern void mbedtls_sha256_finish(mbedtls_sha256_context *, unsigned char [32]);
extern void mbedtls_sha256(const unsigned char *, size_t,
             unsigned char *, int);
extern unsigned int *Decrypt(unsigned int *, unsigned int *,
		unsigned int *, unsigned int);
#endif

const struct nx_bl0_fn bl0fn = {
	&pbm,

	/*=================================================*/
	/* library                                         */
	/*=================================================*/
	printchar,
	prints,
	printi,
	print,
	printf,

	buildinfo,
	DebugInit,

	/* string */
	udelay,
	memcpy,
	memset,
	memcmp,

	/* math */
	__div0,
	__udivmodsi4,
	__aeabi_uidiv,
	__aeabi_idiv,

#ifdef NXP3220
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
#endif

	/*=================================================*/
	/* crypto                                          */
	/*=================================================*/

#ifndef BL1SIMUL
#ifdef NXP3220
	gethash,
	getkeyhash,
	getimagehash,
	aesdecrypt,
#endif
	headercheck,
	keycheck,
	imagedecrypt,
	/*=================================================*/
	/* authentication                                  */
	/*=================================================*/
	nxalloc,
	mbedtls_calloc,
	nxfree,
	mbedtls_free,
	mbedtls_rsa_check_pubkey,
	get_hash,
	mgf_mask,
	mbedtls_rsa_public,
	mbedtls_rsa_rsassa_pss_verify_ext,
	verify_image,
	authenticate_image,

	/*=================================================*/
	/* nand boot                                       */
	/*=================================================*/
#ifdef NXP3220
	nandcontroller_init,
	nandcontroller_deinit,
	nandreset,
	nandread,
#endif
	iNANDBOOTEC,
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
	/* gpt */
	&gpt_crc_table,
	crc32_no_comp,
	crc32,
	is_gpt_valid,
	get_first_lba,
	get_first_empty_lba,
	is_gpt_part,

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
	NX_SDMMC_ReadBootSector,
	eMMCBootRead,
	eMMCBoot_Normal,
	eMMCBoot_Alt,
	SDMMCBOOT,
	NX_SDPADSetALT,
	NX_SDPADSetGPIO,
	iSDXCBOOT,
	ieMMCBOOT,
	/*=================================================*/
	/* sdfs boot                                       */
	/*=================================================*/
	move_window,
	get_cluster,
	clust2sect,
	dir_seek,
	dir_next,
	dir_find,
	create_name,
	follow_path,
	check_fs,
	f_mount,
	validate,
	f_open,
	f_read,
	f_close,

	disk_initialize,
	disk_status,
	disk_read,

	FSBoot,
	SDMMCFSBOOT,
	iSDXCFSBOOT,
	
	/*=================================================*/
	/* spi boot                                        */
	/*=================================================*/
	SPIPortInit,
	SPIPortDeinit,
	SPIInit,
	SPIDeinit,
	read_flash,
	iSPIBOOT,

	/*=================================================*/
	/* uart boot                                        */
	/*=================================================*/
	uartboot_init,
	uartboot_deinit,
	getch,
	putch,
	isrxready,
	&crc16_tab,
	cyg_crc16,
	CYGACC_COMM_IF_GETC_TIMEOUT,
	CYGACC_COMM_IF_PUTC,
	parse_num,
	xyzModem_getc,
	xyzModem_flush,
	xyzModem_get_hdr,
	xyzModem_stream_open,
	xyzModem_stream_read,
	xyzModem_stream_close,
	xyzModem_stream_terminate,
	xyzModem_error,
	iUARTBOOT,

	/*=================================================*/
	&iv,
	&pbootkeyhash,
	iROMBOOT,

	/*=================================================*/
	/* aes cbc                                         */
	/*=================================================*/
#ifdef NXP4330
	&bootkey,
	mbedtls_aes_init,
	mbedtls_aes_free,
	mbedtls_aes_setkey_enc,
	mbedtls_aes_setkey_dec,
	mbedtls_internal_aes_decrypt,
	mbedtls_aes_decrypt_cbc,
#endif
	/*=================================================*/
	/* sha256                                          */
	/*=================================================*/
	mbedtls_sha256_init,
	mbedtls_sha256_free,
	mbedtls_sha256_starts,
	mbedtls_sha256_process,
	mbedtls_sha256_update,
	mbedtls_sha256_finish,
	mbedtls_sha256,
	Decrypt
#endif
};
