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
//
// Module	:
// File		:
// Description	:
// Author	: Hans
// History	: 2013.02.06 First implementation
//		2013.08.31 rev1 (port 0, 1, 2 selectable)
//		2017.10.09 rev0 for nxp3220
////////////////////////////////////////////////////////////////////////////////
#include "nx_antares.h"
#include <nx_type.h>
#include "nx_antares_bootoption.h"
#include "nx_antares_bootheader.h"
#include "printf.h"


#include <nx_gpio.h>

#include <nx_sdmmc.h>

#include "iSDHCBOOT.h"

#ifdef NXP3220
#include <nx_chip_iomux.h>
#include <nx_chip_sfr.h>
#include <nx_cmu.h>

#define NX_ASSERT(x)
#endif

#include "libarm.h"
#include "libplat.h"
#include "crypto.h"
#include "fnptr.h"

#ifdef NXP4330
#include <nx_chip.h>
#include <nx_clkgen.h>

struct NX_CLKGEN_RegisterSet * const pgSDClkGenReg[3] =
{
	(struct NX_CLKGEN_RegisterSet *)PHY_BASEADDR_CLKGEN18_MODULE,
	(struct NX_CLKGEN_RegisterSet *)PHY_BASEADDR_CLKGEN19_MODULE,
	(struct NX_CLKGEN_RegisterSet *)PHY_BASEADDR_CLKGEN20_MODULE
};
U32 const SDResetNum[3] =
{
	RESETINDEX_OF_SDMMC0_MODULE_i_nRST,
	RESETINDEX_OF_SDMMC1_MODULE_i_nRST,
	RESETINDEX_OF_SDMMC2_MODULE_i_nRST
};

extern struct NX_GPIO_RegisterSet (* const pGPIOReg)[1];
#endif
#ifdef NXP3220
//const struct cmu_device_clk sdmmcclk[3][2] = {
const struct cmu_device_clk sdmmcclk[3][3] = {
	{
		{0x0400,  1, 2, NX_CLKSRC_PLL0, 1},	// ahb
		{0x3000, 23, 0, NX_CLKSRC_PLL0, 1},	// core
		{0x3000, 23, 0, NX_CLKSRC_PLL0, 250}	// core
	}, {
		{0x0400,  1, 3, NX_CLKSRC_PLL0, 1},	// ahb
		{0x3200, 24, 0, NX_CLKSRC_PLL0, 1},	// core
		{0x3200, 24, 0, NX_CLKSRC_PLL0, 250}	// core
	}, {
		{0x0400,  1, 4, NX_CLKSRC_PLL0, 1},	// ahb
		{0x3400, 25, 0, NX_CLKSRC_PLL0, 1},	// core
		{0x3400, 25, 0, NX_CLKSRC_PLL0, 250}	// core
	}
};
#endif

struct NX_SDMMC_RegisterSet * const pgSDXCReg[3] =
{
	(struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC0_MODULE,
	(struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC1_MODULE,
	(struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC2_MODULE
};


//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetClock(SDXCBOOTSTATUS *pSDXCBootStatus,
				CBOOL enb, U32 divider)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	volatile U32 timeout;
	U32 i = pSDXCBootStatus->SDPort;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[i];
#ifdef NXP4330
	register struct NX_CLKGEN_RegisterSet * const pSDClkGenReg =
					pgSDClkGenReg[i];
#endif


	#if defined(VERBOSE)
	pbl0fn->printf("divider = %d\r\n", divider);
	#endif

	//--------------------------------------------------------------------------
	// 1. Confirm that no card is engaged in any transaction.
	//	If there's a transaction, wait until it has been finished.
//	while(NX_SDXC_IsDataTransferBusy())
//		;
//	while(NX_SDXC_IsCardDataBusy())
//		;

	#if defined(NX_DEBUG)
	if (pSDXCReg->STATUS & (NX_SDXC_STATUS_DATABUSY |
				NX_SDXC_STATUS_FSMBUSY)) {
		#if 1//defined(NX_DEBUG)
		if (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY)
			pbl0fn->printf("%s : ERROR - Data is busy\r\n", __func__);

		if (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY)
			pbl0fn->printf("%s : ERROR - Data Transfer is busy\n", __func__);
		#endif
		//return CFALSE;
		INFINTE_LOOP();
	}
	#endif

	//--------------------------------------------------------------------------
	// 2. Disable the output clock.
	// low power mode & clock disable
	pSDXCReg->CLKENA |= NX_SDXC_CLKENA_LOWPWR;
	pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_CLKENB;

#ifdef NXP4330
	pSDClkGenReg->CLKENB = NX_PCLKMODE_ALWAYS << 3 |
				NX_BCLKMODE_DYNAMIC << 0;
	pSDClkGenReg->CLKGEN[0] =
			(pSDClkGenReg->CLKGEN[0] & ~(0x7 << 2 | 0xFF << 5))
			| (SDXC_CLKSRC << 2)		// set clock source
			| ((divider - 1) << 5)	// set clock divisor
			| (0UL << 1);			// set clock invert

	//--------------------------------------------------------------------------
	// 3. Program the clock divider as required.
	if (divider == SDXC_DIVIDER_400KHZ)
		pSDXCReg->CLKSRC = 0;

	if (divider == SDXC_CLKDIV_LOW)
		pSDXCReg->CLKSRC = 1;

	pSDClkGenReg->CLKENB |= 0x1UL << 2;		// clock generation enable
	
#endif

#ifdef NXP3220
	if (divider == SDXC_DIVIDER_400KHZ)
		pbl0fn->setdeviceclock(&sdmmcclk[i][2], 1, 1);
	if (divider == SDXC_CLKDIV_LOW)
		pbl0fn->setdeviceclock(&sdmmcclk[i][1], 1, 1);
#endif

	pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_LOWPWR;	// low power mode & clock disable


	//--------------------------------------------------------------------------
	// 4. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
repeat_4 :
	pSDXCReg->CMD = 0 | NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_UPDATECLKONLY |
			NX_SDXC_CMDFLAG_STOPABORT;

	//--------------------------------------------------------------------------
	// 5. Wait until a update clock command is taken by the SDXC module.
	//	If a HLE is occurred, repeat 4.
	timeout = 0;
	while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
		if (++timeout > NX_SDMMC_TIMEOUT) {
			pbl0fn->printf("clk TO\r\n");
			INFINTE_LOOP();
			return CFALSE;
		}
	}

	if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
		INFINTE_LOOP();
		pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
		goto repeat_4;
	}

	if (CFALSE == enb)
		return CTRUE;

	//--------------------------------------------------------------------------
	// 6. Enable the output clock.
	pSDXCReg->CLKENA |= NX_SDXC_CLKENA_CLKENB;

	//--------------------------------------------------------------------------
	// 7. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
