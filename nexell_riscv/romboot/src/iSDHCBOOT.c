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
#include "include/nx_bootheader.h"

#ifdef QEMU_RISCV
#include "kprintf.h"
#include "test.h"
#else
#include "printf.h"
#endif

#include "include/nx_sdmmc.h"

#include "iSDHCBOOT.h"

#include "include/nx_chip_sfr.h"
#include "include/nx_cmu.h"

#define NX_ASSERT(x)

extern struct nx_bootmm * const pbm;

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

NX_SDMMC_RegisterSet * const pgSDXCReg[3] =
{
    (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC0_MODULE,
    (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC1_MODULE,
    (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC2_MODULE
};

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetClock(SDXCBOOTSTATUS *pSDXCBootStatus,
				CBOOL enb, U32 divider)
{
#ifndef QEMU_RISCV
    volatile U32 timeout;
#endif
    U32 i = pSDXCBootStatus->SDPort;
        
    register NX_SDMMC_RegisterSet * const pSDXCReg =
					pgSDXCReg[i];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif
        
	//--------------------------------------------------------------------------
	// 1. Confirm that no card is engaged in any transaction.
	//	If there's a transaction, wait until it has been finished.
//	while(NX_SDXC_IsDataTransferBusy())
//		;
//	while(NX_SDXC_IsCardDataBusy())
//		;

#if defined(DEBUG)
    if (pSDXCReg->STATUS & (NX_SDXC_STATUS_DATABUSY | NX_SDXC_STATUS_FSMBUSY))
    {
	if (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY)
            _dprintf("<<bootrom>>%s : ERROR - Data is busy\r\n", __func__);

	if (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY)
            _dprintf("<<bootrom>>%s : ERROR - Data Transfer is busy\n", __func__);

	//return CFALSE;
	INFINTE_LOOP();
    }
#endif

    //--------------------------------------------------------------------------
    // 2. Disable the output clock.
    // low power mode & clock disable
    pSDXCReg->CLKENA |= NX_SDXC_CLKENA_LOWPWR;
    pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_CLKENB;

#ifndef QEMU_RISCV    
    if (divider == SDXC_DIVIDER_400KHZ)
        setdeviceclock(&sdmmcclk[i][2], 1, 1);
    if (divider == SDXC_CLKDIV_LOW)
        setdeviceclock(&sdmmcclk[i][1], 1, 1);
#else
    _dprintf("<<bootrom>>%s : setdeviceclock cannot setting on QEMU mode\n", __func__);
#endif
    
    pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_LOWPWR;	// low power mode & clock disable


    //--------------------------------------------------------------------------
    // 4. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
#ifndef QEMU_RISCV    
 repeat_4 :
#endif    
    pSDXCReg->CMD = 0 | NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_UPDATECLKONLY |
        NX_SDXC_CMDFLAG_STOPABORT;

    //--------------------------------------------------------------------------
    // 5. Wait until a update clock command is taken by the SDXC module.
    //	If a HLE is occurred, repeat 4.
#ifndef QEMU_RISCV
    timeout = 0;
    while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
        if (++timeout > NX_SDMMC_TIMEOUT) {
            _dprintf("<<bootrom>> %s, clk TO\r\n",__func__);
            INFINTE_LOOP();
            return CFALSE;
        }
    }
    if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
        INFINTE_LOOP();
        pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
        goto repeat_4;
    }
#endif
    if (CFALSE == enb)
        return CTRUE;

    //--------------------------------------------------------------------------
    // 6. Enable the output clock.
    pSDXCReg->CLKENA |= NX_SDXC_CLKENA_CLKENB;

	//--------------------------------------------------------------------------
	// 7. Start a command with NX_SDXC_CMDFLAG_UPDATECLKONLY flag.
#ifndef QEMU_RISCV
repeat_7 :
#endif
	pSDXCReg->CMD = 0 | NX_SDXC_CMDFLAG_STARTCMD |
		NX_SDXC_CMDFLAG_UPDATECLKONLY |
		NX_SDXC_CMDFLAG_STOPABORT;

	//--------------------------------------------------------------------------
	// 8. Wait until a update clock command is taken by the SDXC module.
	//	If a HLE is occurred, repeat 7.
#ifndef QEMU_RISCV        
	timeout = 0;
	while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
		if (++timeout > NX_SDMMC_TIMEOUT) {
			_dprintf("clk2 TO\r\n");
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

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

	return CTRUE;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommandInternal(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		NX_SDMMC_COMMAND *pCommand)
{
    U32		cmd, flag;
    U32		status = 0;
#ifndef QEMU_RISCV    
    volatile U32	timeout;
#endif    
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

    NX_ASSERT(CNULL != pCommand);

    cmd	= pCommand->cmdidx & 0xFF;
    flag	= pCommand->flag;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/*     _dprintf("<<bootrom>> %s cmd = 0x%x\n",__func__,cmd); */
/* #endif */

    pSDXCReg->RINTSTS = 0xFFFFFFFF;

    //--------------------------------------------------------------------------
    // Send Command
#ifndef QEMU_RISCV
    timeout = 0;
    do {
        pSDXCReg->RINTSTS	= NX_SDXC_RINTSTS_HLE;
        pSDXCReg->CMDARG	= pCommand->arg;
        pSDXCReg->CMD		= cmd | flag |
            NX_SDXC_CMDFLAG_USE_HOLD_REG;
        while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
            if (++timeout > NX_SDMMC_TIMEOUT) {
                _dprintf("TO send cmd\r\n");
                status |= NX_SDMMC_STATUS_CMDBUSY;
                INFINTE_LOOP();
                goto End;
            }
        }
    } while (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE);
#else
    do {
        pSDXCReg->RINTSTS	= NX_SDXC_RINTSTS_HLE;
        pSDXCReg->CMDARG	= pCommand->arg;
        pSDXCReg->CMD		= cmd | flag |  NX_SDXC_CMDFLAG_USE_HOLD_REG;
    } while (0);
#endif
    //--------------------------------------------------------------------------
    // Wait until Command sent to card and got response from card.
#ifndef QEMU_RISCV
    timeout = 0;
    while (1) {
        if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_CD)
            break;

        if (++timeout > NX_SDMMC_TIMEOUT) {
            _dprintf("TO cmd done\r\n");
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
#endif
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
#ifndef QEMU_RISCV
            timeout = 0;
            do {
                if (++timeout > NX_SDMMC_TIMEOUT) {
                    _dprintf("TO card data ready\r\n");
                    status |= NX_SDMMC_STATUS_DATABUSY;
                    INFINTE_LOOP();
                    goto End;
                }
            } while (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY);
#endif
        }
    }

#ifndef QEMU_RISCV
End:
    if (NX_SDMMC_STATUS_NOERROR != status) {
        //		_dprintf("err: cmd:%x, arg:%x => sts:%x, resp:%x\r\n",
        //			pCommand->cmdidx, pCommand->arg,
        //			status, pCommand->response[0]);
    }
#endif
    pCommand->status = status;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s pCommand->status 0x%x\n",__func__,status); */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */

    return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendStatus(SDXCBOOTSTATUS *pSDXCBootStatus)
{
    U32 status;
    NX_SDMMC_COMMAND cmd;

    cmd.cmdidx	= SEND_STATUS;
    cmd.arg		= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/* #endif */
        
    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);

#ifdef DEBUG
    if (NX_SDMMC_STATUS_NOERROR == status)
        return status;

    //	_dprintf("err: cmd.resp:%x\r\n", cmd.response[0]);

    if (cmd.response[0] & (1UL << 31))
        _dprintf("\t\t ERROR : OUT_OF_RANGE\n");
    if (cmd.response[0] & (1UL << 30))
        _dprintf("\t\t ERROR : ADDRESS_ERROR\n");
    if (cmd.response[0] & (1UL << 29))
        _dprintf("\t\t ERROR : BLOCK_LEN_ERROR\n");
    if (cmd.response[0] & (1UL << 28))
        _dprintf("\t\t ERROR : ERASE_SEQ_ERROR\n");
    if (cmd.response[0] & (1UL << 27))
        _dprintf("\t\t ERROR : ERASE_PARAM\n");
    if (cmd.response[0] & (1UL << 26))
        _dprintf("\t\t ERROR : WP_VIOLATION\n");
    if (cmd.response[0] & (1UL << 24))
        _dprintf("\t\t ERROR : LOCK_UNLOCK_FAILED\n");
    if (cmd.response[0] & (1UL << 23))
        _dprintf("\t\t ERROR : COM_CRC_ERROR\n");
    if (cmd.response[0] & (1UL << 22))
        _dprintf("\t\t ERROR : ILLEGAL_COMMAND\n");
    if (cmd.response[0] & (1UL << 21))
        _dprintf("\t\t ERROR : CARD_ECC_FAILED\n");
    if (cmd.response[0] & (1UL << 20))
        _dprintf("\t\t ERROR : Internal Card Controller ERROR\n");
    if (cmd.response[0] & (1UL << 19))
        _dprintf("\t\t ERROR : General Error\n");
    if (cmd.response[0] & (1UL << 17))
        _dprintf("\t\t ERROR : Deferred Response\n");
    if (cmd.response[0] & (1UL << 16))
        _dprintf("\t\t ERROR : CID/CSD_OVERWRITE_ERROR\n");
    if (cmd.response[0] & (1UL << 15))
        _dprintf("\t\t ERROR : WP_ERASE_SKIP\n");
    if (cmd.response[0] & (1UL << 3))
        _dprintf("\t\t ERROR : AKE_SEQ_ERROR\n");

    switch ((cmd.response[0] >> 9) & 0xF) {
    case 0: _dprintf("\t\t CURRENT_STATE : Idle\n");
        break;
    case 1: _dprintf("\t\t CURRENT_STATE : Ready\n");
        break;
    case 2: _dprintf("\t\t CURRENT_STATE : Identification\n");
        break;
    case 3: _dprintf("\t\t CURRENT_STATE : Standby\n");
        break;
    case 4: _dprintf("\t\t CURRENT_STATE : Transfer\n");
        break;
    case 5: _dprintf("\t\t CURRENT_STATE : Data\n");
        break;
    case 6: _dprintf("\t\t CURRENT_STATE : Receive\n");
        break;
    case 7: _dprintf("\t\t CURRENT_STATE : Programming\n");
        break;
    case 8: _dprintf("\t\t CURRENT_STATE : Disconnect\n");
        break;
    case 9: _dprintf("\t\t CURRENT_STATE : Sleep\n");
        break;
    default: _dprintf("\t\t CURRENT_STATE : Reserved\n");
        break;
    }
#endif

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */

    return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommand(SDXCBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
    U32 status;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/* #endif */

    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, pCommand);
    if (NX_SDMMC_STATUS_NOERROR != status) {
        NX_SDMMC_SendStatus(pSDXCBootStatus);
    }

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */

    return status;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendAppCommand(SDXCBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
    U32 status;
    NX_SDMMC_COMMAND cmd;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/* #endif */
    
    cmd.cmdidx	= APP_CMD;
    cmd.arg		= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);
    if (NX_SDMMC_STATUS_NOERROR == status) {
        NX_SDMMC_SendCommand(pSDXCBootStatus, pCommand);
    }

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */
    
    return status;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_IdentifyCard(SDXCBOOTSTATUS *pSDXCBootStatus)
{
#ifndef QEMU_RISCV
    S32 timeout;
#endif
    U32 HCS, RCA;
    NX_SDMMC_CARDTYPE CardType = NX_SDMMC_CARDTYPE_UNKNOWN;
    NX_SDMMC_COMMAND cmd;
    NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif
        
    if (CFALSE == NX_SDMMC_SetClock(pSDXCBootStatus,
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

    NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

    // Check SD Card Version
    cmd.cmdidx	= SEND_IF_COND;
    // argument = VHS : 2.7~3.6V and Check Pattern(0xAA)
    cmd.arg		= (1 << 8) | 0xAA;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    if (NX_SDMMC_STATUS_NOERROR ==
        NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
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
#ifndef QEMU_RISCV
    timeout = NX_SDMMC_TIMEOUT_IDENTIFY;
#endif
    cmd.cmdidx	= APP_CMD;
    cmd.arg	= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    if (NX_SDMMC_STATUS_NOERROR ==
        NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
        //----------------------------------------------------------------------
        // SD memory card
#define FAST_BOOT	(1<<29)

        cmd.cmdidx	= SD_SEND_OP_COND;
        cmd.arg		= (HCS | FAST_BOOT | 0x00FC0000);	// 3.0 ~ 3.6V
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_WAITPRVDAT |
            NX_SDXC_CMDFLAG_SHORTRSP;

        if (NX_SDMMC_STATUS_NOERROR !=
            NX_SDMMC_SendCommandInternal(
                                                 pSDXCBootStatus, &cmd)) {
            return CFALSE;
        }
#ifndef QEMU_RISCV
        /* Wait until card has finished the power up routine */
        while (0==(cmd.response[0] & (1UL << 31))) {
            if (NX_SDMMC_STATUS_NOERROR !=
                NX_SDMMC_SendAppCommand(
						pSDXCBootStatus, &cmd)) {
                return CFALSE;
            }

            if (timeout-- <= 0) {
                _dprintf("TO pwrup SD\r\n");
                return CFALSE;
            }
        }
#endif
        _dprintf("SD ");

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

        NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
#ifndef QEMU_RISCV
        do {
            cmd.cmdidx	= SEND_OP_COND;
            cmd.arg		= 0x40FC0000;	// MMC High Capacity -_-???
            cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
                NX_SDXC_CMDFLAG_WAITPRVDAT |
                NX_SDXC_CMDFLAG_SHORTRSP;
            if (NX_SDMMC_STATUS_NOERROR !=
                NX_SDMMC_SendCommand(pSDXCBootStatus,
                                             &cmd)) {
                return CFALSE;
            }

            if (timeout-- <= 0) {
                _dprintf("TO to wait pow-up for MMC\r\n");
                return CFALSE;
            }
            /* Wait until card has finished the power up routine */
        } while (0==(cmd.response[0] & (1UL << 31)));
#endif
        _dprintf("MMC ");
#if defined(VERBOSE)
        _dprintf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]);
#endif

        CardType	= NX_SDMMC_CARDTYPE_MMC;
        RCA		= 2 << 16;
    }

    //	NX_ASSERT( (cmd.response[0] & 0x00FC0000) == 0x00FC0000 );
    pSDXCBootStatus->bHighCapacity =
        (cmd.response[0] & (1 << 30)) ? CTRUE : CFALSE;

    _dprintf("%sCard.\r\n",
                     (pSDXCBootStatus->bHighCapacity) ? "High Capacity " : "");

    //--------------------------------------------------------------------------
    // Get CID
    cmd.cmdidx	= ALL_SEND_CID;
    cmd.arg	= 0;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
                  NX_SDXC_CMDFLAG_WAITPRVDAT |
                  NX_SDXC_CMDFLAG_CHKRSPCRC |
                  NX_SDXC_CMDFLAG_LONGRSP;
    if (NX_SDMMC_STATUS_NOERROR !=
        NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd)) {
        _dprintf("cannot read CID\r\n");
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
        NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd)) {
        _dprintf("cannot read RCA\r\n");
        return CFALSE;
    }

    if (CardType == NX_SDMMC_CARDTYPE_SDMEM)
        pSDXCBootStatus->rca = cmd.response[0] & 0xFFFF0000;
    else
        pSDXCBootStatus->rca = RCA;

    pSDXCBootStatus->CardType = CardType;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif
    
    return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectCard(SDXCBOOTSTATUS *pSDXCBootStatus)
{
    U32 status;
    NX_SDMMC_COMMAND cmd;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif
    
    cmd.cmdidx	= SELECT_CARD;
    cmd.arg		= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif
    
    return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetCardDetectPullUp(SDXCBOOTSTATUS *pSDXCBootStatus,
					CBOOL bEnb)
{
    U32 status;
    NX_SDMMC_COMMAND cmd;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif
   
    cmd.cmdidx	= SET_CLR_CARD_DETECT;
    cmd.arg		= (bEnb) ? 1 : 0;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    status = NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif
    
    return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBusWidth(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 buswidth )
{
    U32 status;
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif

    NX_ASSERT( buswidth==1 || buswidth==4 );

    if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
        cmd.cmdidx	= SET_BUS_WIDTH;
        cmd.arg		= (buswidth >> 1);
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_WAITPRVDAT |
            NX_SDXC_CMDFLAG_CHKRSPCRC |
            NX_SDXC_CMDFLAG_SHORTRSP;
        status = NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);
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
        status = NX_SDMMC_SendCommand( pSDXCBootStatus, &cmd );
    }

    if (NX_SDMMC_STATUS_NOERROR != status)
        return CFALSE;

    /* 0 : 1-bit mode, 1 : 4-bit mode */
    pSDXCReg->CTYPE = buswidth >> 2;
    pSDXCReg->TIEMODE = buswidth >> 2;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

    return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBlockLength(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 blocklength)
{
    U32 status;
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif

    cmd.cmdidx	= SET_BLOCKLEN;
    cmd.arg		= blocklength;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;
    status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

    if (NX_SDMMC_STATUS_NOERROR != status)
        return CFALSE;

    pSDXCReg->BLKSIZ = blocklength;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

    return CTRUE;
}

#if 0
//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectPartition(SDXCBOOTSTATUS *pSDXCBootStatus,
					U32 partition)
{
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

	status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

	if (NX_SDMMC_STATUS_NOERROR != status) {
		return CFALSE;
	}

	if (cmd.response[0] & DEFERRED_RESPONSE) {
		status = NX_SDMMC_SendStatus(pSDXCBootStatus);
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
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[i];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif
        
    setdeviceclock(&sdmmcclk[i][0], 2, 1);

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
#ifndef QEMU_RISCV
    while (pSDXCReg->CTRL & (NX_SDXC_CTRL_DMARST |
                             NX_SDXC_CTRL_FIFORST |
                             NX_SDXC_CTRL_CTRLRST))
        pSDXCReg->CTRL;
#endif
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

    // Wake up & Power on fifo sram
    pSDXCReg->TIESRAM = 0x3;
    pSDXCReg->TIEMODE = 1;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

    return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Terminate(SDXCBOOTSTATUS *pSDXCBootStatus)
{
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];
    
#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif

    // Clear All interrupts
    pSDXCReg->RINTSTS = 0xFFFFFFFF;

    // Reset the controller & DMA interface & FIFO
    pSDXCReg->CTRL = NX_SDXC_CTRL_DMARST |
        NX_SDXC_CTRL_FIFORST |
        NX_SDXC_CTRL_CTRLRST;
#ifndef QEMU_RISCV        
    while (pSDXCReg->CTRL & (NX_SDXC_CTRL_DMARST |
                             NX_SDXC_CTRL_FIFORST |
                             NX_SDXC_CTRL_CTRLRST))
        pSDXCReg->CTRL;
#endif

    setdeviceclock(&sdmmcclk[pSDXCBootStatus->SDPort][0], 2, 0);

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

    return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Open(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
    U32 SDSpeed;

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif

#ifdef QEMU_RISCV
    return CTRUE;
#endif

    if (pSDXCBootStatus->bHighSpeed == CTRUE)
        SDSpeed = SDXC_CLKDIV_HIGH;
    else
        SDSpeed = SDXC_CLKDIV_LOW;
    //--------------------------------------------------------------------------
    // card identification mode : Identify & Initialize
    if (CFALSE == NX_SDMMC_IdentifyCard(pSDXCBootStatus)) {
        _dprintf("Identify Fail\r\n");
        return CFALSE;
    }

    //--------------------------------------------------------------------------
    // data transfer mode : Stand-by state
    if (CFALSE == NX_SDMMC_SetClock(pSDXCBootStatus, CTRUE,
                                            SDSpeed)) {
        _dprintf("Card Clk rst fail\r\n");
        return CFALSE;
    }
    if (CFALSE == NX_SDMMC_SelectCard(pSDXCBootStatus)) {
        _dprintf("Card Sel Fail\r\n");
        return CFALSE;
    }

    //--------------------------------------------------------------------------
    // data transfer mode : Transfer state
    if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
        NX_SDMMC_SetCardDetectPullUp(pSDXCBootStatus, CFALSE);
    }

    if (CFALSE == NX_SDMMC_SetBlockLength(pSDXCBootStatus,
                                                  BLOCK_LENGTH)) {
        _dprintf("Set Blk Lng Fail\r\n");
        return CFALSE;
    }

    NX_SDMMC_SetBusWidth(pSDXCBootStatus, 4);

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n",__func__);
#endif

    return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Close(SDXCBOOTSTATUS *pSDXCBootStatus)
{
    NX_SDMMC_SetClock(pSDXCBootStatus, CFALSE, SDXC_DIVIDER_400KHZ);

#ifdef DEBUG
    _dprintf("<<bootrom>> %s\n",__func__);
#endif
        
    return CTRUE;
}

//------------------------------------------------------------------------------
#ifdef QEMU_RISCV
CBOOL NX_SDMMC_ReadSectorData(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		U32 numberOfSector,
                U32 step,
		U32 *pdwBuffer)
{
    U32	count;
    U32 i = 0;
    U32 *temp;
    U32 seek_qemu = step * 512;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

    if (seek_qemu > sd_body_size)
        return CFALSE;
    
#ifdef DEBUG
    _dprintf("<<bootrom>> ---- start ---- %s\n",__func__);
    _dprintf("<<bootrom>> ---------------------------------------\n");
    _dprintf("<<bootrom>> %s pdwBuffer start addr = 0x%x\n",__func__, pdwBuffer);
    _dprintf("<<bootrom>> ---------------------------------------\n");
#endif
        
    NX_ASSERT(0 == ((U32)pdwBuffer & 3));

    count = numberOfSector * BLOCK_LENGTH;
    NX_ASSERT(0 == (count % 32));

#ifdef DEBUG
    _dprintf("<<bootrom>> --- test binary write check -------\n",numberOfSector);
    _dprintf("<<bootrom>> %s seek number1 : 0x%x\n",__func__,seek_qemu);
    _dprintf("<<bootrom>> %s BOOT_STATUS : 0x%x\n",__func__,pSDXCReg->STATUS);    
    _dprintf("<<bootrom>> %s sd_body check: 0x%x\n",__func__,sd_body);
    _dprintf("<<bootrom>> %s sd bin size : 0x%x\n\n",__func__,sd_body_size);
    _dprintf("**** Original sdboot.bin binary data ****\n");
    for(U32 j = seek_qemu; j < count; j++) {
        _dprintf("%c",(U8)(sd_body[j]));
    }
    _dprintf("*****************************************\n\n");
    _dprintf("<<bootrom>>********copy to pdwbuffer*******\n",__func__);
#endif

    while (0 < count) {
        *pdwBuffer++ = (U32)sd_body[i+seek_qemu];
        _dprintf("%c",(U8)*pdwBuffer);
        count -= 1;
        i++;

        NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
    }
    
    pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

    _dprintf("<<bootrom>>*******************************\n",__func__);
    _dprintf("\n\n<<bootrom>> %s ---- end ----\n\n",__func__);

    return CTRUE;
}
#else //QEMU_RISCV
CBOOL NX_SDMMC_ReadSectorData(
		SDXCBOOTSTATUS *pSDXCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
    U32		count;
    register NX_SDMMC_RegisterSet * const pSDXCReg =
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
                _dprintf("%u",pSDXCReg->DATA);
                FSize--;
            }

            pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_RXDR;
        }

        // Check Errors
        if (pSDXCReg->RINTSTS & (NX_SDXC_RINTSTS_DRTO |
                                 NX_SDXC_RINTSTS_EBE |
                                 NX_SDXC_RINTSTS_SBE |
                                 NX_SDXC_RINTSTS_DCRC)) {
            _dprintf("Read left = %x\r\n", count);

            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
                _dprintf("DRTO\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
                _dprintf("EBE\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
                _dprintf("SBE\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
                _dprintf("DCRC\r\n");

            return CFALSE;
        }

        if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO) {
            if (count == 0) {
                pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;
                break;
            }
        }

        if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO) {
            _dprintf("HTO\r\n");
            pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HTO;
        }

        NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
    }
    
    pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

    return CTRUE;
}
#endif //QEMU_RISCV

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectors(SDXCBOOTSTATUS *pSDXCBootStatus,
			U32 SectorNum, U32 numberOfSector, U32 *pdwBuffer)
{
    CBOOL	result = CFALSE;
    U32		status;
#if defined(QEMU_RISCV) && defined(DEBUG)
    U32	response;
#endif
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];
#ifdef DEBUG
    _dprintf("<<bootrom>> %s ---------------start----------------\n",__func__);
    _dprintf("<<bootrom>> %s bootStatus           rca: 0x%x\n",__func__,pSDXCBootStatus->rca);
    _dprintf("<<bootrom>> %s bootStatus bHighCapacity: 0x%x\n",__func__,pSDXCBootStatus->bHighCapacity);
    _dprintf("<<bootrom>> %s bootStatus        SDPort: 0x%x\n",__func__,pSDXCBootStatus->SDPort);
    _dprintf("<<bootrom>> %s bootStatus    bHighSpeed: 0x%x\n",__func__,pSDXCBootStatus->bHighSpeed);
    _dprintf("<<bootrom>> %s bootStatus      CardType: 0x%x\n",__func__,pSDXCBootStatus->CardType);    
    _dprintf("<<bootrom>> %s SectorNum: 0x%x\n",__func__,SectorNum);
    _dprintf("<<bootrom>> %s numberOfSector: 0x%x\n",__func__,numberOfSector);
    _dprintf("<<bootrom>> %s pdwBuffer addr: 0x%x\n",__func__,pdwBuffer);
    _dprintf("<<bootrom>> %s ----------------------------------\n\n",__func__);
#endif
        
    NX_ASSERT(0 == ((U32)pdwBuffer & 3));

    // wait while data busy or data transfer busy
    while (pSDXCReg->STATUS & (1 << 9 | 1 << 10))
        pSDXCReg->STATUS;

    //--------------------------------------------------------------------------
    // wait until 'Ready for data' is set and card is in transfer state.
#ifndef QEMU_RISCV
    do {
        cmd.cmdidx	= SEND_STATUS;
        cmd.arg		= pSDXCBootStatus->rca;
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_CHKRSPCRC |
            NX_SDXC_CMDFLAG_SHORTRSP;
        status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
        if (NX_SDMMC_STATUS_NOERROR != status)
            goto End;
    } while (!((cmd.response[0] & (1 << 8)) &&
               (((cmd.response[0] >> 9) & 0xF) == 4)));
#else
    cmd.cmdidx	= SEND_STATUS;
        cmd.arg		= pSDXCBootStatus->rca;
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_CHKRSPCRC |
            NX_SDXC_CMDFLAG_SHORTRSP;
        status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
        //    (*qemu_extern_var)[0] = SectorNum;
    //    _dprintf("<<bootrom>> %s qemu_extern_var 0: 0x%x\n",__func__,(*qemu_extern_var)[0]);
        //    _dprintf("<<bootrom>> %s qemu_extern_var 0: 0x%x\n",__func__,(*qemu_extern_var)[0]);
#endif //QEMU_RISCV
    
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

    status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

#ifndef QEMU_RISCV
    if (NX_SDMMC_STATUS_NOERROR != status)
        goto End;

    //--------------------------------------------------------------------------
    // Read data
    if (CTRUE != NX_SDMMC_ReadSectorData(pSDXCBootStatus,
                                                 numberOfSector, pdwBuffer))
        goto End;
#else
    if (CTRUE != NX_SDMMC_ReadSectorData(pSDXCBootStatus,
                                                 numberOfSector, SectorNum, pdwBuffer))
        goto End;
#endif //QEMU_RISCV

    NX_ASSERT(NX_SDXC_STATUS_FIFOEMPTY ==
              (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY));
    NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOFULL));
    NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOCOUNT));

