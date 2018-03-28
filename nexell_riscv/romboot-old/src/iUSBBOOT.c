////////////////////////////////////////////////////////////////////////////////
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
// File		: iUSBBOOT.c
// Description	:
// Author	: Hans
// History	: 2010.08.03
// 		Hans 2017.10.09 modify for nxp3220
// 		Hans 2016.08.29 modify for nxp5540
// 		Hans 2013.08.24 phy power off add when disconnected
// 		Hans 2013.01.10 create
//
////////////////////////////////////////////////////////////////////////////////

#include "include/nx_swallow.h"
#include "include/nx_swallow_bootoption.h"
#include "include/nx_type.h"


#include "include/nx_usb20otg.h"
#include "iUSBBOOT.h"
#include "include/nx_swallow_bootheader.h"


#include "include/nx_chip_sfr.h"
#include "include/nx_alive.h"
#include "include/nx_cmu.h"

#define NX_ASSERT(x)

#include "include/nx_ecid.h"

#include "libplat.h"
#ifdef QEMU_RISCV
#include "kprintf.h"
#else
#include "printf.h"
#endif
#include "fnptr.h"

//#define DBGMSG pbl0fn->printf
#define DBGMSG(x,...)

struct NX_USB_OTG_RegisterSet * const pUOReg =
  (struct NX_USB_OTG_RegisterSet *)0x40000000;//PHY_BASEADDR_USB20OTG_MODULE_CFG;

const U8 __attribute__ ((aligned(4)))
	gs_DevDescFS[DEVICE_DESCRIPTOR_SIZE] =
{
	18,				//  0 desc size
	(U8)(DESCRIPTORTYPE_DEVICE),	//  1 desc type (DEVICE)
	(U8)(FULL_USB_VER % 0x100),	//  2 USB release
	(U8)(FULL_USB_VER / 0x100),	//  3 => 1.00
	0xFF,				//  4 class
	0xFF,				//  5 subclass
	0xFF,				//  6 protocol
	(U8)FULL_MAX_PKT_SIZE_EP0,	//  7 max pack size
	(U8)(VENDORID % 0x100),		//  8 vendor ID LSB
	(U8)(VENDORID / 0x100),		//  9 vendor ID MSB
	(U8)(PRODUCTID % 0x100),	// 10 product ID LSB (second product)
	(U8)(PRODUCTID / 0x100),	// 11 product ID MSB
	0x00,				// 12 device release LSB
	0x00,				// 13 device release MSB
	0x00,				// 14 manufacturer string desc index
	0x00,				// 15 product string desc index
	0x00,				// 16 serial num string desc index
	0x01				// 17 num of possible configurations
};

const U8 __attribute__ ((aligned(4)))
	gs_DevDescHS[DEVICE_DESCRIPTOR_SIZE] =
{
	18,				//  0 desc size
	(U8)(DESCRIPTORTYPE_DEVICE),	//  1 desc type (DEVICE)
	(U8)(HIGH_USB_VER % 0x100),	//  2 USB release
	(U8)(HIGH_USB_VER / 0x100),	//  3 => 1.00
	0xFF,				//  4 class
	0xFF,				//  5 subclass
	0xFF,				//  6 protocol
	(U8)HIGH_MAX_PKT_SIZE_EP0,	//  7 max pack size
	(U8)(VENDORID	% 0x100),	//  8 vendor ID LSB
	(U8)(VENDORID	/ 0x100),	//  9 vendor ID MSB
	(U8)(PRODUCTID % 0x100),	// 10 product ID LSB	(second product)
	(U8)(PRODUCTID / 0x100),	// 11 product ID MSB
	0x00,				// 12 device release LSB
	0x00,				// 13 device release MSB
	0x00,				// 14 manufacturer string desc index
	0x00,				// 15 product string desc index
	0x00,				// 16 serial num string desc index
	0x01				// 17 num of possible configurations
};


const U8	__attribute__ ((aligned(4)))
	gs_ConfigDescFS[CONFIG_DESCRIPTOR_SIZE] =
{
	//--------------------------------------------------------------------------
	// Configuration Descriptor
	0x09,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_CONFIGURATION),	// [ 1] desc type (CONFIGURATION)
	(U8)(CONFIG_DESCRIPTOR_SIZE % 0x100),	// [ 2] total length of data returned LSB
	(U8)(CONFIG_DESCRIPTOR_SIZE / 0x100),	// [ 3] total length of data returned MSB
	0x01,					// [ 4] num of interfaces
	0x01,					// [ 5] value to select config (1 for now)
	0x00,					// [ 6] index of string desc ( 0 for now)
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,	// [ 7] bus powered
	25,					// [ 8] max power, 50mA for now

	//--------------------------------------------------------------------------
	// Interface Decriptor
	0x09,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_INTERFACE),		// [ 1] desc type (INTERFACE)
	0x00,					// [ 2] interface index.
	0x00,					// [ 3] value for alternate setting
	0x02,					// [ 4] bNumEndpoints (number endpoints used, excluding EP0)
	0xFF,					// [ 5]
	0xFF,					// [ 6]
	0xFF,					// [ 7]
	0x00,					// [ 8] string index,

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 1 Bulk IN)
	0x07,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	BULK_IN_EP|EP_ADDR_IN,			// [ 2] endpoint address: endpoint 1, IN
	EP_ATTR_BULK,				// [ 3] endpoint attributes: Bulk
	(U8)(FULL_MAX_PKT_SIZE_EP1 % 0x100),	// [ 4] max packet size LSB
	(U8)(FULL_MAX_PKT_SIZE_EP1 / 0x100),	// [ 5] max packet size MSB
	0x00,					// [ 6] polling interval (4ms/bit=time,500ms)

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 2 Bulk OUT)
	0x07,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	BULK_OUT_EP|EP_ADDR_OUT,		// [ 2] endpoint address: endpoint 2, OUT
	EP_ATTR_BULK,				// [ 3] endpoint attributes: Bulk
	(U8)(FULL_MAX_PKT_SIZE_EP2 % 0x100),	// [ 4] max packet size LSB
	(U8)(FULL_MAX_PKT_SIZE_EP2 / 0x100),	// [ 5] max packet size MSB
	0x00					// [ 6] polling interval (4ms/bit=time,500ms)
};

