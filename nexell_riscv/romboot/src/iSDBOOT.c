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
#include <nx_swallow.h>
#include <nx_bootheader.h>

#if defined(QEMU_RISCV) || defined(SOC_SIM)
#include <nx_qemu_sim_printf.h>
#else
#include <nx_swallow_printf.h>
#endif

#ifdef QEMU_RISCV
#include <test.h>
#endif

#include <nx_sdmmc.h>
#include <nx_chip_iomux.h>
#include <nx_gpio.h>
#include <nx_clock.h>
#include <nx_lib.h>
#include <iSDBOOT.h>

#define NX_ASSERT(x)

//extern struct nx_bootmm * const pbm;

NX_SDMMC_RegisterSet * const pgSDXCReg[2] =
{
    (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC0_MODULE,
    (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC1_MODULE,
};

const union nxpad sdmmcpad[2][6] = {
{
    {PADINDEX_OF_OSDMMC0_CDATA_0_},
    {PADINDEX_OF_OSDMMC0_CDATA_1_},
    {PADINDEX_OF_OSDMMC0_CDATA_2_},
    {PADINDEX_OF_OSDMMC0_CDATA_3_},
    {PADINDEX_OF_OSDMMC0_CCLK},
    {PADINDEX_OF_OSDMMC0_CMD},
},
{
    {PADINDEX_OF_OSDMMC1_CDATA_0_},
    {PADINDEX_OF_OSDMMC1_CDATA_1_},
    {PADINDEX_OF_OSDMMC1_CDATA_2_},
    {PADINDEX_OF_OSDMMC1_CDATA_3_},
    {PADINDEX_OF_OSDMMC1_CCLK},
    {PADINDEX_OF_OSDMMC1_CMD},
}};

//------------------------------------------------------------------------------
int NX_SDMMC_SetClock(SDBOOTSTATUS *pSDXCBootStatus, int enb, int divider)
{
#ifdef QEMU_RISCV
    _dprintf("<<bootrom>>%s : nxSetDeviceClock cannot setting on QEMU mode\n", __func__);
    return 1;
#else
    
    volatile unsigned int timeout;
    unsigned int i = pSDXCBootStatus->SDPort;
        
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[i];

    //--------------------------------------------------------------------------
    // 1. Confirm that no card is engaged in any transaction.
    //	If there's a transaction, wait until it has been finished.

#if defined(DEBUG) && !defined(SOC_SIM)
    _dprintf("<<bootrom>> %s start\n",__func__);
    if (pSDXCReg->STATUS & (NX_SDXC_STATUS_DATABUSY | NX_SDXC_STATUS_FSMBUSY))
    {
	if (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY)
            _dprintf("<<bootrom>>%s : ERROR - Data is busy\r\n", __func__);

	if (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY)
            _dprintf("<<bootrom>>%s : ERROR - Data Transfer is busy\n", __func__);

	INFINTE_LOOP();
    }
#endif
    
    //--------------------------------------------------------------------------
    // 2. Disable the output clock.
    // low power mode & clock disable
    pSDXCReg->CLKENA |= NX_SDXC_CLKENA_LOWPWR;
    pSDXCReg->CLKENA &= ~NX_SDXC_CLKENA_CLKENB;
    //    nxSetDeviceClock(&sdmmcclk[i][1], 1, 1);

    if (divider == SDCLK_DIVIDER_400KHZ) {
        pSDXCReg->CLKDIV = (256 >> 1);
        //400KHZ
        //pSDXCReg->CLKSRC = 0;
        //todo
    }
    else {
        pSDXCReg->CLKDIV = 2;//(64 >> 1);
    //pSDXCReg->CLKSRC = 1;
        //100MHZ
        //todo
    }

    //    pSDXCReg->CLKSRC = (unsigned int)divider;
    
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
            INFINTE_LOOP();
            return 0;
        }
    }
    if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
        INFINTE_LOOP();
        pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
        goto repeat_4;
    }
    if (0 == enb)
        return 1;

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
			INFINTE_LOOP();
			return 0;
		}
	}
	if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HLE) {
		INFINTE_LOOP();
		pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HLE;
		goto repeat_7;
	}

	return 1;
#endif //QEMU_RISCV        
}