#ifndef QEMU_RISCV
    if (numberOfSector > 1) {
        // Wait until the Auto-stop command has been finished.
        while (0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_ACD))
            pSDXCReg->RINTSTS;

        NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY));

#ifdef DEBUG
        // Get Auto-stop response and then check it.
        response = pSDXCReg->RESP[1];
        if (response & 0xFDF98008) {
            _dprintf("Auto Stop Resp Fail:%x\r\n", response);
            //goto End;
        }
#endif
    }
#endif //QEMU_RISCV
    result = CTRUE;

End:
    if (CFALSE == result) {
        //DateSheet 7.4.4.1 H/W Reset Programming Sequence
        cmd.cmdidx	= STOP_TRANSMISSION;
        cmd.arg		= 0;
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
                          NX_SDXC_CMDFLAG_CHKRSPCRC |
                          NX_SDXC_CMDFLAG_SHORTRSP |
                          NX_SDXC_CMDFLAG_STOPABORT;
        NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);


        if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
            pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;
#ifndef QEMU_RISCV            
            while (pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST)
                pSDXCReg->CTRL;
#endif //QEMU_RISCV
        }
    }

#ifdef DEBUG
    _dprintf("<<bootrom>> %s end\n\n",__func__);
#endif

    return result;
}

//------------------------------------------------------------------------------
CBOOL SDMMCBOOT(SDXCBOOTSTATUS *pSDXCBootStatus, U32 option)
{
    CBOOL	result = CFALSE;
    NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> %s start\n",__func__);
#endif

    if (CTRUE != NX_SDMMC_Open(pSDXCBootStatus, option)) {
        _dprintf("device open fail\r\n");
        //goto error;
        return CFALSE;
    }

    if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
#ifndef QEMU_RISCV
        volatile U32 tempcount = 0x100000;
#endif
        pSDXCReg->CTRL = NX_SDXC_CTRL_FIFORST;
        /* Wait until the FIFO reset is completed. */
#ifndef QEMU_RISCV                
        while ((pSDXCReg->CTRL & NX_SDXC_CTRL_FIFORST) &&
               tempcount--)
            pSDXCReg->CTRL;
#endif
    }

    unsigned int *psector = (unsigned int *)&(pbm)->bi;
    unsigned int rsn = 0;