repeat_7 :
	pSDXCReg->CMD = 0 | NX_SDXC_CMDFLAG_STARTCMD |
		NX_SDXC_CMDFLAG_UPDATECLKONLY |
		NX_SDXC_CMDFLAG_STOPABORT;

	//--------------------------------------------------------------------------
	// 8. Wait until a update clock command is taken by the SDXC module.
	//	If a HLE is occurred, repeat 7.
	timeout = 0;
	while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
		if (++timeout > NX_SDMMC_TIMEOUT) {
			pbl0fn->printf("clk2 TO\r\n");
			INFINTE_LOOP();
			return CFALSE;
		}
	}

	if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
		INFINTE_LOOP();
		pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
		goto repeat_7;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommandInternal(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		NX_SDMMC_COMMAND *pCommand)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32		cmd, flag;
	U32		status = 0;
	volatile U32	timeout;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];

	NX_ASSERT(CNULL != pCommand);

	#ifdef VERBOSE
	pbl0fn->printf("%s : Command(%Xh), Argument(%Xh)\n",
			__func__, pCommand->cmdidx, pCommand->arg);
	#endif

	cmd	= pCommand->cmdidx & 0xFF;
	flag	= pCommand->flag;

	pSDXCReg->RINTSTS = 0xFFFFFFFF;

	//--------------------------------------------------------------------------
	// Send Command
	timeout = 0;
	do {
		pSDXCReg->RINTSTS	= NX_SDXC_RINTSTS_HLE;
		pSDXCReg->CMDARG	= pCommand->arg;
		pSDXCReg->CMD		= cmd | flag |
					NX_SDXC_CMDFLAG_USE_HOLD_REG;
		while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
			if (++timeout > NX_SDMMC_TIMEOUT) {
				pbl0fn->printf("TO send cmd\r\n");
				status |= NX_SDMMC_STATUS_CMDBUSY;
				INFINTE_LOOP();
				goto End;
			}
		}
	} while (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE);

	//--------------------------------------------------------------------------
	// Wait until Command sent to card and got response from card.
	timeout = 0;
	while (1) {
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_CD)
			break;

		if (++timeout > NX_SDMMC_TIMEOUT) {
			pbl0fn->printf("TO cmd done\r\n");
			status |= NX_SDMMC_STATUS_CMDTOUT;
			INFINTE_LOOP();
			goto End;
		}

		if ((flag & NX_SDXC_CMDFLAG_STOPABORT) &&
			(pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO)) {
			// You have to clear FIFO when HTO is occurred.
			// After that, SDXC module leaves in stopped state and takes next command.
			while (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
				pSDXCReg->DATA;
			}
		}
	}

	// Check Response Error
	if (pSDXCReg->RINTSTS & (NX_SDXC_RINTSTS_RCRC |
				NX_SDXC_RINTSTS_RE |
				NX_SDXC_RINTSTS_RTO)) {
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RCRC)
			status |= NX_SDMMC_STATUS_RESCRCFAIL;
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RE)
			status |= NX_SDMMC_STATUS_RESERROR;
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RTO)
			status |= NX_SDMMC_STATUS_RESTOUT;
	}

	if ((status == NX_SDMMC_STATUS_NOERROR) &&
		(flag & NX_SDXC_CMDFLAG_SHORTRSP)) {
		pCommand->response[0] = pSDXCReg->RESP[0];
		if ((flag & NX_SDXC_CMDFLAG_LONGRSP) ==
				NX_SDXC_CMDFLAG_LONGRSP) {
			pCommand->response[1] = pSDXCReg->RESP[1];
			pCommand->response[2] = pSDXCReg->RESP[2];
			pCommand->response[3] = pSDXCReg->RESP[3];
		}

		if (NX_SDMMC_RSPIDX_R1B == ((pCommand->cmdidx >> 8) & 0xFF)) {
			timeout = 0;
			do {
				if (++timeout > NX_SDMMC_TIMEOUT) {
					pbl0fn->printf("TO card data ready\r\n");
					status |= NX_SDMMC_STATUS_DATABUSY;
					INFINTE_LOOP();
					goto End;
				}
			} while (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY);
		}
	}

End:

	if (NX_SDMMC_STATUS_NOERROR != status) {
//		pbl0fn->printf("err: cmd:%x, arg:%x => sts:%x, resp:%x\r\n",
//			pCommand->cmdidx, pCommand->arg,
//			status, pCommand->response[0]);
	}

	pCommand->status = status;

	return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendStatus(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	cmd.cmdidx	= SEND_STATUS;
	cmd.arg		= pSDXCBootStatus->rca;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);

	#if defined(VERBOSE) && defined(NX_DEBUG) && (1)
	if (NX_SDMMC_STATUS_NOERROR == status)
		return status;

