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


typedef enum
{
  NX_SDMMC_INTERRUPT_TYPE_CARD_INSERTION,
  NX_SDMMC_INTERRUPT_TYPE_CARD_REMOVAL,
  NX_SDMMC_INTERRUPT_TYPE_CARD_ALREADY_EXIST,
} NX_SDMMC_INTERRUPT_TYPE;

// BLOCKSIZE_R detail
typedef enum
{
  NX_SDMMC_XFER_BLOCK_SIZE         =            0,           //11:0       Transfer Block Size, 1:1byte,2:2byte... 0x200:512byte
  NX_SDMMC_SDMA_VUG_BDARY          =            1UL<<12,     //14:12      SDMA Buffer Boundary
  NX_SDMMC_RSVD_BLOCKSIZE15        =            1UL<<15,     //15         It always returns 0.
} NX_SDMMC_BLOCKSIZE_R;

// XFER_MODE_R
typedef struct xfer_mode_r
{
  volatile U8  NX_SDMMC_DMA_ENABLE;             //0          DMA Enable
                                       //0x1:DMA Data transfer enable, 0x0:Disable
  volatile U8  NX_SDMMC_BLOCK_COUNT_ENABLE;     //1          Block Count Enable
  volatile U8  NX_SDMMC_AUTO_CMD_ENABLE;        //3:2        Auto Command Enable
  volatile U8  NX_SDMMC_DATA_XFER_DIR;          //4          Data Transfer Direction Select
  volatile U8  NX_SDMMC_MULTI_BLK_SEL;          //5          Multi/Single Block Select
  volatile U8  NX_SDMMC_RESP_TYPE;              //6          Response Type R1/R5
  volatile U8  NX_SDMMC_RESP_ERR_CHK_ENABLE;    //7          Response Error Check Enable
  volatile U8  NX_SDMMC_RESP_INT_DISABLE;       //8          Response Interrupt Disable
  volatile U8  NX_SDMMC_RSVD;                   //15:9       Reserved
} NX_SDMMC_XFER_MODE_R;

// CMD_R
typedef struct cmd_r
{
  volatile U8  NX_SDMMC_RESP_TYPE_SELECT;       //1:0        Response Type Select
  volatile U8  NX_SDMMC_SUB_CMD_FLAG;           //2          Sub Command Flag
  volatile U8  NX_SDMMC_CMD_CRC_CHK_ENABLE;     //3          Command CRC Check Enable
  volatile U8  NX_SDMMC_CMD_IDX_CHK_ENABLE;     //4          Command Index Check Enable
  volatile U8  NX_SDMMC_DATA_PRESENT_SEL;       //5          Data Present Select
  volatile U8  NX_SDMMC_CMD_TYPE;               //7:6        Command Type
  volatile U8  NX_SDMMC_CMD_INDEX;              //13:8       Command Index
  volatile U8  RSVD;                   //15:14      Reserved
} NX_SDMMC_CMD_R;
  

// PSTATE_REG
typedef enum
{
  NX_SDMMC_PSTATE_REG_CMD_INHIBIT      =    0,        //0           //Command Inhibit (DAT)
  NX_SDMMC_PSTATE_REG_CMD_INHIBIT_DAT  =  1UL,        //1           //Command Inhibit (DAT)
  NX_SDMMC_PSTATE_REG_DAT_LINE_ACTIVE  =  2UL,        //2           //DAT Line Active (SD/eMMC Mode only)
  NX_SDMMC_PSTATE_REG_RE_TUNE_REQ      =  3UL,        //3           //Re-Tuning Request
  NX_SDMMC_PSTATE_REG_DAT_7_4          =  4UL,        //7:4         //DAT[7:4] Line Signal Level
  NX_SDMMC_PSTATE_REG_WR_XFER_ACTIVE   =  8UL,        //8           //Write Transfer Active
  NX_SDMMC_PSTATE_REG_RD_XFER_ACTIVE   =  9UL,        //9           //Read Transfer Active
  NX_SDMMC_PSTATE_REG_BUF_WR_ENABLE    = 10UL,        //10          //Buffer Write Enable
  NX_SDMMC_PSTATE_REG_BUF_RD_ENABLE    = 11UL,        //11          //Buffer Read Enable
  NX_SDMMC_PSTATE_REG_RSVD_15_12       = 12UL,        //15:12       //Reserved
  NX_SDMMC_PSTATE_REG_CARD_INSERTED    = 16UL,        //16          //Card Inserted
  NX_SDMMC_PSTATE_REG_CARD_STABLE      = 17UL,        //17          //Card Stable
  NX_SDMMC_PSTATE_REG_CARD_DETECT_PIN_LEVEL =   18UL, //18          //Card Detect Pin Level
  NX_SDMMC_PSTATE_REG_WR_PROTECT_SW_LVL     =   19UL, //19          //Write Protect Switch Pin Level
  NX_SDMMC_PSTATE_REG_DAT_3_0         =  20UL,        //23:20       //DAT[3:0] Line Signal Level
  NX_SDMMC_PSTATE_REG_CMD_LINE_LVL    =  24UL,        //24          //Command-Line Signal Level
  NX_SDMMC_PSTATE_REG_HOST_REG_VOL    =  25UL,        //25          //Host Regulator Voltage Stable
  NX_SDMMC_PSTATE_REG_RSVD_26         =  26UL,        //26          //Reserved
  NX_SDMMC_PSTATE_REG_CMD_ISSUE_ERR   =  27UL,        //27          //Command Not Issued by Error
  NX_SDMMC_PSTATE_REG_SUB_CMD_STAT    =  28UL,        //28          //Sub Command Status
  NX_SDMMC_PSTATE_REG_IN_DORMANT_ST   =  29UL,        //29          //In Dormant Status
  NX_SDMMC_PSTATE_REG_LANE_SYNC       =  30UL,        //30          //Lane Synchronization
  NX_SDMMC_PSTATE_REG_UHS2_IF_DETECT  =  31UL,        //31          //UHS-II Interface Detection
} NX_SDMMC_PSTATE_REG;

