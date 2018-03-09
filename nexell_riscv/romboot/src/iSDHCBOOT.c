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
#include "include/nx_swallow.h"
#include "include/nx_type.h"
#include "include/nx_swallow_bootoption.h"
#include "include/nx_swallow_bootheader.h"
#ifdef QEMU_RISCV
#include "kprintf.h"
#include "include/qemu_platform.h"
#else
#include "printf.h"
#include "include/platform.h"
#endif

#include "include/nx_gpio.h"

#include "include/nx_sdmmc.h"

#include "iSDHCBOOT.h"

#include "include/nx_chip_iomux.h"
#include "include/nx_chip_sfr.h"
#include "include/nx_cmu.h"

#define NX_ASSERT(x)

#include "libplat.h"
#include "fnptr.h"


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

struct NX_SDMMC_RegisterSet * const pgSDXCReg[3] =
{
  (struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC0_MODULE,
  (struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC1_MODULE,
  (struct NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC2_MODULE
};


//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Init(SDXCBOOTSTATUS *pSDXCBootStatus)
{
	U32 i = pSDXCBootStatus->SDPort;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[i];

        NXBL0FN *pbl0fn = Getbl0fnPtr();

#ifdef DEBUG
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s : NX_SDMMC_Init address = 0x%x\n",__func__,pbl0fn->NX_SDMMC_Init);      
        pbl0fn->_dprintf("[SUKER-bootrom] %s : pSDXCReg = 0x%x\n",__func__,pSDXCReg);
        pbl0fn->_dprintf("[SUKER-bootrom] %s : NX_SDMMC_RegisterSet size = 0x%x\n",__func__,sizeof(struct NX_SDMMC_RegisterSet));        

        pbl0fn->_dprintf("[SUKER-bootrom] %s : pSDXCBootStatus->SDPort = 0x%x\n",__func__,pSDXCBootStatus->SDPort);
#endif

#if !defined(QEMU_RISCV)        
        pbl0fn->setdeviceclock(&sdmmcclk[i][0], 2, 1);
#endif
	pSDXCReg->PWREN = 0 << 0;	// Set Power Disable

	pSDXCReg->CLKENA = NX_SDXC_CLKENA_LOWPWR;// low power mode & clock disable

	pSDXCReg->TIEDRVPHASE   = NX_SDMMC_CLOCK_SHIFT_180 << 8;
	pSDXCReg->TIESMPPHASE   = NX_SDMMC_CLOCK_SHIFT_0  << 8;

	pSDXCReg->CLKSRC = 0;	// prescaler 0
	pSDXCReg->CLKDIV = (SDCLK_DIVIDER_WORK >> 1) << 8 |
			(SDCLK_DIVIDER_ENUM >> 1) << 0;	// 2*n divider (0 : bypass)
	// fifo mode, not read wait(only use sdio mode)
	pSDXCReg->CTRL &= ~(NX_SDXC_CTRL_DMAMODE_EN | NX_SDXC_CTRL_READWAIT);
	// Reset the controller & DMA interface & FIFO
	pSDXCReg->CTRL = NX_SDXC_CTRL_DMARST |
			NX_SDXC_CTRL_FIFORST |
			NX_SDXC_CTRL_CTRLRST;

#if !defined(QEMU_RISCV)
	while (pSDXCReg->CTRL & (NX_SDXC_CTRL_DMARST |
				NX_SDXC_CTRL_FIFORST |
				NX_SDXC_CTRL_CTRLRST))
		pSDXCReg->CTRL;
#endif
	pSDXCReg->PWREN = 0x1 << 0;	// Set Power Enable

	// Data Timeout = 0xFFFFFF, Response Timeout = 0x64
	pSDXCReg->TMOUT = (unsigned int)((0xFFFFFFU << 8) | (0x64 << 0));

	// Data Bus Width : 0(1-bit), 1(4-bit)
	pSDXCReg->CTYPE = 0;

	// Block size
	pSDXCReg->BLKSIZ = BLOCK_LENGTH;

	// Issue when RX FIFO Count >= 8 x 4 bytes & TX FIFO Count <= 8 x 4 bytes
	pSDXCReg->FIFOTH = ((8 - 1) << 16) |		// Rx threshold
				(8 << 0);		// Tx threshold

	// Mask & Clear All interrupts
	pSDXCReg->INTMASK = 0;
	pSDXCReg->RINTSTS = (unsigned int)(0xFFFFFFFF);

	// Wake up & Power on fifo sram
	pSDXCReg->TIESRAM = (unsigned int)0x3;
	pSDXCReg->TIEMODE = 1;

#ifdef QEMU_RISCV
        _dprintf("[SUKER-bootrom] pSDXCReg->PWREN = 0x%x\n",pSDXCReg->PWREN);
        _dprintf("[SUKER-bootrom] pSDXCReg->CLKENA = 0x%x\n",pSDXCReg->CLKENA);
        _dprintf("[SUKER-bootrom] pSDXCReg->TIEDRVPHASE = 0x%x\n",pSDXCReg->TIEDRVPHASE);
        _dprintf("[SUKER-bootrom] pSDXCReg->TIESMPPHASE = 0x%x\n",pSDXCReg->TIESMPPHASE);
        _dprintf("[SUKER-bootrom] pSDXCReg->CLKSRC = 0x%x\n",pSDXCReg->CLKSRC);
        _dprintf("[SUKER-bootrom] pSDXCReg->CLKDIV = 0x%x\n",pSDXCReg->CLKDIV);
        _dprintf("[SUKER-bootrom] pSDXCReg->CTRL = 0x%x\n",pSDXCReg->CTRL);
        _dprintf("[SUKER-bootrom] pSDXCReg->TMOUT = 0x%x\n",pSDXCReg->TMOUT);
        _dprintf("[SUKER-bootrom] pSDXCReg->CTYPE = 0x%x\n",pSDXCReg->CTYPE);
        _dprintf("[SUKER-bootrom] pSDXCReg->BLKSIZ = 0x%x\n",pSDXCReg->BLKSIZ);
        _dprintf("[SUKER-bootrom] pSDXCReg->FIFOTH = 0x%x\n",pSDXCReg->FIFOTH);
        _dprintf("[SUKER-bootrom] pSDXCReg->INTMASK = 0x%x\n",pSDXCReg->INTMASK);
        _dprintf("[SUKER-bootrom] pSDXCReg->RINTSTS = 0x%x\n",pSDXCReg->RINTSTS);
        _dprintf("[SUKER-bootrom] pSDXCReg->TIESRAM = 0x%x\n",pSDXCReg->TIESRAM);
        _dprintf("[SUKER-bootrom] pSDXCReg->TIEMODE = 0x%x\n",pSDXCReg->TIEMODE);
        
        pbl0fn->_dprintf("[SUKER-bootrom] %s : END\n",__func__);
#endif

	return CTRUE;
}


//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetClock(SDXCBOOTSTATUS *pSDXCBootStatus,
				CBOOL enb, U32 divider)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
#ifndef QEMU_RISCV        
	volatile U32 timeout;
#endif        
	U32 i = pSDXCBootStatus->SDPort;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[i];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->kputs("[SUKER-bootrom] NX_SDMMC_SetClock start");
#endif
        
	//--------------------------------------------------------------------------
	// 1. Confirm that no card is engaged in any transaction.
	//	If there's a transaction, wait until it has been finished.
//	while(NX_SDXC_IsDataTransferBusy())
//		;
//	while(NX_SDXC_IsCardDataBusy())
//		;
#if 1
	//--------------------------------------------------------------------------
	// 2. Disable the output clock.
	// low power mode & clock disable
	pSDXCReg->CLKENA |= NX_SDXC_CLKENA_LOWPWR;
	pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_CLKENB;


	/* if (divider == SDXC_DIVIDER_400KHZ) */
	/* 	pbl0fn->setdeviceclock(&sdmmcclk[i][2], 1, 1); */
	/* if (divider == SDXC_CLKDIV_LOW) */
	/* 	pbl0fn->setdeviceclock(&sdmmcclk[i][1], 1, 1); */


	pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_LOWPWR;	// low power mode & clock disable


	//--------------------------------------------------------------------------
	// 4. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
#ifndef QEMU_RISCV
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
			pbl0fn->_dprintf("clk TO\r\n");
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
#endif
	//--------------------------------------------------------------------------
	// 6. Enable the output clock.
	pSDXCReg->CLKENA |= NX_SDXC_CLKENA_CLKENB;

	//--------------------------------------------------------------------------
	// 7. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
#ifndef QEMU_RISCV
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
			pbl0fn->_dprintf("clk2 TO\r\n");
			INFINTE_LOOP();
			return CFALSE;
		}
	}

	if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
		INFINTE_LOOP();
		pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
		goto repeat_7;
	}
