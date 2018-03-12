/*
 * Copyright (C) 2018 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module	: SDMMC
 * File		: nx_sdmmc.h
 * Description  :
 * ReferenceDoc : DesignWare Cores Mobile Storage Host Controller Databook, Version 1.60a
 * Author	: S/W A, suker
 * History	: 2018.03.12 Created
 */
#ifndef __NX_SDMMC_H__
#define __NX_SDMMC_H__

#include "nx_type.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/*
--------------------------------------------------------------
Read (or Write) Behavior Description
--------------------------------------------------------------
RC   -   A read clears this register field.
RS   -   A read sets this register field.
RM   -   A read modifies the contents of this register field.
Wo   -   You can only write to this register once field.
W1C  -   A write of 1 clears this register field.
W1S  -   A write of 1 sets this register field.
W1T  -   A write of 1 toggles this register field.
W0C  -   A write of 0 clears this register field.
W0S  -   A write of 0 sets this register field.
W0T  -   A write of 0 toggles this register field.
WC   -   Any write clears this register field.
WS   -   Any write sets this register field.
WM   -   Any write toggles this register field.
no Read Behavior attribute    -   You cannot read this register. It is Write-Only.
no Write Behavior attribute   -   You cannot write to this register. It is Read-Only.
---------------------------------------------------------------

---------------------------------------------------------------
Memory Access Description
---------------------------------------------------------------
R       -   Read-only register field.
W       -   Write-only register field.
R/W     -   Read/write register field.
R/W1C   -   You can read this register field. Writing 1 clears it.
RC/W1C  -   Reading this register field clears it. Writing 1 clears it.
R/Wo    -   You can read this register field. You can only write to it once.
---------------------------------------------------------------
*/

// SDMMC Module's Register Map
typedef struct SnopsysRegisterSet
{
  //                                   offset       Description   
  volatile U32 SDMASA_R;               //0x0        SDMA System Address Register
  volatile U16 BLOCKSIZE_R;            //0x4        Block Size register
  volatile U16 BLOCKCOUNT_R;           //0x6        16-bit Block Count Register
  volatile U32 ARGUMENT_R;             //0x8        Argument Register
  volatile U16 XFER_MODE_R;            //0xc        Transfer Mode Register
  volatile U16 CMD_R;                  //0xe        Command Register
  volatile U32 RESP01_R;               //0x10       Response Register 0/1
  volatile U32 RESP23_R;               //0x14       Response Register 2/3
  volatile U32 RESP45_R;               //0x18       Response Register 4/5
  volatile U32 RESP67_R;               //0x1c       Response Register 6/7
  volatile U32 BUF_DATA_R;             //0x20       Buffer DataPort Register
  volatile U32 PSTATE_REG;             //0x24       Present State Register
  volatile U8  HOST_CTRL1_R;           //0x28       Host Control 1 Register
  volatile U8  PWR_CTRL_R;             //0x29       Power Control Register
  volatile U8  BGAP_CTRL_R;            //0x2a       Block Gap Contrl Register
  volatile U8  WUP_CTRL_R;             //0x2b       Wakeup Contrl Register
  volatile U16 CLK_CTRL_R;             //0x2c       Clock Control Register
  volatile U8  TOUT_CTRL_R;            //0x2e       Timeout Control Register
  volatile U8  SW_RST_R;               //0x2f       Software Reset Register
  volatile U16 NORMAL_INT_STAT_R;      //0x30       Normal Interrupt Status Register
  volatile U32 ERROR_INT_STAT_R;       //0x32       Error Interrupt Status Register
} NX_SDMMC_RegisterSet;


// BLOCKSIZE_R detail
typedef struct blocksize_r
{
  volatile U16 XFER_BLOCK_SIZE;        //11:0
                                       //Transfer Block Size, 1:1byte,2:2byte... 0x200:512byte
  volatile U8  SDMA_VUG_BDARY;         //14:12      SDMA Buffer Boundary
  volatile U8  RSVD_BLOCKSIZE15;       //15         It always returns 0.
} NX_SDMMC_BLOCKSIZE_R;

