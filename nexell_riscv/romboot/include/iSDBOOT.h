/*==============================================================================
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	: bl0
// File		: iSDHCBOOT.h
// Description	:
// Author	: Hans
// History	:
//		2014.01.12 First implementation
==============================================================================*/
#ifndef __ISDHCBOOT_H__
#define __ISDHCBOOT_H__

#include "nx_swallow.h"
#include "nx_sdmmc.h"

#define	SDCLK_DIVIDER_ENUM		(256) //1	/* Internal Clock Enable 0:Stop, 1:Oscillate */
#define	SDCLK_DIVIDER_WORK		(16)	/* Base Clock */
#define SDCLK_DIVIDER_400KHZ      	0
#define SDCLK_DIVIDER_NORMAL            1

#define BLOCK_LENGTH			(512)

#define NX_SDMMC_TIMEOUT		(0x100000)
#define NX_SDMMC_TIMEOUT_IDENTIFY	(0x100000)

#if (0)
	#define INFINTE_LOOP()		{ while (1); }
#else
	#define INFINTE_LOOP()
#endif

#define HEADER_ID                               \
                ((((unsigned int)'N')<< 0) |    \
                 (((unsigned int)'S')<< 8) |    \
                 (((unsigned int)'I')<<16) |    \
                 (((unsigned int)'H')<<24))

/* #define HEADER_ID                               \ */
/*                 ((((unsigned int)'H')<< 0) |    \ */
/*                  (((unsigned int)'I')<< 8) |    \ */
/*                  (((unsigned int)'S')<<16) |    \ */
/*                  (((unsigned int)'N')<<24)) */

/*----------------------------------------------------------------------------*/
#define	NX_SDMMC_STATUS_NOERROR		0
#define	NX_SDMMC_STATUS_ERROR		(1U << 31)
#define	NX_SDMMC_STATUS_CMDBUSY		(NX_SDMMC_STATUS_ERROR | (1U << 0))
#define	NX_SDMMC_STATUS_CMDTOUT		(NX_SDMMC_STATUS_ERROR | (1U << 1))
#define	NX_SDMMC_STATUS_RESCRCFAIL	(NX_SDMMC_STATUS_ERROR | (1U << 2))
#define	NX_SDMMC_STATUS_RESERROR	(NX_SDMMC_STATUS_ERROR | (1U << 3))
#define	NX_SDMMC_STATUS_RESTOUT		(NX_SDMMC_STATUS_ERROR | (1U << 4))
#define NX_SDMMC_STATUS_UNKNOWNCMD	(NX_SDMMC_STATUS_ERROR | (1U << 5))
#define	NX_SDMMC_STATUS_DATABUSY	(NX_SDMMC_STATUS_ERROR | (1U << 6))

/*----------------------------------------------------------------------------*/
#define	NX_SDMMC_RSPIDX_NONE	(0)
#define	NX_SDMMC_RSPIDX_R1	(1)
#define	NX_SDMMC_RSPIDX_R1B	(1 | 0x80)
#define	NX_SDMMC_RSPIDX_R2	(2)
#define	NX_SDMMC_RSPIDX_R3	(3)
#define	NX_SDMMC_RSPIDX_R4	(4)
#define	NX_SDMMC_RSPIDX_R5	(5)
#define	NX_SDMMC_RSPIDX_R6	(6)
#define	NX_SDMMC_RSPIDX_R7	(7)

