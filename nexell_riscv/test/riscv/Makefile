# RISCV environment variable must be set

CC=riscv64-unknown-elf-gcc
OBJCOPY=riscv64-unknown-elf-objcopy
CFLAGS=-march=rv64imafdc -mcmodel=medany -O2 -std=gnu11 -Wall -nostartfiles 
CFLAGS+= -fno-common -g -DENTROPY=0 -mabi=lp64d -DNONSMP_HART=0 
CFLAGS+= -I ./include -I.
LFLAGS=-static -nostdlib -L ./linker -T linker.ld -g
LFLAGS+=-Wl,-Map=build/test.map,--cref
BUILD_DIR=./build

all: elf bin hex

elf := $(BUILD_DIR)/test.elf
$(elf): kprintf.c test.c 
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ test.c kprintf.c

.PHONY: elf
elf: $(elf)

bin := $(BUILD_DIR)/test.bin
$(bin): $(elf)
	$(OBJCOPY) -O binary $< $@

.PHONY: bin
bin: $(bin)

hex := $(BUILD_DIR)/test.hex
$(hex): $(bin)
	od -t x4 -An -w4 -v $< > $@

.PHONY: hex
hex: $(hex)

.PHONY: clean
clean::
	rm -rf $(hex) $(elf)
