#include "sysheader.h"

#define UARTSRC 		2
#define SOURCE_DIVID		(4UL)
#define BAUD_RATE 		(115200)

void ResetCon(U32 devicenum, CBOOL en);
void GPIOSetAltFunction(U32 AltFunc);
U32 NX_CLKPWR_GetPLLFrequency(U32 PllNumber);

static struct NX_UART_RegisterSet *pReg_Uart;

const U32 UARTBASEADDR[] = {
	PHY_BASEADDR_UART0_MODULE, 
	PHY_BASEADDR_pl01115_Uart_modem_MODULE,
	PHY_BASEADDR_UART1_MODULE, 
	PHY_BASEADDR_pl01115_Uart_nodma0_MODULE,
	PHY_BASEADDR_pl01115_Uart_nodma1_MODULE
};
const U32 UARTCLKGENADDR[] = {
	PHY_BASEADDR_CLKGEN22_MODULE, 
	PHY_BASEADDR_CLKGEN24_MODULE,
	PHY_BASEADDR_CLKGEN23_MODULE, 
	PHY_BASEADDR_CLKGEN25_MODULE,
	PHY_BASEADDR_CLKGEN26_MODULE
};

const U8 RESETNUM[] = {
	RESETINDEX_OF_UART0_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_modem_MODULE_nUARTRST,
	RESETINDEX_OF_UART1_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_nodma0_MODULE_nUARTRST,
	RESETINDEX_OF_pl01115_Uart_nodma1_MODULE_nUARTRST
};

const U32 GPIOALTNUM[] = {
	PADINDEX_OF_UART0_UARTRXD,
	PADINDEX_OF_UART0_UARTTXD,
	PADINDEX_OF_pl01115_Uart_modem_UARTRXD,
	PADINDEX_OF_pl01115_Uart_modem_UARTTXD, 
	PADINDEX_OF_UART1_UARTRXD,
	PADINDEX_OF_UART1_UARTTXD, 
	PADINDEX_OF_pl01115_Uart_nodma0_UARTRXD,
	PADINDEX_OF_pl01115_Uart_nodma0_UARTTXD,
	PADINDEX_OF_pl01115_Uart_nodma1_UARTRXD,
	PADINDEX_OF_pl01115_Uart_nodma1_UARTTXD,
};
const U32 UARTSMC[] = {
	TIEOFFINDEX_OF_UART0_USESMC,
	TIEOFFINDEX_OF_UART0_SMCTXENB,
	TIEOFFINDEX_OF_UART0_SMCRXENB,
	TIEOFFINDEX_OF_UART_MODEM0_USESMC,
	TIEOFFINDEX_OF_UART_MODEM0_SMCTXENB,
	TIEOFFINDEX_OF_UART_MODEM0_SMCRXENB,
	TIEOFFINDEX_OF_UART1_USESMC,
	TIEOFFINDEX_OF_UART1_SMCTXENB,
	TIEOFFINDEX_OF_UART1_SMCRXENB,
	TIEOFFINDEX_OF_UART_NODMA0_USESMC,
	TIEOFFINDEX_OF_UART_NODMA0_SMCTXENB,
	TIEOFFINDEX_OF_UART_NODMA0_SMCRXENB,
	TIEOFFINDEX_OF_UART_NODMA1_USESMC,
	TIEOFFINDEX_OF_UART_NODMA1_SMCTXENB,
	TIEOFFINDEX_OF_UART_NODMA1_SMCRXENB
};

CBOOL DebugInit(U32 ch)
{
	U32 SOURCE_CLOCK;
	struct NX_CLKGEN_RegisterSet *const pReg_UartClkGen =
		(struct NX_CLKGEN_RegisterSet *)UARTCLKGENADDR[ch];

	pReg_Uart = (struct NX_UART_RegisterSet *)UARTBASEADDR[ch];

//	NX_CLKPWR_SetOSCFreq(OSC_KHZ);

	SOURCE_CLOCK = NX_CLKPWR_GetPLLFrequency(UARTSRC);

	GPIOSetAltFunction(GPIOALTNUM[ch * 2 + 0]);
	GPIOSetAltFunction(GPIOALTNUM[ch * 2 + 1]);

	pReg_Tieoff->TIEOFFREG[((UARTSMC[ch * 3 + 0]) & 0xFFFF) >> 5] &=
		(~(1 << ((UARTSMC[ch * 3 + 0]) & 0x1F)));
	pReg_Tieoff->TIEOFFREG[((UARTSMC[ch * 3 + 1]) & 0xFFFF) >> 5] &=
		(~(1 << ((UARTSMC[ch * 3 + 1]) & 0x1F)));
	pReg_Tieoff->TIEOFFREG[((UARTSMC[ch * 3 + 2]) & 0xFFFF) >> 5] &=
		(~(1 << ((UARTSMC[ch * 3 + 2]) & 0x1F)));

	/* reset assert */
	ResetCon(RESETNUM[ch], CTRUE);
	/* reset negate */
	ResetCon(RESETNUM[ch], CFALSE);
	/* PCLKMODE : always, Clock Gen Disable */
	pReg_UartClkGen->CLKENB =(1 << 3);
	pReg_UartClkGen->CLKGEN[0] =
		((SOURCE_DIVID - 1) << 5) | (UARTSRC << 2);
	/* 8 bit, none parity, stop 1, normal mode */
	pReg_Uart->LCR_H = 0x0070;
	/* rx, tx enable */
	pReg_Uart->CR = 0x0300;
	/* IBRD = 8, 115200bps */
	pReg_Uart->IBRD =
		(U16)((SOURCE_CLOCK / SOURCE_DIVID) / ((BAUD_RATE / 1) * 16));
	pReg_Uart->FBRD = (U16)((((((SOURCE_CLOCK / SOURCE_DIVID) %
					((BAUD_RATE / 1) * 16)) + 32) * 64) /
					((BAUD_RATE / 1) * 16))); /* FBRD = 0 */
	/* PCLKMODE : always, Clock Gen Enable */
	pReg_UartClkGen->CLKENB =
		(1 << 3) | (1 << 2);
	/* rx, tx, uart enable */
	pReg_Uart->CR = 0x0301;

	return CTRUE;
}

void DebugPutch(S8 ch)
{
	const U16 TX_FIFO_FULL = 1 << 5;
	while (pReg_Uart->FR & TX_FIFO_FULL);
	pReg_Uart->DR = (U32)ch;
}

CBOOL DebugIsUartTxDone(void)
{
	const U16 UART_TX_BUSY = 1 << 3;
	const U16 TX_FIFO_EMPTY = 1 << 7;
	return ((pReg_Uart->FR & (UART_TX_BUSY | TX_FIFO_EMPTY)) ==
			TX_FIFO_EMPTY ? CTRUE : CFALSE);
}

CBOOL DebugIsTXEmpty(void)
{
	const U16 TX_FIFO_EMPTY = 1 << 7;
	return (CBOOL)(pReg_Uart->FR & TX_FIFO_EMPTY);
}

CBOOL DebugIsBusy(void)
{
	const U16 UART_TX_BUSY = 1 << 3;
	return (CBOOL)(pReg_Uart->FR & UART_TX_BUSY);
}

S8 DebugGetch(void)
{
	const U16 RX_FIFO_EMPTY = 1 << 4;
	while (pReg_Uart->FR & RX_FIFO_EMPTY);
	return (S8)pReg_Uart->DR;
}