//------------------------------------------------------------------------------
unsigned int NX_SDMMC_SendCommandInternal(
		SDBOOTSTATUS *pSDXCBootStatus,
		NX_SDMMC_COMMAND *pCommand)
{
    unsigned int		cmd, flag;
    unsigned int		status = 0;
#ifndef QEMU_RISCV    
    volatile unsigned int	timeout;
#endif    
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

    NX_ASSERT(CNULL != pCommand);

    cmd	= pCommand->cmdidx & 0xFF;
    flag	= pCommand->flag;

    pSDXCReg->RINTSTS = 0xFFFFFFFF;

#ifdef DEBUG
    _dprintf("<<bootrom>> SendCommandInternal step1\n");
#endif

    //--------------------------------------------------------------------------
    // Send Command
#ifndef QEMU_RISCV
    timeout = 0;
    do {
        pSDXCReg->RINTSTS	= NX_SDXC_RINTSTS_HLE;
        pSDXCReg->CMDARG	= pCommand->arg;
        pSDXCReg->CMD		= cmd | flag | NX_SDXC_CMDFLAG_USE_HOLD_REG;

        while (pSDXCReg->CMD & NX_SDXC_CMDFLAG_STARTCMD) {
            _dprintf("*.");
            if (++timeout > NX_SDMMC_TIMEOUT) {
                //#ifndef SOC_SIM                
                _dprintf("TO send cmd\r\n");
                //#endif
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
            //#ifndef SOC_SIM
            _dprintf("TO cmd done\r\n");
            //#endif
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

#ifdef DEBUG
    _dprintf("<<bootrom>> SendCommandInternal step2\n");
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
#ifndef SOC_SIM
                    _dprintf("TO card data ready\r\n");
#endif
                    status |= NX_SDMMC_STATUS_DATABUSY;
                    INFINTE_LOOP();
                    goto End;
                }
            } while (pSDXCReg->STATUS & NX_SDXC_STATUS_DATABUSY);
#endif
        }
    }

 End:
/* #if !defined(QEMU_RISCV) && defined(DEBUG) */
/* End: */
/*     if (NX_SDMMC_STATUS_NOERROR != status) { */
/*         		_dprintf("err: cmd:%x, arg:%x => sts:%x, resp:%x\r\n", */
/*         			pCommand->cmdidx, pCommand->arg, */
/*         			status, pCommand->response[0]); */
/*     } */
/* #endif */
    pCommand->status = status;

    return status;
}

//------------------------------------------------------------------------------
unsigned int NX_SDMMC_SendStatus(SDBOOTSTATUS *pSDXCBootStatus)
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;

    cmd.cmdidx	= SEND_STATUS;
    cmd.arg	= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

#ifdef DEBUG
    _dprintf("<<bootrom>> SendStatus: cmd = 0x%x\n",&cmd);
#endif

    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);

    if (NX_SDMMC_STATUS_NOERROR == status)
        return status;

#if defined(DEBUG) //&& !defined(SOC_SIM)
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

    return status;
}

//------------------------------------------------------------------------------
unsigned int NX_SDMMC_SendCommand(SDBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
    unsigned int status;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> SendCommand\n"); */
/* #endif */
    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, pCommand);
    if (NX_SDMMC_STATUS_NOERROR != status) {
        NX_SDMMC_SendStatus(pSDXCBootStatus);
    }

    return status;
}

//------------------------------------------------------------------------------
unsigned int NX_SDMMC_SendAppCommand(SDBOOTSTATUS *pSDXCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;
    
    cmd.cmdidx	= APP_CMD;
    cmd.arg		= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

#ifdef DEBUG
    _dprintf("<<bootrom>> SendAppCommand\n");
#endif

    status = NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd);
    if (NX_SDMMC_STATUS_NOERROR == status) {
        NX_SDMMC_SendCommand(pSDXCBootStatus, pCommand);
    }
    
    return status;
}