// HOST_CTRL1_R
typedef struct host_ctrl1_r
{
  volatile U8  NX_SDMMC_LED_CTRL;               //0           //LED Control
  volatile U8  NX_SDMMC_DAT_XFER_WIDTH;         //1           //Data Transfer Width
  volatile U8  NX_SDMMC_HIGH_SPEED_EN;          //2           //High Speed Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_DMA_SEL;                //4:3         //DMA Select
  volatile U8  NX_SDMMC_EXT_DAT_XFER;           //5           //Extended Data Transfer Width
  volatile U8  NX_SDMMC_CARD_DETECT_TEST_LVL;   //6           //Card Detect Test Level
  volatile U8  NX_SDMMC_CARD_DETECT_SIG_SEL;    //7           //Card Detect Signal Selection
} NX_SDMMC_HOST_CTRL1_R;

//PWR_CTL_R
typedef enum
{
    NX_SDMMC_PWR_CTRL_SD_BUS_PWR_VDD1 = 0,        //0           //SD Bus Power for VDD1
    NX_SDMMC_PWR_CTRL_SD_BUS_VOL_VDD1 = 7UL<<1,   //3:1         //SD Bus Voltage Select for VDD1/eMMC Bus Voltage Select for VDD
    NX_SDMMC_PWR_CTRL_SD_BUS_PWR_VDD2 = 1UL<<4,   //4           //SD Bus Power for VDD2
    NX_SDMMC_PWR_CTRL_SD_BUS_VOL_VDD2 = 7UL<<5,   //7:5         //SD Bus Voltage Select for VDD2
} NX_SDMMC_PWR_CTRL_R;


  
//BGAP_CTRL_R
typedef struct bgap_ctrl_r
{
  volatile U8  NX_SDMMC_STOP_BG_REQ;            //0           //Stop At Block Gap Request
  volatile U8  NX_SDMMC_CONTINUE_REQ;;          //1           //Continue Request
  volatile U8  NX_SDMMC_RD_WAIT_CTRL;           //2           //Read Wait Control
  volatile U8  NX_SDMMC_INT_AT_BGAP;            //3           //Interrupt At Block Gap
  volatile U8  NX_SDMMC_RSVD_7_4;               //7:4         //Reserved
} NX_SDMMC_BGAP_CTRL_R;

//WUP_CTRL_R
typedef struct wup_ctrl_r
{
  volatile U8  NX_SDMMC_CARD_INT;               //0           //Wakeup Event Enable on Card Interrupt
  volatile U8  NX_SDMMC_CARD_INSERT;            //1           //Wakeup Event Enable on SD Card Insertion
  volatile U8  NX_SDMMC_CARD_REMOVAL;           //2           //Wakeup Event Enable on SD Card Removal, 0:Disable, 1:Enable
  volatile U8  NX_SDMMC_RSVD_7_3;               //7:3         //Reserved
} NX_SDMMC_WUP_CTRL_R;

//CLK_CTRL_R 16
typedef enum
{
    NX_SDMMC_CLK_CTRL_INTERNAL_CLK_EN       =   0,         //0       //Internal Clock Enable
    NX_SDMMC_CLK_CTRL_INTERNAL_CLK_STABLE   =   1UL<<1,    //1       //Internal Clock Stable
    NX_SDMMC_CLK_CTRL_SD_CLK_EN             =   1UL<<2,    //2       //SD/eMMC Clock Enable
    NX_SDMMC_CLK_CTRL_PLL_ENABLE            =   1UL<<3,    //3       //PLL Enable
    NX_SDMMC_CLK_CTRL_RSVD_4                =   1UL<<4,    //4       //Reserved
    NX_SDMMC_CLK_CTRL_CLK_GEN_SELECT        =   1UL<<5,    //5       //Clock Generator Select
    NX_SDMMC_CLK_CTRL_UPPER_FREQ_SEL        =   3UL<<6,    //7:6     //The upper 2bits of 10-bit SDCLK/RCLK Frequency Select Control
    NX_SDMMC_CLK_CTRL_FREQ_SEL              =   255UL<<8,  //15:8    //SDCLK/RCLK Frequency Select
} NX_SDMMC_CLK_CTRL_R;

//TOUT_CTRL_R
typedef enum
{
    NX_SDMMC_TOUT_CNT  =  15UL<<0,           //3:0         //Data Timeout Counter Value
    NX_SDMMC_RSVD_7_4  =  1UL<<4,            //7:4         //Reserved
} NX_SDMMC_TOUT_CTRL_R;

//SW_RST_R
typedef struct sw_rst_r
{
  volatile U8  NX_SDMMC_SW_RST_ALL;             //0           //Software Reset For All
  volatile U8  NX_SDMMC_SW_RST_CMD;             //1           //Software Reset For CMD line
  volatile U8  NX_SDMMC_SW_RST_DAT;             //2           //Software Reset For DAT line
  volatile U8  NX_SDMMC_RSVD_7_3;               //7:3         //Reserved, always return 0
} NX_SDMMC_SW_RST_R;