#ifdef DEBUG
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>>%s: 1st ReadSectors\n",__func__);
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == CFALSE) {
        _dprintf("<<bootrom>> %s: MBR read fail.\n",__func__);
        goto error;
    }
    
#ifdef DEBUG    
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>>%s: 2nd ReadSectors\n",__func__);
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    int ret=1;
    //    ret = is_gpt_part((unsigned char *)psector);

    if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == CFALSE) {
    	_dprintf("%s Header read fail\r\n", ret ? "Boot" : "GPT");
    	goto error;
    }
    
#ifdef DEBUG
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>>%s: 3rd ReadSectors\n",__func__);
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    /* if (!ret) { */
    /* 	ret = is_gpt_valid((unsigned char *)psector); */
    /* 	if (!ret) { */
    /* 		rsn = get_first_empty_lba((unsigned char *)psector); */
    /* 		_dprintf("read bootimage from sector %d.\r\n", rsn); */
    /* 		if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, */
    /* 				psector) == CFALSE) { */
    /* 			_dprintf("Boot Header read fail.\r\n"); */
    /* 			goto error; */
    /* 		} */
    /* 	} else */
    /* 		_dprintf("invalid GPT.\r\n"); */
    /* } else */
    _dprintf("not GPT, read header from sector 1.\r\n");

