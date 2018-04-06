OUTPUT_ARCH("riscv")
ENTRY(_prog_start)

MEMORY
{
  debug_ctrl (rwx) : ORIGIN = 0x0, LENGTH = 0x100
  maskrom_mem (rx) : ORIGIN = 0x10000, LENGTH = 0x10000
  clint_ctrl (rw) : ORIGIN = 0x2000000, LENGTH = 0x10000
  plic_ctrl (rw) : ORIGIN = 0xc000000, LENGTH = 0x4000000
  gpio0_ctrl (rw) : ORIGIN = 0x20700000, LENGTH = 0x1000
  gpio1_ctrl (rw) : ORIGIN = 0x20710000, LENGTH = 0x1000
  gpio2_ctrl (rw) : ORIGIN = 0x20720000, LENGTH = 0x1000
  gpio3_ctrl (rw) : ORIGIN = 0x20730000, LENGTH = 0x1000
  gpio4_ctrl (rw) : ORIGIN = 0x20740000, LENGTH = 0x1000
  gpio5_ctrl (rw) : ORIGIN = 0x20750000, LENGTH = 0x1000
  gpio6_ctrl (rw) : ORIGIN = 0x20760000, LENGTH = 0x1000
  gpio7_ctrl (rw) : ORIGIN = 0x20770000, LENGTH = 0x1000
  uart0_ctrl (rw) : ORIGIN = 0x208A0000, LENGTH = 0x1000
  uart1_ctrl (rw) : ORIGIN = 0x208B0000, LENGTH = 0x1000
  uart2_ctrl (rw) : ORIGIN = 0x208C0000, LENGTH = 0x1000
  uart3_ctrl (rw) : ORIGIN = 0x208D0000, LENGTH = 0x1000
  spi0_ctrl  (rw) : ORIGIN = 0x20800000, LENGTH = 0x1000
  spi1_ctrl  (rw) : ORIGIN = 0x20810000, LENGTH = 0x1000
  spi2_ctrl  (rw) : ORIGIN = 0x20820000, LENGTH = 0x1000  
  sdmmc0_ctrl (rw) : ORIGIN = 0x20930000, LENGTH = 0x10000
  sdmmc1_ctrl (rw) : ORIGIN = 0x20940000, LENGTH = 0x10000
  memory_mem (rwx) : ORIGIN = 0x40000000, LENGTH = 0x8000
}


PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}

SECTIONS
{
  PROVIDE(_ram = ORIGIN(memory_mem));
  PROVIDE(_ram_end = _ram + LENGTH(memory_mem));
  
  .text ALIGN((ORIGIN(maskrom_mem) + 0x0), 8) : AT(ALIGN((ORIGIN(maskrom_mem) + 0x0), 8)) {
    PROVIDE(_ftext = .);
    *(.text.init)
    *(.text.unlikely .text.unlikely.*)
    *(.text .text.* .gnu.linkonce.t.*)
    PROVIDE(_etext = .);
    . += 0x40; /* to create a gap between .text and .data b/c ifetch can fetch ahead from .data */
  } >maskrom_mem  :text

  .eh_frame ALIGN((ADDR(.text) + SIZEOF(.text)), 8) : AT(ALIGN((LOADADDR(.text) + SIZEOF(.text)), 8)) {
    *(.eh_frame)
  } >maskrom_mem  :text

  .rodata ALIGN((ADDR(.eh_frame) + SIZEOF(.eh_frame)), 8) : AT(ALIGN((LOADADDR(.eh_frame) + SIZEOF(.eh_frame)), 8)) ALIGN_WITH_INPUT {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  } >maskrom_mem  :data

  .srodata ALIGN((ADDR(.rodata) + SIZEOF(.rodata)), 8) : AT(ALIGN((LOADADDR(.rodata) + SIZEOF(.rodata)), 8)) ALIGN_WITH_INPUT {
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata.*)
  } >maskrom_mem  :data

  .data ALIGN((ADDR(.srodata) + SIZEOF(.srodata)), 8) : AT(ALIGN((LOADADDR(.srodata) + SIZEOF(.srodata)), 8)) ALIGN_WITH_INPUT {
    *(.data .data.* .gnu.linkonce.d.*)
    *(.tohost) /* TODO: Support sections that aren't explicitly listed in this linker script */
  } >maskrom_mem  :data

  .sdata ALIGN((ADDR(.data) + SIZEOF(.data)), 8) : AT(ALIGN((LOADADDR(.data) + SIZEOF(.data)), 8)) ALIGN_WITH_INPUT {
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } >maskrom_mem  :data

  PROVIDE(_data = ADDR(.rodata));
  PROVIDE(_data_lma = LOADADDR(.rodata));
  PROVIDE(_edata = .);

  .bss ALIGN((ORIGIN(memory_mem) + 0x0), 8) : AT(ALIGN((ORIGIN(memory_mem) + 0x0), 8)) ALIGN(8) {
    PROVIDE(_fbss = .);
    PROVIDE(__global_pointer$ = . + 0x7C0);
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.bss .bss.* .gnu.linkonce.b.*)
    . = ALIGN(8);
    PROVIDE(_ebss = .);
  } >memory_mem  :bss

  PROVIDE(_end = .);

  /*
   * heap_stack_region_usable_end: (ORIGIN(memory_mem) + LENGTH(memory_mem))
   * heap_stack_min_size: 4096
   * heap_stack_max_size: 1048576
   */
  PROVIDE(_sp = ALIGN(MIN((ORIGIN(memory_mem) + LENGTH(memory_mem)), _ebss + 1048576) - 7, 8));
  PROVIDE(_heap_end = _sp - 2048);

  /* This section is a noop and is only used for the ASSERT */
  .stack : {
    ASSERT(_sp >= (_ebss + 4096), "Error: No room left for the heap and stack");
  }
}
