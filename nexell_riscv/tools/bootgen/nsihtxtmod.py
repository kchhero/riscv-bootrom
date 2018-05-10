import os
from tempfile import mkstemp
from shutil import move
from os import fdopen, remove

BL1_FILE_NAME = "bl1.bin"
NSIH_TXT_FILE_NAME = "nsih.txt"

ZERO_PAD_FILE_NAME = "zeropad.bin"

def bl1padAppend():
    tempBl1Size = os.stat(BL1_FILE_NAME).st_size
    temp = (tempBl1Size + 512 - 1)/512
    padSize = (temp * 512) - tempBl1Size

    genFile = file("bl1.bin",'ab')
    print "bl1 padSize = %d"%padSize

    with open(ZERO_PAD_FILE_NAME, 'rb') as data :
        genFile.write(data.read(padSize)) #0~0x1f0 + 16byte

    genFile.close()

def getNSIHSize() :
    temp = os.stat(BL1_FILE_NAME).st_size
    # tempInt = int(temp)
    # tempAlign = 512*int((tempInt + 512 -1)/512)
    temp = "%08x"%temp
    return temp

def modNSIHTXT() :
    tempFile1, tempFile2 = mkstemp()
    with fdopen(tempFile1,'wt') as new_file:
        with open(NSIH_TXT_FILE_NAME) as org_file:
            for line in org_file:
                if "0x040" in  line and "Load Size" in line :
                    new_file.write(str(getNSIHSize()) + "   " + "".join(line.split('   ')[1:]))
                else :
                    new_file.write(line)

    remove(NSIH_TXT_FILE_NAME)
    move(tempFile2, NSIH_TXT_FILE_NAME)

    
def main():
    bl1padAppend()
    modNSIHTXT()
    
if __name__ == "__main__":
    try : 
        main()
    finally : 
        pass
