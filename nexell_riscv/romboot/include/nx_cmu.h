#ifndef __NX_CMU_H__
#define __NX_CMU_H__

struct NX_CMUSRC_RegisterSet {
	volatile unsigned int rev0[8];		/* 0x000 -- 0x01C */
	volatile unsigned int CLKDIV_STOP[4];	/* 0x020 -- 0x02C */
	volatile unsigned int rev1[4];		/* 0x030 -- 0x03C */
	volatile unsigned int SSRCOFF[4];	/* 0x040 -- 0x04C */
	volatile unsigned int CSRCOFF[4];	/* 0x050 -- 0x05C */
	volatile unsigned int rev2[104];	/* 0x060 -- 0x1FC */
};

struct NX_CMUBLK_RegisterSet {
	volatile unsigned int CLKMUXSEL;	/* 0x000 */
	volatile unsigned int CLKDIVRST;	/* 0x004 */
	volatile unsigned int _Rev0[2];		/* 0x008 -- 0x00C */
	volatile unsigned int SCLKENB[4];	/* 0x010 -- 0x01C */
	volatile unsigned int CCLKENB[4];	/* 0x020 -- 0x02C */
	volatile unsigned int SRST[4];		/* 0x030 -- 0x03C */
	volatile unsigned int CRST[4];		/* 0x040 -- 0x04C */
	volatile unsigned int RSTMODE[4];	/* 0x050 -- 0x05C */
	volatile unsigned int DIV[7];		/* 0x060 -- 0x078 */
	volatile unsigned int _Rev1;		/* 0x07C */
	volatile unsigned int CURDIV[7];	/* 0x080 -- 0x098 */
	volatile unsigned int _Rev2;		/* 0x09C */
	volatile unsigned int DBGCLKGEN;	/* 0x0A0 */
	volatile unsigned int _Rev3[3];		/* 0x0A4 -- 0x0AC */
	volatile unsigned int DBGCURDIV[7];	/* 0x0B0 -- 0x0C8 */
	volatile unsigned int _Rev4[13];	/* 0x0CC -- 0x0FC */
	volatile unsigned int _Rev5[64];	/* 0x100 -- 0x1FC */
};

struct NX_CMU_RegisterSet {
	struct NX_CMUSRC_RegisterSet CMUSRC;
	struct NX_CMUBLK_RegisterSet CMUBLK[1];
};

enum NX_CLKSRC {
	NX_CLKSRC_PLL0,
	NX_CLKSRC_PLL1,
	NX_CLKSRC_PLL_CPU,
	NX_CLKSRC_PLL_DDR0,
	NX_CLKSRC_PLL_DDR1,
	NX_CLKSRC_EXT_PAD,
	NX_CLKSRC_OSC,
	NX_CLKSRC_PLL_CPU_DIV,
	NX_CLKSRC_PLL_DDR0_DIV,
	NX_CLKSRC_PLL_DDR1_DIV,
	NX_CLKSRC_CPU_BACKUP
};

struct cmu_device_clk {
	unsigned short offset;
	unsigned char srcbit;
	unsigned char clkenbbit;
	unsigned char clksrc;
	unsigned char div;
};

#endif /* __NX_CMU_H__ */
