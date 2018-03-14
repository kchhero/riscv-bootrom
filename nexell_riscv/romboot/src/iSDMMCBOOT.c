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
// Author	: Suker
// History	: 2018.03.12 Created (Drone Soc)
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

#include "include/nx_sdmmc_1_6.h"
#include "iSDMMCBOOT.h"

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

NX_SDMMC_RegisterSet * const pgSDMMCReg[3] =
{
  (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC0_MODULE,
  (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC1_MODULE,
  (NX_SDMMC_RegisterSet *)PHY_BASEADDR_SDMMC2_MODULE
};


//------------------------------------------------------------------------------
void NX_SDMMC_Init(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
	U32 i = pSDMMCBootStatus->SDPort;
	register NX_SDMMC_RegisterSet * const pSDMMCReg = pgSDMMCReg[i];

        NXBL0FN *pbl0fn = Getbl0fnPtr();

        nx_memset(pSDMMCReg, 0x00, sizeof(NX_SDMMC_RegisterSet));
        
#ifdef DEBUG
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[bootrom] %s : NX_SDMMC_Init address = 0x%x\n",__func__,pbl0fn->NX_SDMMC_Init);      
        pbl0fn->_dprintf("[bootrom] %s : pSDMMCReg = 0x%x\n",__func__,pSDMMCReg);
        pbl0fn->_dprintf("[bootrom] %s : NX_SDMMC_RegisterSet size = 0x%x\n",__func__,sizeof(NX_SDMMC_RegisterSet));        

        pbl0fn->_dprintf("[bootrom] %s : pSDMMCBootStatus->SDPort = 0x%x\n",__func__,pSDMMCBootStatus->SDPort);
#endif

        //Card Detection
        NX_SDMMC_SetCardDetection(pSDMMCReg);
        NX_SDMMC_ClearCardDetectIntStatus(pSDMMCReg, NX_SDMMC_INTERRUPT_TYPE_CARD_ALREADY_EXIST);
        if (((pSDMMCReg->PSTATE_REG >> NX_SDMMC_PSTATE_REG_CARD_INSERTED) & 0x1) == 1) { //card inserted
          ;//todo something
        }
        NX_SDMMC_HostController_Setup(pSDMMCReg);
        //NX_SDMMC_HostController_Clock_Setup(pSDMMCReg, 0xb8);

/* 	pSDMMCReg->PWREN = 0 << 0;	// Set Power Disable */

/* 	pSDMMCReg->CLKENA = NX_SDMMC_CLKENA_LOWPWR;// low power mode & clock disable */

/* 	pSDMMCReg->TIEDRVPHASE   = NX_SDMMC_CLOCK_SHIFT_180 << 8; */
/* 	pSDMMCReg->TIESMPPHASE   = NX_SDMMC_CLOCK_SHIFT_0  << 8; */

/* 	pSDMMCReg->CLKSRC = 0;	// prescaler 0 */
/* 	pSDMMCReg->CLKDIV = (SDCLK_DIVIDER_WORK >> 1) << 8 | */
/* 			(SDCLK_DIVIDER_ENUM >> 1) << 0;	// 2*n divider (0 : bypass) */
/* 	// fifo mode, not read wait(only use sdio mode) */
/* 	pSDMMCReg->CTRL &= ~(NX_SDMMC_CTRL_DMAMODE_EN | NX_SDMMC_CTRL_READWAIT); */
/* 	// Reset the controller & DMA interface & FIFO */
/* 	pSDMMCReg->CTRL = NX_SDMMC_CTRL_DMARST | */
/* 			NX_SDMMC_CTRL_FIFORST | */
/* 			NX_SDMMC_CTRL_CTRLRST; */

/* #if !defined(QEMU_RISCV) */
/* 	while (pSDMMCReg->CTRL & (NX_SDMMC_CTRL_DMARST | */
/* 				NX_SDMMC_CTRL_FIFORST | */
/* 				NX_SDMMC_CTRL_CTRLRST)) */
/* 		pSDMMCReg->CTRL; */
/* #endif */
/* 	pSDMMCReg->PWREN = 0x1 << 0;	// Set Power Enable */

/* 	// Data Timeout = 0xFFFFFF, Response Timeout = 0x64 */
/* 	pSDMMCReg->TMOUT = (unsigned int)((0xFFFFFFU << 8) | (0x64 << 0)); */

/* 	// Data Bus Width : 0(1-bit), 1(4-bit) */
/* 	pSDMMCReg->CTYPE = 0; */

/* 	// Block size */
/* 	pSDMMCReg->BLKSIZ = BLOCK_LENGTH; */

/* 	// Issue when RX FIFO Count >= 8 x 4 bytes & TX FIFO Count <= 8 x 4 bytes */
/* 	pSDMMCReg->FIFOTH = ((8 - 1) << 16) |		// Rx threshold */
/* 				(8 << 0);		// Tx threshold */

/* 	// Mask & Clear All interrupts */
/* 	pSDMMCReg->INTMASK = 0; */
/* 	pSDMMCReg->RINTSTS = (unsigned int)(0xFFFFFFFF); */

/* 	// Wake up & Power on fifo sram */
/* 	pSDMMCReg->TIESRAM = (unsigned int)0x3; */
/* 	pSDMMCReg->TIEMODE = 1; */

/* #ifdef QEMU_RISCV */
/*         _dprintf("[bootrom] pSDMMCReg->PWREN = 0x%x\n",pSDMMCReg->PWREN); */
/*         _dprintf("[bootrom] pSDMMCReg->CLKENA = 0x%x\n",pSDMMCReg->CLKENA); */
/*         _dprintf("[bootrom] pSDMMCReg->TIEDRVPHASE = 0x%x\n",pSDMMCReg->TIEDRVPHASE); */
/*         _dprintf("[bootrom] pSDMMCReg->TIESMPPHASE = 0x%x\n",pSDMMCReg->TIESMPPHASE); */
/*         _dprintf("[bootrom] pSDMMCReg->CLKSRC = 0x%x\n",pSDMMCReg->CLKSRC); */
/*         _dprintf("[bootrom] pSDMMCReg->CLKDIV = 0x%x\n",pSDMMCReg->CLKDIV); */
/*         _dprintf("[bootrom] pSDMMCReg->CTRL = 0x%x\n",pSDMMCReg->CTRL); */
/*         _dprintf("[bootrom] pSDMMCReg->TMOUT = 0x%x\n",pSDMMCReg->TMOUT); */
/*         _dprintf("[bootrom] pSDMMCReg->CTYPE = 0x%x\n",pSDMMCReg->CTYPE); */
/*         _dprintf("[bootrom] pSDMMCReg->BLKSIZ = 0x%x\n",pSDMMCReg->BLKSIZ); */
/*         _dprintf("[bootrom] pSDMMCReg->FIFOTH = 0x%x\n",pSDMMCReg->FIFOTH); */
/*         _dprintf("[bootrom] pSDMMCReg->INTMASK = 0x%x\n",pSDMMCReg->INTMASK); */
/*         _dprintf("[bootrom] pSDMMCReg->RINTSTS = 0x%x\n",pSDMMCReg->RINTSTS); */
/*         _dprintf("[bootrom] pSDMMCReg->TIESRAM = 0x%x\n",pSDMMCReg->TIESRAM); */
/*         _dprintf("[bootrom] pSDMMCReg->TIEMODE = 0x%x\n",pSDMMCReg->TIEMODE); */
        
/*         pbl0fn->_dprintf("[bootrom] %s : END\n",__func__); */
/* #endif */

/* 	return CTRUE; */
}

void NX_SDMMC_HostController_Setup(NX_SDMMC_RegisterSet* p)
{
#ifdef QEMU_RISCV
    NXBL0FN *pbl0fn = Getbl0fnPtr();
    pbl0fn->kputs("------------------------------------");
    pbl0fn->_dprintf("[bootrom] %s start\n",__func__);
#endif

    //step 1 : Set Common Parameters for all Versions
    p->PWR_CTRL_R = NX_SDMMC_PWR_CTRL_SD_BUS_VOL_VDD1 & (SD_BUS_VOL_VDD_1_8 << 1);
    p->TOUT_CTRL_R = NX_SDMMC_TOUT_CNT & TMCLK_2__20;

    //step 2 : Read HOST_CNTRL_VERS_R
    //NX_SDMMC_SD_HOST_CONTROLLER_VERSION
    
    //step 3 : Check Version with 3.0
    if ((U32)NX_SDMMC_SD_HOST_CONTROLLER_VERSION >= (U32)SPEC_VERSION_NUM3) {
      //step 4 : Set Version 3 Parameters
      NX_SDMMC_setClkMode(p, (U16)NX_SDMMC_CLK_MODE_VAL, NX_SDMMC_CLK_MODE, 1, 1, 1, 1);
      //step 6 : check if P reset Value is used?
      if (NX_SDMMC_PRESETn == 1) {
        //step 7 : set version 3 parameters
        p->PRESET_INIT_R = 0x1234;
        p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_PRESET_VAL_ENABLE;
      }
      //step 8 : Check Host Driver Version
      if ((U32)NX_SDMMC_SD_HOST_DRIVER_VERSION == 4) {
        //step 9 : Set Version 4 Parameters
        p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_HOST_VER4_ENABLE;
        if (((p->CAPABILITIES1_R >> NX_SDMMC_CAPA1_SYS_ADDR_64_V4) & 0x1) == 1)
            p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_ADDRESSING;

        //Todo : UHS.. skip capabilities2
        //skip preset uhs2
      }
    }
    else {
      //step 5 : Set Version 1 and 2 Parameters, 8-bit divided ???
      NX_SDMMC_setClkMode(p, (U16)0xFF, NX_SDMMC_CLK_MODE, 1, 1, 1, 1);
    }
#if 0
    //#ifdef QEMU_RISCV
  pbl0fn->_dprintf("[bootrom]%s PWR_CTRL_R \n =====> offset=0x%x, values=0x%x\n",__func__,&(p->PWR_CTRL_R), p->PWR_CTRL_R);
  pbl0fn->_dprintf("[bootrom]%s TOUT_CTRL_R \n =====>  offset=0x%x, values=0x%x\n",__func__,&(p->TOUT_CTRL_R), p->TOUT_CTRL_R);
  pbl0fn->_dprintf("[bootrom]%s CLK_CTRL_R \n =====>  offset=0x%x, values=0x%x\n",__func__,&(p->CLK_CTRL_R), p->CLK_CTRL_R);  
  pbl0fn->_dprintf("[bootrom]%s PRESET_INIT_R \n =====> offset=0x%x, values=0x%x\n",__func__,&(p->PRESET_INIT_R), p->PRESET_INIT_R);
  pbl0fn->_dprintf("[bootrom]%s HOST_CTRL2_R \n =====> offset=0x%x, values=0x%x\n",__func__,&(p->HOST_CTRL2_R), p->HOST_CTRL2_R);
#endif
      
}

//suker
//Todo
void NX_SDMMC_HostController_Clock_Setup(NX_SDMMC_RegisterSet* p, U8 freqVal)
{
    U8 clockMode;

#ifdef QEMU_RISCV
    NXBL0FN *pbl0fn = Getbl0fnPtr();
    pbl0fn->kputs("------------------------------------");
    pbl0fn->_dprintf("[bootrom] %s start\n",__func__);
#endif

    //step 1 : Calculate a divisor for SD clock freq
    //CAPA2 CLK_MUL is non-zero, use programmable clock mode
    if (p->CAPABILITIES2_R & NX_SDMMC_CAPA2_CLK_MUL) {
        clockMode = CLK_CTRL_MODE_PROGRAMMABLE;
    }
    //or CAPA1 BASE_CLK_FREQ is 0, use a different methood
    
    //step 2 : set clk_ctrl
    p->CLK_CTRL_R |= ((freqVal << 8) & NX_SDMMC_CLK_CTRL_FREQ_SEL);
    
}

void NX_SDMMC_setClkMode(NX_SDMMC_RegisterSet* p, U16 val,
                         U8 clkCtrlMode, U8 pllEnable, U8 sdClkEnable,
                         U8 clkEnable, U8 clkStable)
{
    p->CLK_CTRL_R = (((val << 8) & NX_SDMMC_CLK_CTRL_FREQ_SEL) |
                     (((val >> 8) << 6) & NX_SDMMC_CLK_CTRL_UPPER_FREQ_SEL));
    if (clkCtrlMode)
      p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_CLK_GEN_SELECT;
    else
      p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_CLK_GEN_SELECT;

    if (pllEnable)
      p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_PLL_ENABLE;
    else
      p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_PLL_ENABLE;

    if (sdClkEnable)
      p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_SD_CLK_EN;
    else
      p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_SD_CLK_EN;

    if (clkEnable)
      p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_INTERNAL_CLK_EN;
    else
      p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_INTERNAL_CLK_EN;

    if (clkStable)
      p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_INTERNAL_CLK_STABLE;
    else
      p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_INTERNAL_CLK_STABLE;
}

void NX_SDMMC_CardClockSupply(NX_SDMMC_RegisterSet* p)
{
    p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_SD_CLK_EN;
}

void NX_SDMMC_CardClockStop(NX_SDMMC_RegisterSet* p)
{
    p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_SD_CLK_EN;
}

void NX_SDMMC_ClockFreqChange(NX_SDMMC_RegisterSet* p, U8 freq)
{
#ifdef QEMU_RISCV
    NXBL0FN *pbl0fn = Getbl0fnPtr();
    pbl0fn->kputs("------------------------------------");
    pbl0fn->_dprintf("[bootrom] %s start\n",__func__);
#endif
    //step 1 : Execute SD clock stop
    NX_SDMMC_CardClockStop(p);

    //step 2 : Set pll disable
    p->CLK_CTRL_R &= ~NX_SDMMC_CLK_CTRL_PLL_ENABLE;

    //step 3 : check preset_val_enable
    if ((p->HOST_CTRL2_R >> 15) == 1) {
        //step 4 : Set bus speed mode
        p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_UHS_MODE_SEL;
    }
    else {
        //step 5 : calculate a divisor
        if (p->CAPABILITIES2_R & NX_SDMMC_CAPA2_CLK_MUL) { //non-zero
          ;
          //todo
          //use programmable clock mode
        }
        //todo
        //Set CLK_CTRL_R.FREQ_SEL     
    }

    //step 7 : set CLK_CTRL_R.PLL ENABLE
    p->CLK_CTRL_R |= NX_SDMMC_CLK_CTRL_PLL_ENABLE;

repeat8 :
    //timeout check
    //todo : timeout failed return FALSE;
    /* timeout = 0; */
    /* while (pSDMMCReg->CMD & NX_SDMMC_CMDFLAG_STARTCMD) { */
    /*     if (++timeout > NX_SDMMC_TIMEOUT) { */
    /*     	pbl0fn->_dprintf("clk2 TO\r\n"); */
    /*     	INFINTE_LOOP(); */
    /*     	return CFALSE; */
    /*     } */
    /* } */

    //step 8 : check CLK_CTRL_R.INTERNAL_CLK_STABLE
    if ((p->CLK_CTRL_R & NX_SDMMC_CLK_CTRL_INTERNAL_CLK_STABLE) >> 1) {
        ;
        //DONE
    }
    else {
        goto repeat8;
    }
}

void NX_SDMMC_CardInterfaceDetection(NX_SDMMC_RegisterSet* p)
{
    //step 1 : check uhs2 support
    if ((p->CAPABILITIES2_R & NX_SDMMC_CAPA2_UHS2_SUPPORT) >> 3) {
        NX_SDMMC_UHS_Detect(p);
    }
    else {
        NX_SDMMC_SD_Detect(p);
    }
}

void NX_SDMMC_UHS_Detect(NX_SDMMC_RegisterSet* p)
{
    //step 2 : select UHS-II mode
    p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_UHS2_IF_ENABLE;
    p->PWR_CTRL_R |= NX_SDMMC_PWR_CTRL_SD_BUS_PWR_VDD2;
    p->PWR_CTRL_R |= (NX_SDMMC_PWR_CTRL_SD_BUS_VOL_VDD2 & (0x5 << 5));
    p->HOST_CTRL2_R |= NX_SDMMC_HOST_CTRL2_UHS_MODE_SEL;

    //step 3 : Host Controller Clock Setup
    NX_SDMMC_HostController_Clock_Setup(p, 0xb8);

    //step 4 : Wait Power Ramp up Time
    //todo

    //step 5 : sd clock supply
    NX_SDMMC_CardClockSupply(p);

    //step 6 : wait for 200us
    //todo

    //step 7 : check PSTATE_REG UHS-II detect
    if ((p->PSTATE_REG & NX_SDMMC_PSTATE_REG_UHS2_IF_DETECT) >> 31) {
        //detect yes
        //step 8 : check lane sync
        //Timeout of 150ms
        do {
            //timeout check
            //Card Power Cycle
            //stop RCLK
            //stop Power Supply VDD2
            //NX_SDMMC_SD_Detect(p);
            ;
        } while(((p->PSTATE_REG & NX_SDMMC_PSTATE_REG_LANE_SYNC) >> 30) == 0);

        //step 9 : UHS-II init, setting
        //todo
    }
    else {
        //change to SD Mode
        //stop RCLK
        //stop Power Supply VDD2
        NX_SDMMC_SD_Detect(p);
    }
}

void NX_SDMMC_SD_Detect(NX_SDMMC_RegisterSet* p)
{
    //step 12 : select SD 4-bit mode
    p->HOST_CTRL2_R &= ~NX_SDMMC_HOST_CTRL2_UHS2_IF_ENABLE;
    p->PWR_CTRL_R |= NX_SDMMC_PWR_CTRL_SD_BUS_PWR_VDD1;
    p->HOST_CTRL2_R &= ~NX_SDMMC_HOST_CTRL2_UHS_MODE_SEL;

    //step 13 : SD clock freq change
    NX_SDMMC_ClockFreqChange(p, 0xa8);

    //step 14 : wait power ramp up time
    //todo

    //step 15 : sd clock supply
    NX_SDMMC_CardClockSupply(p);

    //step 16 : provide at leat 74 clocks before issuing SD command

    //step 17 : SD 4-bit Mode initialization
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetClock(SDMMCBOOTSTATUS *pSDMMCBootStatus,
				CBOOL enb, U32 divider)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* #ifndef QEMU_RISCV         */
/* 	volatile U32 timeout; */
/* #endif         */
/* 	U32 i = pSDMMCBootStatus->SDPort; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[i]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->kputs("[bootrom] NX_SDMMC_SetClock start"); */
/* #endif */
        
/* 	//-------------------------------------------------------------------------- */
/* 	// 1. Confirm that no card is engaged in any transaction. */
/* 	//	If there's a transaction, wait until it has been finished. */
/* //	while(NX_SDMMC_IsDataTransferBusy()) */
/* //		; */
/* //	while(NX_SDMMC_IsCardDataBusy()) */
/* //		; */
/* #if 1 */
/* 	//-------------------------------------------------------------------------- */
/* 	// 2. Disable the output clock. */
/* 	// low power mode & clock disable */
/* 	pSDMMCReg->CLKENA |= NX_SDMMC_CLKENA_LOWPWR; */
/* 	pSDMMCReg->CLKENA &= ~NX_SDMMC_CLKENA_CLKENB; */


/* 	/\* if (divider == SDMMC_DIVIDER_400KHZ) *\/ */
/* 	/\* 	pbl0fn->setdeviceclock(&sdmmcclk[i][2], 1, 1); *\/ */
/* 	/\* if (divider == SDMMC_CLKDIV_LOW) *\/ */
/* 	/\* 	pbl0fn->setdeviceclock(&sdmmcclk[i][1], 1, 1); *\/ */


/* 	pSDMMCReg->CLKENA &= ~NX_SDMMC_CLKENA_LOWPWR;	// low power mode & clock disable */


/* 	//-------------------------------------------------------------------------- */
/* 	// 4. Start a command with NX_SDMMC_CMDFLAG_UPDATECLKONLY flag. */
/* #ifndef QEMU_RISCV */
/* repeat_4 : */
/* 	pSDMMCReg->CMD = 0 | NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_UPDATECLKONLY | */
/* 			NX_SDMMC_CMDFLAG_STOPABORT; */

/* 	//-------------------------------------------------------------------------- */
/* 	// 5. Wait until a update clock command is taken by the SDMMC module. */
/* 	//	If a HLE is occurred, repeat 4. */
/* 	timeout = 0; */

/* 	while (pSDMMCReg->CMD & NX_SDMMC_CMDFLAG_STARTCMD) { */
/* 		if (++timeout > NX_SDMMC_TIMEOUT) { */
/* 			pbl0fn->_dprintf("clk TO\r\n"); */
/* 			INFINTE_LOOP(); */
/* 			return CFALSE; */
/* 		} */
/* 	} */
/* 	if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HLE) { */
/* 		INFINTE_LOOP(); */
/* 		pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_HLE; */
/* 		goto repeat_4; */
/* 	} */

/* 	if (CFALSE == enb) */
/* 		return CTRUE; */
/* #endif */
/* 	//-------------------------------------------------------------------------- */
/* 	// 6. Enable the output clock. */
/* 	pSDMMCReg->CLKENA |= NX_SDMMC_CLKENA_CLKENB; */

/* 	//-------------------------------------------------------------------------- */
/* 	// 7. Start a command with NX_SDMMC_CMDFLAG_UPDATECLKONLY flag. */
/* #ifndef QEMU_RISCV */
/* repeat_7 : */
/* 	pSDMMCReg->CMD = 0 | NX_SDMMC_CMDFLAG_STARTCMD | */
/* 		NX_SDMMC_CMDFLAG_UPDATECLKONLY | */
/* 		NX_SDMMC_CMDFLAG_STOPABORT; */

/* 	//-------------------------------------------------------------------------- */
/* 	// 8. Wait until a update clock command is taken by the SDMMC module. */
/* 	//	If a HLE is occurred, repeat 7. */
/* 	timeout = 0; */
/* 	while (pSDMMCReg->CMD & NX_SDMMC_CMDFLAG_STARTCMD) { */
/* 		if (++timeout > NX_SDMMC_TIMEOUT) { */
/* 			pbl0fn->_dprintf("clk2 TO\r\n"); */
/* 			INFINTE_LOOP(); */
/* 			return CFALSE; */
/* 		} */
/* 	} */

/* 	if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HLE) { */
/* 		INFINTE_LOOP(); */
/* 		pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_HLE; */
/* 		goto repeat_7; */
/* 	} */
/* #endif */
/* #endif */
	return CTRUE;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendCommandInternal(
		SDMMCBOOTSTATUS *pSDMMCBootStatus,
		NX_SDMMC_COMMAND *pCommand)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32		cmd, flag; */
/* 	U32		status = 0; */
/* 	volatile U32	timeout; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 				pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */

/* 	NX_ASSERT(CNULL != pCommand); */

/* #ifdef QEMU_RISCV         */
/* 	pbl0fn->_dprintf("[bootrom]s : Command(%Xh), Argument(%Xh)\n", */
/* 			__func__, pCommand->cmdidx, pCommand->arg); */
/* #endif */

/* 	cmd	= pCommand->cmdidx & 0xFF; */
/* 	flag	= pCommand->flag; */

/* 	pSDMMCReg->RINTSTS = 0xFFFFFFFF; */

/* 	//-------------------------------------------------------------------------- */
/* 	// Send Command */
/* 	timeout = 0; */
/* 	do { */
/* 		pSDMMCReg->RINTSTS	= NX_SDMMC_RINTSTS_HLE; */
/* 		pSDMMCReg->CMDARG	= pCommand->arg; */
/* 		pSDMMCReg->CMD		= cmd | flag | */
/* 					NX_SDMMC_CMDFLAG_USE_HOLD_REG; */
/* 		while (pSDMMCReg->CMD & NX_SDMMC_CMDFLAG_STARTCMD) { */
/* 			if (++timeout > NX_SDMMC_TIMEOUT) { */
/* 				pbl0fn->_dprintf("TO send cmd\r\n"); */
/* 				status |= NX_SDMMC_STATUS_CMDBUSY; */
/* 				INFINTE_LOOP(); */
/* 				goto End; */
/* 			} */
/* 		} */
/* 	} while (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HLE); */

/* 	//-------------------------------------------------------------------------- */
/* 	// Wait until Command sent to card and got response from card. */
/* 	timeout = 0; */
/* 	while (1) { */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_CD) */
/* 			break; */

/* 		if (++timeout > NX_SDMMC_TIMEOUT) { */
/* 			pbl0fn->_dprintf("TO cmd done\r\n"); */
/* 			status |= NX_SDMMC_STATUS_CMDTOUT; */
/* 			INFINTE_LOOP(); */
/* 			goto End; */
/* 		} */

/* 		if ((flag & NX_SDMMC_CMDFLAG_STOPABORT) && */
/* 			(pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HTO)) { */
/* 			// You have to clear FIFO when HTO is occurred. */
/* 			// After that, SDMMC module leaves in stopped state and takes next command. */
/* 			while (0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY)) { */
/* 				pSDMMCReg->DATA; */
/* 			} */
/* 		} */
/* 	} */

/* 	// Check Response Error */
/* 	if (pSDMMCReg->RINTSTS & (NX_SDMMC_RINTSTS_RCRC | */
/* 				NX_SDMMC_RINTSTS_RE | */
/* 				NX_SDMMC_RINTSTS_RTO)) { */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_RCRC) */
/* 			status |= NX_SDMMC_STATUS_RESCRCFAIL; */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_RE) */
/* 			status |= NX_SDMMC_STATUS_RESERROR; */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_RTO) */
/* 			status |= NX_SDMMC_STATUS_RESTOUT; */
/* 	} */

