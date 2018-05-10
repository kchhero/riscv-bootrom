#!/bin/bash

argc=$#
BINTYPE=$1

if [ $argc -lt 1 ]
then
    echo "Invalid argument check usage please"
    exit
fi

#nsih.txt modified for bl1.bin size compatible
python nsihtxtmod.py

#create sdboot.bin, select type gpt or dos
python bootbingen.py ${BINTYPE}

#create sdboot.bin 
#dd if=/dev/zero of=sdboot.bin bs=512 count=1

#Add nsih.bin to sdboot.bin
dd if=nsih.bin bs=512 >> sdboot.bin

#Add bl1.bin to sdboot.bin
dd if=bl1.bin >> sdboot.bin

#Convert from ascii to hex
python converthex.py

mv sdboot.hex sdboot_${BINTYPE}.hex

cp sdboot_dos.hex ~/RISC-V/nexell/soc-reference/SOC/board/drone_soc/chip/drone_soc/evt0/design/RISCV/sim/l4_rtl/bootrom_test/compile/build/
cp sdboot_gpt.hex ~/RISC-V/nexell/soc-reference/SOC/board/drone_soc/chip/drone_soc/evt0/design/RISCV/sim/l4_rtl/bootrom_test/compile/build/
