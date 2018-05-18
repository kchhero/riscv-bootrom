make clean;make
riscv64-unknown-elf-objdump -D build/swallow.elf > build/swallow.dump
#python romboot32Kfill.py

cp build/swallow.* ~/RISC-V/nexell/soc-reference/SOC/board/drone_soc/chip/drone_soc/evt0/design/RISCV/sim/l4_rtl/bootrom_test/compile/build/