/* 	if ((status == NX_SDMMC_STATUS_NOERROR) && */
/* 		(flag & NX_SDMMC_CMDFLAG_SHORTRSP)) { */
/* 		pCommand->response[0] = pSDMMCReg->RESP[0]; */
/* 		if ((flag & NX_SDMMC_CMDFLAG_LONGRSP) == */
/* 				NX_SDMMC_CMDFLAG_LONGRSP) { */
/* 			pCommand->response[1] = pSDMMCReg->RESP[1]; */
/* 			pCommand->response[2] = pSDMMCReg->RESP[2]; */
/* 			pCommand->response[3] = pSDMMCReg->RESP[3]; */
/* 		} */

/* 		if (NX_SDMMC_RSPIDX_R1B == ((pCommand->cmdidx >> 8) & 0xFF)) { */
/* 			timeout = 0; */
/* 			do { */
/* 				if (++timeout > NX_SDMMC_TIMEOUT) { */
/* 					pbl0fn->_dprintf("TO card data ready\r\n"); */
/* 					status |= NX_SDMMC_STATUS_DATABUSY; */
/* 					INFINTE_LOOP(); */
/* 					goto End; */
/* 				} */
/* 			} while (pSDMMCReg->STATUS & NX_SDMMC_STATUS_DATABUSY); */
/* 		} */
/* 	} */

