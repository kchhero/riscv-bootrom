make clean
make
riscv64-unknown-elf-objdump -D build/bl1.elf > build/bl1.dump
cp build/bl1.bin ~/RISC-V/nexell/riscv-bootrom/nexell_riscv/tools/bootgen/