#ifdef DEBUG
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    /* unsigned int iv[4]; */
    /* iv[0] = (*iv)[0]; */
    /* iv[1] = (*iv)[1]; */
    /* iv[2] = (*iv)[2]; */
    /* iv[3] = (*iv)[3]; */
    /* #if 1 */
    /* 	if (!headercheck(iv, option & 1 << DECRYPT)) { */
    /* 		return 0; */
    /* 	} */
    /* #else */
    /* 	if (option & 1 << DECRYPT) { */
    /* 		Decrypt((U32*)&(*pbm)->bi, */
    /* 				(U32*)&(*pbm)->bi, */
    /* 				iv, sizeof(struct nx_bootinfo)); */
    /* 	} */

    /* 	if (pbi->signature != HEADER_ID) { */
    /* 		_dprintf("not bootable image(%08x).\r\n", */
    /* 				pbi->signature); */
    /* 		return 0; */
    /* 	} */
    /* #endif */

    struct nx_bootinfo *pbi = &(pbm)->bi;
    U32 BootSize = pbi->LoadSize;
#ifdef DEBUG
    _dprintf("<<bootrom>> %s pbi addr : 0x%x \n",__func__, pbi);
    _dprintf("<<bootrom>> %s LoadAddr:%x LoadSize:%x StartAddr:%x\r\n",__func__,
                     pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr);