/* End: */

/* 	if (NX_SDMMC_STATUS_NOERROR != status) { */
/* //		pbl0fn->printf("err: cmd:%x, arg:%x => sts:%x, resp:%x\r\n", */
/* //			pCommand->cmdidx, pCommand->arg, */
/* //			status, pCommand->response[0]); */
/* 	} */

/* 	pCommand->status = status; */

//	return status;
  return 1;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendStatus(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */

/* 	cmd.cmdidx	= SEND_STATUS; */
/* 	cmd.arg		= pSDMMCBootStatus->rca; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, &cmd); */


//	return status;
  return 1;
}

/* //------------------------------------------------------------------------------ */
U32 NX_SDMMC_SendCommand(SDMMCBOOTSTATUS *pSDMMCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, pCommand); */
/* 	if (NX_SDMMC_STATUS_NOERROR != status) { */
/* 		pbl0fn->NX_SDMMC_SendStatus(pSDMMCBootStatus); */
/* 	} */

//	return status;
  return 1;
}

//------------------------------------------------------------------------------
U32 NX_SDMMC_SendAppCommand(SDMMCBOOTSTATUS *pSDMMCBootStatus,
				NX_SDMMC_COMMAND *pCommand)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	cmd.cmdidx	= APP_CMD; */
/* 	cmd.arg		= pSDMMCBootStatus->rca; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 	status = pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, &cmd); */
/* 	if (NX_SDMMC_STATUS_NOERROR == status) { */
/* 		pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, pCommand); */
/* 	} */