//------------------------------------------------------------------------------
int NX_SDMMC_IdentifyCard(SDBOOTSTATUS *pSDXCBootStatus)
{
#ifndef QEMU_RISCV
    int timeout;
#endif
    unsigned int HCS, RCA;
    NX_SDMMC_CARDTYPE CardType = NX_SDMMC_CARDTYPE_UNKNOWN;
    NX_SDMMC_COMMAND cmd;
    NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> IdentifyCard start\n");
#endif
        
    if (0 == NX_SDMMC_SetClock(pSDXCBootStatus,1,SDCLK_DIVIDER_400KHZ))
        return 0;

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

#ifdef DEBUG
    _dprintf("<<bootrom>> IdentifyCard: cmd = 0x%x\n",&cmd);
    _dprintf("<<bootrom>> IdentifyCard: cmd.flag = 0x%x\n",cmd.flag);
#endif
    NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

    // Check SD Card Version
    cmd.cmdidx	= SEND_IF_COND;
    // argument = VHS : 2.7~3.6V and Check Pattern(0xAA)
    cmd.arg		= (1 << 8) | 0xAA;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

#ifdef DEBUG
    _dprintf("<<bootrom>> IdentifyCard step2: cmd = 0x%x\n",&cmd);
#endif

    if (NX_SDMMC_STATUS_NOERROR ==
        NX_SDMMC_SendCommandInternal(pSDXCBootStatus, &cmd)) {
        // Ver 2.0 or later SD Memory Card
        if (cmd.response[0] != ((1 << 8) | 0xAA))
            return 0;

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
            return 0;
        }
#ifndef QEMU_RISCV
        /* Wait until card has finished the power up routine */
        while (0==(cmd.response[0] & (1UL << 31))) {
            if (NX_SDMMC_STATUS_NOERROR !=
                NX_SDMMC_SendAppCommand(
						pSDXCBootStatus, &cmd)) {
                return 0;
            }

            if (timeout-- <= 0) {
                //                _dprintf("TO pwrup SD\r\n");
                return 0;
            }
        }
#endif
#ifdef DEBUG
        _dprintf("---- SD ----\n");
#endif
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
                return 0;
            }

            if (timeout-- <= 0) {
                //                _dprintf("TO to wait pow-up for MMC\r\n");
                return 0;
            }
            /* Wait until card has finished the power up routine */
        } while (0==(cmd.response[0] & (1UL << 31)));
#endif

#if defined(DEBUG)
        _dprintf("---- MMC ----\n");
        //        _dprintf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]);
#endif

        CardType	= NX_SDMMC_CARDTYPE_MMC;
        RCA		= 2 << 16;
    }

    //	NX_ASSERT( (cmd.response[0] & 0x00FC0000) == 0x00FC0000 );
    pSDXCBootStatus->bHighCapacity =
        (cmd.response[0] & (1 << 30)) ? 1 : 0;

    //    _dprintf("%sCard.\r\n", (pSDXCBootStatus->bHighCapacity) ? "High Capacity " : "");

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
        //        _dprintf("cannot read CID\r\n");
        return 0;
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
        //        _dprintf("cannot read RCA\r\n");
        return 0;
    }

    if (CardType == NX_SDMMC_CARDTYPE_SDMEM)
        pSDXCBootStatus->rca = cmd.response[0] & 0xFFFF0000;
    else
        pSDXCBootStatus->rca = RCA;

    pSDXCBootStatus->CardType = CardType;

#if defined(DEBUG)
    _dprintf("<<bootrom>> CardType = 0x%x\n",CardType);
        //        _dprintf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]);
#endif
    
    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_SelectCard(SDBOOTSTATUS *pSDXCBootStatus)
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/* #endif */
    
    cmd.cmdidx	= SELECT_CARD;
    cmd.arg		= pSDXCBootStatus->rca;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */
    
    return (NX_SDMMC_STATUS_NOERROR == status) ? 1 : 0;
}

//------------------------------------------------------------------------------
int NX_SDMMC_SetCardDetectPullUp(SDBOOTSTATUS *pSDXCBootStatus,
					int bEnb)
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;

    cmd.cmdidx	= SET_CLR_CARD_DETECT;
    cmd.arg		= (bEnb) ? 1 : 0;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;

    status = NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);
    
    return (NX_SDMMC_STATUS_NOERROR == status) ? 1 : 0;
}

//------------------------------------------------------------------------------
int NX_SDMMC_SetBusWidth(SDBOOTSTATUS *pSDXCBootStatus,
					unsigned int buswidth )
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

    NX_ASSERT( buswidth==1 || buswidth==4 );

    if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
#ifdef DEBUG
    _dprintf("<<bootrom>> BootStatus CardType NX_SDMMC_CARDTYPE_SDMEM\n");