//NORMAL_INT_STAT_R
typedef enum
{
  NX_SDMMC_NORMAL_INT_STAT_CMD_COMPLETE     =        0,    //0           //Command Complete
  NX_SDMMC_NORMAL_INT_STAT_XFER_COMPLETE    =      1UL,    //1           //Transfer Complete
  NX_SDMMC_NORMAL_INT_STAT_BGAP_EVENT       =      2UL,    //2           //Block Gap Event
  NX_SDMMC_NORMAL_INT_STAT_DMA_INTERRUPT    =      3UL,    //3           //DMA Interrupt
  NX_SDMMC_NORMAL_INT_STAT_BUF_WR_READY     =      4UL,    //4           //Buffer Write Ready
  NX_SDMMC_NORMAL_INT_STAT_BUF_RD_READY     =      5UL,    //5           //Buffer Read Ready
  NX_SDMMC_NORMAL_INT_STAT_CARD_INSERTION   =      6UL,    //6           //Card Insertion
  NX_SDMMC_NORMAL_INT_STAT_CARD_REMOVAL     =      7UL,    //7           //Card Removal
  NX_SDMMC_NORMAL_INT_STAT_CARD_INTERRUPT   =      8UL,    //8           //Card Interrupt
  NX_SDMMC_NORMAL_INT_STAT_INT_A            =      9UL,    //9           //INT_A
  NX_SDMMC_NORMAL_INT_STAT_INT_B            =     10UL,    //10          //INT_B
  NX_SDMMC_NORMAL_INT_STAT_INT_C            =     11UL,    //11          //INT_C
  NX_SDMMC_NORMAL_INT_STAT_RE_TUNE_EVENT    =     12UL,    //12          //Re-tuning Event
  NX_SDMMC_NORMAL_INT_STAT_FX_EVENT         =     13UL,    //13          //FX Event
  NX_SDMMC_NORMAL_INT_STAT_CQE_EVNET        =     14UL,    //14          //Command Queuing Event
  NX_SDMMC_NORMAL_INT_STAT_ERR_INTERRUPT    =     15UL,    //15          //Error Interrupt, 0(FALSE):No Error, 1(TRUE):Error
} NX_SDMMC_NORMAL_INT_STAT_R;

//ERROR_INT_STAT_R
typedef struct error_int_stat_r
{
  volatile U8  NX_SDMMC_CMD_TOUT_ERR;           //0           //Command Timeout Error
  volatile U8  NX_SDMMC_CMD_CRC_ERR;            //1           //Command CRC Error
  volatile U8  NX_SDMMC_CMD_END_BIT_ERR;        //2           //Command End Bit Error
  volatile U8  NX_SDMMC_CMD_IDX_ERR;            //3           //Command Index Error
  volatile U8  NX_SDMMC_DATA_TOUT_ERR;          //4           //Data Timeout Error
  volatile U8  NX_SDMMC_DATA_CRC_ERR;           //5           //Data CRC Error
  volatile U8  NX_SDMMC_DATA_END_BIT_ERR;       //6           //Data End Bit Error
  volatile U8  NX_SDMMC_CUR_LMT_ERR;            //7           //Current Limit Error
  volatile U8  NX_SDMMC_AUTO_CMD_ERR;           //8           //Auto CMD Error
  volatile U8  NX_SDMMC_ADMA_ERR;               //9           //ADMA Error
  volatile U8  NX_SDMMC_TUNING_ERR;             //10          //Tuning Error
  volatile U8  NX_SDMMC_RESP_ERR;               //11          //Response Error
  volatile U8  NX_SDMMC_BOOT_ACK_ERR;           //12          //Boot Acknowledgement Error
  volatile U8  NX_SDMMC_VENDOR_ERR1;            //13          //ERROR_INT_STAT_R resigter
  volatile U8  NX_SDMMC_VENDOR_ERR2;            //14          //ERROR_INT_STAT_R resigter
  volatile U8  NX_SDMMC_VENDOR_ERR3;            //15          //ERROR_INT_STAT_R resigter
} NX_SDMMC_ERROR_INT_STAT_R;