//	pbl0fn->printf("err: cmd.resp:%x\r\n", cmd.response[0]);

	if (cmd.response[0] & (1UL << 31))
		pbl0fn->printf("\t\t ERROR : OUT_OF_RANGE\n");
	if (cmd.response[0] & (1UL << 30))
		pbl0fn->printf("\t\t ERROR : ADDRESS_ERROR\n");
	if (cmd.response[0] & (1UL << 29))
		pbl0fn->printf("\t\t ERROR : BLOCK_LEN_ERROR\n");
	if (cmd.response[0] & (1UL << 28))
		pbl0fn->printf("\t\t ERROR : ERASE_SEQ_ERROR\n");
	if (cmd.response[0] & (1UL << 27))
		pbl0fn->printf("\t\t ERROR : ERASE_PARAM\n");
	if (cmd.response[0] & (1UL << 26))
		pbl0fn->printf("\t\t ERROR : WP_VIOLATION\n");
	if (cmd.response[0] & (1UL << 24))
		pbl0fn->printf("\t\t ERROR : LOCK_UNLOCK_FAILED\n");
	if (cmd.response[0] & (1UL << 23))
		pbl0fn->printf("\t\t ERROR : COM_CRC_ERROR\n");
	if (cmd.response[0] & (1UL << 22))
		pbl0fn->printf("\t\t ERROR : ILLEGAL_COMMAND\n");
	if (cmd.response[0] & (1UL << 21))
		pbl0fn->printf("\t\t ERROR : CARD_ECC_FAILED\n");
	if (cmd.response[0] & (1UL << 20))
		pbl0fn->printf("\t\t ERROR : Internal Card Controller ERROR\n");
	if (cmd.response[0] & (1UL << 19))
		pbl0fn->printf("\t\t ERROR : General Error\n");
	if (cmd.response[0] & (1UL << 17))
		pbl0fn->printf("\t\t ERROR : Deferred Response\n");
	if (cmd.response[0] & (1UL << 16))
		pbl0fn->printf("\t\t ERROR : CID/CSD_OVERWRITE_ERROR\n");
	if (cmd.response[0] & (1UL << 15))
		pbl0fn->printf("\t\t ERROR : WP_ERASE_SKIP\n");
	if (cmd.response[0] & (1UL << 3))
		pbl0fn->printf("\t\t ERROR : AKE_SEQ_ERROR\n");

	switch ((cmd.response[0] >> 9) & 0xF) {
	case 0: pbl0fn->printf("\t\t CURRENT_STATE : Idle\n");
		break;
	case 1: pbl0fn->printf("\t\t CURRENT_STATE : Ready\n");
		break;
	case 2: pbl0fn->printf("\t\t CURRENT_STATE : Identification\n");
		break;
	case 3: pbl0fn->printf("\t\t CURRENT_STATE : Standby\n");
		break;
	case 4: pbl0fn->printf("\t\t CURRENT_STATE : Transfer\n");
		break;
	case 5: pbl0fn->printf("\t\t CURRENT_STATE : Data\n");
		break;
	case 6: pbl0fn->printf("\t\t CURRENT_STATE : Receive\n");
		break;
	case 7: pbl0fn->printf("\t\t CURRENT_STATE : Programming\n");
		break;
	case 8: pbl0fn->printf("\t\t CURRENT_STATE : Disconnect\n");
		break;
	case 9: pbl0fn->printf("\t\t CURRENT_STATE : Sleep\n");
		break;
	default: pbl0fn->printf("\t\t CURRENT_STATE : Reserved\n");
		 break;
	}
	#endif

	return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommand(SDXCBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;

	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, pCommand);
	if (NX_SDMMC_STATUS_NOERROR != status) {
		pbl0fn->NX_SDMMC_SendStatus(pSDXCBootStatus);
	}

	return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendAppCommand(SDXCBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	cmd.cmdidx	= APP_CMD;
	cmd.arg		= pSDXCBootStatus->rca;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);
	if (NX_SDMMC_STATUS_NOERROR == status) {
		pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, pCommand);
	}

	return status;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_IdentifyCard(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	S32 timeout;
	U32 HCS, RCA;
	NX_SDMMC_CARDTYPE CardType = NX_SDMMC_CARDTYPE_UNKNOWN;
	NX_SDMMC_COMMAND cmd;
	struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus,
					CTRUE,
					SDXC_DIVIDER_400KHZ))
		return CFALSE;

	// Data Bus Width : 0(1-bit), 1(4-bit)
	pSDXCReg->CTYPE = 0;

	pSDXCBootStatus->rca = 0;

	//--------------------------------------------------------------------------
	//	Identify SD/MMC card
	//--------------------------------------------------------------------------
	// Go idle state
	cmd.cmdidx	= GO_IDLE_STATE;
	cmd.arg		= 0;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_SENDINIT |
			NX_SDXC_CMDFLAG_STOPABORT;

	pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

	// Check SD Card Version
	cmd.cmdidx	= SEND_IF_COND;
	// argument = VHS : 2.7~3.6V and Check Pattern(0xAA)
	cmd.arg		= (1 << 8) | 0xAA;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	if (NX_SDMMC_STATUS_NOERROR ==
			pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
		// Ver 2.0 or later SD Memory Card
		if (cmd.response[0] != ((1 << 8) | 0xAA))
			return CFALSE;

		HCS = 1 << 30;
	} else {
		// voltage mismatch or Ver 1.X SD Memory Card or not SD Memory Card
		HCS = 0;
	}

	//--------------------------------------------------------------------------
	// voltage validation
	timeout = NX_SDMMC_TIMEOUT_IDENTIFY;

	cmd.cmdidx	= APP_CMD;
	cmd.arg		= pSDXCBootStatus->rca;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	if (NX_SDMMC_STATUS_NOERROR ==
			pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
		//----------------------------------------------------------------------
		// SD memory card
		#define FAST_BOOT	(1<<29)

		cmd.cmdidx	= SD_SEND_OP_COND;
		cmd.arg		= (HCS | FAST_BOOT | 0x00FC0000);	// 3.0 ~ 3.6V
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_SHORTRSP;

		if (NX_SDMMC_STATUS_NOERROR !=
				pbl0fn->NX_SDMMC_SendCommandInternal(
					pSDXCBootStatus, &cmd)) {
			return CFALSE;
		}
		/* Wait until card has finished the power up routine */
		while (0==(cmd.response[0] & (1UL << 31))) {
			if (NX_SDMMC_STATUS_NOERROR !=
					pbl0fn->NX_SDMMC_SendAppCommand(
						pSDXCBootStatus, &cmd)) {
				return CFALSE;
			}

			if (timeout-- <= 0) {
				pbl0fn->printf("TO pwrup SD\r\n");
				return CFALSE;
			}
		}

		pbl0fn->printf("SD ");

		CardType	= NX_SDMMC_CARDTYPE_SDMEM;
		RCA		= 0;
	} else {
		//----------------------------------------------------------------------
		// MMC memory card
		cmd.cmdidx	= GO_IDLE_STATE;
		cmd.arg		= 0;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_SENDINIT |
				NX_SDXC_CMDFLAG_STOPABORT;

		pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

		do {
			cmd.cmdidx	= SEND_OP_COND;
			cmd.arg		= 0x40FC0000;	// MMC High Capacity -_-???
			cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
					NX_SDXC_CMDFLAG_WAITPRVDAT |
					NX_SDXC_CMDFLAG_SHORTRSP;
			if (NX_SDMMC_STATUS_NOERROR !=
					pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus,
								&cmd)) {
				return CFALSE;
			}

			if (timeout-- <= 0) {
				pbl0fn->printf("TO to wait pow-up for MMC\r\n");
				return CFALSE;
			}
		/* Wait until card has finished the power up routine */
		} while (0==(cmd.response[0] & (1UL << 31)));

		pbl0fn->printf("MMC ");
		#if defined(VERBOSE)
		pbl0fn->printf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]);
		#endif

		CardType	= NX_SDMMC_CARDTYPE_MMC;
		RCA		= 2 << 16;
	}

//	NX_ASSERT( (cmd.response[0] & 0x00FC0000) == 0x00FC0000 );
	pSDXCBootStatus->bHighCapacity =
		(cmd.response[0] & (1 << 30)) ? CTRUE : CFALSE;

	pbl0fn->printf("%sCard.\r\n",
			(pSDXCBootStatus->bHighCapacity) ? "High Capacity " : "");

	//--------------------------------------------------------------------------
	// Get CID
	cmd.cmdidx	= ALL_SEND_CID;
	cmd.arg		= 0;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_LONGRSP;
	if (NX_SDMMC_STATUS_NOERROR !=
			pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd)) {
		pbl0fn->printf("cannot read CID\r\n");
		return CFALSE;
	}

	//--------------------------------------------------------------------------
	// Get RCA and change to Stand-by State in data transfer mode
	cmd.cmdidx	= (CardType == NX_SDMMC_CARDTYPE_SDMEM) ?
					SEND_RELATIVE_ADDR : SET_RELATIVE_ADDR;
	cmd.arg		= RCA;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;
	if (NX_SDMMC_STATUS_NOERROR !=
			pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd)) {
		pbl0fn->printf("cannot read RCA\r\n");
		return CFALSE;
	}

	if (CardType == NX_SDMMC_CARDTYPE_SDMEM)
		pSDXCBootStatus->rca = cmd.response[0] & 0xFFFF0000;
	else
		pSDXCBootStatus->rca = RCA;

	pSDXCBootStatus->CardType = CardType;

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectCard(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	cmd.cmdidx	= SELECT_CARD;
	cmd.arg		= pSDXCBootStatus->rca;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	status = pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

	return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetCardDetectPullUp(SDXCBOOTSTATUS *pSDXCBootStatus,
					CBOOL bEnb)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	cmd.cmdidx	= SET_CLR_CARD_DETECT;
	cmd.arg		= (bEnb) ? 1 : 0;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	status = pbl0fn->NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);

	return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBusWidth(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 buswidth )
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	NX_ASSERT( buswidth==1 || buswidth==4 );

	if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
		cmd.cmdidx	= SET_BUS_WIDTH;
		cmd.arg		= (buswidth >> 1);
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP;
		status = pbl0fn->NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);
	} else {
		/* ExtCSD[183] : BUS_WIDTH <= 0 : 1-bit, 1 : 4-bit, 2 : 8-bit */
		cmd.cmdidx	= SWITCH_FUNC;
		cmd.arg		=		  (3 << 24) |
						(183 << 16) |
				    ((buswidth >> 2) << 8) |
						  (0 << 0);
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP;
		status = pbl0fn->NX_SDMMC_SendCommand( pSDXCBootStatus, &cmd );
	}

	if (NX_SDMMC_STATUS_NOERROR != status)
		return CFALSE;

	/* 0 : 1-bit mode, 1 : 4-bit mode */
	pSDXCReg->CTYPE = buswidth >> 2;
