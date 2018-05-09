import os
from tempfile import mkstemp
from shutil import move
from os import fdopen, remove

BL1_FILE_NAME = "bl1.bin"
NSIH_TXT_FILE_NAME = "nsih.txt"

def getNSIHSize() :
    temp = os.stat(BL1_FILE_NAME).st_size + 32
    temp = "%08x"%temp
    return str(temp)

def modNSIHTXT() :
    tempFile1, tempFile2 = mkstemp()
    with fdopen(tempFile1,'wt') as new_file:
        with open(NSIH_TXT_FILE_NAME) as org_file:
            for line in org_file:
                if "0x040" in  line and "Load Size" in line :
                    new_file.write(getNSIHSize() + "   " + "".join(line.split('   ')[1:]))
                else :
                    new_file.write(line)

    remove(NSIH_TXT_FILE_NAME)
    move(tempFile2, NSIH_TXT_FILE_NAME)

    
def main():
    modNSIHTXT()
    
if __name__ == "__main__":
    try : 
        main()
    finally : 
        pass