#endif
        cmd.cmdidx	= SET_BUS_WIDTH;
        cmd.arg		= (buswidth >> 1);
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_WAITPRVDAT |
            NX_SDXC_CMDFLAG_CHKRSPCRC |
            NX_SDXC_CMDFLAG_SHORTRSP;
        status = NX_SDMMC_SendAppCommand(pSDXCBootStatus, &cmd);
    } else {
#ifdef DEBUG
    _dprintf("<<bootrom>> BootStatus CardType another\n");
#endif
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
        return 0;

    /* 0 : 1-bit mode, 1 : 4-bit mode */
    pSDXCReg->CTYPE = buswidth >> 2;
    pSDXCReg->TIEMODE = buswidth >> 2;

    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_SetBlockLength(SDBOOTSTATUS *pSDXCBootStatus,
					unsigned int blocklength)
{
    unsigned int status;
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s start\n",__func__); */
/* #endif */

    cmd.cmdidx	= SET_BLOCKLEN;
    cmd.arg		= blocklength;
    cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
        NX_SDXC_CMDFLAG_WAITPRVDAT |
        NX_SDXC_CMDFLAG_CHKRSPCRC |
        NX_SDXC_CMDFLAG_SHORTRSP;
    status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);

    if (NX_SDMMC_STATUS_NOERROR != status)
        return 0;

    pSDXCReg->BLKSIZ = blocklength;

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s end\n",__func__); */
/* #endif */

    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_Init(SDBOOTSTATUS *pSDXCBootStatus)
{
    unsigned int i = pSDXCBootStatus->SDPort;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[i];

#ifdef DEBUG
    _dprintf("<<bootrom>> Init start \r\n");
#endif

    //nxSetDeviceClock(&sdmmcclk[i][0], 2, 1);

    pSDXCReg->PWREN = 0 << 0;	// Set Power Disable

    pSDXCReg->CLKENA = NX_SDXC_CLKENA_LOWPWR;// low power mode & clock disable

    pSDXCReg->TIEDRVPHASE = NX_SDMMC_CLOCK_SHIFT_180 << 8;
    pSDXCReg->TIESMPPHASE = NX_SDMMC_CLOCK_SHIFT_0  << 8;

    pSDXCReg->CLKSRC = 0;	// prescaler 0
    pSDXCReg->CLKDIV = 0;//(SDCLK_DIVIDER_WORK >> 1) << 8 | (SDCLK_DIVIDER_ENUM >> 1) << 0;
/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> SDCLK_DIVIDER_WORK = 0x%x, SDCLK_DIVIDER_ENUM = 0x%x\n",SDCLK_DIVIDER_WORK,SDCLK_DIVIDER_ENUM); */
/*     _dprintf("<<bootrom>> CLKDIV = 0x%x\n",pSDXCReg->CLKDIV); */
/* #endif     */
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

    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_Terminate(SDBOOTSTATUS *pSDXCBootStatus)
{
    register NX_SDMMC_RegisterSet * const pSDXCReg =
        pgSDXCReg[pSDXCBootStatus->SDPort];
    
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
    {
        pSDXCReg->CTRL;
    }
    //    nxSetDeviceClock(&sdmmcclk[pSDXCBootStatus->SDPort][0], 2, 0);
#endif

/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s Done\n",__func__); */
/* #endif */

    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_Open(SDBOOTSTATUS *pSDXCBootStatus, unsigned int option)
{
#ifdef DEBUG
    _dprintf("<<bootrom>> NX_SDMMC_Open step1\n");
#endif

#ifdef QEMU_RISCV
    return 1;
#endif

    //--------------------------------------------------------------------------
    // card identification mode : Identify & Initialize
    if (0 == NX_SDMMC_IdentifyCard(pSDXCBootStatus)) {
        _dprintf("Identify Fail\r\n");
        return 0;
    }

#ifdef DEBUG
    _dprintf("<<bootrom>> NX_SDMMC_Open step2\n");
#endif
    
    //--------------------------------------------------------------------------
    // data transfer mode : Stand-by state
    if (0 == NX_SDMMC_SetClock(pSDXCBootStatus, 1, SDCLK_DIVIDER_NORMAL)) {
        _dprintf("Card Clk rst fail\r\n");
        return 0;
    }
    if (0 == NX_SDMMC_SelectCard(pSDXCBootStatus)) {
        _dprintf("Card Sel Fail\r\n");
        return 0;
    }

    //--------------------------------------------------------------------------
    // data transfer mode : Transfer state
    if (pSDXCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) {
        NX_SDMMC_SetCardDetectPullUp(pSDXCBootStatus, 0);
    }

    if (0 == NX_SDMMC_SetBlockLength(pSDXCBootStatus,
                                                  BLOCK_LENGTH)) {
        _dprintf("Set Blk Lng Fail\r\n");
        return 0;
    }

    //2018/05/08 EBE error
    NX_SDMMC_SetBusWidth(pSDXCBootStatus, 4);

    return 1;
}

//------------------------------------------------------------------------------
int NX_SDMMC_Close(SDBOOTSTATUS *pSDXCBootStatus)
{
#ifdef DEBUG
    _dprintf("<<bootrom>> NX_SDMMC_Close\n");
#endif
    
    NX_SDMMC_SetClock(pSDXCBootStatus, 0, SDCLK_DIVIDER_400KHZ);
       
    return 1;
}

//------------------------------------------------------------------------------
#ifdef QEMU_RISCV
int NX_SDMMC_ReadSectorData(
		SDBOOTSTATUS *pSDXCBootStatus,
		unsigned int numberOfSector,
                unsigned int step,
		unsigned int *pdwBuffer)
{
    unsigned int	count;
    unsigned int i = 0;
    unsigned int *temp;
    unsigned int seek_qemu = step * 512;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

    if (seek_qemu > sd_body_size)
        return 0;
    
/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> ---- start ---- %s\n",__func__); */
/*     _dprintf("<<bootrom>> ---------------------------------------\n"); */
/*     _dprintf("<<bootrom>> %s pdwBuffer start addr = 0x%x\n",__func__, pdwBuffer); */
/*     _dprintf("<<bootrom>> ---------------------------------------\n"); */
/* #endif */
        
    NX_ASSERT(0 == ((unsigned int)pdwBuffer & 3));

    count = numberOfSector * BLOCK_LENGTH;
    NX_ASSERT(0 == (count % 32));

/* #ifdef DEBUG */
/*     for(unsigned int j = seek_qemu; j < count; j++) { */
/*         _dprintf("%c",(unsigned char)(sd_body[j])); */
/*     } */
/*     _dprintf("*****************************************\n\n"); */
/*     _dprintf("**************copy to pdwbuffer*******\n",__func__); */
/* #endif */

    while (0 < count) {
        *pdwBuffer++ = (unsigned int)sd_body[i+seek_qemu];
        //        _dprintf("%c",(unsigned char)*pdwBuffer);
        count -= 1;
        i++;

        NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
    }
    
    pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

    //    _dprintf("*************************************\n",__func__);
    //    _dprintf("\n<<bootrom>> %s ---- end ----\n\n",__func__);

    return 1;
}
#else //QEMU_RISCV
int NX_SDMMC_ReadSectorData(
		SDBOOTSTATUS *pSDXCBootStatus,
		unsigned int numberOfSector,
		unsigned int *pdwBuffer)
{
    unsigned int		count;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];
#ifdef DEBUG    
    _dprintf("ReadSectorData : pSDXCReg = 0x%x\n",pSDXCReg);
    _dprintf("ReadSectorData : pdwBuffer= 0x%x\n",pdwBuffer);
    _dprintf("ReadSectorData : numberOfSector= 0x%x\n",numberOfSector);
    _dprintf("ReadSectorData : BLOCK_LENGTH = 0x%x\n",BLOCK_LENGTH);    
#endif
    NX_ASSERT(0 == ((unsigned int)pdwBuffer & 3));

    count = numberOfSector * BLOCK_LENGTH;
    NX_ASSERT(0 == (count % 32));

    while (0 < count) {
        _dprintf("ReadSectorData : count = 0x%x\n",count);
        if ((pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_RXDR) || (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO)) {            
            unsigned int FSize, Timeout = NX_SDMMC_TIMEOUT;
            _dprintf("ReadSectorData : Timeout = 0x%x\n",Timeout);
            while ((pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY) && Timeout--) {
                pSDXCReg->STATUS;
            }
            if (0 == Timeout)
                break;
            FSize = (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOCOUNT) >> 17;
            _dprintf("ReadSectorData : FSize = 0x%x\n",FSize);
            count -= (FSize * 4);
            while (FSize) {
                *pdwBuffer++ = pSDXCReg->DATA;
/* #ifdef DEBUG                 */
/*                 _dprintf("%x ",pSDXCReg->DATA); */
/* #endif */
                FSize--;
            }
            _dprintf("\n");
            pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_RXDR;
        }
        
        // Check Errors
        if (pSDXCReg->RINTSTS & (NX_SDXC_RINTSTS_DRTO |
                                 NX_SDXC_RINTSTS_EBE |
                                 NX_SDXC_RINTSTS_SBE |
                                 NX_SDXC_RINTSTS_DCRC)) {
#if defined(DEBUG)            
            _dprintf("Read left = %x\r\n", count);
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DRTO)
                _dprintf("DRTO\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_EBE)
                _dprintf("EBE\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_SBE)
                _dprintf("SBE\r\n");
            if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DCRC)
                _dprintf("DCRC\r\n");
#endif
            return 0;
        }

        if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_DTO) {
            if (count == 0) {
                pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;
                break;
            }
        }

        if (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_HTO) {
            //            _dprintf("HTO\r\n");
            pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_HTO;
        }

        NX_ASSERT(0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_FRUN));
    }
    
    pSDXCReg->RINTSTS = NX_SDXC_RINTSTS_DTO;

    return 1;
}
#endif //QEMU_RISCV