#ifdef NXP3220
	pSDXCReg->TIEMODE = buswidth >> 2;
#endif

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBlockLength(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 blocklength)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	cmd.cmdidx	= SET_BLOCKLEN;
	cmd.arg		= blocklength;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;
	status = pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

	if (NX_SDMMC_STATUS_NOERROR != status)
		return CFALSE;

	pSDXCReg->BLKSIZ = blocklength;

	return CTRUE;
}

#if 0
//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectPartition(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 partition)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	#define IMMEDIATE_RESPONSE		(1 <<  0)
	#define DEFERRED_RESPONSE		(1 << 17)

	if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
		NX_ASSERT( partition < 256 );

		cmd.cmdidx	= SELECT_PARTITION;
		cmd.arg		= (partition << 24) | IMMEDIATE_RESPONSE;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP;
	} else {
		NX_ASSERT(partition == 1 || partition == 2);

		/* Set Bits : BOOT_CONFIG[1:0] = 1 or 2	// R/W Boot Partition */
		cmd.cmdidx	= SWITCH_FUNC;
		cmd.arg		=	  (1 << 24) |
					(179 << 16) |
				  (partition << 8) |
					  (0 << 0);
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP;
	}

	status = pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

	if (NX_SDMMC_STATUS_NOERROR != status) {
		return CFALSE;
	}

	if (cmd.response[0] & DEFERRED_RESPONSE) {
		status = pbl0fn->NX_SDMMC_SendStatus(pSDXCBootStatus);
		if (NX_SDMMC_STATUS_NOERROR != status)
			return CFALSE;
	}

	return CTRUE;
}
#endif

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Init(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	U32 i = pSDXCBootStatus->SDPort;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[i];
#ifdef NXP4330
 	register struct NX_CLKGEN_RegisterSet * const pSDClkGenReg =
					pgSDClkGenReg[i];

 	pSDClkGenReg->CLKENB = NX_PCLKMODE_ALWAYS << 3 | NX_BCLKMODE_DYNAMIC << 0;
 	pSDClkGenReg->CLKGEN[0] =
			(pSDClkGenReg->CLKGEN[0] & ~(0x7 << 2 | 0xFF << 5))
			  | ((SDXC_DIVIDER_400KHZ - 1) << 5)// set clock divisor
			  | (SDXC_CLKSRC << 2)		// set clock source
			  | (0UL << 1);			// set clock invert
 	pSDClkGenReg->CLKENB |= 0x1UL << 2;		// clock generation enable

 	ResetCon(SDResetNum[i], CTRUE);	// reset on
 	ResetCon(SDResetNum[i], CFALSE);	// reset negate
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setdeviceclock(&sdmmcclk[i][0], 2, 1);
#endif

	pSDXCReg->PWREN = 0 << 0;	// Set Power Disable

	pSDXCReg->CLKENA = NX_SDXC_CLKENA_LOWPWR;// low power mode & clock disable
#ifdef NXP4330
	pSDXCReg->CLKCTRL =
		0 << 24 |	// sample clock phase shift 0:0 1:90 2:180 3:270
		2 << 16 |	// drive clock phase shift 0:0 1:90 2:180 3:270
		0 << 8  |	// sample clock delay
		0 << 0;		// drive clock delay
#endif
#ifdef NXP3220
	pSDXCReg->TIEDRVPHASE   = NX_SDMMC_CLOCK_SHIFT_180 << 8;
	pSDXCReg->TIESMPPHASE   = NX_SDMMC_CLOCK_SHIFT_0  << 8;
#endif

	pSDXCReg->CLKSRC = 0;	// prescaler 0
	pSDXCReg->CLKDIV = (SDCLK_DIVIDER_WORK >> 1) << 8 |
			(SDCLK_DIVIDER_ENUM >> 1) << 0;	// 2*n divider (0 : bypass)
	// fifo mode, not read wait(only use sdio mode)
	pSDXCReg->CTRL &= ~(NX_SDXC_CTRL_DMAMODE_EN | NX_SDXC_CTRL_READWAIT);
	// Reset the controller & DMA interface & FIFO
	pSDXCReg->CTRL = NX_SDXC_CTRL_DMARST |
			NX_SDXC_CTRL_FIFORST |
			NX_SDXC_CTRL_CTRLRST;
	while (pSDXCReg->CTRL & (NX_SDXC_CTRL_DMARST |
				NX_SDXC_CTRL_FIFORST |
				NX_SDXC_CTRL_CTRLRST))
		pSDXCReg->CTRL;

	pSDXCReg->PWREN = 0x1 << 0;	// Set Power Enable

	// Data Timeout = 0xFFFFFF, Response Timeout = 0x64
	pSDXCReg->TMOUT = (0xFFFFFFU << 8) | (0x64 << 0);

	// Data Bus Width : 0(1-bit), 1(4-bit)
	pSDXCReg->CTYPE = 0;

	// Block size
	pSDXCReg->BLKSIZ = BLOCK_LENGTH;

	// Issue when RX FIFO Count >= 8 x 4 bytes & TX FIFO Count <= 8 x 4 bytes
	pSDXCReg->FIFOTH = ((8 - 1) << 16) |		// Rx threshold
				(8 << 0);		// Tx threshold

	// Mask & Clear All interrupts
	pSDXCReg->INTMASK = 0;
	pSDXCReg->RINTSTS = 0xFFFFFFFF;

#ifdef NXP3220
	// Wake up & Power on fifo sram
	pSDXCReg->TIESRAM = 0x3;
	pSDXCReg->TIEMODE = 1;
#endif

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Terminate(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];
#ifdef NXP4330
	register struct NX_CLKGEN_RegisterSet * const pSDClkGenReg =
					pgSDClkGenReg[pSDXCBootStatus->SDPort];
#endif
	// Clear All interrupts
	pSDXCReg->RINTSTS = 0xFFFFFFFF;

	// Reset the controller & DMA interface & FIFO
	pSDXCReg->CTRL = NX_SDXC_CTRL_DMARST |
			NX_SDXC_CTRL_FIFORST |
			NX_SDXC_CTRL_CTRLRST;
	while (pSDXCReg->CTRL & (NX_SDXC_CTRL_DMARST |
				NX_SDXC_CTRL_FIFORST |
				NX_SDXC_CTRL_CTRLRST))
		pSDXCReg->CTRL;

#ifdef NXP4330
	// Disable CLKGEN
	pSDClkGenReg->CLKENB = 0;
	ResetCon(SDResetNum[pSDXCBootStatus->SDPort], CTRUE);	// reset on
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setdeviceclock(&sdmmcclk[pSDXCBootStatus->SDPort][0], 2, 0);
#endif

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Open(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 SDSpeed;
	if (pSDXCBootStatus->bHighSpeed == CTRUE)
		SDSpeed = SDXC_CLKDIV_HIGH;
	else
		SDSpeed = SDXC_CLKDIV_LOW;
	//--------------------------------------------------------------------------
	// card identification mode : Identify & Initialize
	if (CFALSE == pbl0fn->NX_SDMMC_IdentifyCard(pSDXCBootStatus)) {
		pbl0fn->printf("Identify Fail\r\n");
		return CFALSE;
	}

	//--------------------------------------------------------------------------
	// data transfer mode : Stand-by state
	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CTRUE,
				SDSpeed)) {
		pbl0fn->printf("Card Clk rst fail\r\n");
		return CFALSE;
	}
	if (CFALSE == pbl0fn->NX_SDMMC_SelectCard(pSDXCBootStatus)) {
		pbl0fn->printf("Card Sel Fail\r\n");
		return CFALSE;
	}

	//--------------------------------------------------------------------------
	// data transfer mode : Transfer state
	if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
		pbl0fn->NX_SDMMC_SetCardDetectPullUp(pSDXCBootStatus, CFALSE);
	}

	if (CFALSE == pbl0fn->NX_SDMMC_SetBlockLength(pSDXCBootStatus,
				BLOCK_LENGTH)) {
		pbl0fn->printf("Set Blk Lng Fail\r\n");
		return CFALSE;
	}

	if (option & (1 << eMMCBUSWIDTH))
		pbl0fn->NX_SDMMC_SetBusWidth(pSDXCBootStatus, 8);
	else
		pbl0fn->NX_SDMMC_SetBusWidth(pSDXCBootStatus, 4);

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Close(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CFALSE, SDXC_DIVIDER_400KHZ);
	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectorData(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32		count;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	NX_ASSERT(0 == ((U32)pdwBuffer & 3));

	count = numberOfSector * BLOCK_LENGTH;
	NX_ASSERT(0 == (count % 32));

	while (0 < count) {
		if ((pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RXDR)
		 || (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO)) {
			U32 FSize, Timeout = NX_SDMMC_TIMEOUT;
			while ((pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY) &&
					Timeout--)
				pSDXCReg->STATUS;
			if (0 == Timeout)
				break;
			FSize = (pSDXCReg->STATUS &
					NX_SDXC_STATUS_FIFOCOUNT) >> 17;
			count -= (FSize * 4);
			while (FSize) {
				*pdwBuffer++ = pSDXCReg->DATA;
				FSize--;
			}

			pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_RXDR;
		}

		// Check Errors
		if (pSDXCReg->RINTSTS & (NX_SDXC_RINTSTS_DRTO |
					NX_SDXC_RINTSTS_EBE |
					NX_SDXC_RINTSTS_SBE |
					NX_SDXC_RINTSTS_DCRC)) {
			pbl0fn->printf("Read left = %x\r\n", count);

			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
				pbl0fn->printf("DRTO\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
				pbl0fn->printf("EBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
				pbl0fn->printf("SBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
				pbl0fn->printf("DCRC\r\n");

			return CFALSE;
		}

		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO) {
			if (count == 0) {
				pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;
				break;
			}
		}

		#if defined(NX_DEBUG)
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO) {
			pbl0fn->printf("HTO\r\n");
			pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HTO;
		}
		#endif

		NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
	}

	pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectors(SDXCBOOTSTATUS *pSDXCBootStatus,
			U32 SectorNum, U32 numberOfSector, U32 *pdwBuffer)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL	result = CFALSE;
	U32		status;
	#if defined(NX_DEBUG)
	U32	response;
	#endif
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	NX_ASSERT(0 == ((U32)pdwBuffer & 3));

	// wait while data busy or data transfer busy
	while (pSDXCReg->STATUS & (1 << 9 | 1 << 10))
		pSDXCReg->STATUS;

	//--------------------------------------------------------------------------
	// wait until 'Ready for data' is set and card is in transfer state.
	do {
		cmd.cmdidx	= SEND_STATUS;
		cmd.arg		= pSDXCBootStatus->rca;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP;
		status = pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
		if (NX_SDMMC_STATUS_NOERROR != status)
			goto End;
	} while (!((cmd.response[0] & (1 << 8)) &&
		(((cmd.response[0] >> 9) & 0xF) == 4)));

	NX_ASSERT(NX_SDXC_STATUS_FIFOEMPTY ==
			(pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY));
	NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY));

	// Set byte count
	pSDXCReg->BYTCNT = numberOfSector * BLOCK_LENGTH;

	//--------------------------------------------------------------------------
	// Send Command
	if (numberOfSector > 1) {
		cmd.cmdidx	= READ_MULTIPLE_BLOCK;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP |
				NX_SDXC_CMDFLAG_BLOCK |
				NX_SDXC_CMDFLAG_RXDATA |
				NX_SDXC_CMDFLAG_SENDAUTOSTOP;
	} else {
		cmd.cmdidx	= READ_SINGLE_BLOCK;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_WAITPRVDAT |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP |
				NX_SDXC_CMDFLAG_BLOCK |
				NX_SDXC_CMDFLAG_RXDATA;
	}
	cmd.arg		= (pSDXCBootStatus->bHighCapacity) ?
				SectorNum : SectorNum*BLOCK_LENGTH;

	status = pbl0fn->NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
	if (NX_SDMMC_STATUS_NOERROR != status)
		goto End;

	//--------------------------------------------------------------------------
	// Read data
	if (CTRUE != pbl0fn->NX_SDMMC_ReadSectorData(pSDXCBootStatus,
						numberOfSector, pdwBuffer))
		goto End;

	NX_ASSERT(NX_SDXC_STATUS_FIFOEMPTY ==
			(pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY));
	NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOFULL));
	NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOCOUNT));

	if (numberOfSector > 1) {
		// Wait until the Auto-stop command has been finished.
		while (0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_ACD))
			pSDXCReg->RINTSTS;

		NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY));

		#if defined(NX_DEBUG)
		// Get Auto-stop response and then check it.
		response = pSDXCReg->RESP[1];
		if (response & 0xFDF98008) {
			pbl0fn->printf("Auto Stop Resp Fail:%x\r\n", response);
			//goto End;
		}
		#endif
	}

	result = CTRUE;