//	return status;
  return 1;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_IdentifyCard(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	S32 timeout; */
/* 	U32 HCS, RCA; */
/* 	NX_SDMMC_CARDTYPE CardType = NX_SDMMC_CARDTYPE_UNKNOWN; */
/* 	NX_SDMMC_COMMAND cmd; */
/* 	struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */

/* 	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDMMCBootStatus, */
/* 					CTRUE, */
/* 					SDMMC_DIVIDER_400KHZ)) */
/* 		return CFALSE; */

/* 	// Data Bus Width : 0(1-bit), 1(4-bit) */
/* 	pSDMMCReg->CTYPE = 0; */

/* 	pSDMMCBootStatus->rca = 0; */

/* 	//-------------------------------------------------------------------------- */
/* 	//	Identify SD/MMC card */
/* 	//-------------------------------------------------------------------------- */
/* 	// Go idle state */
/* 	cmd.cmdidx	= GO_IDLE_STATE; */
/* 	cmd.arg		= 0; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_SENDINIT | */
/* 			NX_SDMMC_CMDFLAG_STOPABORT; */

/* 	pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */

/* 	// Check SD Card Version */
/* 	cmd.cmdidx	= SEND_IF_COND; */
/* 	// argument = VHS : 2.7~3.6V and Check Pattern(0xAA) */
/* 	cmd.arg		= (1 << 8) | 0xAA; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 	if (NX_SDMMC_STATUS_NOERROR == */
/* 			pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, &cmd)) { */
/* 		// Ver 2.0 or later SD Memory Card */
/* 		if (cmd.response[0] != ((1 << 8) | 0xAA)) */
/* 			return CFALSE; */

/* 		HCS = 1 << 30; */
/* 	} else { */
/* 		// voltage mismatch or Ver 1.X SD Memory Card or not SD Memory Card */
/* 		HCS = 0; */
/* 	} */

/* 	//-------------------------------------------------------------------------- */
/* 	// voltage validation */
/* 	timeout = NX_SDMMC_TIMEOUT_IDENTIFY; */

/* 	cmd.cmdidx	= APP_CMD; */
/* 	cmd.arg		= pSDMMCBootStatus->rca; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 	if (NX_SDMMC_STATUS_NOERROR == */
/* 			pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, &cmd)) { */
/* 		//---------------------------------------------------------------------- */
/* 		// SD memory card */
/* 		#define FAST_BOOT	(1<<29) */

/* 		cmd.cmdidx	= SD_SEND_OP_COND; */
/* 		cmd.arg		= (HCS | FAST_BOOT | 0x00FC0000);	// 3.0 ~ 3.6V */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 		if (NX_SDMMC_STATUS_NOERROR != */
/* 				pbl0fn->NX_SDMMC_SendCommandInternal( */
/* 					pSDMMCBootStatus, &cmd)) { */
/* 			return CFALSE; */
/* 		} */
/* 		/\* Wait until card has finished the power up routine *\/ */
/* 		while (0==(cmd.response[0] & (1UL << 31))) { */
/* 			if (NX_SDMMC_STATUS_NOERROR != */
/* 					pbl0fn->NX_SDMMC_SendAppCommand( */
/* 						pSDMMCBootStatus, &cmd)) { */
/* 				return CFALSE; */
/* 			} */

/* 			if (timeout-- <= 0) { */
/* 				pbl0fn->_dprintf("TO pwrup SD\r\n"); */
/* 				return CFALSE; */
/* 			} */
/* 		} */

/* 		pbl0fn->_dprintf("SD "); */

/* 		CardType	= NX_SDMMC_CARDTYPE_SDMEM; */
/* 		RCA		= 0; */
/* 	} else { */
/* 		//---------------------------------------------------------------------- */
/* 		// MMC memory card */
/* 		cmd.cmdidx	= GO_IDLE_STATE; */
/* 		cmd.arg		= 0; */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_SENDINIT | */
/* 				NX_SDMMC_CMDFLAG_STOPABORT; */

/* 		pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */

/* 		do { */
/* 			cmd.cmdidx	= SEND_OP_COND; */
/* 			cmd.arg		= 0x40FC0000;	// MMC High Capacity -_-??? */
/* 			cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 					NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 					NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 			if (NX_SDMMC_STATUS_NOERROR != */
/* 					pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, */
/* 								&cmd)) { */
/* 				return CFALSE; */
/* 			} */

/* 			if (timeout-- <= 0) { */
/* 				pbl0fn->_dprintf("TO to wait pow-up for MMC\r\n"); */
/* 				return CFALSE; */
/* 			} */
/* 		/\* Wait until card has finished the power up routine *\/ */
/* 		} while (0==(cmd.response[0] & (1UL << 31))); */

/* 		pbl0fn->_dprintf("MMC "); */
/* 		#if defined(VERBOSE) */
/* 		pbl0fn->_dprintf("--> SEND_OP_COND Response = 0x%X\n", cmd.response[0]); */
/* 		#endif */

/* 		CardType	= NX_SDMMC_CARDTYPE_MMC; */
/* 		RCA		= 2 << 16; */
/* 	} */

/* //	NX_ASSERT( (cmd.response[0] & 0x00FC0000) == 0x00FC0000 ); */
/* 	pSDMMCBootStatus->bHighCapacity = */
/* 		(cmd.response[0] & (1 << 30)) ? CTRUE : CFALSE; */

/* 	pbl0fn->_dprintf("%sCard.\r\n", */
/* 			(pSDMMCBootStatus->bHighCapacity) ? "High Capacity " : ""); */

/* 	//-------------------------------------------------------------------------- */
/* 	// Get CID */
/* 	cmd.cmdidx	= ALL_SEND_CID; */
/* 	cmd.arg		= 0; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_LONGRSP; */
/* 	if (NX_SDMMC_STATUS_NOERROR != */
/* 			pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd)) { */
/* 		pbl0fn->_dprintf("cannot read CID\r\n"); */
/* 		return CFALSE; */
/* 	} */

/* 	//-------------------------------------------------------------------------- */
/* 	// Get RCA and change to Stand-by State in data transfer mode */
/* 	cmd.cmdidx	= (CardType == NX_SDMMC_CARDTYPE_SDMEM) ? */
/* 					SEND_RELATIVE_ADDR : SET_RELATIVE_ADDR; */
/* 	cmd.arg		= RCA; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 	if (NX_SDMMC_STATUS_NOERROR != */
/* 			pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd)) { */
/* 		pbl0fn->_dprintf("cannot read RCA\r\n"); */
/* 		return CFALSE; */
/* 	} */

/* 	if (CardType == NX_SDMMC_CARDTYPE_SDMEM) */
/* 		pSDMMCBootStatus->rca = cmd.response[0] & 0xFFFF0000; */
/* 	else */
/* 		pSDMMCBootStatus->rca = RCA; */

/* 	pSDMMCBootStatus->CardType = CardType; */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectCard(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
/* 	cmd.cmdidx	= SELECT_CARD; */
/* 	cmd.arg		= pSDMMCBootStatus->rca; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* 	status = pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */

/* 	return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE; */
  return 1;
}

//suker
void NX_SDMMC_SetCardDetection(NX_SDMMC_RegisterSet *p)
{
    p->NORMAL_INT_STAT_EN_R |= (1 << NX_SDMMC_NORMAL_INT_CARD_INSERTION_STAT_EN);
    p->NORMAL_INT_STAT_EN_R |= (1 << NX_SDMMC_NORMAL_INT_CARD_REMOVAL_STAT_EN);
    p->NORMAL_INT_SIGNAL_EN_R |= (1 << NX_SDMMC_NORMAL_INT_SIGNAL_CARD_INSERTION_SIGNAL_EN);
    p->NORMAL_INT_SIGNAL_EN_R |= (1 << NX_SDMMC_NORMAL_INT_SIGNAL_CARD_REMOVAL_SIGNAL_EN);
}

//suker
void NX_SDMMC_ClearCardDetectIntStatus(NX_SDMMC_RegisterSet *p, NX_SDMMC_INTERRUPT_TYPE interruptType)
{
    if (interruptType == NX_SDMMC_INTERRUPT_TYPE_CARD_INSERTION) {
        p->NORMAL_INT_STAT_R |= (1 << NX_SDMMC_NORMAL_INT_STAT_CARD_INSERTION);
    }
    else if (interruptType == NX_SDMMC_INTERRUPT_TYPE_CARD_REMOVAL) {
        p->NORMAL_INT_STAT_R |= (1 << NX_SDMMC_NORMAL_INT_STAT_CARD_REMOVAL); 
    }
    else {
      ; //nothing
    }
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetCardDetectPullUp(SDMMCBOOTSTATUS *pSDMMCBootStatus,
					CBOOL bEnb)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */
       
/* 	cmd.cmdidx	= SET_CLR_CARD_DETECT; */
/* 	cmd.arg		= (bEnb) ? 1 : 0; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/*         status = pbl0fn->NX_SDMMC_SendAppCommand(pSDMMCBootStatus, &cmd); */

/* 	return (NX_SDMMC_STATUS_NOERROR == status) ? CTRUE : CFALSE; */
  return 1;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBusWidth(SDMMCBOOTSTATUS *pSDMMCBootStatus,
					U32 buswidth )
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	NX_ASSERT( buswidth==1 || buswidth==4 ); */

/* 	if (pSDMMCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) { */
/* 		cmd.cmdidx	= SET_BUS_WIDTH; */
/* 		cmd.arg		= (buswidth >> 1); */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 		status = pbl0fn->NX_SDMMC_SendAppCommand(pSDMMCBootStatus, &cmd); */
/* 	} else { */
/* 		/\* ExtCSD[183] : BUS_WIDTH <= 0 : 1-bit, 1 : 4-bit, 2 : 8-bit *\/ */
/* 		cmd.cmdidx	= SWITCH_FUNC; */
/* 		cmd.arg		=		  (3 << 24) | */
/* 						(183 << 16) | */
/* 				    ((buswidth >> 2) << 8) | */
/* 						  (0 << 0); */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 		status = pbl0fn->NX_SDMMC_SendCommand( pSDMMCBootStatus, &cmd ); */
/* 	} */

/* 	if (NX_SDMMC_STATUS_NOERROR != status) */
/* 		return CFALSE; */

/* 	/\* 0 : 1-bit mode, 1 : 4-bit mode *\/ */
/* 	pSDMMCReg->CTYPE = buswidth >> 2; */

/* 	pSDMMCReg->TIEMODE = buswidth >> 2; */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SetBlockLength(SDMMCBOOTSTATUS *pSDMMCBootStatus,
					U32 blocklength)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 status; */
/* 	NX_SDMMC_COMMAND cmd; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	cmd.cmdidx	= SET_BLOCKLEN; */
/* 	cmd.arg		= blocklength; */
/* 	cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 			NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 			NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 			NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 	status = pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */

/* 	if (NX_SDMMC_STATUS_NOERROR != status) */
/* 		return CFALSE; */

/*         pSDMMCReg->BLKSIZ = blocklength; */

	return CTRUE;
}

#if 0
//------------------------------------------------------------------------------
CBOOL NX_SDMMC_SelectPartition(SDMMCBOOTSTATUS *pSDMMCBootStatus,
					U32 partition)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	U32 status;
	NX_SDMMC_COMMAND cmd;

	#define IMMEDIATE_RESPONSE		(1 <<  0)
	#define DEFERRED_RESPONSE		(1 << 17)

#ifdef QEMU_RISCV
        kputs("[bootrom] NX_SDMMC_SelectPartition start");
#endif
        

	return CTRUE;
}
#endif

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Terminate(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
/*         NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	// Clear All interrupts */
/* 	pSDMMCReg->RINTSTS = 0xFFFFFFFF; */

/* 	// Reset the controller & DMA interface & FIFO */
/* 	pSDMMCReg->CTRL = NX_SDMMC_CTRL_DMARST | */
/* 			NX_SDMMC_CTRL_FIFORST | */
/* 			NX_SDMMC_CTRL_CTRLRST; */
/* 	while (pSDMMCReg->CTRL & (NX_SDMMC_CTRL_DMARST | */
/* 				NX_SDMMC_CTRL_FIFORST | */
/* 				NX_SDMMC_CTRL_CTRLRST)) */
/* 		pSDMMCReg->CTRL; */

/*        	pbl0fn->setdeviceclock(&sdmmcclk[pSDMMCBootStatus->SDPort][0], 2, 0); */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Open(SDMMCBOOTSTATUS *pSDMMCBootStatus, U32 option)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32 SDSpeed; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	if (pSDMMCBootStatus->bHighSpeed == CTRUE) */
/* 		SDSpeed = SDMMC_CLKDIV_HIGH; */
/* 	else */
/* 		SDSpeed = SDMMC_CLKDIV_LOW; */
/* 	//-------------------------------------------------------------------------- */
/* 	// card identification mode : Identify & Initialize */
/* 	if (CFALSE == pbl0fn->NX_SDMMC_IdentifyCard(pSDMMCBootStatus)) { */
/* 		pbl0fn->_dprintf("Identify Fail\r\n"); */
/* 		return CFALSE; */
/* 	} */

/* 	//-------------------------------------------------------------------------- */
/* 	// data transfer mode : Stand-by state */
/* 	if (CFALSE == pbl0fn->NX_SDMMC_SetClock(pSDMMCBootStatus, CTRUE, */
/* 				SDSpeed)) { */
/* 		pbl0fn->_dprintf("Card Clk rst fail\r\n"); */
/* 		return CFALSE; */
/* 	} */
/* 	if (CFALSE == pbl0fn->NX_SDMMC_SelectCard(pSDMMCBootStatus)) { */
/* 		pbl0fn->_dprintf("Card Sel Fail\r\n"); */
/* 		return CFALSE; */
/* 	} */

/* 	//-------------------------------------------------------------------------- */
/* 	// data transfer mode : Transfer state */
/* 	if (pSDMMCBootStatus->CardType == NX_SDMMC_CARDTYPE_SDMEM) { */
/* 		pbl0fn->NX_SDMMC_SetCardDetectPullUp(pSDMMCBootStatus, CFALSE); */
/* 	} */

/* 	if (CFALSE == pbl0fn->NX_SDMMC_SetBlockLength(pSDMMCBootStatus, */
/* 				BLOCK_LENGTH)) { */
/* 		pbl0fn->_dprintf("Set Blk Lng Fail\r\n"); */
/* 		return CFALSE; */
/* 	} */

/* 	if (option & (1 << eMMCBUSWIDTH)) */
/* 		pbl0fn->NX_SDMMC_SetBusWidth(pSDMMCBootStatus, 8); */
/* 	else */
/* 		pbl0fn->NX_SDMMC_SetBusWidth(pSDMMCBootStatus, 4); */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_Close(SDMMCBOOTSTATUS *pSDMMCBootStatus)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
/*         pbl0fn->NX_SDMMC_SetClock(pSDMMCBootStatus, CFALSE, SDMMC_DIVIDER_400KHZ); */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectorData(
		SDMMCBOOTSTATUS *pSDMMCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32		count; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	NX_ASSERT(0 == ((U32)pdwBuffer & 3)); */

/* 	count = numberOfSector * BLOCK_LENGTH; */
/* 	NX_ASSERT(0 == (count % 32)); */

/* 	while (0 < count) { */
/* 		if ((pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_RXDR) */
/* 		 || (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DTO)) { */
/* 			U32 FSize, Timeout = NX_SDMMC_TIMEOUT; */
/* 			while ((pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY) && */
/* 					Timeout--) */
/* 				pSDMMCReg->STATUS; */
/* 			if (0 == Timeout) */
/* 				break; */
/* 			FSize = (pSDMMCReg->STATUS & */
/* 					NX_SDMMC_STATUS_FIFOCOUNT) >> 17; */
/* 			count -= (FSize * 4); */
/* 			while (FSize) { */
/* 				*pdwBuffer++ = pSDMMCReg->DATA; */
/* 				FSize--; */
/* 			} */

/* 			pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_RXDR; */
/* 		} */

/* 		// Check Errors */
/* 		if (pSDMMCReg->RINTSTS & (NX_SDMMC_RINTSTS_DRTO | */
/* 					NX_SDMMC_RINTSTS_EBE | */
/* 					NX_SDMMC_RINTSTS_SBE | */
/* 					NX_SDMMC_RINTSTS_DCRC)) { */
/* 			pbl0fn->_dprintf("Read left = %x\r\n", count); */

/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DRTO) */
/* 				pbl0fn->_dprintf("DRTO\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_EBE) */
/* 				pbl0fn->_dprintf("EBE\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_SBE) */
/* 				pbl0fn->_dprintf("SBE\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DCRC) */
/* 				pbl0fn->_dprintf("DCRC\r\n"); */

/* 			return CFALSE; */
/* 		} */

/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DTO) { */
/* 			if (count == 0) { */
/* 				pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_DTO; */
/* 				break; */
/* 			} */
/* 		} */

/* 		#if defined(NX_DEBUG) */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HTO) { */
/* 			pbl0fn->_dprintf("HTO\r\n"); */
/* 			pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_HTO; */
/* 		} */
/* 		#endif */

/* 		NX_ASSERT(0 == (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_FRUN)); */
/* 	} */

/* 	pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_DTO; */

	return CTRUE;
}

//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadSectors(SDMMCBOOTSTATUS *pSDMMCBootStatus,
			U32 SectorNum, U32 numberOfSector, U32 *pdwBuffer)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	CBOOL	result = CFALSE; */