const U8	__attribute__ ((aligned(4)))
	gs_ConfigDescHS[CONFIG_DESCRIPTOR_SIZE] =
{
	//--------------------------------------------------------------------------
	// Configuration Descriptor
	0x09,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_CONFIGURATION),	// [ 1] desc type (CONFIGURATION)
	(U8)(CONFIG_DESCRIPTOR_SIZE % 0x100),	// [ 2] total length of data returned LSB
	(U8)(CONFIG_DESCRIPTOR_SIZE / 0x100),	// [ 3] total length of data returned MSB
	0x01,					// [ 4] num of interfaces
	0x01,					// [ 5] value to select config (1 for now)
	0x00,					// [ 6] index of string desc ( 0 for now)
	CONF_ATTR_DEFAULT | CONF_ATTR_SELFPOWERED,// [ 7] bus powered
	25,					// [ 8] max power, 50mA for now

	//--------------------------------------------------------------------------
	// Interface Decriptor
	0x09,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_INTERFACE),		// [ 1] desc type (INTERFACE)
	0x00,					// [ 2] interface index.
	0x00,					// [ 3] value for alternate setting
	0x02,					// [ 4] bNumEndpoints (number endpoints used, excluding EP0)
	0xFF,					// [ 5]
	0xFF,					// [ 6]
	0xFF,					// [ 7]
	0x00,					// [ 8] string index,

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 1 Bulk IN)
	0x07,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	BULK_IN_EP|EP_ADDR_IN,			// [ 2] endpoint address: endpoint 1, IN
	EP_ATTR_BULK,				// [ 3] endpoint attributes: Bulk
	(U8)(HIGH_MAX_PKT_SIZE_EP1 % 0x100),	// [ 4] max packet size LSB
	(U8)(HIGH_MAX_PKT_SIZE_EP1 / 0x100),	// [ 5] max packet size MSB
	0x00,					// [ 6] polling interval (4ms/bit=time,500ms)

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 2 Bulk OUT)
	0x07,					// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	BULK_OUT_EP|EP_ADDR_OUT,		// [ 2] endpoint address: endpoint 2, OUT
	EP_ATTR_BULK,				// [ 3] endpoint attributes: Bulk
	(U8)(HIGH_MAX_PKT_SIZE_EP2 % 0x100),	// [ 4] max packet size LSB
	(U8)(HIGH_MAX_PKT_SIZE_EP2 / 0x100),	// [ 5] max packet size MSB
	0x00					// [ 6] polling interval (4ms/bit=time,500ms)
};


void nx_usb_write_in_fifo(U32 ep, U8 *buf, S32 num)
{
	S32 i;
	U32* dwbuf = (U32*)buf;	/* assume all data ptr is 4 bytes aligned */
	for (i = 0; i < (num + 3) / 4; i++)
		pUOReg->EPFifo[ep][0] = dwbuf[i];
}

void nx_usb_read_out_fifo(U32 ep, U8 *buf, S32 num)
{
	S32 i;
	U32* dwbuf = (U32*)buf;
	for (i = 0; i < (num + 3) / 4; i++)
		dwbuf[i] = pUOReg->EPFifo[ep][0];
}