// XFER_MODE_R
typedef struct xfer_mode_r
{
  volatile U8  DMA_ENABLE;             //0          DMA Enable
                                       //0x1:DMA Data transfer enable, 0x0:Disable
  volatile U8  BLOCK_COUNT_ENABLE;     //1          Block Count Enable
  volatile U8  AUTO_CMD_ENABLE;        //3:2        Auto Command Enable
  volatile U8  DATA_XFER_DIR;          //4          Data Transfer Direction Select
  volatile U8  MULTI_BLK_SEL;          //5          Multi/Single Block Select
  volatile U8  RESP_TYPE;              //6          Response Type R1/R5
  volatile U8  RESP_ERR_CHK_ENABLE;    //7          Response Error Check Enable
  volatile U8  RESP_INT_DISABLE;       //8          Response Interrupt Disable
  volatile U8  RSVD;                   //15:9       Reserved
} NX_SDMMC_XFER_MODE_R;

// CMD_R
typedef struct cmd_r
{
  volatile U8  RESP_TYPE_SELECT;       //1:0        Response Type Select
  volatile U8  SUB_CMD_FLAG;           //2          Sub Command Flag
  volatile U8  CMD_CRC_CHK_ENABLE;     //3          Command CRC Check Enable
  volatile U8  CMD_IDX_CHK_ENABLE;     //4          Command Index Check Enable
  volatile U8  DATA_PRESENT_SEL;       //5          Data Present Select
  volatile U8  CMD_TYPE;               //7:6        Command Type
  volatile U8  CMD_INDEX;              //13:8       Command Index
  volatile U8  RSVD;                   //15:14      Reserved
} NX_SDMMC_CMD_R;
  

// PSTATE_REG
typedef struct pstate_reg
{
  volatile U8  CMD_INHIBIT;            //0           //Command Inhibit (DAT)
  volatile U8  CMD_INHIBIT_DAT;        //1           //Command Inhibit (DAT)
  volatile U8  DAT_LINE_ACTIVE;        //2           //DAT Line Active (SD/eMMC Mode only)
  volatile U8  RE_TUNE_REQ;            //3           //Re-Tuning Request
  volatile U8  DAT_7_4;                //7:4         //DAT[7:4] Line Signal Level
  volatile U8  WR_XFER_ACTIVE;         //8           //Write Transfer Active
  volatile U8  RD_XFER_ACTIVE;         //9           //Read Transfer Active
  volatile U8  BUF_WR_ENABLE;          //10          //Buffer Write Enable
  volatile U8  BUF_RD_ENABLE;          //11          //Buffer Read Enable
  volatile U8  RSVD_15_12;             //15:12       //Reserved
  volatile U8  CARD_INSERTED;          //16          //Card Inserted
  volatile U8  CARD_STABLE;            //17          //Card Stable
  volatile U8  CARD_DETECT_PIN_LEVEL;  //18          //Card Detect Pin Level
  volatile U8  WR_PROTECT_SW_LVL;      //19          //Write Protect Switch Pin Level
  volatile U8  DAT_3_0;                //23:20       //DAT[3:0] Line Signal Level
  volatile U8  CMD_LINE_LVL;           //24          //Command-Line Signal Level
  volatile U8  HOST_REG_VOL;           //25          //Host Regulator Voltage Stable
  volatile U8  RSVD_26;                //26          //Reserved
  volatile U8  CMD_ISSUE_ERR;          //27          //Command Not Issued by Error
  volatile U8  SUB_CMD_STAT;           //28          //Sub Command Status
  volatile U8  IN_DORMANT_ST;          //29          //In Dormant Status
  volatile U8  LANE_SYNC;              //30          //Lane Synchronization
  volatile U8  UHS2_IF_DETECT;         //31          //UHS-II Interface Detection
} NX_SDMMC_PSTATE_REG;