//NORMAL_INT_STAT_EN_R
typedef enum
{
  NX_SDMMC_NORMAL_INT_CMD_COMPLETE_STAT_EN    =    0,    //0      //Command Complete Status Enable
  NX_SDMMC_NORMAL_INT_XFER_COMPLETE_STAT_EN   =    1UL,  //1      //Transfer Complete Status Enable
  NX_SDMMC_NORMAL_INT_BGAP_EVENT_STAT_EN      =    2UL,  //2      //Block Gap Event Status Enable
  NX_SDMMC_NORMAL_INT_DMA_INTERRUPT_STAT_EN   =    3UL,  //3      //DMA Interrupt Status Enable
  NX_SDMMC_NORMAL_INT_BUF_WR_READY_STAT_EN    =    4UL,  //4      //Buffer Write Ready Status Enable
  NX_SDMMC_NORMAL_INT_BUF_RD_READY_STAT_EN    =    5UL,  //5      //Buffer Read Ready Status Enable
  NX_SDMMC_NORMAL_INT_CARD_INSERTION_STAT_EN  =    6UL,  //6      //Card Insertion Status Enable
  NX_SDMMC_NORMAL_INT_CARD_REMOVAL_STAT_EN    =    7UL,  //7      //Card Removal Status Enable
  NX_SDMMC_NORMAL_INT_CARD_INTERRUPT_STAT_EN  =    8UL,  //8      //Card Interrupt Status Enable
  NX_SDMMC_NORMAL_INT_INT_A_STAT_EN    =    9UL,         //9      //INT_A Status Enable
  NX_SDMMC_NORMAL_INT_INT_B_STAT_EN    =    10UL,        //10     //INT_B Status Enable
  NX_SDMMC_NORMAL_INT_INT_C_STAT_EN    =    11UL,        //11     //INT_C Status Enable
  NX_SDMMC_NORMAL_INT_RE_TUNE_EVENT_STAT_EN   =    12UL, //12     //Re-tuning Event Status Enable(UHS-I only)
  NX_SDMMC_NORMAL_INT_FX_EVENT_STAT_EN     =    13UL,    //13     //FX Event Status Enable
  NX_SDMMC_NORMAL_INT_CQE_EVNET_STAT_EN    =    14UL,    //14     //CQE Event Status Enable
  NX_SDMMC_NORMAL_INT_RSVD_15              =    15UL,    //15     //Reserved
} NX_SDMMC_NORMAL_INT_STAT_EN_R;

//ERROR_INT_STAT_EN_R
typedef struct error_int_stat_en_r
{
  volatile U8  NX_SDMMC_CMD_TOUT_ERR_STAT_EN;           //0           //Command Timeout Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_CMD_CRC_ERR_STAT_EN;            //1           //Command CRC Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_CMD_END_BIT_ERR_STAT_EN;        //2           //Command End Bit Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_CMD_IDX_ERR_STAT_EN;            //3           //Command Index Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_DATA_TOUT_ERR_STAT_EN;          //4           //Data Timeout Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_DATA_CRC_ERR_STAT_EN;           //5           //Data CRC Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_DATA_END_BIT_ERR_STAT_EN;       //6           //Data End Bit Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_CUR_LMT_ERR_STAT_EN;            //7           //Current Limit Error Status Enable
  volatile U8  NX_SDMMC_AUTO_CMD_ERR_STAT_EN;           //8           //Auto CMD Error Status Enable (SD/eMMC Mode only)
  volatile U8  NX_SDMMC_ADMA_ERR_STAT_EN;               //9           //ADMA Error Status Enable
  volatile U8  NX_SDMMC_TUNING_ERR_STAT_EN;             //10          //Tuning Error Status Enable (UHS-I Mode only)
  volatile U8  NX_SDMMC_RESP_ERR_STAT_EN;               //11          //Response Error Status Enable (SD Mode only)
  volatile U8  NX_SDMMC_BOOT_ACK_ERR_STAT_EN;           //12          //Boot Acknowledgement Error (eMMC Mode only)
  volatile U8  NX_SDMMC_VENDOR_ERR_STAT_EN1;            //13          //13th bit Error Interrupt Status Enable register Reserved
  volatile U8  NX_SDMMC_VENDOR_ERR_STAT_EN2;            //14          //14th bit Error Interrupt Status Enable register Reserved
  volatile U8  NX_SDMMC_VENDOR_ERR_STAT_EN3;            //15          //15th bit Error Interrupt Status Enable register Reserved
} NX_SDMMC_ERROR_INT_STAT_EN_R;

//NORMAL_INT_SIGNAL_EN_R
typedef enum
{
  NX_SDMMC_NORMAL_INT_SIGNAL_CMD_COMPLETE_SIGNAL_EN    =   0,    //0  //Command Complete Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_XFER_COMPLETE_SIGNAL_EN   =   1UL,  //1  //Transfer Complete Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_BGAP_EVENT_SIGNAL_EN      =   2UL,  //2  //Block Gap Event Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_DMA_INTERRUPT_SIGNAL_EN   =   3UL,  //3  //DMA Interrupt Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_BUF_WR_READY_SIGNAL_EN    =   4UL,  //4  //Buffer Write Ready Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_BUF_RD_READY_SIGNAL_EN    =   5UL,  //5  //Buffer Read Ready Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_CARD_INSERTION_SIGNAL_EN  =   6UL,  //6  //Card Insertion Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_CARD_REMOVAL_SIGNAL_EN    =   7UL,  //7  //Card Removal Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_CARD_INTERRUPT_SIGNAL_EN  =   8UL,  //8  //Card Interrupt Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_INT_A_SIGNAL_EN    =    9UL,        //9  //INT_A Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_INT_B_SIGNAL_EN    =    10UL,       //10 //INT_B Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_INT_C_SIGNAL_EN    =    11UL,       //11 //INT_C Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_RE_TUNE_EVENT_SIGNAL_EN   =  12UL, //12  //Re-tuning Event Signal Enable (UHS-I only)
  NX_SDMMC_NORMAL_INT_SIGNAL_FX_EVENT_SIGNAL_EN        =  13UL, //13  //FX Event Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_CQE_EVNET_SIGNAL_EN       =  14UL, //14  //CQE Event Signal Enable
  NX_SDMMC_NORMAL_INT_SIGNAL_RSVD_15                   =  15UL, //15  //Reserved
} NX_SDMMC_NORMAL_INT_SIGNAL_EN_R;