End:
	if (CFALSE == result) {
		cmd.cmdidx	= STOP_TRANSMISSION;
		cmd.arg		= 0;
		cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
				NX_SDXC_CMDFLAG_CHKRSPCRC |
				NX_SDXC_CMDFLAG_SHORTRSP |
				NX_SDXC_CMDFLAG_STOPABORT;
		pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);

		if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
			pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;
			while (pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST)
				pSDXCReg->CTRL;
		}
	}

	return result;
}
#if 1
//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadBootSector(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32		count;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

	NX_ASSERT(0==((U32)pdwBuffer & 3));

	count = numberOfSector * BLOCK_LENGTH;
	NX_ASSERT(0 == (count % 32));

	while (0 < count) {
		if ((pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RXDR)
		 || (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO)) {
			U32 FSize, Timeout = NX_SDMMC_TIMEOUT;
			while ((pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY) &&
					Timeout--)
				pSDXCReg->STATUS;
//			if (0 == Timeout)
//				break;
			FSize = (pSDXCReg->STATUS &
					NX_SDXC_STATUS_FIFOCOUNT) >> 17;
			count -= (FSize * 4);
			while (FSize) {
				*pdwBuffer++ = pSDXCReg->DATA;
				FSize--;
			}

			pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_RXDR;
		}
#if 0
		// Check Errors
		if (pSDXCReg->RINTSTS & (NX_SDXC_RINTSTS_DRTO |
					NX_SDXC_RINTSTS_EBE |
					NX_SDXC_RINTSTS_SBE |
					NX_SDXC_RINTSTS_DCRC)) {
			pbl0fn->printf("Read left = %x\r\n", count);

			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
				pbl0fn->printf("DRTO\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
				pbl0fn->printf("EBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
				pbl0fn->printf("SBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
				pbl0fn->printf("DCRC\r\n");

			return CFALSE;
		}
#endif
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO) {
			if (count == 0) {
				pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;
				pbl0fn->printf("DTO\r\n");
				break;
			}
		}

		#if defined(NX_DEBUG)
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO) {
			pbl0fn->printf("HTO\r\n");
			pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HTO;
		}
		#endif

		NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
	}

	pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

	return CTRUE;
}
#endif
#ifdef NXP4330
void DataDump(U32 * Addr, U32 Size)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 i, j;
	for (i = 0; i < Size / 16; i++) {
		pbl0fn->printf("0x%08X: ", (U32)Addr);
		for (j = 0; j < 4; j++) {
			pbl0fn->printf("%08X ", Addr[j]);
			if (Size <= i * 16 + j * 4)
				break;
		}
		Addr += 4;
		pbl0fn->printf("\r\n");
	}
	pbl0fn->printf("\r\n");
}
#endif