#endif
#endif
	return CTRUE;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommandInternal(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		NX_SDMMC_COMMAND *pCommand)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32		cmd, flag;
	U32		status = 0;
	volatile U32	timeout;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif

	NX_ASSERT(CNULL != pCommand);

#ifdef QEMU_RISCV        
	pbl0fn->_dprintf("[SUKER-bootrom]s : Command(%Xh), Argument(%Xh)\n",
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
				pbl0fn->_dprintf("TO send cmd\r\n");
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
			pbl0fn->_dprintf("TO cmd done\r\n");
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
					pbl0fn->_dprintf("TO card data ready\r\n");
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif

	cmd.cmdidx	= SEND_STATUS;
	cmd.arg		= pSDXCBootStatus->rca;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);


	return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommand(SDXCBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	S32 timeout;
	U32 HCS, RCA;
	NX_SDMMC_CARDTYPE CardType = NX_SDMMC_CARDTYPE_UNKNOWN;
	NX_SDMMC_COMMAND cmd;
	struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif

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
				pbl0fn->_dprintf("TO pwrup SD\r\n");
				return CFALSE;
			}
		}

		pbl0fn->_dprintf("SD ");

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
				pbl0fn->_dprintf("TO to wait pow-up for MMC\r\n");
				return CFALSE;
			}
		/* Wait until card has finished the power up routine */
		} while (0==(cmd.response[0] & (1UL << 31)));

		pbl0fn->_dprintf("MMC ");
		#if defined(VERBOSE)
		pbl0fn->_dprintf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]);
		#endif

		CardType	= NX_SDMMC_CARDTYPE_MMC;
		RCA		= 2 << 16;
	}