void nx_usb_ep0_int_hndlr(USBBOOTSTATUS *pUSBBtSts)
{
	U32 buf[2];
	SetupPacket *pSetupPacket = (SetupPacket *)buf;
	U16 addr;

	DBGMSG("Event EP0\r\n");

	if (pUSBBtSts->ep0_state != EP0_STATE_INIT)
		goto noinit;

	buf[0] = pUOReg->EPFifo[CONTROL_EP][0];
	buf[1] = pUOReg->EPFifo[CONTROL_EP][0];

	DBGMSG("Req:%X %X %X %X %X\n", pSetupPacket->bmRequestType,
			pSetupPacket->bRequest,
			pSetupPacket->wValue,
			pSetupPacket->wIndex,
			pSetupPacket->wLength);
	switch (pSetupPacket->bRequest) {
	case STANDARD_SET_ADDRESS:
		/* Set Address Update bit */
		addr = (pSetupPacket->wValue & 0xFF);
		DBGMSG("STANDARD_SET_ADDRESS:%X\n", addr);
		pUOReg->DCSR.DCFG =	1 << 18 |
					addr << 4 |
					pUSBBtSts->speed << 0;
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	case STANDARD_SET_DESCRIPTOR:
		DBGMSG("STANDARD_SET_DESCRIPTOR\r\n");
		break;

	case STANDARD_SET_CONFIGURATION:
		DBGMSG("STANDARD_SET_CONFIGURATION\r\n");
		/* Configuration value in configuration descriptor */
		pUSBBtSts->CurConfig = pSetupPacket->wValue;
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	case STANDARD_GET_CONFIGURATION:
		DBGMSG("STANDARD_GET_CONFIGURATION\r\n");
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ = (1 << 19) | (1 << 0);
		/*ep0 enable, clear nak, next ep0, 8byte */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL = EPEN_CNAK_EP0_8;
		pUOReg->EPFifo[CONTROL_EP][0] = pUSBBtSts->CurConfig;
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	case STANDARD_GET_DESCRIPTOR:
		DBGMSG("STANDARD_GET_DESCRIPTOR :");
		pUSBBtSts->Remain_size = (U32)pSetupPacket->wLength;
		switch (pSetupPacket->wValue >> 8) {
		case DESCRIPTORTYPE_DEVICE:
			pUSBBtSts->Current_ptr =
				(U8*)pUSBBtSts->DevDesc;
			pUSBBtSts->Current_Fifo_Size =
				pUSBBtSts->ctrl_max_pktsize;
			if (pUSBBtSts->Remain_size >
					DEVICE_DESCRIPTOR_SIZE)
				pUSBBtSts->Remain_size =
					DEVICE_DESCRIPTOR_SIZE;
			pUSBBtSts->ep0_state = EP0_STATE_GET_DSCPT;
			break;

		case DESCRIPTORTYPE_CONFIGURATION:
			pUSBBtSts->Current_ptr =
				(U8*)pUSBBtSts->ConfigDesc;
			pUSBBtSts->Current_Fifo_Size =
				pUSBBtSts->ctrl_max_pktsize;
			if (pUSBBtSts->Remain_size >
					CONFIG_DESCRIPTOR_SIZE)
				pUSBBtSts->Remain_size =
					CONFIG_DESCRIPTOR_SIZE;
			pUSBBtSts->ep0_state = EP0_STATE_GET_DSCPT;
			break;

	//	case DESCRIPTORTYPE_STRING :
	//	case DESCRIPTORTYPE_INTERFACE:
	//	case DESCRIPTORTYPE_ENDPOINT:
		default:
			pUOReg->DCSR.DEPIR[0].DIEPCTL |= DEPCTL_STALL;
			break;
		}
		break;

	case STANDARD_CLEAR_FEATURE:
		DBGMSG("STANDARD_CLEAR_FEATURE :");
		break;

	case STANDARD_SET_FEATURE:
		DBGMSG("STANDARD_SET_FEATURE :");
		break;

	case STANDARD_GET_STATUS:
		DBGMSG("STANDARD_GET_STATUS :");
		pUSBBtSts->ep0_state = EP0_STATE_GET_STATUS;
		break;

	case STANDARD_GET_INTERFACE:
		DBGMSG("STANDARD_GET_INTERFACE\r\n");
		pUSBBtSts->ep0_state = EP0_STATE_GET_INTERFACE;
		break;

	case STANDARD_SET_INTERFACE:
		DBGMSG("STANDARD_SET_INTERFACE\r\n");
		pUSBBtSts->CurInterface= pSetupPacket->wValue;
		pUSBBtSts->CurSetting = pSetupPacket->wValue;
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	case STANDARD_SYNCH_FRAME:
		DBGMSG("STANDARD_SYNCH_FRAME\r\n");
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	default:
		break;
	}

noinit:

	pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ =
		(1 << 19) | (pUSBBtSts->ctrl_max_pktsize << 0);

	if (pUSBBtSts->speed == USB_HIGH) {
		/*clear nak, next ep0, 64byte */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
				((1 << 26) | (CONTROL_EP << 11) | (0 << 0));
	} else {
		/*clear nak, next ep0, 8byte */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
				((1 << 26) | (CONTROL_EP << 11) | (3 << 0));
	}
}

void nx_usb_transfer_ep0(USBBOOTSTATUS *pUSBBtSts)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	switch (pUSBBtSts->ep0_state) {
	case EP0_STATE_INIT:
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ = (1 << 19) | (0 << 0);
		/*ep0 enable, clear nak, next ep0, 8byte */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL = EPEN_CNAK_EP0_8;
		DBGMSG("EP0_STATE_INIT\r\n");
		break;

	/* GET_DESCRIPTOR:DEVICE */
	case EP0_STATE_GET_DSCPT:
		DBGMSG("EP0_STATE_GD_DEV_0 :");
		if (pUSBBtSts->speed == USB_HIGH) {
			/*ep0 enable, clear nak, next ep0, max 64byte */
			pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
							EPEN_CNAK_EP0_64;
		} else {
			pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
							EPEN_CNAK_EP0_8;
		}
		if (pUSBBtSts->Current_Fifo_Size >=
				pUSBBtSts->Remain_size)
		{
			pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ =
				(1 << 19) | (pUSBBtSts->Remain_size << 0);
			pbl0fn->nx_usb_write_in_fifo(CONTROL_EP,
					pUSBBtSts->Current_ptr,
					pUSBBtSts->Remain_size);
			pUSBBtSts->ep0_state = EP0_STATE_INIT;
		} else {
			pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ =
				(1 << 19) | (pUSBBtSts->Current_Fifo_Size << 0);
			pbl0fn->nx_usb_write_in_fifo(CONTROL_EP,
					pUSBBtSts->Current_ptr,
					pUSBBtSts->Current_Fifo_Size);
			pUSBBtSts->Remain_size -= pUSBBtSts->Current_Fifo_Size;
			pUSBBtSts->Current_ptr += pUSBBtSts->Current_Fifo_Size;
		}
		break;

	case EP0_STATE_GET_INTERFACE:
	case EP0_STATE_GET_CONFIG:
	case EP0_STATE_GET_STATUS:
		DBGMSG("EP0_STATE_INTERFACE_GET\r\n");
		DBGMSG("EP0_STATE_GET_STATUS\r\n");

		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPTSIZ = (1 << 19) | (1 << 0);
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL = EPEN_CNAK_EP0_8;

		if (pUSBBtSts->ep0_state == EP0_STATE_GET_INTERFACE)
			pUOReg->EPFifo[CONTROL_EP][0] =
				pUSBBtSts->CurInterface;
		else if(pUSBBtSts->ep0_state == EP0_STATE_GET_CONFIG)
			pUOReg->EPFifo[CONTROL_EP][0] =
				pUSBBtSts->CurConfig;
		else
			pUOReg->EPFifo[CONTROL_EP][0] = 0;
		pUSBBtSts->ep0_state = EP0_STATE_INIT;
		break;

	default:
		break;
	}
}


