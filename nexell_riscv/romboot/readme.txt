how to bl1 generate.

first of all, generate bl1 for simulation

$ make clean
$ make SIMUL=y
$ cp build/antares_simul.bin ../tools/rsasign/
$ cp sample_nsih.txt ../tools/bootgen/

make two rsa private key (keysize=2048, pem format)

$ cd ../tools/pkkeygen/
$ ./pkkeygen filename=boot.key.pem
$ ./pkkeygen filename=user.key.pem

copy for signating directory

$ cp ./boot.key.pem ../rsasign/
$ cp ./user.key.pem ../rsasign/

generate image signature file & rsa public key file
$ cd ../rsasign
$ ./rsa_verify_pss boot.key.pem user.key.pem antares_simul.bin
$ cp antares_simul.bin.raw ../bootgen
$ cp antares_simul.bin.raw.sig ../bootgen

boot binary generating
$ cd ../bootgen
$ ./bootgen -n sample_nsih.txt -i antares_simul.bin.raw -l 0xffff0200 -s 0xffff0000
$ cp antares_simul.bin.raw.img ../../img

if you do not use secure boot you can use antares_simul.bin.raw.img file.


================================================================================
================================================================================

aes encryption for secure boot 

$ cp antares_simul.bin.raw.img ../nxaes
$ cd ../nxaes
$ ./aes_cbc -n antares_simul.bin.raw.img -m enc -b 128 -k 310AA092032084C8BD8E1AA4A81A3905 -v 73FC7B44B996F9990261A01C9CB93C8F
$ cp antares_simul.bin.raw.img.enc ../../img

================================================================================
================================================================================

for nand boot

$ cp antares_simul.bin.raw.img ../nandgen
or
$ cp antares_simul.bin.raw.img.enc ../nandgen
$ cd ../nandgen

$ ./nandbingen -p 1024 -i antares_simul.bin.raw.img
$ cp antares_simul.bin.raw.img_1024.ecc ../../img
or
$ ./nandbingen -p 512 -i antares_simul.bin.raw.img
$ cp antares_simul.bin.raw.img_512.ecc ../../img
or
$ ./nandbingen -p 1024 -i antares_simul.bin.raw.img.enc
$ cp antares_simul.bin.raw.img.enc_1024.ecc ../../img
or
$ ./nandbingen -p 512 -i antares_simul.bin.raw.img.enc
$ cp antares_simul.bin.raw.img.enc_512.ecc ../../img


for sdboot with gpt
$ cp antares_simul.bin.raw.img ../gptimagegen
or
$ cp antares_simul.bin.raw.img.enc ../gptimagegen

$ sudo dd if=/dev/zero of=./disk.img bs=1M count=5 && sync
$ sudo chown my_id disk.img
$ sudo chgrp my_id disk.img
$ chmod +x disk.img
$ fdisk ./disk.img
: g
: n, start(enter), end(enter)
: w
$ ./gptst disk.img antares_simul.bin.raw.img
$ cp disk.img.disk ../../img/
or
$ ./gptst disk.img antares_simul.bin.raw.img.enc
$ cp disk.img.disk ../../img/disk.img.disk.enc

