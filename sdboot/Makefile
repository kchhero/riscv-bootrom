# RISCV environment variable must be set

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
CFLAGS=-march=rv64imac -mcmodel=medany -O2 -std=gnu11 -Wall -nostartfiles 
CFLAGS+= -fno-common -g -DENTROPY=0 -mabi=lp64 -DNONSMP_HART=0 
CFLAGS+= -I ./include -I.
LFLAGS=-static -nostdlib -L ./linker -T sdboot.elf.lds -g
LFLAGS+=-Wl,-Map=build/sdboot.map,--cref
BUILD_DIR=./build

elf := $(BUILD_DIR)/sdboot.elf
$(elf): head.S kprintf.c sd.c 
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ head.S sd.c kprintf.c

.PHONY: elf
elf: $(elf)

bin := $(BUILD_DIR)/sdboot.bin
$(bin): $(elf)
	$(OBJCOPY) -O binary $< $@

.PHONY: bin
bin: $(bin)

hex := $(BUILD_DIR)/sdboot.hex
$(hex): $(bin)
	od -t x4 -An -w4 -v $< > $@

.PHONY: hex
hex: $(hex)

romgen := $(BUILD_DIR)/rom.v
$(romgen): $(hex)
	$(rocketchip_dir)/scripts/vlsi_rom_gen $(ROMCONF) $< > $@

.PHONY: romgen
romgen: $(romgen)

.PHONY: clean
clean::
	rm -rf $(hex) $(elf)
