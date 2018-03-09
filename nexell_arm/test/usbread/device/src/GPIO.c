#include <nx_pyrope.h>
#include <nx_type.h>

#include <nx_chip.h>

#include <nx_gpio.h>

struct NXPYROPE_GPIO_RegSet {
	struct NX_GPIO_RegisterSet NXGPIO;
	U8 Reserved[0x1000 - sizeof(struct NX_GPIO_RegisterSet)];
};

#if !DIRECT_IO
static struct NXPYROPE_GPIO_RegSet (*const pBaseGPIOReg)[1] =
    (struct NXPYROPE_GPIO_RegSet(*)[])(PHY_BASEADDR_GPIOA_MODULE);
void GPIOSetAltFunction(U32 AltFunc)
{
	register struct NX_GPIO_RegisterSet *pGPIOReg =
		&pBaseGPIOReg[(AltFunc >> 8) & 0x7]->NXGPIO;
	pGPIOReg->GPIOxALTFN[(AltFunc >> 7) & 0x1] =
		(pGPIOReg->GPIOxALTFN[(AltFunc >> 7) & 0x1] &
		 ~(0x3UL << (((AltFunc >> 3) & 0xF) * 2))) |
		((AltFunc & 0x3) << (((AltFunc >> 3) & 0xF) * 2));
}
#endif
