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
 * History	:
 */
#ifndef __NX_SDMMC_H__
#define __NX_SDMMC_H__

#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct  NX_SDMMC_RegisterSet_2_9
{
	volatile unsigned int CTRL;			// 0x000 : Control
	volatile unsigned int PWREN;			// 0x004 : Power Enable
	volatile unsigned int CLKDIV;			// 0x008 : Clock Divider
	volatile unsigned int CLKSRC;			// 0x00C : Clock Source
	volatile unsigned int CLKENA;			// 0x010 : Clock Enable
	volatile unsigned int TMOUT;			// 0x014 : Time-Out
	volatile unsigned int CTYPE;			// 0x018 : Card Type
	volatile unsigned int BLKSIZ;			// 0x01C : Block Size
	volatile unsigned int BYTCNT;			// 0x020 : Byte Count
	volatile unsigned int INTMASK;			// 0x024 : Interrupt Mask
	volatile unsigned int CMDARG;			// 0x028 : Command Argument
	volatile unsigned int CMD;			// 0x02C : Command
	volatile unsigned int RESP[4];			// 0x030 : Response 0
	volatile unsigned int MINTSTS;			// 0x040 : Masked Interrupt Status
	volatile unsigned int RINTSTS;			// 0x044 : Raw Interrupt Status
	volatile unsigned int STATUS;			// 0x048 : Status - Mainly for Debug Purpose
	volatile unsigned int FIFOTH;			// 0x04C : FIFO Threshold
	volatile unsigned int CDETECT;			// 0x050 : Card Detect
	volatile unsigned int WRTPRT;			// 0x054 : Write Protect
	volatile unsigned int GPIO;			// 0x058 : General Purpose Input/Output
	volatile unsigned int TCBCNT;			// 0x05C : Transferred CIU card byte count
	volatile unsigned int TBBCNT;			// 0x060 : Transferred Host Byte Count
	volatile unsigned int DEBNCE;			// 0x064 : Card Detect Debounce
	volatile unsigned int USRID;			// 0x068 : User ID
	volatile unsigned int VERID;			// 0x06C : Version ID
	volatile unsigned int HCON;			// 0x070 : Hardware Configuration
	volatile unsigned int UHS_REG;			// 0x074 : UHS_REG register
	volatile unsigned int RSTn;			// 0x078 : Hardware reset register
	volatile unsigned int _Rev0;			// 0x07C
	volatile unsigned int BMOD;			// 0x080 : Bus Mode
	volatile unsigned int PLDMND;			// 0x084 : Poll Demand
	volatile unsigned int DBADDR;			// 0x088 : Descriptor List Base Address
	volatile unsigned int IDSTS;			// 0x08C : Internal DMAC Status
	volatile unsigned int IDINTEN;			// 0x090 : Internal DMAC Interrupt Enable
	volatile unsigned int DSCADDR;			// 0x094 : Current Host Descriptor Address
	volatile unsigned int BUFADDR;			// 0x098 : Current Buffer Descriptor Address
	volatile unsigned char  _Rev1[0x100-0x09C];	// 0x09C ~ 0x100 reserved area
	volatile unsigned int CARDTHRCTL;		// 0x100 : Card Read Threshold Enable
	volatile unsigned int BACKEND_POWER;		// 0x104 : Back-end Power
	volatile unsigned int UHS_REG_EXT;		// 0x108 : eMMC 4.5 1.2V
	volatile unsigned int EMMC_DDR_REG;		// 0x10C : eMMC DDR START bit detection control
	volatile unsigned int ENABLE_SHIFT;		// 0x110 : Phase shift control
	volatile unsigned int CLKCTRL;			// 0x114 : External clock phase & delay control
	volatile unsigned char  _Rev2[0x200-0x118];	// 0x118 ~ 0x200
	volatile unsigned int DATA;			// 0x200 : Data
	volatile unsigned char  _Rev3[0x400-0x204];	// 0x204 ~ 0x400
	volatile unsigned int TIEMODE;			// 0x400
	volatile unsigned int TIESRAM;			// 0x404
	volatile unsigned int TIEDRVPHASE;		// 0x408
	volatile unsigned int TIESMPPHASE;		// 0x40C
	volatile unsigned int TIEDSDELAY;		// 0x410

        volatile unsigned int sd_body_size;   
        volatile unsigned char sd_body[505];
} NX_SDMMC_RegisterSet;