void nx_usb_int_bulkin(USBBOOTSTATUS *pUSBBtSts)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U8* bulkin_buf;
	U32 remain_cnt;

	DBGMSG("Bulk In Function\r\n");
	pbl0fn->udelay(100 * 106);		// any delay need

	bulkin_buf = (U8*)pUSBBtSts->up_ptr;
	remain_cnt = pUSBBtSts->up_size -
			(pUSBBtSts->up_ptr - pUSBBtSts->up_addr);

	if (remain_cnt >= pUSBBtSts->bulkin_max_pktsize) {
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPTSIZ =
			(1 << 19) | (pUSBBtSts->bulkin_max_pktsize << 0);

		/* ep1 enable, clear nak, bulk, usb active, next ep2, max pkt 64 */
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPCTL =
			1u << 31| 1 << 26 | 2 << 18 | 1 << 15 |
			pUSBBtSts->bulkin_max_pktsize << 0;

		pbl0fn->nx_usb_write_in_fifo(BULK_IN_EP, bulkin_buf,
				pUSBBtSts->bulkin_max_pktsize);

		pUSBBtSts->up_ptr += pUSBBtSts->bulkin_max_pktsize;

	} else if (remain_cnt > 0) {
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPTSIZ =
			(1 << 19) | (remain_cnt << 0);

		/* ep1 enable, clear nak, bulk, usb active, next ep2, max pkt 64 */
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPCTL =
			1u << 31 | 1 << 26 | 2 << 18 | 1 << 15 |
			remain_cnt << 0;

		pbl0fn->nx_usb_write_in_fifo(BULK_IN_EP, bulkin_buf, remain_cnt);

		pUSBBtSts->up_ptr += remain_cnt;

	} else { /* remain_cnt = 0 */
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPCTL =
					(DEPCTL_SNAK | DEPCTL_BULK_TYPE);
	}
	if (pUSBBtSts->up_ptr - pUSBBtSts->up_addr == 512)
		pUSBBtSts->up_ptr = pUSBBtSts->up_addr;
}

void nx_usb_int_bulkout(USBBOOTSTATUS *pUSBBtSts,
				U32 fifo_cnt_byte, U32 option)
{
#if 0
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	if (CTRUE != pUSBBtSts->bHeaderReceived) {
		U32 *pdwBuf = (U32*)(*pbl0fn->pbm);
		pbl0fn->nx_usb_read_out_fifo(BULK_OUT_EP,
			(U8 *)&pdwBuf[pUSBBtSts->iRxHeaderSize / 4],
				fifo_cnt_byte);

		if ((fifo_cnt_byte & 3) == 0) {
			pUSBBtSts->iRxHeaderSize += fifo_cnt_byte;
		} else {
			pbl0fn->_dprintf("STALL:size align:%d\r\n", fifo_cnt_byte);
			pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL |= DEPCTL_STALL;
		}

		if ((S32)sizeof(struct nx_bootinfo) <=
				pUSBBtSts->iRxHeaderSize) {
			struct nx_bootinfo *pSBI =
				(struct nx_bootinfo *)pdwBuf;
			pbl0fn->_dprintf("boot header receive done\r\n");
			pUSBBtSts->iRxHeaderSize = 0;
		}
	} else if (CTRUE != pUSBBtSts->bRSAKeyReceived) {
		U32 *pdwBuf = (U32*)&(*pbl0fn->pbm)->rsa_public;

		int readsize = (fifo_cnt_byte > (sizeof(struct asymmetrickey) -
					pUSBBtSts->iRxHeaderSize)) ?
			fifo_cnt_byte - sizeof(struct asymmetrickey) +
			pUSBBtSts->iRxHeaderSize : fifo_cnt_byte;

		pbl0fn->nx_usb_read_out_fifo(BULK_OUT_EP,
			(U8 *)&pdwBuf[pUSBBtSts->iRxHeaderSize / 4],
				readsize);

		if ((fifo_cnt_byte & 3) == 0) {
			pUSBBtSts->iRxHeaderSize += fifo_cnt_byte;
		} else {
			pbl0fn->_dprintf("STALL:size align:%d\r\n", fifo_cnt_byte);
			pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL |= DEPCTL_STALL;
		}
		fifo_cnt_byte -= readsize;

		if ((S32)sizeof(struct asymmetrickey) <=
				pUSBBtSts->iRxHeaderSize) {

			pbl0fn->_dprintf("key receive done\r\n");

			pUSBBtSts->bRSAKeyReceived = CTRUE;
			pUSBBtSts->iRxHeaderSize = 0;

			/* remain 256 byte */
			pbl0fn->nx_usb_read_out_fifo(BULK_OUT_EP,
				(U8 *)pUSBBtSts->RxBuffAddr,
					fifo_cnt_byte);
			pUSBBtSts->RxBuffAddr += fifo_cnt_byte;
			pUSBBtSts->iRxSize -= fifo_cnt_byte;

#if 1
			if (!pbl0fn->keycheck(pUSBBtSts->AESCBC_IV, 1,
						(option >> VERIFY) & 0x3)) {
				pUSBBtSts->bHeaderReceived = CFALSE;
				pUSBBtSts->bRSAKeyReceived = CFALSE;
				pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL |=
					DEPCTL_STALL;
			}
#else
			U32 ehash = (option >> VERIFY) & 0x3;
			if (ehash) {
				U32 bhash[32 / 4];
				pbl0fn->mbedtls_sha256(
					(*pbl0fn->pbm)->rsa_public.rsapublicbootkey,
					256, (unsigned char *)bhash, 0);

				if (ehash == 1)
					ehash--;
				const U32 *pbootkeyhash =
					&(*(*pbl0fn->pbootkeyhash))[ehash][0];
				if (pbl0fn->memcmp(bhash, pbootkeyhash, 32) == 0) {
					pbl0fn->_dprintf("boot key verify passed.\r\n");
				} else {
					pbl0fn->_dprintf("cannot verify boot key\r\n");
				pbl0fn->_dprintf("\r\n%08X %08X %08X %08X\r\n",
						bhash[0], bhash[1], bhash[2], bhash[3]);
				pbl0fn->_dprintf("%08X %08X %08X %08X\r\n",
						bhash[4], bhash[5], bhash[6], bhash[7]);

					pUSBBtSts->bHeaderReceived = CFALSE;
					pUSBBtSts->bRSAKeyReceived = CFALSE;
					pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL |=
						DEPCTL_STALL;
				}
			}
#endif
		}
	} else {
		NX_ASSERT((pUSBBtSts->iRxSize) > 0);
		NX_ASSERT(0 == ((U32)pUSBBtSts->RxBuffAddr & 3));

		pbl0fn->nx_usb_read_out_fifo(BULK_OUT_EP,
				(U8 *)pUSBBtSts->RxBuffAddr,
				fifo_cnt_byte);

		pUSBBtSts->RxBuffAddr += fifo_cnt_byte;
		pUSBBtSts->iRxSize -= fifo_cnt_byte;
#if (0)
		pbl0fn->_dprintf("Bin Packet Size = %x ==> %x\r\n",
				pUSBBtSts->iRxSize,
				pUSBBtSts->RxBuffAddr);
#endif

		if (pUSBBtSts->iRxSize <= 0) {
			if (option & 1 << DECRYPT) {
#if 1
				pbl0fn->imagedecrypt(pUSBBtSts->AESCBC_IV);
#else
				U32 *pdwBuf = (U32*)((*pbl0fn->pbm)->image);
				
				pbl0fn->Decrypt((U32*)pdwBuf, (U32*)pdwBuf,
						pUSBBtSts->AESCBC_IV,
						(*pbl0fn->pbm)->bi.LoadSize);
#endif
			}
			pbl0fn->_dprintf("Download done!\r\n");

			pUSBBtSts->bDownLoading	= CFALSE;
			pUSBBtSts->bHeaderReceived = CFALSE;
			pUSBBtSts->bRSAKeyReceived = CFALSE;
		}
	}

	pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPTSIZ =
		(1 << 19) | (pUSBBtSts->bulkout_max_pktsize << 0);

	/*ep2 enable, clear nak, bulk, usb active, next ep2, max pkt 64*/
	pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL =
		1u << 31 | 1 << 26 | 2 << 18 | 1 << 15 |
		pUSBBtSts->bulkout_max_pktsize << 0;
#endif //if 0        
}

