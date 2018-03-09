#define __SET_GLOBAL_VARIABLES

#include "sysheader.h"


extern CBOOL iUSBBOOT(struct NX_SecondBootInfo * pTBI);
CBOOL buildinfo(void);

int BootMain(void)
{
	struct NX_SecondBootInfo TBI;
	struct NX_SecondBootInfo *pTBI = &TBI; // third boot info
	int ret = CFALSE;
	U32 debug_ch = 3;

	/* Console initialize */
	DebugInit(debug_ch);

	/* Build information */
	buildinfo();

	if (pSBI->SIGNATURE != HEADER_ID) {
		printf("2nd Boot Header is invalid, Please check it out!\r\n");
	}

	printf("test usb start...\r\n");
	ret = iUSBBOOT(pTBI);			// for USB boot

	printf("usb test done\r\n");
	while (!DebugIsUartTxDone());

	return ret;
}