//------------------------------------------------------------------------------
int NX_SDMMC_ReadSectors(SDBOOTSTATUS *pSDXCBootStatus,
			unsigned int SectorNum, unsigned int numberOfSector, unsigned int *pdwBuffer)
{
    int	result = 0;
    unsigned int    status;
#if defined(DEBUG)
    unsigned int    response;
#endif
    NX_SDMMC_COMMAND cmd;
    register NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];
        
    NX_ASSERT(0 == ((unsigned int)pdwBuffer & 3));

    _dprintf("<<bootrom>> ReadSectors step 1\n");
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
        if (NX_SDMMC_STATUS_NOERROR != status) {
            _dprintf("<<bootrom>> NX_SDMMC_STATUS_NOERROR != status\n");
            goto End;
        }
    } while (!((cmd.response[0] & (1 << 8)) &&
               (((cmd.response[0] >> 9) & 0xF) == 4)));
#else
    cmd.cmdidx	= SEND_STATUS;
        cmd.arg		= pSDXCBootStatus->rca;
        cmd.flag	= NX_SDXC_CMDFLAG_STARTCMD |
            NX_SDXC_CMDFLAG_CHKRSPCRC |
            NX_SDXC_CMDFLAG_SHORTRSP;
        status = NX_SDMMC_SendCommand(pSDXCBootStatus, &cmd);