void nx_usb_reset(USBBOOTSTATUS *pUSBBtSts)
{
	U32 i;
	/* set all out ep nak */
	for (i = 0; i < 16; i++)
		pUOReg->DCSR.DEPOR[i].DOEPCTL |= DEPCTL_SNAK;

	pUSBBtSts->ep0_state = EP0_STATE_INIT;
	pUOReg->DCSR.DAINTMSK = ((1 << BULK_OUT_EP) |
				(1 << CONTROL_EP)) << 16 |
				((1 << BULK_IN_EP) |
				 (1 << CONTROL_EP));
	pUOReg->DCSR.DOEPMSK = CTRL_OUT_EP_SETUP_PHASE_DONE |
				AHB_ERROR |
				TRANSFER_DONE;
	pUOReg->DCSR.DIEPMSK = INTKN_TXFEMP |
				NON_ISO_IN_EP_TIMEOUT |
				AHB_ERROR |
				TRANSFER_DONE;

	/* Rx FIFO Size */
	pUOReg->GCSR.GRXFSIZ = RX_FIFO_SIZE;

	/* Non Periodic Tx FIFO Size */
	pUOReg->GCSR.GNPTXFSIZ = NPTX_FIFO_SIZE << 16 |
				NPTX_FIFO_START_ADDR << 0;

	/* clear all out ep nak */
	for (i = 0; i < 16; i++)
		pUOReg->DCSR.DEPOR[i].DOEPCTL |=
			(DEPCTL_EPENA | DEPCTL_CNAK);

	/*clear device address */
	pUOReg->DCSR.DCFG &= ~(0x7F << 4);
}

S32 nx_usb_set_init(USBBOOTSTATUS *pUSBBtSts)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status = pUOReg->DCSR.DSTS; /* System status read */

	pUSBBtSts->bHeaderReceived = CFALSE;
	pUSBBtSts->bRSAKeyReceived = CFALSE;
	pUSBBtSts->iRxHeaderSize = 0;

	/* Set if Device is High speed or Full speed */
	if (((status & 0x6) >> 1) == USB_HIGH) {
		pUSBBtSts->speed = USB_HIGH;
		pbl0fn->_dprintf("High Speed\r\n");
	} else if (((status & 0x6) >> 1) == USB_FULL) {
		pUSBBtSts->speed = USB_FULL;
		pbl0fn->_dprintf("Full Speed\r\n");
	} else {
		pbl0fn->_dprintf("Neither High nor Full\r\n");
		return CFALSE;
	}

	/* set endpoint */
	/* Unmask NX_OTG_DAINT source */
	pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPINT = 0xFF;
	pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPINT = 0xFF;
	pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPINT = 0xFF;
	pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPINT = 0xFF;

	/* Init For Ep0*/
	if (pUSBBtSts->speed == USB_HIGH) {
		pUSBBtSts->ctrl_max_pktsize = HIGH_MAX_PKT_SIZE_EP0;
		pUSBBtSts->bulkin_max_pktsize = HIGH_MAX_PKT_SIZE_EP1;
		pUSBBtSts->bulkout_max_pktsize = HIGH_MAX_PKT_SIZE_EP2;
		pUSBBtSts->DevDesc = pUSBBtSts->HSDevDesc;
		pUSBBtSts->ConfigDesc = *pbl0fn->gs_ConfigDescHS;

		/* MPS:64bytes */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
			((1 << 26) | (CONTROL_EP << 11) | (0 << 0));
		/* ep0 enable, clear nak */
		pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPCTL =
			(1u << 31) | (1 << 26) | (0 << 0);
	} else {
		pUSBBtSts->ctrl_max_pktsize = FULL_MAX_PKT_SIZE_EP0;
		pUSBBtSts->bulkin_max_pktsize = FULL_MAX_PKT_SIZE_EP1;
		pUSBBtSts->bulkout_max_pktsize = FULL_MAX_PKT_SIZE_EP2;
		pUSBBtSts->DevDesc = pUSBBtSts->FSDevDesc;
		pUSBBtSts->ConfigDesc = *pbl0fn->gs_ConfigDescFS;

		/* MPS:8bytes */
		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPCTL =
			((1 << 26) | (CONTROL_EP << 11) | (3 << 0));
		/* ep0 enable, clear nak */
		pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPCTL =
			(1u << 31) | (1 << 26) | (3 << 0);
	}

	/* set_opmode */
	pUOReg->GCSR.GINTMSK = INT_RESUME | INT_OUT_EP |INT_IN_EP |
		INT_ENUMDONE | INT_RESET | INT_SUSPEND | INT_RX_FIFO_NOT_EMPTY;

	pUOReg->GCSR.GAHBCFG = MODE_SLAVE | BURST_SINGLE | GBL_INT_UNMASK;

	pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPTSIZ =
		(1 << 19) | (pUSBBtSts->bulkout_max_pktsize << 0);

	pUOReg->DCSR.DEPOR[BULK_IN_EP].DOEPTSIZ = (1 << 19) | (0 << 0);

	/* bulk out ep enable, clear nak, bulk, usb active, next ep2, max pkt */
	pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPCTL =
		1u << 31 | 1 << 26 | 2 << 18 | 1 << 15 |
		pUSBBtSts->bulkout_max_pktsize << 0;

	/* bulk in ep enable, clear nak, bulk, usb active, next ep1, max pkt */
	pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPCTL =
		0u << 31 | 1 << 26 | 2 << 18 | 1 << 15 |
		pUSBBtSts->bulkin_max_pktsize << 0;

	return CTRUE;
}

