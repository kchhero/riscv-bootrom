#include <riscv_csr.h>
#include <nx_swallow_printf.h>
#include <nx_swallow_platform.h>

static void read_mstatus(void);
static void read_mie(void);
static void read_mip(void);
static void read_mcause(void);
static void clr_mie(void);
static void clr_mip(void);

void clint_test_main(void)
{
    _dprintf ("<<bootrom>> clint test Start!!\n");
    /* _dprintf("----------------------------\n"); */
    /* _dprintf("MSR_IE = 0x%x\n",MSR_IE); */
    /* _dprintf("MSR_PIE = 0x%x\n",MSR_PIE); */
    /* _dprintf("MSR_PS = 0x%x\n",MSR_PS); */
    /* _dprintf("----------------------------\n"); */

    read_mstatus();
    read_mie();
    read_mip();
    read_mcause();

    //    csr_write(mtvec, 0x20948888);
    _dprintf("1 mtvec = 0x%x\n", csr_read(mtvec));

    /* _dprintf("----------------------------\n"); */
    /* csr_clear(mstatus, MSR_IE); */
    /* read_mstatus(); */
    /* read_mip(); */

    /* _dprintf("----------------------------\n"); */
    /* csr_write(mip, 0x8); */
    /* //    WriteIODW(0x02000004, 1); */

    /* read_mstatus(); */
    /* read_mie(); */
    /* read_mip(); */
    /* read_mcause(); */
}

void clint_test_main2(void)
{
    _dprintf ("<<bootrom>> clint test2 Start!!\n");
    //    csr_write(mtvec, 0x20948888);
    _dprintf("2 mtvec = 0x%x\n", csr_read(mtvec));
}

static void read_mstatus(void)
{
    _dprintf ("mstatus = 0x%x\n", csr_read(mstatus));
}

static void read_mie(void)
{
    _dprintf ("mie = 0x%x\n", csr_read(mie));
}

static void read_mip(void)
{
    _dprintf ("mip = 0x%x\n", csr_read(mip));
}

static void read_mcause(void)
{
    _dprintf ("mcause = 0x%x\n", csr_read(mcause));
}

static void clr_mie(void)
{
    csr_clear(mie, 0);
}

static void clr_mip(void)
{
    csr_clear(mip, 0);
}