// HOST_CTRL1_R
typedef struct host_ctrl1_r
{
  volatile U8  LED_CTRL;               //0           //LED Control
  volatile U8  DAT_XFER_WIDTH;         //1           //Data Transfer Width
  volatile U8  HIGH_SPEED_EN;          //2           //High Speed Enable (SD/eMMC Mode only)
  volatile U8  DMA_SEL;                //4:3         //DMA Select
  volatile U8  EXT_DAT_XFER;           //5           //Extended Data Transfer Width
  volatile U8  CARD_DETECT_TEST_LVL;   //6           //Card Detect Test Level
  volatile U8  CARD_DETECT_SIG_SEL;    //7           //Card Detect Signal Selection
} NX_SDMMC_HOST_CTRL1_R;

typedef struct pwr_ctrl_r
{
  volatile U8  SD_BUS_PWR_VDD1;        //0           //SD Bus Power for VDD1
  volatile U8  SD_BUS_VOL_VDD1;        //3:1         //SD Bus Voltage Select for VDD1/eMMC Bus Voltage Select for VDD
  volatile U8  SD_BUS_PWR_VDD2;        //4           //SD Bus Power for VDD2
                                       /* 0x0 : Power off
                                          0x1 : Power on */
  volatile U8  SD_BUS_VOL_VDD2;        //7:5         //SD Bus Voltage Select for VDD2
                                       /* 0x7 (NOT_USED7): Not used
                                          0x6 (NOT_USED6): Not used
                                          0x5 (V_1_8): 1.8V
                                          0x4 (V_1_2): Reserved for 1.2V
                                          0x3 (RSVD3): Reserved
                                          0x2 (RSVD2): Reserved
                                          0x1 (RSVD1): Reserved
                                          0x0 (NO_VDD2): VDD2 Not Supported   */
} NX_SDMMC_PWR_CTRL_R;

typedef struct bgap_ctrl_r
{
  volatile U8  STOP_BG_REQ;            //0           //Stop At Block Gap Request
  volatile U8  CONTINUE_REQ;;          //1           //Continue Request
  volatile U8  RD_WAIT_CTRL;           //2           //Read Wait Control
  volatile U8  INT_AT_BGAP;            //3           //Interrupt At Block Gap
  volatile U8  RSVD_7_4;               //7:4         //Reserved
} NX_SDMMC_BGAP_CTRL_R;

typedef struct wup_ctrl_r
{
  volatile U8  CARD_INT;               //0           //Wakeup Event Enable on Card Interrupt
  volatile U8  CARD_INSERT;            //1           //Wakeup Event Enable on SD Card Insertion
  volatile U8  CARD_REMOVAL;           //2           //Wakeup Event Enable on SD Card Removal, 0:Disable, 1:Enable
  volatile U8  RSVD_7_3;               //7:3         //Reserved
} NX_SDMMC_WUP_CTRL_R;
  
typedef struct clk_ctrl_r
{
  volatile U8  INTERNAL_CLK_EN;        //0           //Internal Clock Enable
  volatile U8  INTERNAL_CLK_STABLE;    //1           //Internal Clock Stable
  volatile U8  SD_CLK_EN;              //2           //SD/eMMC Clock Enable
  volatile U8  PLL_ENABLE;             //3           //PLL Enable
  volatile U8  RSVD_4;                 //4           //Reserved
  volatile U8  CLK_GEN_SELECT;         //5           //Clock Generator Select
  volatile U8  UPPER_FREQ_SEL;         //7:6         //The upper 2bits of 10-bit SDCLK/RCLK Frequency Select Control
  volatile U8  FREQ_SEL;               //15:8        //SDCLK/RCLK Frequency Select
} NX_SDMMC_CLK_CTRL_R;