#endif
    if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK)
        BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;


    U32 sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
    unsigned int tdata[1024 / 4];
    unsigned int *pdata = (unsigned int *)(pbm)->image;
    result = NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn, 2, tdata);
    rsn += 2;

    /* nx_memcpy(&(*pbm)->rsa_public, tdata, sizeof(struct asymmetrickey)); */
    /* nx_memcpy(pdata, tdata, 1024 - sizeof(struct asymmetrickey)); */

    /*===========================================================
     * boot key certify
     * ========================================================*/
    /* #if 1 */
    /* 	if (!keycheck(iv, option & 1 << DECRYPT, (option >> VERIFY) & 0x3)) { */
    /* 		ret = 0; */
    /* 		goto error; */
    /* 	} */
    /* #else */
    /* 	if (option & 1 << DECRYPT) { */
    /* 		Decrypt((U32*)&(*pbm)->rsa_public, */
    /* 				(U32*)&(*pbm)->rsa_public, */
    /* 				iv, sizeof(struct asymmetrickey)); */
    /* 	} */

    /* 	U32 ehash = (option >> VERIFY) & 0x3; */
    /* 	if (ehash) { */
    /* 		U32 bhash[32 / 4]; */
    /* 		mbedtls_sha256((*pbm)->rsa_public.rsapublicbootkey, */
    /* 			256, (unsigned char *)bhash, 0); */
    /* 		if (ehash == 1) */
    /* 			ehash--; */
    /* 		const U32 *pbootkeyhash = */
    /* 			&(*(*pbootkeyhash))[ehash][0]; */
    /* 		if (memcmp(bhash, pbootkeyhash, 32) != 0) { */
    /* 			_dprintf("boot key verify fail.\r\n"); */
    /* 			return 0; */
    /* 		} */
    /* 	} */
    /* #endif */

    result = NX_SDMMC_ReadSectors(pSDXCBootStatus,
                                          rsn, sectorsize, &pdata[256]);

    /* 	if (option & 1 << DECRYPT) { */
    /* #if 1 */
    /* 		imagedecrypt(iv); */
    /* #else */
    /* 		Decrypt((U32*)(*pbm)->image, */
    /* 				(U32*)(*pbm)->image, */
    /* 				iv, (*pbm)->bi.LoadSize); */
    /* #endif */
    /* 	} */