/* 	U32		status; */
/* 	#if defined(NX_DEBUG) */
/* 	U32	response; */
/* 	#endif */
/* 	NX_SDMMC_COMMAND cmd; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */
/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */

/* 	NX_ASSERT(0 == ((U32)pdwBuffer & 3)); */

/* 	// wait while data busy or data transfer busy */
/* 	while (pSDMMCReg->STATUS & (1 << 9 | 1 << 10)) */
/* 		pSDMMCReg->STATUS; */

/* 	//-------------------------------------------------------------------------- */
/* 	// wait until 'Ready for data' is set and card is in transfer state. */
/* 	do { */
/* 		cmd.cmdidx	= SEND_STATUS; */
/* 		cmd.arg		= pSDMMCBootStatus->rca; */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP; */
/* 		status = pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */
/* 		if (NX_SDMMC_STATUS_NOERROR != status) */
/* 			goto End; */
/* 	} while (!((cmd.response[0] & (1 << 8)) && */
/* 		(((cmd.response[0] >> 9) & 0xF) == 4))); */

/* 	NX_ASSERT(NX_SDMMC_STATUS_FIFOEMPTY == */
/* 			(pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY)); */
/* 	NX_ASSERT(0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FSMBUSY)); */

/* 	// Set byte count */
/* 	pSDMMCReg->BYTCNT = numberOfSector * BLOCK_LENGTH; */