/// @brief  SDMMC BIU
struct strNxSdmmcBiu
{
	volatile unsigned int CTRL;
	volatile unsigned int BSIZE;
	volatile unsigned int BADDR;
	struct strNxSdmmcBiu *Next;
};

typedef struct strNxSdmmcBiu    NX_SDMMC_BIU;

/// @brief  SDMMC BIU Config
typedef struct
{
	int   OWN;
	int   IntDisable;
	unsigned int     BuffAddr;
	unsigned int     DataSize;
} NX_SDMMC_BIUConfig;

/// @brief  SDMMC Interrupts for Interrupt Interface
enum    NX_SDMMC_INT
{
	NX_SDMMC_INT_SDIO   = 16,   ///< SDIO Interrupt.\r\n
	///     Interrupt from SDIO card.
	NX_SDMMC_INT_EBE    = 15,   ///< End Bit Error for reading, Write no CRC for wrting.\r\n
	///     Error in end-bit during read operations, or no data CRC or negative CRC
	///     received during write operation.\r\n\r\n
	///     \b Note> \r\n
	///     For MMC CMD19, there may be no CRC status returned by the card.
	///     Hence, NX_SDMMC_INT_EBE is set for CMD19. The application should not treat
	///     this as an error.
	NX_SDMMC_INT_ACD    = 14,   ///< Auto Command Done.\r\n
	///     Stop/abort commands automatically sent by card unit and not initiated by
	///     host; similar to Command Done (NX_SDMMC_INT_CD) interrupt.
	NX_SDMMC_INT_SBE    = 13,   ///< Start Bit Error.\r\n
	///     Error in data start bit when data is read from a card. In 4-bit mode, if all
	///     data bits do not have start bit, then this error is set.
	NX_SDMMC_INT_HLE    = 12,   ///< Hardware Locked Write Error.\r\n
	///     This interrupt is genearted when the SDMMC module cannot load a command
	///     issued by the user. When the user sets a command with NX_SDMMC_CMDFLAG_STARTCMD
	///     flag, the SDMMC module tries to load the command. If the command buffer is
	///     already filled with a command, this error is raised.
	///     And also if the user try to modify any of settings while a command loading is in
	///     progress, then the modification is ignored and the SDMMC module generates this
	///     error.
	NX_SDMMC_INT_FRUN   = 11,   ///< FIFO underrun/overrun Error.\r\n
	///     Host tried to push data when FIFO was full, or host tried to read data
	///     when FIFO was empty. Typically this should not happen, except due to
	///     error in software.\r\n
	///     SDMMC module never pushes data into FIFO when FIFO is full, and pop data
	///     when FIFO is empty.
	NX_SDMMC_INT_HTO    = 10,   ///< Data Starvation by Host Timeout.\r\n
	///     To avoid data loss, SDCLK is stopped if FIFO is empty
	///     when writing to card, or FIFO is full when reading from card. Whenever
	///     SDCLK is stopped to avoid data loss, data-starvation timeout counter
	///     is started with data-timeout value. This interrupt is set if host does not fill
	///     data into FIFO during write to card, or does not read from FIFO during
	///     read from card before timeout period.\r\n
	///     Even after timeout, SDCLK stays in stopped state with SDMMC module waiting.
	///     It is responsibility of host to push or pop data into FIFO upon interrupt,
	///     which automatically restarts SDCLK and SDMMC module.\r\n
	///     Even if host wants to send stop/abort command, it still needs to ensure it
	///     has to push or pop FIFO so that clock starts in order for stop/abort
	///     command to send on cmd signal along with data that is sent or received
	///     on data line.
	NX_SDMMC_INT_DRTO   = 9,    ///< Data Read Timeout.\r\n
	///     Data timeout occurred. Data Transfer Over (NX_SDMMC_INT_DTO) also set if data
	///     timeout occurs.
	NX_SDMMC_INT_RTO    = 8,    ///< Response Timeout.\r\n
	///     Response timeout occurred. Command Done (NX_SDMMC_INT_CD) also set if response
	///     timeout occurs. If command involves data transfer and when response
	///     times out, no data transfer is attempted by SDMMC module.
	NX_SDMMC_INT_DCRC   = 7,    ///< Data CRC Error.\r\n
	///     Received Data CRC does not match with locally-generated CRC in SDMMC module.
	NX_SDMMC_INT_RCRC   = 6,    ///< Response CRC Error.\r\n
	///     Response CRC does not match with locally-generated CRC in SDMMC module.
	NX_SDMMC_INT_RXDR   = 5,    ///< Receive FIFO Data Request.\r\n
	///     Interrupt set during read operation from card when FIFO level is greater
	///     than Receive-Threshold level.\r\n\r\n
	///     \b Recommendation> In DMA modes, this interrupt should not be enabled.\r\n\r\n
	///     ISR, in non-DMA mode: \r\n
	/// @code
	/// pop RX_WMark + 1 data from FIFO
	/// @endcode
	NX_SDMMC_INT_TXDR   = 4,    ///< Transmit FIFO Data Request.\r\n
	///     Interrupt set during write operation to card when FIFO level reaches less
	///     than or equal to Transmit-Threshold level.\r\n\r\n
	///     \b Recommendation> In DMA modes, this interrupt should not be enabled.\r\n\r\n
	///     ISR in non-DMA mode: \r\n
	/// @code
	/// if (pending_bytes > (16 - TX_WMark))
	///     push (16 - TX_WMark) data into FIFO
	/// else
	///     push pending_bytes data into FIFO
	/// @endcode
	NX_SDMMC_INT_DTO    = 3,    ///< Data Transfer Over.\r\n
	///     Data transfer completed, even if there is Start Bit Error or CRC error.
	///     This bit is also set when "read data-timeout" occurs.\r\n\r\n
	///     \b Recommendation> \r\n
	///     In non-DMA mode, when data is read from card, on seeing interrupt,
	///     host should read any pending data from FIFO.\r\n
	///     In DMA mode, DMA controllers guarantee FIFO is flushed before interrupt.\r\n\r\n
	///     \b Note> \r\n
	///     NX_SDMMC_INT_DTO is set at the end of the last data block, even if
	///     the device asserts MMC busy after the last data block.
	NX_SDMMC_INT_CD     = 2,    ///< Command Done.\r\n
	///     Command sent to card and got response from card, even if Response
	///     Error or CRC error occurs. Also set when response timeout occurs.
	NX_SDMMC_INT_RE     = 1,    ///< Response Error.\r\n
	///     Error in received response set if one of following occurs:
	///     - Transmission bit != 0
	///     - Command index mismatch
	///     - End-bit != 1
	NX_SDMMC_INT_CDET   = 0     ///< Card Detected.
};