#ifdef DEBUG
    _dprintf("<<bootrom>> %s ----end----\n",__func__);
#endif
    return result;
    
 error:
#ifdef DEBUG
    _dprintf("<<bootrom>> %s: goto error ~!\n",__func__);
#endif
    return result;
}

void NX_SDPADSetALT(U32 PortNum)
{
    //    setpad(sdmmcpad[PortNum], 10, 1);
#ifdef DEBUG
    _dprintf("<<bootrom>> %s end \n",__func__);
#endif    
}

void NX_SDPADSetGPIO(U32 PortNum)
{
    //    setpad(sdmmcpad[PortNum], 10, 0);
#ifdef DEBUG
    _dprintf("<<bootrom>> %s end \n",__func__);
#endif    
}

//------------------------------------------------------------------------------
U32 iSDXCBOOT(U32 option)
{
    SDXCBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
    CBOOL	result = CFALSE;

#ifdef DEBUG    
    _dprintf("<<bootrom>>%s, start\n",__func__);
#endif
    
    pSDXCBootStatus = &SDXCBootStatus;

    pSDXCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

    if (pSDXCBootStatus->SDPort >= 3) {
        pSDXCBootStatus->SDPort = 0;
        pSDXCBootStatus->bHighSpeed = CTRUE;
    } else
        pSDXCBootStatus->bHighSpeed = CFALSE;

#ifndef QEMU_RISCV
    NX_SDPADSetALT(pSDXCBootStatus->SDPort);
#endif
    NX_SDMMC_Init(pSDXCBootStatus);

    _dprintf("<<bootrom>> SD boot:%d\r\n", pSDXCBootStatus->SDPort);
    result = SDMMCBOOT(pSDXCBootStatus, option);

    //    _dprintf("<<bootrom>> %s: SDMMCBOOT Done!! result=0x%x\n",__func__,result);
    NX_SDMMC_Close(pSDXCBootStatus);
    NX_SDMMC_Terminate(pSDXCBootStatus);

#ifndef QEMU_RISCV
    NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);
#endif
    
#ifdef DEBUG
    _dprintf("<<bootrom>> %s end \n",__func__);
#endif    

    return result;
}