CBOOL eMMCBootRead(SDXCBOOTSTATUS *pSDXCBootStatus, unsigned int option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL result;
	unsigned int iv[4];
	iv[0] = (*pbl0fn->iv)[0];
	iv[1] = (*pbl0fn->iv)[1];
	iv[2] = (*pbl0fn->iv)[2];
	iv[3] = (*pbl0fn->iv)[3];

	struct nx_bootinfo *pbi =
		(struct nx_bootinfo *)&(*pbl0fn->pbm)->bi;

	if (pbl0fn->NX_SDMMC_ReadBootSector(pSDXCBootStatus, 1, (U32 *)pbi)
			== CFALSE) {
		pbl0fn->printf("Boot header read Fail.\r\n");
		return CFALSE;
	}
#if 1
	if (!pbl0fn->headercheck(iv, option & 1 << DECRYPT))
		return CFALSE;
#else
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((U32*)pbi, (U32*)pbi,
				iv, sizeof(struct nx_bootinfo));
	}

	if (pbi->signature != HEADER_ID) {
		pbl0fn->printf("no boot header(%08X)\r\n", pbi->signature);
		return CFALSE;
	}
#endif

	U32 BootSize = pbi->LoadSize;
	pbl0fn->printf("LoadAddr:%x LoadSize:%x StartAddr:%x\r\n",
		pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr);

	if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK)
		BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;


	U32 sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
	unsigned int tdata[1024 / 4];
	unsigned int *pdata = (unsigned int *)(*pbl0fn->pbm)->image;
	result = pbl0fn->NX_SDMMC_ReadBootSector(pSDXCBootStatus, 2, tdata);


	pbl0fn->memcpy(&(*pbl0fn->pbm)->rsa_public, tdata, sizeof(struct asymmetrickey));
	pbl0fn->memcpy(pdata, tdata, 1024 - sizeof(struct asymmetrickey));

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
			pbl0fn->printf("boot key verify fail\r\n");
			return 0;
		}
	}
#endif

	result = pbl0fn->NX_SDMMC_ReadBootSector(pSDXCBootStatus,
			sectorsize, &pdata[256]);

	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((U32*)(*pbl0fn->pbm)->image,
				(U32*)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}
	
	return result;
}
//------------------------------------------------------------------------------
CBOOL eMMCBoot_Normal(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	register CBOOL	result = CFALSE;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];
	register U32 SDSpeed;
	U32 timeout;

	if (pSDXCBootStatus->bHighSpeed == CTRUE)
		SDSpeed = SDXC_CLKDIV_HIGH;
	else
		SDSpeed = SDXC_CLKDIV_LOW;
	pbl0fn->printf("Normal mode %s speed\n", SDSpeed ? "High" : "Normal");

	pSDXCReg->CTYPE		= 1;	/* Data Bus Width : 0(1-bit), 1(4-bit) */
	pSDXCReg->BYTCNT	= 64 * 1024;	/* for normal */

	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CTRUE, SDSpeed)) {
		pbl0fn->printf("clk init fail\r\n");
		return CFALSE;
	}

	/* just send boot command. no expect response */
	timeout = 0;
	do {
	//	Send Alternative boot command
		pSDXCReg->RINTSTS	= NX_SDXC_RINTSTS_HLE;
		pSDXCReg->CMDARG	= 0xFFFFFFFA;
		pSDXCReg->CMD		= GO_IDLE_STATE |
					NX_SDXC_CMDFLAG_STARTCMD |
					NX_SDXC_CMDFLAG_SENDINIT |
					NX_SDXC_CMDFLAG_BLOCK |
					NX_SDXC_CMDFLAG_RXDATA |
					NX_SDXC_CMDFLAG_ENABLE_BOOT |
					NX_SDXC_CMDFLAG_USE_HOLD_REG;

		while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
			if (++timeout > NX_SDMMC_TIMEOUT) {
				pbl0fn->printf("TO send cmd\r\n");
				INFINTE_LOOP();
				goto error;
			}
		}
	} while (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE);

	result = pbl0fn->eMMCBootRead(pSDXCBootStatus, option);

error:
	timeout = 0;
	while (1) {
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_CD)
			break;

		if (++timeout > NX_SDMMC_TIMEOUT) {
			pbl0fn->printf("TO NB cmd done\r\n");
			INFINTE_LOOP();
			break;
		}
	}

	return result;
}
//------------------------------------------------------------------------------
CBOOL eMMCBoot_Alt(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	register CBOOL	result = CFALSE;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];
	register U32 SDSpeed;
	NX_SDMMC_COMMAND cmd;

	if (pSDXCBootStatus->bHighSpeed == CTRUE)
		SDSpeed = SDXC_CLKDIV_HIGH;
	else
		SDSpeed = SDXC_CLKDIV_LOW;
	pbl0fn->printf("alt mode %s speed\n", SDSpeed ? "High" : "Normal");

	pSDXCReg->CTYPE		= 1;	/* Data Bus Width : 0(1-bit), 1(4-bit) */
	pSDXCReg->BYTCNT	= 64 * 1024;	/* for Alternative */

	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CTRUE, SDSpeed)) {
		pbl0fn->printf("clk init fail\r\n");
		return CFALSE;
	}

	//----------------------------------------------------------------------
	//	Send Alternative boot command
	cmd.cmdidx	= GO_IDLE_STATE;
	cmd.arg		= 0xFFFFFFFA;		/* Alternative boot mode */
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD | NX_SDXC_CMDFLAG_SENDINIT |
			  NX_SDXC_CMDFLAG_BLOCK | NX_SDXC_CMDFLAG_RXDATA;

	/* just send boot command. no expect response */
	if (NX_SDMMC_STATUS_NOERROR !=
			pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
			pbl0fn->printf("Send Cmd Internal Err.\r\n");
	} else
		result = pbl0fn->eMMCBootRead(pSDXCBootStatus, option);
	
	//--------------------------------------------------------------------------
	cmd.cmdidx	= GO_IDLE_STATE;
	cmd.arg		= 0;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_STOPABORT |
			NX_SDXC_CMDFLAG_DISABLE_BOOT;
	pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);

	if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
		pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;
		while (pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST)
			pSDXCReg->CTRL;
	}

	return result;
}

