#!/bin/bash

python bootbingen.py

#create sdboot.bin 
dd if=/dev/zero of=sdboot.bin bs=512 count=1

#append nsih.bin to sdboot.bin
dd if=nsih.bin bs=512 >> sdboot.bin

#append bl1.bin to sdboot.bin
dd if=bl1.bin >> sdboot.bin