typedef struct tout_ctrl_r
{
  volatile U8  TOUT_CNT;               //3:0         //Data Timeout Counter Value
  volatile U8  RSVD_7_4;               //7:4         //Reserved
} NX_SDMMC_TOUT_CTRL_R;

typedef struct sw_rst_r
{
  volatile U8  SW_RST_ALL;             //0           //Software Reset For All
  volatile U8  SW_RST_CMD;             //1           //Software Reset For CMD line
  volatile U8  SW_RST_DAT;             //2           //Software Reset For DAT line
  volatile U8  RSVD_7_3;               //7:3         //Reserved, always return 0
} NX_SDMMC_SW_RST_R;

typedef struct normal_int_stat_r
{
  volatile U8  CMD_COMPLETE;           //0           //Command Complete
  volatile U8  XFER_COMPLETE;          //1           //Transfer Complete
  volatile U8  BGAP_EVENT;             //2           //Block Gap Event
  volatile U8  DMA_INTERRUPT;          //3           //DMA Interrupt
  volatile U8  BUF_WR_READY;           //4           //Buffer Write Ready
  volatile U8  BUF_RD_READY;           //5           //Buffer Read Ready
  volatile U8  CARD_INSERTION;         //6           //Card Insertion
  volatile U8  CARD_REMOVAL;           //7           //Card Removal
  volatile U8  CARD_INTERRUPT;         //8           //Card Interrupt
  volatile U8  INT_A;                  //9           //INT_A
  volatile U8  INT_B;                  //10          //INT_B
  volatile U8  INT_C;                  //11          //INT_C
  volatile U8  RE_TUNE_EVENT;          //12          //Re-tuning Event
  volatile U8  FX_EVENT;               //13          //FX Event
  volatile U8  CQE_EVNET;              //14          //Command Queuing Event
  volatile U8  ERR_INTERRUPT;          //15          //Error Interrupt, 0(FALSE):No Error, 1(TRUE):Error
} NX_SDMMC_NORMAL_INT_STAT_R;

typedef struct 
{  
} ;

typedef struct 
{  
} ;
  







  

/// @brief  Command FSM state for NX_SDMMC_GetCommandFSM().
typedef enum
{
	NX_SDMMC_CMDFSM_IDLE            = 0,    ///< Idle.
	NX_SDMMC_CMDFSM_SENDINIT        = 1,    ///< Send init sequence.
	NX_SDMMC_CMDFSM_TXCMDSTART      = 2,    ///< Tx command start bit
	NX_SDMMC_CMDFSM_TXCMDTX         = 3,    ///< Tx command tx bit.
	NX_SDMMC_CMDFSM_TXCMDINDEXARG   = 4,    ///< Tx command index + arg.
	NX_SDMMC_CMDFSM_TXCMDCRC7       = 5,    ///< Tx command CRC7.
	NX_SDMMC_CMDFSM_TXCMDEND        = 6,    ///< Tx command end bit.
	NX_SDMMC_CMDFSM_RXRSPSTART      = 7,    ///< Rx response start bit.
	NX_SDMMC_CMDFSM_RXRSPIRQ        = 8,    ///< Rx response IRQ response.
	NX_SDMMC_CMDFSM_RXRSPTX         = 9,    ///< Rx response tx bit.
	NX_SDMMC_CMDFSM_TXRSPCMDIDX     = 10,   ///< Rx response command index.
	NX_SDMMC_CMDFSM_RXRSPDATA       = 11,   ///< Rx response data.
	NX_SDMMC_CMDFSM_RXRSPCRC7       = 12,   ///< Rx response CRC7.
	NX_SDMMC_CMDFSM_RXRSPEND        = 13,   ///< Rx response end bit.
	NX_SDMMC_CMDFSM_CMDWAITNCC      = 14,   ///< Command path wait NCC.
	NX_SDMMC_CMDFSM_WAIT            = 15    ///< Wait; Comamnd to Response turnaround.
} NX_SDMMC_CMDFSM;