//------------------------------------------------------------------------------
CBOOL SDMMCBOOT(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	CBOOL	result = CFALSE;
	struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];

	if (CTRUE != pbl0fn->NX_SDMMC_Open(pSDXCBootStatus, option)) {
		pbl0fn->printf("device open fail\r\n");
		goto error;
	}

	if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
		volatile U32 tempcount = 0x100000;
		pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;
		/* Wait until the FIFO reset is completed. */
		while ((pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST) &&
				tempcount--)
			pSDXCReg->CTRL;
	}

	unsigned int *psector = (unsigned int *)&(*pbl0fn->pbm)->bi;
	unsigned int rsn = 0;

	if (pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == CFALSE) {
		pbl0fn->printf("MBR read fail.\r\n");
		goto error;
	}

	int ret;
	ret = pbl0fn->is_gpt_part((unsigned char *)psector);

	if (pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == CFALSE) {
		pbl0fn->printf("%s Header read fail\r\n", ret ? "Boot" : "GPT");
		goto error;
	}

	if (!ret) {
		ret = pbl0fn->is_gpt_valid((unsigned char *)psector);
		if (!ret) {
			rsn = pbl0fn->get_first_empty_lba((unsigned char *)psector);
			pbl0fn->printf("read bootimage from sector %d.\r\n", rsn);
			if (pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1,
					psector) == CFALSE) {
				pbl0fn->printf("Boot Header read fail.\r\n");
				goto error;
			}
		} else
			pbl0fn->printf("invalid GPT.\r\n");
	} else
		pbl0fn->printf("not GPT, read header from sector 1.\r\n");

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
	if (option & 1 << DECRYPT) {
		pbl0fn->Decrypt((U32*)&(*pbl0fn->pbm)->bi,
				(U32*)&(*pbl0fn->pbm)->bi,
				iv, sizeof(struct nx_bootinfo));
	}

	if (pbi->signature != HEADER_ID) {
		pbl0fn->printf("not bootable image(%08x).\r\n",
				pbi->signature);
		return 0;
	}
#endif

	struct nx_bootinfo *pbi = &(*pbl0fn->pbm)->bi;
	U32 BootSize = pbi->LoadSize;
	pbl0fn->printf("LoadAddr:%x LoadSize:%x StartAddr:%x\r\n",
		pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr);

	if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK)
		BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;


	U32 sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
	unsigned int tdata[1024 / 4];
       	unsigned int *pdata = (unsigned int *)(*pbl0fn->pbm)->image;
	result = pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn, 2, tdata);
	rsn += 2;

	pbl0fn->memcpy(&(*pbl0fn->pbm)->rsa_public, tdata, sizeof(struct asymmetrickey));
	pbl0fn->memcpy(pdata, tdata, 1024 - sizeof(struct asymmetrickey));

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

	result = pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus,
			rsn, sectorsize, &pdata[256]);

	if (option & 1 << DECRYPT) {
#if 1
		pbl0fn->imagedecrypt(iv);
#else
		pbl0fn->Decrypt((U32*)(*pbl0fn->pbm)->image,
				(U32*)(*pbl0fn->pbm)->image,
				iv, (*pbl0fn->pbm)->bi.LoadSize);
#endif
	}
error:

	return result;
}

#ifdef NXP3220
const union nxpad sdmmcpad[3][10] = {
{
	{PI_SDMMC0_CDATA_0_},
	{PI_SDMMC0_CDATA_1_},
	{PI_SDMMC0_CDATA_2_},
	{PI_SDMMC0_CDATA_3_},
	{PI_SDMMC0_CDATA_4_},
	{PI_SDMMC0_CDATA_5_},
	{PI_SDMMC0_CDATA_6_},
	{PI_SDMMC0_CDATA_7_},
	{PI_SDMMC0_CCLK},
	{PI_SDMMC0_CMD}
},
{
	{PI_SDMMC1_CDATA_0_},
	{PI_SDMMC1_CDATA_1_},
	{PI_SDMMC1_CDATA_2_},
	{PI_SDMMC1_CDATA_3_},
	{PI_SDMMC1_CDATA_4_},
	{PI_SDMMC1_CDATA_5_},
	{PI_SDMMC1_CDATA_6_},
	{PI_SDMMC1_CDATA_7_},
	{PI_SDMMC1_CCLK},
	{PI_SDMMC1_CMD}
},
{
	{PI_SDMMC2_CDATA_0_},
	{PI_SDMMC2_CDATA_1_},
	{PI_SDMMC2_CDATA_2_},
	{PI_SDMMC2_CDATA_3_},
	{PI_SDMMC2_CDATA_4_},
	{PI_SDMMC2_CDATA_5_},
	{PI_SDMMC2_CDATA_6_},
	{PI_SDMMC2_CDATA_7_},
	{PI_SDMMC2_CCLK},
	{PI_SDMMC2_CMD}
}};
#endif
void NX_SDPADSetALT(U32 PortNum)
{
#ifdef NXP4330
	if (PortNum == 0) {
		register U32 *pGPIOARegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_A]->GPIOx_ALTFN[1];	// c31, c30, c29, c28, c27
		register U32 *pGPIOBRegA0 =
			(U32 *)&pGPIOReg[GPIO_GROUP_B]->GPIOx_ALTFN[0];	// d05, d04, d03, d02, d01, d00
		*pGPIOARegA1 = (*pGPIOARegA1 & ~0xCC000000) | 0x44000000;	// all alt is 1
		*pGPIOBRegA0 = (*pGPIOBRegA0 & ~0x0000CCCC) | 0x00004444;	// all alt is 1

		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW			&= ~(0x5 << 29);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW_DISABLE_DEFAULT	|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0			|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0_DISABLE_DEFAULT	|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1			|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1_DISABLE_DEFAULT	|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL			|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL_DISABLE_DEFAULT	|=   0x5 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB			|=   0x4 << 29;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB_DISABLE_DEFAULT	|=   0x5 << 29;

		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW			&= ~(0x55 << 1);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW_DISABLE_DEFAULT	|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0			|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0_DISABLE_DEFAULT	|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1			|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1_DISABLE_DEFAULT	|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL			|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL_DISABLE_DEFAULT	|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB			|=   0x55 << 1;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB_DISABLE_DEFAULT	|=   0x55 << 1;
	} else if(PortNum == 1) {
		register U32 *pGPIODRegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_D]->GPIOx_ALTFN[1];	// d16
		*pGPIODRegA1 = (*pGPIODRegA1 & ~0x00FFF000) | 0x00555000;	// all alt is 1
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW			&= ~(0x3F << 22);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW_DISABLE_DEFAULT	|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0			|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0_DISABLE_DEFAULT	|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1			|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1_DISABLE_DEFAULT	|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL			|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL_DISABLE_DEFAULT	|=   0x3F << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB			|=   0x3E << 22;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB_DISABLE_DEFAULT	|=   0x3F << 22;
	} else {   // PortNum == 2
		register U32 *pGPIOCRegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_C]->GPIOx_ALTFN[1];
		*pGPIOCRegA1 = (*pGPIOCRegA1 & ~0x0000FFF0) | 0x000AAA0;	// all alt is 1
		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW			&= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW_DISABLE_DEFAULT	|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0			|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0_DISABLE_DEFAULT	|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1			|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1_DISABLE_DEFAULT	|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL			|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL_DISABLE_DEFAULT	|=   0x3F << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB			|=   0x3E << 18;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB_DISABLE_DEFAULT	|=   0x3F << 18;
	}
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setpad(sdmmcpad[PortNum], 10, 1);
#endif
}