/* 	//-------------------------------------------------------------------------- */
/* 	// Send Command */
/* 	if (numberOfSector > 1) { */
/* 		cmd.cmdidx	= READ_MULTIPLE_BLOCK; */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP | */
/* 				NX_SDMMC_CMDFLAG_BLOCK | */
/* 				NX_SDMMC_CMDFLAG_RXDATA | */
/* 				NX_SDMMC_CMDFLAG_SENDAUTOSTOP; */
/* 	} else { */
/* 		cmd.cmdidx	= READ_SINGLE_BLOCK; */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_WAITPRVDAT | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP | */
/* 				NX_SDMMC_CMDFLAG_BLOCK | */
/* 				NX_SDMMC_CMDFLAG_RXDATA; */
/* 	} */
/* 	cmd.arg		= (pSDMMCBootStatus->bHighCapacity) ? */
/* 				SectorNum : SectorNum*BLOCK_LENGTH; */

/* 	status = pbl0fn->NX_SDMMC_SendCommand(pSDMMCBootStatus, &cmd); */
/* 	if (NX_SDMMC_STATUS_NOERROR != status) */
/* 		goto End; */

/* 	//-------------------------------------------------------------------------- */
/* 	// Read data */
/* 	if (CTRUE != pbl0fn->NX_SDMMC_ReadSectorData(pSDMMCBootStatus, */
/* 						numberOfSector, pdwBuffer)) */
/* 		goto End; */