/// @brief  SDMMC clock phase shift
typedef enum
{
	NX_SDMMC_CLOCK_SHIFT_0          = 0,
	NX_SDMMC_CLOCK_SHIFT_90         = 1,
	NX_SDMMC_CLOCK_SHIFT_180        = 2,
	NX_SDMMC_CLOCK_SHIFT_270        = 3
} NX_SDMMC_CLKSHIFT;

typedef enum
{
	NX_SDMMC_CLOCK_SOURCE_0		= 0,
	NX_SDMMC_CLOCK_SOURCE_1		= 1,
	NX_SDMMC_CLOCK_SOURCE_2		= 2,
	NX_SDMMC_CLOCK_SOURCE_3		= 3,
} NX_SDMMC_CLOCK_SOURCE;

//------------------------------------------------------------------------------
/// @brief  SDMMC Module's MACRO
//------------------------------------------------------------------------------
#define NX_SDMMC_MAX_BIU_DATASIZE   (1UL<<12)

//------------------------------------------------------------------------------
/// @brief  To remove following waring on RVDS compiler
//          Warning : #66-D: enumeration value is out of "int" range
//------------------------------------------------------------------------------
//#ifdef __arm
//#pragma diag_remark 66      // disable #66 warining
//#endif

//------------------------------------------------------------------------------
/// @name   Module Interface
//------------------------------------------------------------------------------
//@{
CBOOL   NX_SDMMC_Initialize( void );
U32     NX_SDMMC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
/// @name   Basic Interface
//------------------------------------------------------------------------------
//@{
U32     NX_SDMMC_GetPhysicalAddress( U32 ModuleIndex );
U32     NX_SDMMC_GetSizeOfRegisterSet( void );

void    NX_SDMMC_SetBaseAddress( U32 ModuleIndex, void* BaseAddress );
void*    NX_SDMMC_GetBaseAddress( U32 ModuleIndex );

CBOOL   NX_SDMMC_OpenModule( U32 ModuleIndex );
CBOOL   NX_SDMMC_CloseModule( U32 ModuleIndex );

CBOOL   NX_SDMMC_CheckBusy( U32 ModuleIndex );
CBOOL   NX_SDMMC_CanPowerDown( U32 ModuelIndex );
//@}

//------------------------------------------------------------------------------
/// @name   Interrupt Interface
//------------------------------------------------------------------------------
//@{
S32     NX_SDMMC_GetInterruptNumber( U32 ModuleIndex );

void    NX_SDMMC_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL   NX_SDMMC_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL   NX_SDMMC_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void    NX_SDMMC_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void    NX_SDMMC_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL   NX_SDMMC_GetInterruptPendingAll( U32 ModuleIndex );
void    NX_SDMMC_ClearInterruptPendingAll( U32 ModuleIndex );
S32     NX_SDMMC_GetInterruptPendingNumber( U32 ModuleIndex );

void    NX_SDMMC_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32     NX_SDMMC_GetInterruptEnable32( U32 ModuleIndex );
U32     NX_SDMMC_GetInterruptPending32( U32 ModuleIndex );
void    NX_SDMMC_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );
//@}