//	NX_ASSERT( (cmd.response[0] & 0x00FC0000) == 0x00FC0000 );
	pSDXCBootStatus->bHighCapacity =
		(cmd.response[0] & (1 << 30)) ? CTRUE : CFALSE;

	pbl0fn->_dprintf("%sCard.\r\n",
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
		pbl0fn->_dprintf("cannot read CID\r\n");
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
		pbl0fn->_dprintf("cannot read RCA\r\n");
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;
       
	cmd.cmdidx	= SET_CLR_CARD_DETECT;
	cmd.arg		= (bEnb) ? 1 : 0;
	cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
			NX_SDXC_CMDFLAG_WAITPRVDAT |
			NX_SDXC_CMDFLAG_CHKRSPCRC |
			NX_SDXC_CMDFLAG_SHORTRSP;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
        status = pbl0fn->NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);

	return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBusWidth(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 buswidth )
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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

	pSDXCReg->TIEMODE = buswidth >> 2;

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBlockLength(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 blocklength)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	#define IMMEDIATE_RESPONSE		(1 <<  0)
	#define DEFERRED_RESPONSE		(1 << 17)

#ifdef QEMU_RISCV
        kputs("[SUKER-bootrom] NX_SDMMC_SelectPartition start");
#endif
        

	return CTRUE;
}
#endif

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Terminate(SDXCBOOTSTATUS *pSDXCBootStatus)
{
        NXBL0FN *pbl0fn = Getbl0fnPtr();
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
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

       	pbl0fn->setdeviceclock(&sdmmcclk[pSDXCBootStatus->SDPort][0], 2, 0);

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Open(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 SDSpeed;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
	if (pSDXCBootStatus->bHighSpeed == CTRUE)
		SDSpeed = SDXC_CLKDIV_HIGH;
	else
		SDSpeed = SDXC_CLKDIV_LOW;
	//--------------------------------------------------------------------------
	// card identification mode : Identify & Initialize
	if (CFALSE == pbl0fn->NX_SDMMC_IdentifyCard(pSDXCBootStatus)) {
		pbl0fn->_dprintf("Identify Fail\r\n");
		return CFALSE;
	}

	//--------------------------------------------------------------------------
	// data transfer mode : Stand-by state
	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CTRUE,
				SDSpeed)) {
		pbl0fn->_dprintf("Card Clk rst fail\r\n");
		return CFALSE;
	}
	if (CFALSE == pbl0fn->NX_SDMMC_SelectCard(pSDXCBootStatus)) {
		pbl0fn->_dprintf("Card Sel Fail\r\n");
		return CFALSE;
	}

	//--------------------------------------------------------------------------
	// data transfer mode : Transfer state
	if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
		pbl0fn->NX_SDMMC_SetCardDetectPullUp(pSDXCBootStatus, CFALSE);
	}

	if (CFALSE == pbl0fn->NX_SDMMC_SetBlockLength(pSDXCBootStatus,
				BLOCK_LENGTH)) {
		pbl0fn->_dprintf("Set Blk Lng Fail\r\n");
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        pbl0fn->NX_SDMMC_SetClock(pSDXCBootStatus, CFALSE, SDXC_DIVIDER_400KHZ);

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectorData(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32		count;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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
			pbl0fn->_dprintf("Read left = %x\r\n", count);

			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
				pbl0fn->_dprintf("DRTO\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
				pbl0fn->_dprintf("EBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
				pbl0fn->_dprintf("SBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
				pbl0fn->_dprintf("DCRC\r\n");

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
			pbl0fn->_dprintf("HTO\r\n");
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	CBOOL	result = CFALSE;
	U32		status;
	#if defined(NX_DEBUG)
	U32	response;
	#endif
	NX_SDMMC_COMMAND cmd;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];
#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif

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
			pbl0fn->_dprintf("Auto Stop Resp Fail:%x\r\n", response);
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
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32		count;
	register struct NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
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
			pbl0fn->_dprintf("Read left = %x\r\n", count);

			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
				pbl0fn->_dprintf("DRTO\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
				pbl0fn->_dprintf("EBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
				pbl0fn->_dprintf("SBE\r\n");
			if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
				pbl0fn->_dprintf("DCRC\r\n");

			return CFALSE;
		}
#endif
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO) {
			if (count == 0) {
				pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;
				pbl0fn->_dprintf("DTO\r\n");
				break;
			}
		}

		#if defined(NX_DEBUG)
		if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO) {
			pbl0fn->_dprintf("HTO\r\n");
			pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HTO;
		}
		#endif

		NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
	}

	pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

	return CTRUE;
}
#endif


//------------------------------------------------------------------------------
CBOOL SDMMCBOOT(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	CBOOL	result = CFALSE;
	struct NX_SDMMC_RegisterSet * const pSDXCReg =
				pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
        
	if (CTRUE != pbl0fn->NX_SDMMC_Open(pSDXCBootStatus, option)) {
		pbl0fn->_dprintf("device open fail\r\n");
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
		pbl0fn->_dprintf("MBR read fail.\r\n");
		goto error;
	}

	/* int ret; */
	/* ret = pbl0fn->is_gpt_part((unsigned char *)psector); */

	/* if (pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == CFALSE) { */
	/* 	pbl0fn->_dprintf("%s Header read fail\r\n", ret ? "Boot" : "GPT"); */
	/* 	goto error; */
	/* } */

	/* if (!ret) { */
	/* 	ret = pbl0fn->is_gpt_valid((unsigned char *)psector); */
	/* 	if (!ret) { */
	/* 		rsn = pbl0fn->get_first_empty_lba((unsigned char *)psector); */
	/* 		pbl0fn->_dprintf("read bootimage from sector %d.\r\n", rsn); */
	/* 		if (pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, */
	/* 				psector) == CFALSE) { */
	/* 			pbl0fn->_dprintf("Boot Header read fail.\r\n"); */
	/* 			goto error; */
	/* 		} */
	/* 	} else */
	/* 		pbl0fn->_dprintf("invalid GPT.\r\n"); */
	/* } else */
	/* 	pbl0fn->_dprintf("not GPT, read header from sector 1.\r\n"); */
        
	/* unsigned int iv[4]; */
	/* iv[0] = (*pbl0fn->iv)[0]; */
	/* iv[1] = (*pbl0fn->iv)[1]; */
	/* iv[2] = (*pbl0fn->iv)[2]; */
	/* iv[3] = (*pbl0fn->iv)[3]; */

	struct nx_bootinfo *pbi = &(*pbl0fn->pbm)->bi;
	U32 BootSize = pbi->LoadSize;
	pbl0fn->_dprintf("LoadAddr:%x LoadSize:%x StartAddr:%x\r\n",
		pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr);

	if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK)
		BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;


	U32 sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
	unsigned int tdata[1024 / 4];
       	unsigned int *pdata = (unsigned int *)(*pbl0fn->pbm)->image;
	result = pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn, 2, tdata);
	rsn += 2;

	/* pbl0fn->nx_memcpy(&(*pbl0fn->pbm)->rsa_public, tdata, sizeof(struct asymmetrickey)); */
	/* pbl0fn->nx_memcpy(pdata, tdata, 1024 - sizeof(struct asymmetrickey)); */

	/*===========================================================
	 * boot key certify
	 * ========================================================*/

	result = pbl0fn->NX_SDMMC_ReadSectors(pSDXCBootStatus,
			rsn, sectorsize, &pdata[256]);

