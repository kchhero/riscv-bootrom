MEMORY
{
  debug_ctrl (rwx) : ORIGIN = 0x0, LENGTH = 0x100
  maskrom_mem (rx) : ORIGIN = 0x1000, LENGTH = 0x20000
  clint_ctrl (rw) : ORIGIN = 0x2000000, LENGTH = 0x10000
  plic_ctrl (rw) : ORIGIN = 0xc000000, LENGTH = 0x4000000
  gpio_ctrl (rw) : ORIGIN = 0x10012000, LENGTH = 0x1000
  uart_ctrl (rw) : ORIGIN = 0x10013000, LENGTH = 0x1000
  spi_ctrl (rw) : ORIGIN = 0x10014000, LENGTH = 0x1000
  sdmmc0_ctrl (rw) : ORIGIN = 0x26080000, LENGTH = 0x10000
  sdmmc1_ctrl (rw) : ORIGIN = 0x26090000, LENGTH = 0x10000
  sdmmc2_ctrl (rw) : ORIGIN = 0x260A0000, LENGTH = 0x10000
  memory_mem (rwx) : ORIGIN = 0x80000000, LENGTH = 0x40000000
}