#endif //QEMU_RISCV

    _dprintf("<<bootrom>> ReadSectors step 2\n");
    
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
    if (NX_SDMMC_STATUS_NOERROR != status) {
        _dprintf("<<bootrom>> NX_SDMMC_STATUS_NOERROR != status 2\n");
        goto End;
    }
    _dprintf("<<bootrom>> ReadSectors step 3\n");
    //--------------------------------------------------------------------------
    // Read data
    if (1 != NX_SDMMC_ReadSectorData(pSDXCBootStatus, numberOfSector, pdwBuffer)) {
        _dprintf("<<bootrom>> ReadSectorData Fail\n");
        goto End;
    }
#else
    if (1 != NX_SDMMC_ReadSectorData(pSDXCBootStatus, numberOfSector, SectorNum, pdwBuffer)) {
        goto End;
    }
#endif //QEMU_RISCV

    NX_ASSERT(NX_SDXC_STATUS_FIFOEMPTY ==
              (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY));
    NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOFULL));
    NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOCOUNT));

    _dprintf("<<bootrom>> ReadSectors step 4\n");
#ifndef QEMU_RISCV
    if (numberOfSector > 1) {
        // Wait until the Auto-stop command has been finished.
        while (0 == (pSDXCReg->RINTSTS & NX_SDXC_RINTSTS_ACD))
            pSDXCReg->RINTSTS;

        NX_ASSERT(0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FSMBUSY));