//ERROR_INT_SIGNAL_EN_R
typedef enum
{
  NX_SDMMC_CMD_TOUT_ERR_SIGNAL_EN  =  1UL<<0,   //0 //Command Timeout Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_CMD_CRC_ERR_SIGNAL_EN   =  1UL<<1,   //1 //Command CRC Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_CMD_END_BIT_ERR_SIGNAL_EN =  1UL<<2, //2 //Command End Bit Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_CMD_IDX_ERR_SIGNAL_EN   =  1UL<<3,   //3 //Command Index Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_DATA_TOUT_ERR_SIGNAL_EN =  1UL<<4,   //4 //Data Timeout Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_DATA_CRC_ERR_SIGNAL_EN  =  1UL<<5,   //5 //Data CRC Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_DATA_END_BIT_ERR_SIGNAL_EN =  1UL<<6, //6 //Data End Bit Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_CUR_LMT_ERR_SIGNAL_EN  =  1UL<<7,    //7 //Current Limit Error Signal Enable
  NX_SDMMC_AUTO_CMD_ERR_SIGNAL_EN =  1UL<<8,    //8 //Auto CMD Error Signal Enable (SD/eMMC Mode only)
  NX_SDMMC_ADMA_ERR_SIGNAL_EN     =  1UL<<9,    //9 //ADMA Error Signal Enable
  NX_SDMMC_TUNING_ERR_SIGNAL_EN   =  1UL<<10,   //10//Tuning Error Signal Enable (UHS-I Mode only)
  NX_SDMMC_RESP_ERR_SIGNAL_EN     =  1UL<<11,   //11//Response Error Signal Enable (SD Mode only)
  NX_SDMMC_BOOT_ACK_ERR_SIGNAL_EN =  1UL<<12,   //12//Boot Acknowledgement Error (eMMC Mode only)
  NX_SDMMC_VENDOR_ERR_SIGNAL_EN1  =  1UL<<13,   //13//14th bit Error Interrupt Signal Enable register Reserved
  NX_SDMMC_VENDOR_ERR_SIGNAL_EN2  =  1UL<<14,   //14//15th bit Error Interrupt Signal Enable register Reserved
  NX_SDMMC_VENDOR_ERR_SIGNAL_EN3  =  1UL<<15,   //15//16th bit Error Interrupt Signal Enable register Reserved
} NX_SDMMC_ERROR_INT_SIGNAL_EN_R;

//AUTO_CMD_STAT_R
typedef enum
{
  NX_SDMMC_AUTO_CMD12_NOT_EXEC       =   1UL<<0,  //0           //Auto CMD12 Not Executed
  NX_SDMMC_AUTO_CMD_TOUT_ERR         =   1UL<<1,  //1           //Auto CMD Timeout Error
  NX_SDMMC_AUTO_CMD_CRC_ERR          =   1UL<<2,  //2           //Auto CMD CRC Error
  NX_SDMMC_AUTO_CMD_EBIT_ERR         =   1UL<<3,  //3           //Auto CMD End Bit Error
  NX_SDMMC_AUTO_CMD_IDX_ERR          =   1UL<<4,  //4           //Auto CMD Index Error
  NX_SDMMC_AUTO_CMD_RESP_ERR         =   1UL<<5,  //5           //Auto CMD Response Error
  NX_SDMMC_RSVD_6                    =   1UL<<6,  //6           //Reserved
  NX_SDMMC_CMD_NOT_ISSUED_AUTO_CMD12 =   1UL<<7,  //7           //Command Not Issued By Auto CMD12
  NX_SDMMC_RSVD_15_8                 = 255UL<<8,  //15:8        //Reserved
} NX_SDMMC_AUTO_CMD_STAT_R;

// HOST_CTRL2_R
typedef enum
{
  NX_SDMMC_HOST_CTRL2_UHS_MODE_SEL        =   7UL<<0,    //2:0        //UHS Mode/eMMC Speed Mode Select
  NX_SDMMC_HOST_CTRL2_SIGNALING_EN        =   1UL<<3,    //3          //1.8V Signaling Enable
  NX_SDMMC_HOST_CTRL2_DRV_STRENGTH_SEL    =   3UL<<4,    //5:4        //Driver Strength Select
  NX_SDMMC_HOST_CTRL2_EXEC_TUNING         =   1UL<<6,    //6          //Execute Tuning
  NX_SDMMC_HOST_CTRL2_SAMPLE_CLK_SEL      =   1UL<<7,    //7          //Sampling Clock Select
  NX_SDMMC_HOST_CTRL2_UHS2_IF_ENABLE      =   1UL<<8,    //8          //UHS-II Interface Enable
  NX_SDMMC_HOST_CTRL2_RSVD_9              =   1UL<<9,    //9          //Reserved
  NX_SDMMC_HOST_CTRL2_ADMA2_LEN_MODE      =   1UL<<10,   //10         //ADMA2 Length Mode
  NX_SDMMC_HOST_CTRL2_CMD23_ENABLE        =   1UL<<11,   //11         //CMD23 Enable
  NX_SDMMC_HOST_CTRL2_HOST_VER4_ENABLE    =   1UL<<12,   //12         //Host Version 4 Enable
  NX_SDMMC_HOST_CTRL2_ADDRESSING          =   1UL<<13,   //13         //64-bit Addressing, 0(FALSE):32 bits addressing, 1(TRUE):64 bits addressing
  NX_SDMMC_HOST_CTRL2_ASYNC_INT_ENABLE    =   1UL<<14,   //14         //Asynchronous Interrupt Enable
  NX_SDMMC_HOST_CTRL2_PRESET_VAL_ENABLE   =   1UL<<15,   //15         //Preset Value Enable
} NX_SDMMC_HOST_CTRL2_R;