void nx_usb_pkt_receive(USBBOOTSTATUS *pUSBBtSts, U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 rx_status;
	U32 fifo_cnt_byte;

	rx_status = pUOReg->GCSR.GRXSTSP;

	if ((rx_status & (0xf << 17)) == SETUP_PKT_RECEIVED) {
		DBGMSG("SETUP_PKT_RECEIVED\r\n");
		pbl0fn->nx_usb_ep0_int_hndlr(pUSBBtSts);
	} else if ((rx_status & (0xf << 17)) == OUT_PKT_RECEIVED) {
		fifo_cnt_byte = (rx_status & 0x7ff0) >> 4;
		DBGMSG("OUT_PKT_RECEIVED\r\n");

		if ((rx_status & BULK_OUT_EP)&&(fifo_cnt_byte)) {
			pbl0fn->nx_usb_int_bulkout(pUSBBtSts,
					fifo_cnt_byte, option);
			pUOReg->GCSR.GINTMSK =
				INT_RESUME | INT_OUT_EP | INT_IN_EP |
				INT_ENUMDONE | INT_RESET | INT_SUSPEND |
				INT_RX_FIFO_NOT_EMPTY;
			return;
		}
	} else if ((rx_status & (0xf << 17)) == GLOBAL_OUT_NAK) {
		DBGMSG("GLOBAL_OUT_NAK\r\n");
	} else if ((rx_status & (0xf << 17)) == OUT_TRNASFER_COMPLETED) {
		DBGMSG("OUT_TRNASFER_COMPLETED\r\n");
	} else if ((rx_status & (0xf << 17)) == SETUP_TRANSACTION_COMPLETED) {
		DBGMSG("SETUP_TRANSACTION_COMPLETED\r\n");
	} else {
		DBGMSG("Reserved\r\n");
	}
}

void nx_usb_transfer(USBBOOTSTATUS *pUSBBtSts)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 ep_int;
	U32 ep_int_status;

	ep_int = pUOReg->DCSR.DAINT;

	if (ep_int & (1 << CONTROL_EP)) {
		ep_int_status = pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPINT;

		if (ep_int_status & INTKN_TXFEMP) {
			while ((pUOReg->GCSR.GNPTXSTS & 0xFFFF) <
					pUSBBtSts->ctrl_max_pktsize)
				pUOReg->GCSR.GNPTXSTS;

			pbl0fn->nx_usb_transfer_ep0(pUSBBtSts);
		}

		pUOReg->DCSR.DEPIR[CONTROL_EP].DIEPINT = ep_int_status;
	}

	if (ep_int & ((1 << CONTROL_EP) << 16)) {
		ep_int_status = pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPINT;

		pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPTSIZ =
				(1 << 29) | (1 << 19) | (8 << 0);
		pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPCTL =
			1u << 31 | 1 << 26; /* ep0 enable, clear nak */

		pUOReg->DCSR.DEPOR[CONTROL_EP].DOEPINT =
			ep_int_status; /* Interrupt Clear */
	}

	if (ep_int & (1 << BULK_IN_EP)) {
		ep_int_status = pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPINT;
		/* Interrupt Clear */
		pUOReg->DCSR.DEPIR[BULK_IN_EP].DIEPINT = ep_int_status;

		if (ep_int_status & INTKN_TXFEMP)
			pbl0fn->nx_usb_int_bulkin(pUSBBtSts);
	}

	if (ep_int & ((1 << BULK_OUT_EP) << 16)) {
		ep_int_status = pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPINT;
		/* Interrupt Clear */
		pUOReg->DCSR.DEPOR[BULK_OUT_EP].DOEPINT = ep_int_status;
	}
}