/* #ifdef DEBUG */
/*         // Get Auto-stop response and then check it. */
/*         response = pSDXCReg->RESP[1]; */
/*         if (response & 0xFDF98008) { */
/*             _dprintf("Auto Stop Resp Fail:%x\r\n", response); */
/*             //goto End; */
/*         } */
/* #endif */
    }
#endif //QEMU_RISCV
    result = 1;

End:
    if (0 == result) {
        _dprintf("<<bootrom>> END : readsector result 0\n");
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

    return result;
}

//------------------------------------------------------------------------------
int SDMMCBOOT(SDBOOTSTATUS *pSDXCBootStatus, unsigned int option)
{
    int	result = 0;
    int ret = 0;
    struct nx_bootmm *const pbm = (struct nx_bootmm * const)BASEADDR_OF_PBM;
    NX_SDMMC_RegisterSet * const pSDXCReg = pgSDXCReg[pSDXCBootStatus->SDPort];

#ifdef DEBUG
    _dprintf("<<bootrom>> SDMMCBOOT start\n");
#endif

    if (1 != NX_SDMMC_Open(pSDXCBootStatus, option)) {
        //        _dprintf("device open fail\r\n");
        goto error;
    }

    if (0 == (pSDXCReg->STATUS & NX_SDXC_STATUS_FIFOEMPTY)) {
#ifndef QEMU_RISCV
        volatile unsigned int tempcount = 0x100000;
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
    _dprintf("<<bootrom>> 1st MBR Read\n");
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == 0) {
        _dprintf("<<bootrom>> MBR read fail.\n");
        goto error;
    }

#ifdef DEBUG
    for (unsigned int i = 0; i < 128; i++) {
        _dprintf("%x ",*(psector+i));
    }
#endif
        
#ifdef DEBUG
    _dprintf("\n<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>> 2nd GPT check ReadSectors\n");
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    ret = is_gpt_part((unsigned char *)psector);
#ifdef DEBUG
    _dprintf("<<bootrom>> Header type = 0x%x\n", ret ? "Boot" : "GPT");
#endif

    if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == 0) {
        //_dprintf("<<bootrom>> Header read fail\n", ret ? "Boot" : "GPT");
    	goto error;
    }
    
#ifdef DEBUG
    _dprintf("\n<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>> 3rd GPT valid Routine ReadSectors\n");
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
    
    if (!ret) {
    	ret = is_gpt_valid((unsigned char *)psector);
    	if (!ret) {
    		rsn = get_first_empty_lba((unsigned char *)psector);
                _dprintf("read bootimage from sector %d.\n", rsn);
    		if (NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn++, 1, psector) == 0) {
                        _dprintf("Boot Header read fail.\n");
    			goto error;
    		}
    	}
        else {
            _dprintf("invalid GPT.\r\n");
        }
    } else {
        _dprintf("<<bootrom>>not GPT, read header from sector 1.\r\n");
    }

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
    /* 		Decrypt((unsigned int*)&(*pbm)->bi, */
    /* 				(unsigned int*)&(*pbm)->bi, */
    /* 				iv, sizeof(struct nx_bootinfo)); */
    /* 	} */

    struct nx_bootinfo *pbi = &(pbm)->bi;
    unsigned int BootSize = pbi->LoadSize;

    //2018/05/09
    //endian check!!!
    if (pbi->signature != HEADER_ID) {
#ifdef DEBUG        
        _dprintf("<<bootrom>> pbi sifnature addr = %x\n", &(pbi->signature));
        _dprintf("<<bootrom>> expected HEADER_ID = %x\n", HEADER_ID);
        _dprintf("<<bootrom>> not bootable image(0x%x)\n", pbi->signature);
#endif        
        return 0;
    }
    /* #endif */