/*----------------------------------------------------------------------------*/
/* Command */
#define GO_IDLE_STATE		(( 0) | ((NX_SDMMC_RSPIDX_NONE	) << 8))
#define SEND_OP_COND		(( 1) | ((NX_SDMMC_RSPIDX_R3	) << 8))	/* for MMC */
#define ALL_SEND_CID		(( 2) | ((NX_SDMMC_RSPIDX_R2	) << 8))
#define SET_RELATIVE_ADDR	(( 3) | ((NX_SDMMC_RSPIDX_R1	) << 8))	/* for MMC */
#define SEND_RELATIVE_ADDR	(( 3) | ((NX_SDMMC_RSPIDX_R6	) << 8))	/* for SD */
#define	SWITCH_FUNC		(( 6) | ((NX_SDMMC_RSPIDX_R1B	) << 8))	/* for MMC */
#define SELECT_CARD		(( 7) | ((NX_SDMMC_RSPIDX_R1B	) << 8))
#define SEND_IF_COND		(( 8) | ((NX_SDMMC_RSPIDX_R7	) << 8))	/* for SD */
#define SEND_EXT_CSD		(( 8) | ((NX_SDMMC_RSPIDX_R1	) << 8))	/* for MMC */
#define SEND_CSD		(( 9) | ((NX_SDMMC_RSPIDX_R2	) << 8))
#define SEND_CID		((10) | ((NX_SDMMC_RSPIDX_R2	) << 8))
#define STOP_TRANSMISSION	((12) | ((NX_SDMMC_RSPIDX_R1B	) << 8))
#define SEND_STATUS		((13) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define SET_BLOCKLEN		((16) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define READ_SINGLE_BLOCK	((17) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define READ_MULTIPLE_BLOCK	((18) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define WRITE_BLOCK		((24) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define WRITE_MULTIPLE_BLOCK	((25) | ((NX_SDMMC_RSPIDX_R1	) << 8))
#define SELECT_PARTITION	((43) | ((NX_SDMMC_RSPIDX_R1B	) << 8))	/* for eSD */
#define APP_CMD			((55) | ((NX_SDMMC_RSPIDX_R1	) << 8))

/* Application Command */
#define SET_BUS_WIDTH		(( 6) | ((NX_SDMMC_RSPIDX_R1	) << 8) | (APP_CMD << 16))
#define SD_STATUS		((13) | ((NX_SDMMC_RSPIDX_R1	) << 8) | (APP_CMD << 16))
#define	SD_SEND_OP_COND		((41) | ((NX_SDMMC_RSPIDX_R3	) << 8) | (APP_CMD << 16))
#define	SET_CLR_CARD_DETECT	((42) | ((NX_SDMMC_RSPIDX_R1	) << 8) | (APP_CMD << 16))
#define SEND_SCR		((51) | ((NX_SDMMC_RSPIDX_R1	) << 8) | (APP_CMD << 16))


#define MMC_SWITCH_MODE_CMD_SET		0x00 /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS	0x01 /* Set bits in EXT_CSD byte
						 addressed by index which are
						 1 in value field */
#define MMC_SWITCH_MODE_CLEAR_BITS	0x02 /* Clear bits in EXT_CSD byte
						 addressed by index, which are
						 1 in value field */
#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

typedef struct tag_NX_SDMMC_COMMAND
{
	unsigned int	cmdidx;
	unsigned int	arg;
	unsigned int	flag;
	unsigned int	status;
	unsigned int	response[4];
} NX_SDMMC_COMMAND;

/*----------------------------------------------------------------------------*/
typedef enum
{
	NX_SDMMC_CARDTYPE_MMC,
	NX_SDMMC_CARDTYPE_SDMEM,
	NX_SDMMC_CARDTYPE_SDIO,
	NX_SDMMC_CARDTYPE_UNKNOWN
} NX_SDMMC_CARDTYPE;

typedef struct __attribute__ ((aligned(4))) tag_SDBOOTSTATUS
{
	unsigned int		rca;	/* relative card address of device */
	int			bHighCapacity;
	unsigned int		SDPort;
	int			bHighSpeed;
	NX_SDMMC_CARDTYPE	CardType;
} SDBOOTSTATUS;

/*------------------------------------------------------------------------------
// SDXC Module's Register Set
//----------------------------------------------------------------------------*/

#define NX_SDXC_CTRL_USEINDMAC 			(1U << 25)
#define NX_SDXC_CTRL_READWAIT	 		(1U <<  6)
#define NX_SDXC_CTRL_DMAMODE_EN			(1U <<  5)
#define NX_SDXC_CTRL_DMARST			(1U <<  2)
#define NX_SDXC_CTRL_FIFORST			(1U <<  1)
#define NX_SDXC_CTRL_CTRLRST			(1U <<  0)

/*--------------------------------------------------------------------*/
#define NX_SDXC_CLKENA_LOWPWR			(1U << 16)
#define NX_SDXC_CLKENA_CLKENB			(1U <<  0)