/// @brief  Command Flags for NX3_SDMMC_SetCommand()
enum    NX_SDMMC_CMDFLAG
{
	NX_SDMMC_CMDFLAG_STARTCMD           = 1UL<<31,  ///< Start Command.
	///     Set a command with this flag to update modified settings to the
	///     SDMMC module. If this flag is set, you have not to modify any
	///     any settings until the command is taken by the SDMMC module.
	NX_SDMMC_CMDFLAG_USE_HOLD           = 1UL<<29,  ///< Use Hold Register
	///     0: CMD and DATA sent to card bypassing HOLD Register
	///     1: CMD and DATA sent to card through the HOLD Register
	NX_SDMMC_CMDFLAG_VOLT_SWITCH        = 1UL<<28,  ///< Volatage switch bit
	NX_SDMMC_CMDFLAG_BOOT_MODE          = 1UL<<27,  ///< boot mode
	NX_SDMMC_CMDFLAG_DISABLE_BOOT       = 1UL<<26,  ///< disable boot
	NX_SDMMC_CMDFLAG_EXPECT_BOOT_ACK    = 1UL<<25,  ///< expect boot ack
	NX_SDMMC_CMDFLAG_ENABLE_BOOT        = 1UL<<24,  ///< enable boot
	NX_SDMMC_CMDFLAG_CCS_EXPECTED       = 1UL<<23,  ///< ccs expected
	NX_SDMMC_CMDFLAG_READ_CEATA         = 1UL<<22,  ///< Read ceata
	NX_SDMMC_CMDFLAG_UPDATECLKONLY      = 1UL<<21,  ///< Just update clock settings for SDCLK into card clock domain.\r\n
	///     Changes card clocks (change frequency, truncate off or on, and
	///     set low-frequency mode); provided in order to change clock
	///     frequency or stop clock without having to send command to
	///     cards.\r\n
	///     Even though this flag is set with NX_SDMMC_CMDFLAG_STARTCMD flag,
	///     SDMMC module does not issue NX_SDMMC_INT_CD signal upon command completion.
	NX_SDMMC_CMDFLAG_CARD_NUMBER        = 1UL<<16,  ///< Card Number in use.
	NX_SDMMC_CMDFLAG_SENDINIT           = 1UL<<15,  ///< Send initialization sequence before sending this command.\r\n
	///     After power on, 80 clocks must be sent to card for initialization
	///     before sending any commands to card. This flag should be set while
	///     sending first command to card so that controller will initialize
	///     clocks before sending command to card.
	NX_SDMMC_CMDFLAG_STOPABORT          = 1UL<<14,  ///< Stop or abort command intended to stop current data transfer in progress.\r\n
	///     When open-ended or predefined data transfer is in progress, and
	///     host issues stop or abort command to stop data transfer, this flag should
	///     be set so that command/data state-machines of SDMMC module can return
	///     correctly to idle state.
	NX_SDMMC_CMDFLAG_WAITPRVDAT         = 1UL<<13,  ///< Wait for previous data transfer completion before sending command.\r\n
	///     Command without this flag typically used to query
	///     status of card during data transfer or to stop current data transfer.
	NX_SDMMC_CMDFLAG_SENDAUTOSTOP       = 1UL<<12,  ///< Send stop command at end of data transfer.\r\n
	///     When set, SDMMC module sends stop command to card at end of data transfer. \r\n
	///     * when this flag should be set, since some data
	///     transfers do not need explicit stop commands.\r\n
	///     * open-ended transfers that software should explicitly send to
	///     stop command.\r\n
	///     Additionally, when "resume" is sent to resume . suspended
	///     memory access of SD-Combo card . this falg should be set correctly if
	///     suspended data transfer needs send_auto_stop.\r\n
	///     Don't care if no data expected from card.
	NX_SDMMC_CMDFLAG_BLOCK              = 0UL<<11,  ///< Block data transfer command.\r\n Don't care if no data expected from card.
	NX_SDMMC_CMDFLAG_STREAM             = 1UL<<11,  ///< Stream data transfer command.\r\n Don't care if no data expected from card.
	NX_SDMMC_CMDFLAG_TXDATA             = 3UL<< 9,  ///< Write to card.\r\n Do not set this flag if no data expected from card.
	NX_SDMMC_CMDFLAG_RXDATA             = 1UL<< 9,  ///< Read form card.\r\n Do not set this flag if no data expected from card.
	NX_SDMMC_CMDFLAG_CHKRSPCRC          = 1UL<< 8,  ///< Check response CRC.\r\n
	///     Some of command responses do not return valid CRC bits.
	///     Software should disable CRC checks for those commands in
	///     order to disable CRC checking by SDMMC module.
	NX_SDMMC_CMDFLAG_SHORTRSP           = 1UL<< 6,  ///< Short response expected from card.\r\n Do not set this flag if no response is expected from card.
	NX_SDMMC_CMDFLAG_LONGRSP            = 3UL<< 6   ///< Long response expected from card.\r\n Do not set this flag if no response is expected from card.
};

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
int   NX_SDMMC_Initialize( void );
unsigned int     NX_SDMMC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
/// @name   Basic Interface
//------------------------------------------------------------------------------
//@{
unsigned int     NX_SDMMC_GetPhysicalAddress( unsigned int ModuleIndex );
unsigned int     NX_SDMMC_GetSizeOfRegisterSet( void );