#ifdef DEBUG
    _dprintf("<<bootrom>> pbi addr : 0x%x \n", pbi);
    _dprintf("<<bootrom>> LoadAddr:%x LoadSize:%x StartAddr:%x\r\n",
                     pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr);
    
    _dprintf("\n<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
    _dprintf("<<bootrom>> 4th Real Image Load to SRAM \n");
    _dprintf("<<bootrom>>+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

    if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK) {
        BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK;
    }

    //unsigned int sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
    unsigned int sectorsize = (BootSize + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
    //    unsigned int tdata[1024 / 4];
    unsigned int *pdata = (unsigned int *)(pbm)->image;
    //    result = NX_SDMMC_ReadSectors(pSDXCBootStatus, rsn, 2, tdata);
    //    rsn += 2;

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
    /* 		Decrypt((unsigned int*)&(*pbm)->rsa_public, */
    /* 				(unsigned int*)&(*pbm)->rsa_public, */
    /* 				iv, sizeof(struct asymmetrickey)); */
    /* 	} */

    /* 	unsigned int ehash = (option >> VERIFY) & 0x3; */
    /* 	if (ehash) { */
    /* 		unsigned int bhash[32 / 4]; */
    /* 		mbedtls_sha256((*pbm)->rsa_public.rsapublicbootkey, */
    /* 			256, (unsigned char *)bhash, 0); */
    /* 		if (ehash == 1) */
    /* 			ehash--; */
    /* 		const unsigned int *pbootkeyhash = */
    /* 			&(*(*pbootkeyhash))[ehash][0]; */
    /* 		if (memcmp(bhash, pbootkeyhash, 32) != 0) { */
    /* 			_dprintf("boot key verify fail.\r\n"); */
    /* 			return 0; */
    /* 		} */
    /* 	} */
    /* #endif */

    /* result = NX_SDMMC_ReadSetors(pSDXCBootStatus, */
    /*                                       rsn, sectorsize, &pdata[256]); */
    result = NX_SDMMC_ReadSectors(pSDXCBootStatus,
                                          rsn, sectorsize, pdata);

    /* 	if (option & 1 << DECRYPT) { */
    /* #if 1 */
    /* 		imagedecrypt(iv); */
    /* #else */
    /* 		Decrypt((unsigned int*)(*pbm)->image, */
    /* 				(unsigned int*)(*pbm)->image, */
    /* 				iv, (*pbm)->bi.LoadSize); */
    /* #endif */
    /* 	} */
#ifdef DEBUG
    _dprintf("<<bootrom>> pbm->image addr = %x\n", pdata);
    _dprintf("<<bootrom>> sectorsize = %x\n", sectorsize);
    {
        unsigned int* temp = (unsigned int*)(BASEADDR_SRAM+0x200);
        _dprintf("SRAM Addr = 0x%x\n",temp);
        _dprintf("First 128byte values are below ---\n");
        for (unsigned int i = 0; i < 128; i++) {
            _dprintf("%x ",*(temp+i));
        }
        _dprintf("SRAM data 256byte Done\n\n");
        for (unsigned int i = 0; i < 128; i++) {
            _dprintf("%x ",*(pdata+i));
        }
        _dprintf("pdata 256byte Done\n\n");
    }
#endif
    
    return result;
    
 error:
    return result;
}

void NX_SDPADSetALT(unsigned int PortNum)
{
    setpad(sdmmcpad[PortNum], 6, 1);
}

void NX_SDPADSetGPIO(unsigned int PortNum)
{
    setpad(sdmmcpad[PortNum], 6, 0);
}

//------------------------------------------------------------------------------
unsigned int iSDBOOT(unsigned int option)
{
    SDBOOTSTATUS SDXCBootStatus, *pSDXCBootStatus;
    int	result = 0;

#ifdef DEBUG
    _dprintf("<<bootrom>> iSDBOOT start \r\n");
#endif
    pSDXCBootStatus = &SDXCBootStatus;

#ifdef DEBUG
    _dprintf("<<bootrom>> pSDXCBootStatus = 0x%x \r\n",pSDXCBootStatus);
#endif
    pSDXCBootStatus->SDPort = 0;
    pSDXCBootStatus->bHighSpeed = 0;
    
    NX_SDPADSetALT(pSDXCBootStatus->SDPort);
    NX_SDMMC_Init(pSDXCBootStatus);

    result = SDMMCBOOT(pSDXCBootStatus, option);

    NX_SDMMC_Close(pSDXCBootStatus);
    NX_SDMMC_Terminate(pSDXCBootStatus);

    NX_SDPADSetGPIO(pSDXCBootStatus->SDPort);
    
/* #ifdef DEBUG */
/*     _dprintf("<<bootrom>> %s Done \n",__func__); */
/* #endif     */

    return result;
}