/* 	NX_ASSERT(NX_SDMMC_STATUS_FIFOEMPTY == */
/* 			(pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY)); */
/* 	NX_ASSERT(0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOFULL)); */
/* 	NX_ASSERT(0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOCOUNT)); */

/* 	if (numberOfSector > 1) { */
/* 		// Wait until the Auto-stop command has been finished. */
/* 		while (0 == (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_ACD)) */
/* 			pSDMMCReg->RINTSTS; */

/* 		NX_ASSERT(0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FSMBUSY)); */

/* 		#if defined(NX_DEBUG) */
/* 		// Get Auto-stop response and then check it. */
/* 		response = pSDMMCReg->RESP[1]; */
/* 		if (response & 0xFDF98008) { */
/* 			pbl0fn->_dprintf("Auto Stop Resp Fail:%x\r\n", response); */
/* 			//goto End; */
/* 		} */
/* 		#endif */
/* 	} */

/* 	result = CTRUE; */

/* End: */
/* 	if (CFALSE == result) { */
/* 		cmd.cmdidx	= STOP_TRANSMISSION; */
/* 		cmd.arg		= 0; */
/* 		cmd.flag	= NX_SDMMC_CMDFLAG_STARTCMD | */
/* 				NX_SDMMC_CMDFLAG_CHKRSPCRC | */
/* 				NX_SDMMC_CMDFLAG_SHORTRSP | */
/* 				NX_SDMMC_CMDFLAG_STOPABORT; */
/* 		pbl0fn->NX_SDMMC_SendCommandInternal(pSDMMCBootStatus, &cmd); */

/* 		if (0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY)) { */
/* 			pSDMMCReg->CTRL = NX_SDMMC_CTRL_FIFORST; */
/* 			while (pSDMMCReg->CTRL & NX_SDMMC_CTRL_FIFORST) */
/* 				pSDMMCReg->CTRL; */
/* 		} */
/* 	} */
        
//	return result;
  return 1;
}
#if 1
//------------------------------------------------------------------------------
CBOOL NX_SDMMC_ReadBootSector(
		SDMMCBOOTSTATUS *pSDMMCBootStatus,
		U32 numberOfSector,
		U32 *pdwBuffer)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	U32		count; */
/* 	register struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 					pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	NX_ASSERT(0==((U32)pdwBuffer & 3)); */

/* 	count = numberOfSector * BLOCK_LENGTH; */
/* 	NX_ASSERT(0 == (count % 32)); */

/* 	while (0 < count) { */
/* 		if ((pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_RXDR) */
/* 		 || (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DTO)) { */
/* 			U32 FSize, Timeout = NX_SDMMC_TIMEOUT; */
/* 			while ((pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY) && */
/* 					Timeout--) */
/* 				pSDMMCReg->STATUS; */
/* //			if (0 == Timeout) */
/* //				break; */
/* 			FSize = (pSDMMCReg->STATUS & */
/* 					NX_SDMMC_STATUS_FIFOCOUNT) >> 17; */
/* 			count -= (FSize * 4); */
/* 			while (FSize) { */
/* 				*pdwBuffer++ = pSDMMCReg->DATA; */
/* 				FSize--; */
/* 			} */