void    NX_SDMMC_SetBaseAddress( unsigned int ModuleIndex, void* BaseAddress );
void*    NX_SDMMC_GetBaseAddress( unsigned int ModuleIndex );

int   NX_SDMMC_OpenModule( unsigned int ModuleIndex );
int   NX_SDMMC_CloseModule( unsigned int ModuleIndex );

int   NX_SDMMC_CheckBusy( unsigned int ModuleIndex );
int   NX_SDMMC_CanPowerDown( unsigned int ModuelIndex );
//@}

//------------------------------------------------------------------------------
/// @name   Interrupt Interface
//------------------------------------------------------------------------------
//@{
int     NX_SDMMC_GetInterruptNumber( unsigned int ModuleIndex );

void    NX_SDMMC_SetInterruptEnable( unsigned int ModuleIndex, int IntNum, int Enable );
int   NX_SDMMC_GetInterruptEnable( unsigned int ModuleIndex, int IntNum );
int   NX_SDMMC_GetInterruptPending( unsigned int ModuleIndex, int IntNum );
void    NX_SDMMC_ClearInterruptPending( unsigned int ModuleIndex, int IntNum );

void    NX_SDMMC_SetInterruptEnableAll( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_GetInterruptEnableAll( unsigned int ModuleIndex );
int   NX_SDMMC_GetInterruptPendingAll( unsigned int ModuleIndex );
void    NX_SDMMC_ClearInterruptPendingAll( unsigned int ModuleIndex );
int     NX_SDMMC_GetInterruptPendingNumber( unsigned int ModuleIndex );

void    NX_SDMMC_SetInterruptEnable32( unsigned int ModuleIndex, unsigned int EnableFlag );
unsigned int     NX_SDMMC_GetInterruptEnable32( unsigned int ModuleIndex );
unsigned int     NX_SDMMC_GetInterruptPending32( unsigned int ModuleIndex );
void    NX_SDMMC_ClearInterruptPending32( unsigned int ModuleIndex, unsigned int PendingFlag );
//@}

//------------------------------------------------------------------------------
/// @name   Clock Control Interface
//------------------------------------------------------------------------------
//@{
unsigned int     NX_SDMMC_GetClockNumber( unsigned int ModuleIndex );
unsigned int     NX_SDMMC_GetResetNumber( unsigned int ModuleIndex );
//@}

//------------------------------------------------------------------------------
/// @name   Module customized operations
//------------------------------------------------------------------------------
//@{
void    NX_SDMMC_SetPowerEnable( unsigned int ModuleIndex, unsigned int PowerIndex, int Enable );
int   NX_SDMMC_GetPowerEnable( unsigned int ModuleIndex, unsigned int PowerIndex );

void    NX_SDMMC_AbortReadData( unsigned int ModuleIndex );
void    NX_SDMMC_SendIRQResponse( unsigned int ModuleIndex );
void    NX_SDMMC_SetReadWait( unsigned int ModuleIndex, int bAssert );

//void    NX_SDMMC_ResetDMA( unsigned int ModuleIndex );
//int   NX_SDMMC_IsResetDMA( unsigned int ModuleIndex );
void    NX_SDMMC_ResetDMAC( unsigned int ModuleIndex );
void    NX_SDMMC_SetDMAMode( unsigned int ModuleIndex, int Enable );
void    NX_SDMMC_SetUseInternalDMAC( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_IsDMAReq( unsigned int ModuleIndex );
int   NX_SDMMC_IsDMAAck( unsigned int ModuleIndex );

void    NX_SDMMC_ResetFIFO( unsigned int ModuleIndex );
int   NX_SDMMC_IsResetFIFO( unsigned int ModuleIndex );
void    NX_SDMMC_ResetController( unsigned int ModuleIndex );
int   NX_SDMMC_IsResetController( unsigned int ModuleIndex );

void    NX_SDMMC_SetClockSource( unsigned int ModuleIndex, NX_SDMMC_CLOCK_SOURCE ClkSrc );
NX_SDMMC_CLOCK_SOURCE    NX_SDMMC_GetClockSource( unsigned int ModuleIndex );
void    NX_SDMMC_SetOutputClockDivider( unsigned int ModuleIndex, unsigned int divider );
unsigned int     NX_SDMMC_GetOutputClockDivider( unsigned int ModuleIndex );
void    NX_SDMMC_SetLowPowerClockMode( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_GetLowPowerClockMode( unsigned int ModuleIndex );
void    NX_SDMMC_SetOutputClockEnable( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_GetOutputClockEnable( unsigned int ModuleIndex );

void    NX_SDMMC_SetDriveClockShiftPhase( unsigned int ModuleIndex, NX_SDMMC_CLKSHIFT ClockShift );
NX_SDMMC_CLKSHIFT   NX_SDMMC_GetDriveClockShiftPhase( unsigned int ModuleIndex );
void    NX_SDMMC_SetSampleClockShiftPhase( unsigned int ModuleIndex, NX_SDMMC_CLKSHIFT ClockShift );
NX_SDMMC_CLKSHIFT   NX_SDMMC_GetSampleClockShiftPhase( unsigned int ModuleIndex );
void    NX_SDMMC_SetDriveClockDelay( unsigned int ModuleIndex, unsigned int Delay );
unsigned int     NX_SDMMC_GetDriveClockDelay( unsigned int ModuleIndex );
void    NX_SDMMC_SetSampleClockDelay( unsigned int ModuleIndex, unsigned int Delay );
unsigned int     NX_SDMMC_GetSampleClockDelay( unsigned int ModuleIndex );

void    NX_SDMMC_SetDataTimeOut( unsigned int ModuleIndex, unsigned int dwTimeOut );
unsigned int     NX_SDMMC_GetDataTimeOut( unsigned int ModuleIndex );
void    NX_SDMMC_SetResponseTimeOut( unsigned int ModuleIndex, unsigned int dwTimeOut );
unsigned int     NX_SDMMC_GetResponseTimeOut( unsigned int ModuleIndex );

void    NX_SDMMC_SetDataBusWidth( unsigned int ModuleIndex, unsigned int width );
unsigned int     NX_SDMMC_GetDataBusWidth( unsigned int ModuleIndex );

void    NX_SDMMC_SetBlockSize( unsigned int ModuleIndex, unsigned int SizeInByte );
unsigned int     NX_SDMMC_GetBlockSize( unsigned int ModuleIndex );
void    NX_SDMMC_SetByteCount( unsigned int ModuleIndex, unsigned int SizeInByte );
unsigned int     NX_SDMMC_GetByteCount( unsigned int ModuleIndex );

void    NX_SDMMC_SetCommandArgument( unsigned int ModuleIndex, unsigned int argument );
void    NX_SDMMC_SetCommand( unsigned int ModuleIndex, unsigned int Cmd, unsigned int flag );
void    NX_SDMMC_StartCommand( unsigned int ModuleIndex );
int   NX_SDMMC_IsCommandBusy( unsigned int ModuleIndex );

unsigned int     NX_SDMMC_GetShortResponse( unsigned int ModuleIndex );
void    NX_SDMMC_GetLongResponse( unsigned int ModuleIndex, unsigned int *pLongResponse );
unsigned int     NX_SDMMC_GetAutoStopResponse( unsigned int ModuleIndex );
unsigned int     NX_SDMMC_GetResponseIndex( unsigned int ModuleIndex );

void    NX_SDMMC_SetFIFORxThreshold( unsigned int ModuleIndex, unsigned int Threshold );
unsigned int     NX_SDMMC_GetFIFORxThreshold( unsigned int ModuleIndex );
void    NX_SDMMC_SetFIFOTxThreshold( unsigned int ModuleIndex, unsigned int Threshold );
unsigned int     NX_SDMMC_GetFIFOTxThreshold( unsigned int ModuleIndex );


unsigned int     NX_SDMMC_GetFIFOCount( unsigned int ModuleIndex );
int   NX_SDMMC_IsFIFOFull( unsigned int ModuleIndex );
int   NX_SDMMC_IsFIFOEmpty( unsigned int ModuleIndex );
int   NX_SDMMC_IsFIFOTxThreshold( unsigned int ModuleIndex );
int   NX_SDMMC_IsFIFORxThreshold( unsigned int ModuleIndex );

unsigned int     NX_SDMMC_GetDataTransferSize( unsigned int ModuleIndex );
unsigned int     NX_SDMMC_GetFIFOTransferSize( unsigned int ModuleIndex );

void    NX_SDMMC_SetData( unsigned int ModuleIndex, unsigned int dwData );
unsigned int     NX_SDMMC_GetData( unsigned int ModuleIndex );
void    NX_SDMMC_SetData32( unsigned int ModuleIndex, const unsigned int *pdwData );
void    NX_SDMMC_GetData32( unsigned int ModuleIndex, unsigned int *pdwData );
volatile unsigned int*   NX_SDMMC_GetDataPointer( unsigned int ModuleIndex );

int   NX_SDMMC_MakeBIU( NX_SDMMC_BIUConfig *pBIUC, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_GetDataBIU( unsigned int ModuleIndex, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_SetDataBIU( unsigned int ModuleIndex, NX_SDMMC_BIU *pBIU );
void    NX_SDMMC_PollDemand( unsigned int ModuleIndex );

unsigned int     NX_SDMMC_GetDMACStatus( unsigned int ModuleIndex );
void    NX_SDMMC_SetDMACStatus( unsigned int ModuleIndex, unsigned int SetData );
void    NX_SDMMC_SetDMACIntEnable( unsigned int ModuleIndex, unsigned int IntFlag );
unsigned int     NX_SDMMC_GetDMACIntEnable( unsigned int ModuleIndex );
int   NX_SDMMC_GetDMACResetStatus( unsigned int ModuleIndex );
void    NX_SDMMC_SetDMACBurstLength( unsigned int ModuleIndex, unsigned int BurstLength );
void    NX_SDMMC_SetIDMACEnable( unsigned int ModuleIndex, int bEnable );
int   NX_SDMMC_GetIDMACEnable( unsigned int ModuleIndex );

void    NX_SDMMC_SetDescSkipLen( unsigned int ModuleIndex, unsigned int uLength );
unsigned int     NX_SDMMC_GetDescSkipLen( unsigned int ModuleIndex );
void    NX_SDMMC_ResetIDMAC( unsigned int ModuleIndex );
int   NX_SDMMC_IsResetIDMAC( unsigned int ModuleIndex );
void    NX_SDMMC_SetDebounce( unsigned int ModuleIndex, unsigned int uDebounce );
unsigned int     NX_SDMMC_GetDebounce( unsigned int ModuleIndex );
int   NX_SDMMC_IsIDMACSupported( unsigned int ModuleIndex );

void    NX_SDMMC_SetCardVoltage( unsigned int ModuleIndex, unsigned int VolBase, unsigned int VolOffset);
unsigned int     NX_SDMMC_GetCardVoltage( unsigned int ModuleIndex, unsigned int VolBase);

int   NX_SDMMC_IsDataTransferBusy( unsigned int ModuleIndex );
int   NX_SDMMC_IsCardDataBusy( unsigned int ModuleIndex );
int   NX_SDMMC_IsCardPresent( unsigned int ModuleIndex );

void    NX_SDMMC_SetDDRMode( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_GetDDRMode( unsigned int ModuleIndex );
void    NX_SDMMC_SetVoltageMode( unsigned int ModuleIndex, int Enable );
int   NX_SDMMC_GetVoltageMode( unsigned int ModuleIndex );

NX_SDMMC_CMDFSM NX_SDMMC_GetCommandFSM( unsigned int ModuleIndex );
//@}


#ifdef  __cplusplus
}
#endif

#endif  // __NX_SDMMC_H__

