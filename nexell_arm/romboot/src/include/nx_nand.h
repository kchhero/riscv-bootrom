#ifndef __NX_NAND_H__
#define __NX_NAND_H__

struct NX_NANDC_RegisterSet
{
	volatile U32 CTRL;		///< 0x000
	volatile U32 BCH_MODE;		///< 0x004
	volatile U32 DMA_CTRL;		///< 0x008
	volatile U32 DMA_ADDR;		///< 0x00C
	volatile U32 DMA_SIZE;		///< 0x010
	volatile U32 DMA_SUBP;		///< 0x014
	volatile U32 DDR_CTRL;		///< 0x018
	volatile U32 CMD_TIME;		///< 0x01C
	volatile U32 DAT_TIME;		///< 0x020
	volatile U32 DDR_TIME;		///< 0x024
	volatile U32 RAND;		///< 0x028
	volatile U32 STATUS;		///< 0x02C
	volatile U32 __rev0[14];	///< 0x030 ~ 0x064
	volatile U32 SRAM_SUBPAGE;	///< 0x068
	volatile U32 ERR_INFO;		///< 0x06C
	volatile U32 CMD;		///< 0x070
	volatile U32 ADR;		///< 0x074
	volatile U32 DAT;		///< 0x078
	volatile U32 DAT_BYPASS;	///< 0x07C
	volatile U32 SRAM_CFG;		///< 0x080
	volatile U32 __rev1[31];	///< 0x084 ~ 0x0FC
	volatile U32 SRAM[0x10];	///< 0x100
};

enum RSTATE {
	NAND_PAGE_READ		= 0,
	NAND_SECTOR_READ	= 1,
	BUF_COPY		= 2
};

enum BCH_MODE {
	NX_NANDC_BCH_512_24	= 4,
	NX_NANDC_BCH_1024_60	= 10
};

#endif // __NX_NAND_H__