/* 			pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_RXDR; */
/* 		} */
/* #if 0 */
/* 		// Check Errors */
/* 		if (pSDMMCReg->RINTSTS & (NX_SDMMC_RINTSTS_DRTO | */
/* 					NX_SDMMC_RINTSTS_EBE | */
/* 					NX_SDMMC_RINTSTS_SBE | */
/* 					NX_SDMMC_RINTSTS_DCRC)) { */
/* 			pbl0fn->_dprintf("Read left = %x\r\n", count); */

/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DRTO) */
/* 				pbl0fn->_dprintf("DRTO\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_EBE) */
/* 				pbl0fn->_dprintf("EBE\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_SBE) */
/* 				pbl0fn->_dprintf("SBE\r\n"); */
/* 			if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DCRC) */
/* 				pbl0fn->_dprintf("DCRC\r\n"); */

/* 			return CFALSE; */
/* 		} */
/* #endif */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_DTO) { */
/* 			if (count == 0) { */
/* 				pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_DTO; */
/* 				pbl0fn->_dprintf("DTO\r\n"); */
/* 				break; */
/* 			} */
/* 		} */

/* 		#if defined(NX_DEBUG) */
/* 		if (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_HTO) { */
/* 			pbl0fn->_dprintf("HTO\r\n"); */
/* 			pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_HTO; */
/* 		} */
/* 		#endif */

/* 		NX_ASSERT(0 == (pSDMMCReg->RINTSTS & NX_SDMMC_RINTSTS_FRUN)); */
/* 	} */

/* 	pSDMMCReg->RINTSTS = NX_SDMMC_RINTSTS_DTO; */

	return CTRUE;
}
#endif


//------------------------------------------------------------------------------
CBOOL SDMMCBOOT(SDMMCBOOTSTATUS *pSDMMCBootStatus, U32 option)
{
/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* 	CBOOL	result = CFALSE; */
/* 	struct NX_SDMMC_RegisterSet * const pSDMMCReg = */
/* 				pgSDMMCReg[pSDMMCBootStatus->SDPort]; */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
        
/* 	if (CTRUE != pbl0fn->NX_SDMMC_Open(pSDMMCBootStatus, option)) { */
/* 		pbl0fn->_dprintf("device open fail\r\n"); */
/* 		goto error; */
/* 	} */

/* 	if (0 == (pSDMMCReg->STATUS & NX_SDMMC_STATUS_FIFOEMPTY)) { */
/* 		volatile U32 tempcount = 0x100000; */
/* 		pSDMMCReg->CTRL = NX_SDMMC_CTRL_FIFORST; */
/* 		/\* Wait until the FIFO reset is completed. *\/ */
/* 		while ((pSDMMCReg->CTRL & NX_SDMMC_CTRL_FIFORST) && */
/* 				tempcount--) */
/* 			pSDMMCReg->CTRL; */
/* 	} */

/* 	unsigned int *psector = (unsigned int *)&(*pbl0fn->pbm)->bi; */
/* 	unsigned int rsn = 0; */

/* 	if (pbl0fn->NX_SDMMC_ReadSectors(pSDMMCBootStatus, rsn++, 1, psector) == CFALSE) { */
/* 		pbl0fn->_dprintf("MBR read fail.\r\n"); */
/* 		goto error; */
/* 	} */

/* 	struct nx_bootinfo *pbi = &(*pbl0fn->pbm)->bi; */
/* 	U32 BootSize = pbi->LoadSize; */
/* 	pbl0fn->_dprintf("LoadAddr:%x LoadSize:%x StartAddr:%x\r\n", */
/* 		pbi->LoadAddr, pbi->LoadSize, pbi->StartAddr); */

/* 	if (BootSize > INTERNAL_SRAM_SIZE - SECONDBOOT_STACK) */
/* 		BootSize = INTERNAL_SRAM_SIZE - SECONDBOOT_STACK; */


/* 	U32 sectorsize = (BootSize - 256 + BLOCK_LENGTH - 1) / BLOCK_LENGTH; */
/* 	unsigned int tdata[1024 / 4]; */
/*        	unsigned int *pdata = (unsigned int *)(*pbl0fn->pbm)->image; */
/* 	result = pbl0fn->NX_SDMMC_ReadSectors(pSDMMCBootStatus, rsn, 2, tdata); */
/* 	rsn += 2; */

/* 	/\* pbl0fn->nx_memcpy(&(*pbl0fn->pbm)->rsa_public, tdata, sizeof(struct asymmetrickey)); *\/ */
/* 	/\* pbl0fn->nx_memcpy(pdata, tdata, 1024 - sizeof(struct asymmetrickey)); *\/ */

/* 	/\*=========================================================== */
/* 	 * boot key certify */
/* 	 * ========================================================*\/ */

/* 	result = pbl0fn->NX_SDMMC_ReadSectors(pSDMMCBootStatus, */
/* 			rsn, sectorsize, &pdata[256]); */

/* error: */
/* 	return result; */
  return 1;
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

/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */

/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
/* 	pbl0fn->setpad(sdmmcpad[PortNum], 10, 1); */
}

void NX_SDPADSetGPIO(U32 PortNum)
{

/* 	NXBL0FN *pbl0fn = Getbl0fnPtr(); */
/* #ifdef QEMU_RISCV */
/*         pbl0fn->kputs("------------------------------------"); */
/*         pbl0fn->_dprintf("[bootrom] %s start\n",__func__); */
/* #endif */
/* 	pbl0fn->setpad(sdmmcpad[PortNum], 10, 0); */
}

//------------------------------------------------------------------------------
U32 iSDMMCBOOT(U32 option)
{
	NXBL0FN *pbl0fn = Getbl0fnPtr();
	SDMMCBOOTSTATUS SDMMCBootStatus, *pSDMMCBootStatus;
	CBOOL	result = CFALSE;

#ifdef QEMU_RISCV
        pbl0fn->kputs("------------------------------------");
        pbl0fn->_dprintf("[bootrom] %s start\n",__func__);
#endif

	pSDMMCBootStatus = &SDMMCBootStatus;

	pSDMMCBootStatus->SDPort = ((option >> SELSDPORT) & 0x3);

	if (pSDMMCBootStatus->SDPort >= 3) {
		pSDMMCBootStatus->SDPort = 0;
		pSDMMCBootStatus->bHighSpeed = CTRUE;
	} else
		pSDMMCBootStatus->bHighSpeed = CFALSE;

#ifndef QEMU_RISCV
	pbl0fn->NX_SDPADSetALT(pSDMMCBootStatus->SDPort);
#endif
        
#ifdef DEBUG
        _dprintf("[bootrom] pSDMMCBootStatus = 0x%x\n",pSDMMCBootStatus);
#endif        
	pbl0fn->NX_SDMMC_Init(pSDMMCBootStatus);

        //	pbl0fn->_dprintf("SD boot:%d\r\n", pSDMMCBootStatus->SDPort);
#ifdef QEMU_RISCV
        pbl0fn->_dprintf("[bootrom] %s : SDBOOT = 0x%lx\n",__func__,pSDMMCBootStatus->SDPort);
#endif
	result = pbl0fn->SDMMCBOOT(pSDMMCBootStatus, option);

	pbl0fn->NX_SDMMC_Close(pSDMMCBootStatus);
	pbl0fn->NX_SDMMC_Terminate(pSDMMCBootStatus);

#ifndef QEMU_RISCV        
	pbl0fn->NX_SDPADSetGPIO(pSDMMCBootStatus->SDPort);
#endif

#ifdef QEMU_RISCV
        pbl0fn->_dprintf("[bootrom] %s end\n",__func__);
        pbl0fn->kputs("------------------------------------");
#endif
	return result;
}