//CAPABILITIES1_R
typedef enum
{
  NX_SDMMC_CAPA1_TOUT_CLK_FREQ       =    0,      //5:0        //Timeout Clock Frequency
  NX_SDMMC_CAPA1_RSVD_6              =    6UL,    //6          //Reserved
  NX_SDMMC_CAPA1_TOUT_CLK_UNIT       =    7UL,    //7          //Timeout Clock Unit
  NX_SDMMC_CAPA1_BASE_CLK_FREQ       =    8UL,    //15:8       //Base Clock Frequency for SD clock
  NX_SDMMC_CAPA1_MAX_BLK_LEN         =    16UL,   //17:16      //Maximum Block Length
  NX_SDMMC_CAPA1_Embedded_8_BIT      =    18UL,   //18         //8-bit Support for Embedded Device
  NX_SDMMC_CAPA1_ADMA2_SUPPORT       =    19UL,   //19         //ADMA2 Support
  NX_SDMMC_CAPA1_RSVD_20             =    20UL,   //20         //Reserved
  NX_SDMMC_CAPA1_HIGH_SPEED_SUPPORT  =    21UL,   //21         //High Speed Support
  NX_SDMMC_CAPA1_SDMA_SUPPORT        =    22UL,   //22         //SDMA Support
  NX_SDMMC_CAPA1_SUS_RES_SUPPORT     =    23UL,   //23         //Suspense/Resume Support
  NX_SDMMC_CAPA1_VOLT_33             =    24UL,   //24         //Voltage Support 3.3V
  NX_SDMMC_CAPA1_VOLT_30             =    25UL,   //25         //Voltage Support 3.0V
  NX_SDMMC_CAPA1_VOLT_18             =    26UL,   //26         //Voltage Support 1.8V
  NX_SDMMC_CAPA1_SYS_ADDR_64_V4      =    27UL,   //27         //64-bit System Address Support for V4
  NX_SDMMC_CAPA1_SYS_ADDR_64_V3      =    28uL,   //28         //64-bit System Address Support for V3
  NX_SDMMC_CAPA1_ASYNC_INT_SUPPORT   =    29UL,   //29         //Asynchoronous Interrupt Support (SD Mode only)
  NX_SDMMC_CAPA1_SLOT_TYPE_R         =    30UL,   //31:30      //Slot Type
} NX_SDMMC_CAPABILITIES1_R;

//CAPABILITIES2_R
typedef enum
{
  NX_SDMMC_CAPA2_SDR50_SUPPORT    =     0<<0,    //0       //SDR50 Support (UHS-I only)
  NX_SDMMC_CAPA2_SDR104_SUPPORT   =   1UL<<1,    //1       //SDR104 Support (UHS-I only)
  NX_SDMMC_CAPA2_DDR50_SUPPORT    =   1UL<<2,    //2       //DDR50 Support (UHS-I only)
  NX_SDMMC_CAPA2_UHS2_SUPPORT     =   1UL<<3,    //3       //UHS-II Support (UHS-I only)
  NX_SDMMC_CAPA2_DRV_TYPEA        =   1UL<<4,    //4       //Driver Type A Support (UHS-I only) 
  NX_SDMMC_CAPA2_DRV_TYPEC        =   1UL<<5,    //5       //Driver Type C Support (UHS-I only)
  NX_SDMMC_CAPA2_DRV_TYPED        =   1UL<<6,    //6       //Driver Type D Support (UHS-I only)
  NX_SDMMC_CAPA2_RSVD_39          =   1UL<<7,    //7       //Reserved
  NX_SDMMC_CAPA2_RETUNE_CNT       =  15UL<<8,    //11:8    //Timer Count for Re-Tuning (UHS-I only)
  NX_SDMMC_CAPA2_RSVD_44          =   1UL<<12,   //12      //Reserved
  NX_SDMMC_CAPA2_USE_TUNING_SDR50 =   1UL<<13,   //13      //Use Tuning for SDR50 (UHS-I only)
  NX_SDMMC_CAPA2_RE_TUNING_MODES  =   3UL<<14,   //15:14   //Re-Tuning Modes (UHS-I only)
  NX_SDMMC_CAPA2_CLK_MUL          = 255UL<<16,   //23:16   //Clock Multiplier
  NX_SDMMC_CAPA2_RSVD_56_58       =   7UL<<24,   //26:24   //Reserved
  NX_SDMMC_CAPA2_ADMA3_SUPPORT    =   1UL<<27,   //27      //ADMA3 Support
  NX_SDMMC_CAPA2_VDD2_18V_SUPPORT =   1UL<<28,   //28      //1.8V VDD2 Support
  NX_SDMMC_CAPA2_RSVD_61          =   1UL<<29,   //29      //Reserved
  NX_SDMMC_CAPA2_RSVD_62_63       =   3UL<<30,   //31:30   //Reserved
} NX_SDMMC_CAPABILITIES2_R;

//CURR_CAPABILITIES1_R
typedef struct curr_capabilities1_r
{
  volatile U8  NX_SDMMC_MAX_CUR_33V;             //7:0        //Maximum Current for 3.3V
  volatile U8  NX_SDMMC_MAX_CUR_30V;             //15:8       //Maximum Current for 3.0V
  volatile U8  NX_SDMMC_MAX_CUR_18V;             //23:16      //Maximum Current for 1.8V
  volatile U8  NX_SDMMC_RSVD_31_24;              //31:24      //Reserved
} NX_SDMMC_CURR_CAPABILITIES1_R;