void nx_udc_int_hndlr(USBBOOTSTATUS *pUSBBtSts, U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 int_status;
	S32 tmp;
	/* Core Interrupt Register */
	int_status = pUOReg->GCSR.GINTSTS;

	if (int_status & INT_RESET) {
		DBGMSG("INT_RESET\r\n");

		pbl0fn->nx_usb_reset(pUSBBtSts);
	}

	if (int_status & INT_ENUMDONE) {
		DBGMSG("INT_ENUMDONE :");

		tmp = pbl0fn->nx_usb_set_init(pUSBBtSts);
		if (tmp == CFALSE) {
			/* Interrupt Clear */
			pUOReg->GCSR.GINTSTS = int_status;
			return;
		}
	}

	if (int_status & INT_RESUME) {
		DBGMSG("INT_RESUME\r\n");
	}

	if (int_status & INT_SUSPEND) {
		DBGMSG("INT_SUSPEND\r\n");
	}

	if (int_status & INT_RX_FIFO_NOT_EMPTY) {
		DBGMSG("INT_RX_FIFO_NOT_EMPTY\r\n");
		/* Read only register field */

		pUOReg->GCSR.GINTMSK =
			INT_RESUME | INT_OUT_EP | INT_IN_EP |
			INT_ENUMDONE | INT_RESET | INT_SUSPEND;
		pbl0fn->nx_usb_pkt_receive(pUSBBtSts, option);
		pUOReg->GCSR.GINTMSK =
			INT_RESUME | INT_OUT_EP | INT_IN_EP |
			INT_ENUMDONE | INT_RESET | INT_SUSPEND |
			INT_RX_FIFO_NOT_EMPTY;
	}

	if ((int_status & INT_IN_EP) || (int_status & INT_OUT_EP)) {
		DBGMSG("INT_IN or OUT_EP\r\n");
		/* Read only register field */

		pbl0fn->nx_usb_transfer(pUSBBtSts);
	}
	pUOReg->GCSR.GINTSTS = int_status; /* Interrupt Clear */
}

struct NX_PBI_RegisterSet * const pPBI =
        (struct NX_PBI_RegisterSet * const)
  (0x41000000/*PHY_BASEADDR_SYSCTRLTOP_MODULE_APB*/ + 0xC400);
struct NX_CMU_RegisterSet * const pCMUUSB =
  (struct NX_CMU_RegisterSet * const)0x50000000;//PHY_BASEADDR_CMU_USB_MODULE;
const struct cmu_device_clk usbclk[3] = {
	{		// ahb
		0x200,		// cmu register offset
		0,		// src bit
		0,		// clkenb bit
		NX_CLKSRC_PLL0,	// clock source
		4		// source clock divid value
	}, {		// sysreg
		0x200,		// cmu register offset
		0,		// src bit
		2,		// clkenb bit
		NX_CLKSRC_PLL0,	// clock source
		4		// source clock divid value
	}, {		// otg20
		0x200,		// cmu register offset
		0,		// src bit
		4,		// clkenb bit
		NX_CLKSRC_PLL0,	// clock source
		4		// source clock divid value
	}
};

