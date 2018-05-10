#include <nx_swallow.h>

static void plic_init();
static void hart_plic_init();

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

typedef struct {
    volatile uint32_t* ipi;
    volatile int mipi_pending;

    volatile uint64_t* timecmp;

    volatile uint32_t* plic_m_thresh;
    volatile uintptr_t* plic_m_ie;
    volatile uint32_t* plic_s_thresh;
    volatile uintptr_t* plic_s_ie;
} hls_t;

#define HLS_SIZE 64
#define MACHINE_STACK_TOP() ({                                  \
            register uintptr_t sp asm ("sp");                   \
            (void*)((sp + SRAM_SIZE) & -SRAM_SIZE); })

#define HLS() ((hls_t*)(MACHINE_STACK_TOP() - HLS_SIZE))

static void plic_init()
{
    for (size_t i = 1; i <= plic_ndevs; i++)
        plic_priorities[i] = 1;
}

static void hart_plic_init()
{
    // clear pending interrupts
    *HLS()->ipi = 0;
    *HLS()->timecmp = -1ULL;
    write_csr(mip, 0);

    if (!plic_ndevs)
        return;

    size_t ie_words = plic_ndevs / sizeof(uintptr_t) + 1;
    for (size_t i = 0; i < ie_words; i++)
        HLS()->plic_s_ie[i] = ULONG_MAX;
    *HLS()->plic_m_thresh = 1;
    *HLS()->plic_s_thresh = 0;
}

#endif
