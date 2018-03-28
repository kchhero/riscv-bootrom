#ifndef __NX_PLL_H__
#define __NX_PLL_H__

struct NX_PLL_RegisterSet {
	volatile unsigned int CTRL;		/* 0x000 */
	volatile unsigned int DBG0;		/* 0x004 */
	volatile unsigned int _Rev0[2];		/* 0x008 -- 0x00C */
	volatile unsigned int CNT[4];		/* 0x010 -- 0x01C */
	volatile unsigned int CFG0;		/* 0x020 */
	volatile unsigned int _Rev1[3];		/* 0x024 -- 0x02C */
	volatile unsigned int CFG1;		/* 0x030 */
	volatile unsigned int _Rev2[3];		/* 0x034 -- 0x03C */
	volatile unsigned int CFG2;		/* 0x040 */
	volatile unsigned int _Rev3[3];		/* 0x044 -- 0x04C */
	volatile unsigned int CTRL1;		/* 0x050 */
	volatile unsigned int _Rev4[3];		/* 0x054 -- 0x05C */
	volatile unsigned int LOCKINT0;		/* 0x060 */
};
#endif /* __NX_PLL_H__ */
