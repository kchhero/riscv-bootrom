bootrom_img = bootrom.img
bootrom_bin = bootrom.bin
bootrom_elf = bootrom.elf

RISCV_GCC?=${RISCV_TOOLCHAIN}/riscv64-unknown-elf-gcc
RISCV_OBJCOPY?=${RISCV_TOOLCHAIN}/riscv64-unknown-elf-objcopy

all: $(bootrom_bin) $(bootrom_elf) $(bootrom_img)

clean:
	rm $(bootrom_bin) $(bootrom_elf) $(bootrom_img)

%.img: %.bin
	dd if=$< of=$@ bs=128 count=1

%.bin: %.elf
	$(RISCV_OBJCOPY) -O binary $< $@

%.elf: %.S linker.ld
	$(RISCV_GCC) -Tlinker.ld $< -nostdlib -g -ggdb -static -Wl,--no-gc-sections -o $@