extern struct NX_ECID_RegisterSet * const pECIDReg;
CBOOL iUSBBOOT(U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	__attribute__ ((aligned(4))) U8 updata[512];
	USBBOOTSTATUS USBBootStatus, *pUSBBtSts;
	U32 i;
	U8 *ptr;
	U16 VID, PID;

	pUSBBtSts = &USBBootStatus;
	ptr = (U8*)pUSBBtSts;
	pbl0fn->nx_memset(pUSBBtSts, 0, sizeof(USBBOOTSTATUS));

	U32 ID = pECIDReg->ECID[3];
	if (ID == 0) {	/* ECID is not burned */
		VID = VENDORID;
		PID = PRODUCTID;
	} else {
		VID = (ID >> 16) & 0xFFFF;
		PID = (ID >>  0) & 0xFFFF;
	}
	pbl0fn->_dprintf("VID:%04X, PID:%04X\r\n", VID, PID);


	for (i = 0; i < sizeof(USBBOOTSTATUS); i++)
		ptr[i] = 0;
	for (i = 0; i < DEVICE_DESCRIPTOR_SIZE; i++) {
		pUSBBtSts->HSDevDesc[i] = (*pbl0fn->gs_DevDescHS)[i];
		pUSBBtSts->FSDevDesc[i] = (*pbl0fn->gs_DevDescFS)[i];
	}

	pUSBBtSts->HSDevDesc[ 8] = (VID >> 0) & 0xFF;
	pUSBBtSts->HSDevDesc[ 9] = (VID >> 8) & 0xFF;
	pUSBBtSts->HSDevDesc[10] = (PID >> 0) & 0xFF;
	pUSBBtSts->HSDevDesc[11] = (PID >> 8) & 0xFF;
	pUSBBtSts->FSDevDesc[ 8] = (VID >> 0) & 0xFF;
	pUSBBtSts->FSDevDesc[ 9] = (VID >> 8) & 0xFF;
	pUSBBtSts->FSDevDesc[10] = (PID >> 0) & 0xFF;
	pUSBBtSts->FSDevDesc[11] = (PID >> 8) & 0xFF;

	pUSBBtSts->up_addr = updata;
	pUSBBtSts->up_ptr = updata;
	pUSBBtSts->up_size = 512;

	U32 *pdwupdata = (U32*)updata;
	pdwupdata[0] = pECIDReg->ECID[0];
	pdwupdata[1] = pECIDReg->ECID[1];
	pdwupdata[2] = pECIDReg->ECID[2];
	pdwupdata[3] = pECIDReg->ECID[3];
	for (i = 4; i < (512 / 4); i++)
		pdwupdata[i] = 0;


	pPBI->NISOLATE |= 1 << 2;		// power block isolation release
//	pPBI->NPWRUP |= 1 << 2;			// power up
	pbl0fn->udelay(1000);			// 1 ms
#if 1
        //	pbl0fn->setdeviceclock(usbclk, 3, 1);
#else
	pCMUUSB->CMUBLK[0].CLKMUXSEL = NX_CLKSR_OSC;	// osc in
	pCMUUSB->CMUBLK[0].CCLKENB[0] = 1 << 0;		// cpu usb group clock disable
	pCMUUSB->CMUSRC.CSRCOFF[0] = 1 << 0;		// src off clr
	pCMUUSB->CMUBLK[0].DIV[0] = 3;			// 1000MHz / 4 = 250MHz
	pCMUUSB->CMUBLK[0].CLKMUXSEL = NX_CLKSR_PLL0;	// pll0
	pCMUUSB->CMUBLK[0].SCLKENB[0] = 1 << 0;		// cpu usb group clock enable

	pCMUUSB->CMUBLK[0].SRST[0] = 1 << 0;		// cmu usb ahb reset negate
	pCMUUSB->CMUBLK[0].SRST[0] = 1 << 2;		// cmu usb sysreg reset negate
	pCMUUSB->CMUBLK[0].SRST[0] = 1 << 4;		// cmu usbotg ahb reset negate

	pbl0fn->udelay(1);			// need 16 usb clock
				// 250MHz 16 clk = 4n * 16 = 64ns about 1us

	volatile U32 *SYS_USB = (U32 *)0x60000000;//PHY_BASEADDR_SYSREG_USB_MODULE;

	SYS_USB[0x80 / 4] &= 1 << 4;            // POR
	SYS_USB[0x80 / 4] |= 1 << 3;            // POR_ENB

	pbl0fn->udelay(40);			// 40 us

	SYS_USB[0x60 / 4] |= 1 << 8;		// nUtmiResetSync

	pbl0fn->udelay(1);

	SYS_USB[0x60 / 4] |= 1 << 7;		// nResetSync

	pbl0fn->udelay(1);
#endif
	/* usb core soft reset */
	pUOReg->GCSR.GRSTCTL = CORE_SOFT_RESET;
	while (!(pUOReg->GCSR.GRSTCTL & AHB_MASTER_IDLE))
		pUOReg->GCSR.GRSTCTL;

	/* init_core */
	pUOReg->GCSR.GAHBCFG = PTXFE_HALF | NPTXFE_HALF | MODE_SLAVE |
				BURST_SINGLE | GBL_INT_UNMASK;
	pUOReg->GCSR.GUSBCFG =
		  0 << 15	/* PHY Low Power Clock sel */
		| 1 << 14	/* Non-Periodic TxFIFO Rewind Enable */
		| 5 << 10	/* Turnaround time */
		| 0 << 9	/* 0:HNP disable, 1:HNP enable */
		| 0 << 8	/* 0:SRP disable, 1:SRP enable */
		| 0 << 7	/* ULPI DDR sel */
		| 0 << 6	/* 0: high speed utmi+, 1: full speed serial */
		| 0 << 4	/* 0: utmi+, 1:ulpi */
		| 1 << 3	/* phy i/f  0:8bit, 1:16bit */
		| 7 << 0;	/* HS/FS Timeout**/

	/* check current mode */
	if ((pUOReg->GCSR.GINTSTS & 0x1) == INT_DEV_MODE) {
		/* soft disconnect on */
		pUOReg->DCSR.DCTL |= SOFT_DISCONNECT;

		pbl0fn->udelay(10 * 106);
		/* soft disconnect off */
		pUOReg->DCSR.DCTL &= ~SOFT_DISCONNECT;

		/* usb init device */
		pUOReg->DCSR.DCFG = 1 << 18;
		pUOReg->GCSR.GINTMSK = INT_RESUME | INT_OUT_EP |
					INT_IN_EP | INT_ENUMDONE | INT_RESET |
					INT_SUSPEND | INT_RX_FIFO_NOT_EMPTY;
	}

	pUSBBtSts->CurConfig = 0;
	pUSBBtSts->CurInterface = 0;
	pUSBBtSts->CurSetting = 0;
	pUSBBtSts->speed = USB_HIGH;
	pUSBBtSts->ep0_state = EP0_STATE_INIT;

	pbl0fn->_dprintf("iRom usb boot ready!\r\n");

	pUSBBtSts->bDownLoading = CTRUE;
	while (pUSBBtSts->bDownLoading) {
		if (pUOReg->GCSR.GINTSTS &
				(WkUpInt | OEPInt | IEPInt | EnumDone |
				 USBRst | USBSusp | RXFLvl)) {
			pbl0fn->nx_udc_int_hndlr(pUSBBtSts, option);
			pUOReg->GCSR.GINTSTS = 0xFFFFFFFF;
		}
	}
	/* usb core soft reset */
	pUOReg->GCSR.GRSTCTL = CORE_SOFT_RESET;
	while (!(pUOReg->GCSR.GRSTCTL & AHB_MASTER_IDLE))
		pUOReg->GCSR.GRSTCTL;


	 /* reset Avalid, Bvalid, sessend */
	pUOReg->GCSR.GOTGCTL = 0x00;

	/* usb core soft reset */
	pUOReg->GCSR.GRSTCTL = CORE_SOFT_RESET;
#if 0
	SYS_USB[0x60 / 4] &= ~(1 << 8);		// nUtmiResetSync
	SYS_USB[0x60 / 4] &= ~(1 << 7);		// nResetSync

	SYS_USB[0x80 / 4] |= 1 << 4;		// POR
#endif
	pCMUUSB->CMUBLK[0].CRST[0] = 1 << 0;	// cmu usb ahb reset assert
	pCMUUSB->CMUBLK[0].CRST[0] = 1 << 2;	// cmu usb sysreg reset assert
	pCMUUSB->CMUBLK[0].CRST[0] = 1 << 4;	// cmu usbotg ahb reset assert
#if 1
        //	pbl0fn->setdeviceclock(usbclk, 1, 1);
#else
	pCMUUSB->CMUSRC.SSRCOFF[0] = 1 << 0;	// src off set
	pCMUUSB->CMUBLK[0].CCLKENB[0] = 1 << 0;	// cpu usb group clock disable
#endif

//	pbl0fn->_dprintf("download done.\r\n");

	return CTRUE;
}