error:
	return result;
}

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

void NX_SDPADSetALT(U32 PortNum)
{

	NXBL0FN *pbl0fn = Getbl0fnPtr();

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
	pbl0fn->setpad(sdmmcpad[PortNum], 10, 1);
}

void NX_SDPADSetGPIO(U32 PortNum)
{

	NXBL0FN *pbl0fn = Getbl0fnPtr();
#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif
	pbl0fn->setpad(sdmmcpad[PortNum], 10, 0);
}

//------------------------------------------------------------------------------
U32 iSDXCBOOT(U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	SDXCBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
	CBOOL	result = CFALSE;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[SUKER-bootrom] %s start\n",__func__);
#endif

	pSDXCBootStatus = &SDXCBootStatus;

	pSDXCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

	if (pSDXCBootStatus->SDPort >= 3) {
		pSDXCBootStatus->SDPort = 0;
		pSDXCBootStatus->bHighSpeed = CTRUE;
	} else
		pSDXCBootStatus->bHighSpeed = CFALSE;

#ifndef QEMU_RISCV
	pbl0fn->NX_SDPADSetALT(pSDXCBootStatus->SDPort);
#endif
        
#ifdef DEBUG
        _dprintf("[SUKER-bootrom] pSDXCBootStatus = 0x%x\n",pSDXCBootStatus);
#endif        
	pbl0fn->NX_SDMMC_Init(pSDXCBootStatus);

        //	pbl0fn->_dprintf("SD boot:%d\r\n", pSDXCBootStatus->SDPort);
#ifdef QEMU_RISCV
        pbl0fn->_dprintf("[SUKER-bootrom] %s : SDBOOT = 0x%lx\n",__func__,pSDXCBootStatus->SDPort);
#endif
	result = pbl0fn->SDMMCBOOT(pSDXCBootStatus, option);

	pbl0fn->NX_SDMMC_Close(pSDXCBootStatus);
	pbl0fn->NX_SDMMC_Terminate(pSDXCBootStatus);

#ifndef QEMU_RISCV        
	pbl0fn->NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);
#endif

#ifdef QEMU_RISCV
        pbl0fn->_dprintf("[SUKER-bootrom] %s end\n",__func__);
        pbl0fn->kputs("------------------------------------");
#endif
	return result;
}