//CURR_CAPABILITIES2_R
typedef struct curr_capabilities2_r
{
  volatile U8  NX_SDMMC_MAX_CUR_VDD2_18V;        //7:0        //Maximum Current for 1.8V VDD2
  volatile U32 NX_SDMMC_RSVD_63_40;              //31:8       //Reserved
} NX_SDMMC_CURR_CAPABILITIES2_R;

  
//FORCE_AUTO_CMD_STAT_R
typedef enum
{
  NX_SDMMC_FORCE_AUTO_CMD12_NOT_EXEC = 1UL<<0,  //0     //Force Event for Auto CMD12 Not Executed
  NX_SDMMC_FORCE_AUTO_CMD_TOUT_ERR   = 1UL<<1,  //1     //Force Event for Auto CMD Timeout Error
  NX_SDMMC_FORCE_AUTO_CMD_CRC_ERR    = 1UL<<2,  //2     //Force Event for Auto CMD CRC Error
  NX_SDMMC_FORCE_AUTO_CMD_EBIT_ERR   = 1UL<<3,  //3     //Force Event for Auto CMD End Bit Error
  NX_SDMMC_FORCE_AUTO_CMD_IDX_ERR    = 1UL<<4,  //4     //Force Event for Auto CMD Index Error
  NX_SDMMC_FORCE_AUTO_CMD_RESP_ERR   = 1UL<<5,  //5     //Force Event for Auto CMD Response Error
  NX_SDMMC_FORCE_AUTO_CMD_RSVD_6     = 1UL<<6,  //6     //Reserved
  NX_SDMMC_FORCE_AUTO_CMD_NOT_ISSUED_AUTO_CMD12 =  1UL<<7,   //7          //Force Event for Command Not Issued By Auto CMD12 Error
  NX_SDMMC_FORCE_AUTO_CMD_RSVD_15_8  = 1UL<<8,  //15:8  //Reserved
} NX_SDMMC_FORCE_AUTO_CMD_STAT_R;

//FORCE_ERROR_INT_STAT_R
typedef struct force_error_int_stat_r
{
  volatile U8  XXX;
} NX_SDMMC_FORCE_ERROR_INT_STAT_R;

typedef enum
{
  NX_SDMMC_PRESET_FREQ_SEL_VAL      =     0x3FF,
  NX_SDMMC_PRESET_CLK_GEN_SEL_VAL   =     1UL<<10,
  NX_SDMMC_PRESET_RSVD_13_11        =     7UL<<11,
  NX_SDMMC_PRESET_DRV_SEL_VAL       =     3UL<<14,
} NX_SDMMC_PRESET_INIT_R;


/* typedef struct  */
/* {   */
/* } ; */






//HOST_CNTRL_VERS_R
typedef struct host_cntrl_vers_r
{
  volatile U8  SPEC_VERSION_NUM;        //7:0               //Specification Version Number
  volatile U8  VENDOR_VERSION_NUM;      //15:8              //Vendor Version Number
} NX_SDMMC_HOST_CNTRL_VERS_R;


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
  volatile U16 NORMAL_INT_STAT_EN_R;   //0x34       Normal Interrupt Status Enable Register
  volatile U16 ERROR_INT_STAT_EN_R;    //0x36       Error Interrupt Status Enable Register
  volatile U16 NORMAL_INT_SIGNAL_EN_R; //0x38       Normal Interrupt Signal Enable Register
  volatile U16 ERROR_INT_SIGNAL_EN_R;  //0x3a       Error Interrupt Signal Enable Register
  volatile U16 AUTO_CMD_STAT_R;        //0x3c       Auto CMD Status Register
  volatile U16 HOST_CTRL2_R;           //0x3e       Host Control 2 Register
  volatile U32 CAPABILITIES1_R;        //0x40       Capabilities 1 Resiger - 0 to 31
  volatile U32 CAPABILITIES2_R;        //0x44       Capabilities 1 Resiger - 32 to 63
  volatile U32 CURR_CAPABILITIES1_R;   //0x48       Maximum Current Capabilities Resiger - 0 to 31
  volatile U32 CURR_CAPABILITIES2_R;   //0x4c       Maximum Current Capabilities Resiger - 31 to 63
  volatile U16 FORCE_AUTO_CMD_STAT_R;  //0x50       Force Event Register for Auto CMD Error Status register
  volatile U16 FORCE_ERROR_INT_STAT_R; //0x52       Force Event Register for Error Interrupt Status

  volatile U16 ADMA_ERR_STAT_R;        //0x54
  volatile U16 ADMA_SA_LOW_R;          //0x58
  volatile U16 ADMA_SA_HIGH_R;         //0x5c

  volatile U16 PRESET_INIT_R;          //0x60       Preset Value for Initialization

  volatile U16 PRESET_DS_R;            //0x62
  volatile U16 PRESET_HS_R;            //0x64
  volatile U16 PRESET_SDR12_R;         //0x66
  volatile U16 PRESET_SDR25_R;         //0x68
  volatile U16 PRESET_SDR50_R;         //0x6a
  volatile U16 PRESET_SDR104_R;        //0x6c
  volatile U16 PRESET_DDR50_R;         //0x6e
  volatile U16 PRESET_UHS2_R;          //0x74
  volatile U16 ADMA_ID_LOW_R;          //0x78
  volatile U16 ADMA_ID_HIGH_R;         //0x7c
  volatile U16 UHS_II_BLOCK_SIZE_R;    //0x80
  volatile U16 UHS_II_BLOCK_COUNT_R;   //0x84
  volatile U16 UHS_II_COMMAND_PKT_0_3_R;   //0x88
  volatile U16 UHS_II_COMMAND_PKT_4_7_R;   //0x8c
  volatile U16 UHS_II_COMMAND_PKT_8_11_R;  //0x90
  volatile U16 UHS_II_COMMAND_PKT_12_15_R; //0x94
  volatile U16 UHS_II_COMMAND_PKT_16_19_R; //0x98
  volatile U16 UHS_II_XFER_MODE_R;      //0x9c
  volatile U16 UHS_II_CMD_R;            //0x9e
  volatile U16 UHS_II_RESP_0_3_R;       //0xa0
  volatile U16 UHS_II_RESP_4_7_R;       //0xa4
  volatile U16 UHS_II_RESP_8_11_R;      //0xa8
  volatile U16 UHS_II_RESP_12_15_R;     //0xac
  volatile U16 UHS_II_RESP_16_19_R;     //0xb0
  volatile U16 UHS_II_MSG_SEL_R;        //0xb4
  volatile U16 UHS_II_MSG_R;            //0xb8
  volatile U16 UHS_II_DEV_INTR_STATUS_R;//0xbc
  volatile U16 UHS_II_DEV_SEL_R;        //0xbe
  volatile U16 UHS_II_DEV_INR_CODE_R;   //0xbf
  volatile U16 UHS_II_SOFT_RESET_R;     //0xc0
  volatile U16 UHS_II_TIMER_CNTRL_R;    //0xc2
  volatile U16 UHS_II_ERR_INTR_STATUS_R;//0xc4
  volatile U16 UHS_II_ERR_INTR_STATUS_EN_R;  //0xc8
  volatile U16 UHS_II_ERR_INTR_SIGNAL_EN_R;  //0xcc
  volatile U16 P_UHS_II_SETTINGS_R;          //0xe0
  volatile U16 P_UHS_II_HOST_CAPAB;          //0xe2
  volatile U16 P_UHS_II_TEST;                //0xe4
  volatile U16 P_EMBEDDED_CNTRL;             //0xe6
  volatile U16 P_VENDOR_SPECIFIC_AREA;       //0xe8
  volatile U16 P_VENDOR2_SPECIFIC_AREA;      //0xea
  volatile U16 SLOT_INTR_STATUS_R;           //0xfc
  volatile U16 HOST_CNTRL_VERS_R;            //0xfe       //Host Controller Version
} NX_SDMMC_RegisterSet;