//------------------------------------------------------------------------------
/// @name   Clock Control Interface
//------------------------------------------------------------------------------
//@{
U32     NX_SDMMC_GetClockNumber( U32 ModuleIndex );
U32     NX_SDMMC_GetResetNumber( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
/// @name   Module customized operations
//------------------------------------------------------------------------------
//@{
void    NX_SDMMC_SetPowerEnable( U32 ModuleIndex, U32 PowerIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetPowerEnable( U32 ModuleIndex, U32 PowerIndex );

void    NX_SDMMC_AbortReadData( U32 ModuleIndex );
void    NX_SDMMC_SendIRQResponse( U32 ModuleIndex );
void    NX_SDMMC_SetReadWait( U32 ModuleIndex, CBOOL bAssert );

//void    NX_SDMMC_ResetDMA( U32 ModuleIndex );
//CBOOL   NX_SDMMC_IsResetDMA( U32 ModuleIndex );
void    NX_SDMMC_ResetDMAC( U32 ModuleIndex );
void    NX_SDMMC_SetDMAMode( U32 ModuleIndex, CBOOL Enable );
void    NX_SDMMC_SetUseInternalDMAC( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_IsDMAReq( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsDMAAck( U32 ModuleIndex );

void    NX_SDMMC_ResetFIFO( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsResetFIFO( U32 ModuleIndex );
void    NX_SDMMC_ResetController( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsResetController( U32 ModuleIndex );

void    NX_SDMMC_SetClockSource( U32 ModuleIndex, NX_SDMMC_CLOCK_SOURCE ClkSrc );
NX_SDMMC_CLOCK_SOURCE    NX_SDMMC_GetClockSource( U32 ModuleIndex );
void    NX_SDMMC_SetOutputClockDivider( U32 ModuleIndex, U32 divider );
U32     NX_SDMMC_GetOutputClockDivider( U32 ModuleIndex );
void    NX_SDMMC_SetLowPowerClockMode( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetLowPowerClockMode( U32 ModuleIndex );
void    NX_SDMMC_SetOutputClockEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetOutputClockEnable( U32 ModuleIndex );

void    NX_SDMMC_SetDriveClockShiftPhase( U32 ModuleIndex, NX_SDMMC_CLKSHIFT ClockShift );
NX_SDMMC_CLKSHIFT   NX_SDMMC_GetDriveClockShiftPhase( U32 ModuleIndex );
void    NX_SDMMC_SetSampleClockShiftPhase( U32 ModuleIndex, NX_SDMMC_CLKSHIFT ClockShift );
NX_SDMMC_CLKSHIFT   NX_SDMMC_GetSampleClockShiftPhase( U32 ModuleIndex );
void    NX_SDMMC_SetDriveClockDelay( U32 ModuleIndex, U32 Delay );
U32     NX_SDMMC_GetDriveClockDelay( U32 ModuleIndex );
void    NX_SDMMC_SetSampleClockDelay( U32 ModuleIndex, U32 Delay );
U32     NX_SDMMC_GetSampleClockDelay( U32 ModuleIndex );

void    NX_SDMMC_SetDataTimeOut( U32 ModuleIndex, U32 dwTimeOut );
U32     NX_SDMMC_GetDataTimeOut( U32 ModuleIndex );
void    NX_SDMMC_SetResponseTimeOut( U32 ModuleIndex, U32 dwTimeOut );
U32     NX_SDMMC_GetResponseTimeOut( U32 ModuleIndex );

void    NX_SDMMC_SetDataBusWidth( U32 ModuleIndex, U32 width );
U32     NX_SDMMC_GetDataBusWidth( U32 ModuleIndex );

void    NX_SDMMC_SetBlockSize( U32 ModuleIndex, U32 SizeInByte );
U32     NX_SDMMC_GetBlockSize( U32 ModuleIndex );
void    NX_SDMMC_SetByteCount( U32 ModuleIndex, U32 SizeInByte );
U32     NX_SDMMC_GetByteCount( U32 ModuleIndex );

void    NX_SDMMC_SetCommandArgument( U32 ModuleIndex, U32 argument );
void    NX_SDMMC_SetCommand( U32 ModuleIndex, U32 Cmd, U32 flag );
void    NX_SDMMC_StartCommand( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsCommandBusy( U32 ModuleIndex );

U32     NX_SDMMC_GetShortResponse( U32 ModuleIndex );
void    NX_SDMMC_GetLongResponse( U32 ModuleIndex, U32 *pLongResponse );
U32     NX_SDMMC_GetAutoStopResponse( U32 ModuleIndex );
U32     NX_SDMMC_GetResponseIndex( U32 ModuleIndex );

void    NX_SDMMC_SetFIFORxThreshold( U32 ModuleIndex, U32 Threshold );
U32     NX_SDMMC_GetFIFORxThreshold( U32 ModuleIndex );
void    NX_SDMMC_SetFIFOTxThreshold( U32 ModuleIndex, U32 Threshold );
U32     NX_SDMMC_GetFIFOTxThreshold( U32 ModuleIndex );


U32     NX_SDMMC_GetFIFOCount( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsFIFOFull( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsFIFOEmpty( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsFIFOTxThreshold( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsFIFORxThreshold( U32 ModuleIndex );

U32     NX_SDMMC_GetDataTransferSize( U32 ModuleIndex );
U32     NX_SDMMC_GetFIFOTransferSize( U32 ModuleIndex );

void    NX_SDMMC_SetData( U32 ModuleIndex, U32 dwData );
U32     NX_SDMMC_GetData( U32 ModuleIndex );
void    NX_SDMMC_SetData32( U32 ModuleIndex, const U32 *pdwData );
void    NX_SDMMC_GetData32( U32 ModuleIndex, U32 *pdwData );
volatile U32*   NX_SDMMC_GetDataPointer( U32 ModuleIndex );

CBOOL   NX_SDMMC_MakeBIU( NX_SDMMC_BIUConfig *pBIUC, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_GetDataBIU( U32 ModuleIndex, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_SetDataBIU( U32 ModuleIndex, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_PollDemand( U32 ModuleIndex );

U32     NX_SDMMC_GetDMACStatus( U32 ModuleIndex );
void    NX_SDMMC_SetDMACStatus( U32 ModuleIndex, U32 SetData );
void    NX_SDMMC_SetDMACIntEnable( U32 ModuleIndex, U32 IntFlag );
U32     NX_SDMMC_GetDMACIntEnable( U32 ModuleIndex );
CBOOL   NX_SDMMC_GetDMACResetStatus( U32 ModuleIndex );
void    NX_SDMMC_SetDMACBurstLength( U32 ModuleIndex, U32 BurstLength );
void    NX_SDMMC_SetIDMACEnable( U32 ModuleIndex, CBOOL bEnable );
CBOOL   NX_SDMMC_GetIDMACEnable( U32 ModuleIndex );

void    NX_SDMMC_SetDescSkipLen( U32 ModuleIndex, U32 uLength );
U32     NX_SDMMC_GetDescSkipLen( U32 ModuleIndex );
void    NX_SDMMC_ResetIDMAC( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsResetIDMAC( U32 ModuleIndex );
void    NX_SDMMC_SetDebounce( U32 ModuleIndex, U32 uDebounce );
U32     NX_SDMMC_GetDebounce( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsIDMACSupported( U32 ModuleIndex );

void    NX_SDMMC_SetCardVoltage( U32 ModuleIndex, U32 VolBase, U32 VolOffset);
U32     NX_SDMMC_GetCardVoltage( U32 ModuleIndex, U32 VolBase);

CBOOL   NX_SDMMC_IsDataTransferBusy( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsCardDataBusy( U32 ModuleIndex );
CBOOL   NX_SDMMC_IsCardPresent( U32 ModuleIndex );

void    NX_SDMMC_SetDDRMode( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetDDRMode( U32 ModuleIndex );
void    NX_SDMMC_SetVoltageMode( U32 ModuleIndex, CBOOL Enable );
CBOOL   NX_SDMMC_GetVoltageMode( U32 ModuleIndex );

NX_SDMMC_CMDFSM NX_SDMMC_GetCommandFSM( U32 ModuleIndex );
//@}


#ifdef  __cplusplus
}
#endif

#endif  // __NX_SDMMC_H__