/*--------------------------------------------------------------------*/
#define NX_SDXC_STATUS_FIFOCOUNT		(0x1FFFU << 17)
#define NX_SDXC_STATUS_FSMBUSY			(1U << 10)
#define NX_SDXC_STATUS_DATABUSY			(1U <<  9)
#define NX_SDXC_STATUS_FIFOFULL			(1U <<  3)
#define NX_SDXC_STATUS_FIFOEMPTY		(1U <<  2)

/*--------------------------------------------------------------------*/
#define NX_SDXC_CMDFLAG_STARTCMD		(1U << 31)
#define NX_SDXC_CMDFLAG_USE_HOLD_REG		(1U << 29)
#define NX_SDXC_CMDFLAG_VOLT_SWITCH		(1U << 28)
#define NX_SDXC_CMDFLAG_BOOT_MODE		(1U << 27)
#define NX_SDXC_CMDFLAG_DISABLE_BOOT		(1U << 26)
#define NX_SDXC_CMDFLAG_EXPECTBOOTACK		(1U << 25)
#define NX_SDXC_CMDFLAG_ENABLE_BOOT		(1U << 24)
#define NX_SDXC_CMDFLAG_CCS_EXPECTED		(1U << 23)
#define NX_SDXC_CMDFLAG_READCEATADEVICE		(1U << 22)
#define NX_SDXC_CMDFLAG_UPDATECLKONLY		(1U << 21)
#define NX_SDXC_CMDFLAG_SENDINIT		(1U << 15)
#define NX_SDXC_CMDFLAG_STOPABORT		(1U << 14)
#define NX_SDXC_CMDFLAG_WAITPRVDAT		(1U << 13)
#define NX_SDXC_CMDFLAG_SENDAUTOSTOP		(1U << 12)
#define NX_SDXC_CMDFLAG_BLOCK			(0U << 11)
#define NX_SDXC_CMDFLAG_STREAM			(1U << 11)
#define NX_SDXC_CMDFLAG_TXDATA			(3U <<  9)
#define NX_SDXC_CMDFLAG_RXDATA			(1U <<  9)
#define NX_SDXC_CMDFLAG_CHKRSPCRC		(1U <<  8)
#define NX_SDXC_CMDFLAG_SHORTRSP		(1U <<  6)
#define NX_SDXC_CMDFLAG_LONGRSP			(3U <<  6)

/*--------------------------------------------------------------------*/
#define NX_SDXC_RINTSTS_SDIO			(1U << 16)
#define NX_SDXC_RINTSTS_EBE			(1U << 15)
#define NX_SDXC_RINTSTS_ACD			(1U << 14)
#define NX_SDXC_RINTSTS_SBE			(1U << 13)
#define NX_SDXC_RINTSTS_HLE			(1U << 12)
#define NX_SDXC_RINTSTS_FRUN			(1U << 11)
#define NX_SDXC_RINTSTS_HTO			(1U << 10)
#define NX_SDXC_RINTSTS_DRTO			(1U <<  9)
#define NX_SDXC_RINTSTS_RTO			(1U <<  8)
#define NX_SDXC_RINTSTS_DCRC			(1U <<  7)
#define NX_SDXC_RINTSTS_RCRC			(1U <<  6)
#define NX_SDXC_RINTSTS_RXDR			(1U <<  5)
#define NX_SDXC_RINTSTS_TXDR			(1U <<  4)
#define NX_SDXC_RINTSTS_DTO			(1U <<  3)
#define NX_SDXC_RINTSTS_CD			(1U <<  2)
#define NX_SDXC_RINTSTS_RE			(1U <<  1)


int NX_SDMMC_Init(SDBOOTSTATUS *pSDXCBootStatus);
int NX_SDMMC_Terminate(SDBOOTSTATUS *pSDXCBootStatus);
int NX_SDMMC_Open(SDBOOTSTATUS *pSDXCBootStatus, unsigned int option);
int NX_SDMMC_Close(SDBOOTSTATUS *pSDXCBootStatus);
int NX_SDMMC_ReadSectors(SDBOOTSTATUS *pSDXCBootStatus, unsigned int SectorNum,
			unsigned int numberOfSector, unsigned int *pdwBuffer);

void NX_SDPADSetALT(unsigned int PortNum);
void NX_SDPADSetGPIO(unsigned int PortNum);
unsigned int iSDBOOT(unsigned int option);

#endif //__ISDHCBOOT_H__