#define SD_BUS_VOL_VDD_3_3    0x7    //3.3V(Typical)
#define SD_BUS_VOL_VDD_3_0    0x6    //3.0V(Typical)
#define SD_BUS_VOL_VDD_1_8    0x5    //1.8V(Typical)
#define SD_BUS_VOL_VDD_0      0x4    //Reserved


enum
{
    TMCLK_2__13 = 0,          // TMCLK x 2^13
    TMCLK_2__14 = 1,          // TMCLK x 2^14
    TMCLK_2__15 = 2,          // TMCLK x 2^15
    TMCLK_2__16 = 3,          // TMCLK x 2^16
    TMCLK_2__17 = 4,          // TMCLK x 2^17
    TMCLK_2__18 = 5,          // TMCLK x 2^18
    TMCLK_2__19 = 6,          // TMCLK x 2^19
    TMCLK_2__20 = 7,          // TMCLK x 2^20
    TMCLK_2__21 = 8,          // TMCLK x 2^21
    TMCLK_2__22 = 9,          // TMCLK x 2^22
    TMCLK_2__23 = 10,         // TMCLK x 2^23
    TMCLK_2__24 = 11,         // TMCLK x 2^24
    TMCLK_2__25 = 12,         // TMCLK x 2^25
    TMCLK_2__26 = 13,         // TMCLK x 2^26
    TMCLK_2__27 = 14,         // TMCLK x 2^27
    TMCLK_NONE = 15,          // Reserved
};

enum
{
  SPEC_VERSION_NUM1 = 0,       //0x0 SD Host Controller Specification Version 1.00
  SPEC_VERSION_NUM2 = 1,       //0x1 SD Host Controller Specification Version 2.00
  SPEC_VERSION_NUM3 = 2,       //0x2 SD Host Controller Specification Version 3.00
  SPEC_VERSION_NUM4 = 3,       //0x3 SD Host Controller Specification Version 4.00
  SPEC_VERSION_NUM4_1 = 4,     //0x4 SD Host Controller Specification Version 4.10
  SPEC_VERSION_NUM4_2 = 5,     //0x5 SD Host Controller Specification Version 4.20
};



  
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

/* CBOOL   NX_SDMMC_MakeBIU( NX_SDMMC_BIUConfig *pBIUC, NX_SDMMC_BIU *pBIU ); */
/* void    NX_SDMMC_GetDataBIU( U32 ModuleIndex, NX_SDMMC_BIU *pBIU ); */
/* void    NX_SDMMC_SetDataBIU( U32 ModuleIndex, NX_SDMMC_BIU *pBIU ); */
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

  //NX_SDMMC_CMDFSM NX_SDMMC_GetCommandFSM( U32 ModuleIndex );
//@}


#ifdef  __cplusplus
}
#endif

#endif  // __NX_SDMMC_H__