void NX_SDPADSetGPIO(U32 PortNum)
{
#ifdef NXP4330
	if (PortNum == 0) {
		register U32 *pGPIOARegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_C]->GPIOx_ALTFN[1];	// c31, c30, c29, c28, c27
		register U32 *pGPIOBRegA0 =
			(U32 *)&pGPIOReg[GPIO_GROUP_D]->GPIOx_ALTFN[0];	// d05, d04, d03, d02, d01, d00
		*pGPIOARegA1 = (*pGPIOARegA1 & ~0xFFC00000);                                // all alt is 1
		*pGPIOBRegA0 = (*pGPIOBRegA0 & ~0x00000FFF);                                // all alt is 1

		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW |= 0x1F << 27;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_SLEW_DISABLE_DEFAULT |= 0x1F << 27;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0 &= ~(0x1F << 27);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV0_DISABLE_DEFAULT |= 0x1F << 27;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1 &= ~(0x1F << 27);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_DRV1_DISABLE_DEFAULT |= 0x1F << 27;
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL &= ~(0x1F << 27);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLSEL_DISABLE_DEFAULT &= ~(0x1F << 27);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB &= ~(0x1F << 27);
		pGPIOReg[GPIO_GROUP_C]->GPIOx_PULLENB_DISABLE_DEFAULT &= ~(0x1F << 27);

		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW |= 0x3F << 0;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW_DISABLE_DEFAULT |= 0x3F << 0;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0 &= ~(0x3F << 0);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0_DISABLE_DEFAULT |= 0x3F << 0;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1 &= ~(0x3F << 0);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1_DISABLE_DEFAULT |= 0x3F << 0;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL &= ~(0x3F << 0);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL_DISABLE_DEFAULT &= ~(0x3F << 0);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB &= ~(0x3F << 0);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB_DISABLE_DEFAULT &= ~(0x3F << 0);
	} else if(PortNum == 1) {
		register U32 *pGPIODRegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_D]->GPIOx_ALTFN[1];	// d16
		register U32 *pGPIODRegA0 =
			(U32 *)&pGPIOReg[GPIO_GROUP_D]->GPIOx_ALTFN[0];	// d15, d14, d13, d12, d11, d10, d09, d08, d07, d06
		*pGPIODRegA1 = (*pGPIODRegA1 & ~0x00000003);                                // all alt is 0
		*pGPIODRegA0 = (*pGPIODRegA1 & ~0xFFFFF000);                                // all alt is 0

		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW |= 0x7FF << 6;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW_DISABLE_DEFAULT |= 0x7FF << 6;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0 &= ~(0x7FF << 6);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0_DISABLE_DEFAULT |= 0x7FF << 6;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1 &= ~(0x7FF << 6);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1_DISABLE_DEFAULT |= 0x7FF << 6;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL &= ~(0x7FF << 6);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL_DISABLE_DEFAULT &= ~(0x7FF << 6);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB &= ~(0x7FF << 6);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB_DISABLE_DEFAULT &= ~(0x7FF << 6);
	} else {
		// c18, c19, c20, c21, c22, c23
		register U32 *pGPIOCRegA1 =
			(U32 *)&pGPIOReg[GPIO_GROUP_C]->GPIOx_ALTFN[1];
		*pGPIOCRegA1 = (*pGPIOCRegA1 & ~0x0000FFF0);	                            // all alt is 0

		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW |= 0x3F << 18;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_SLEW_DISABLE_DEFAULT |= 0x3F << 18;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0 &= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV0_DISABLE_DEFAULT |= 0x3F << 18;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1 &= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_DRV1_DISABLE_DEFAULT |= 0x3F << 18;
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL &= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLSEL_DISABLE_DEFAULT &= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB &= ~(0x3F << 18);
		pGPIOReg[GPIO_GROUP_D]->GPIOx_PULLENB_DISABLE_DEFAULT &= ~(0x3F << 18);
	}
#endif

#ifdef NXP3220
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->setpad(sdmmcpad[PortNum], 10, 0);
#endif
}

//------------------------------------------------------------------------------
U32 iSDXCBOOT(U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	SDXCBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
	CBOOL	result = CFALSE;

	pSDXCBootStatus = &SDXCBootStatus;

	pSDXCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

	if (pSDXCBootStatus->SDPort >= 3) {
		pSDXCBootStatus->SDPort = 0;
		pSDXCBootStatus->bHighSpeed = CTRUE;
	} else
		pSDXCBootStatus->bHighSpeed = CFALSE;

	pbl0fn->NX_SDPADSetALT(pSDXCBootStatus->SDPort);

	pbl0fn->NX_SDMMC_Init(pSDXCBootStatus);

	pbl0fn->printf("SD boot:%d\r\n", pSDXCBootStatus->SDPort);
	result = pbl0fn->SDMMCBOOT(pSDXCBootStatus, option);

	pbl0fn->NX_SDMMC_Close(pSDXCBootStatus);
	pbl0fn->NX_SDMMC_Terminate(pSDXCBootStatus);

	pbl0fn->NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);

	return result;
}
CBOOL ieMMCBOOT(U32 option)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	SDXCBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
	CBOOL	result = CFALSE;

	pSDXCBootStatus = &SDXCBootStatus;

	pSDXCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

	if (pSDXCBootStatus->SDPort >= 3) {
		pSDXCBootStatus->SDPort = 0;
		pSDXCBootStatus->bHighSpeed = CTRUE;
	} else
		pSDXCBootStatus->bHighSpeed = CFALSE;

	pbl0fn->NX_SDPADSetALT(pSDXCBootStatus->SDPort);

	pbl0fn->NX_SDMMC_Init(pSDXCBootStatus);

	//--------------------------------------------------------------------------
	// eMMC or MMC ver 4.3+
	pbl0fn->printf("eMMC boot:%d\r\n", pSDXCBootStatus->SDPort);
	if ((option & 1 << eMMCBOOTMODE) != 0)
		result = pbl0fn->eMMCBoot_Alt(pSDXCBootStatus, option);
	else
		result = pbl0fn->eMMCBoot_Normal(pSDXCBootStatus, option);

	pbl0fn->NX_SDMMC_Close(pSDXCBootStatus);
	pbl0fn->NX_SDMMC_Terminate(pSDXCBootStatus);

	pbl0fn->NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);

	return result;
}
