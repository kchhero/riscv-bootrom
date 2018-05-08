import binascii
import os
import sys

ORG_GPT_FILE_NAME = "sdboot_gpt.bin"
ORG_DOS_FILE_NAME = "sdboot_dos.bin"
ZERO_PAD_FILE_NAME = "zeropad.bin"
GEN_FILE_NAME = "sdboot.bin"

def nsihgen():
    nsihFilePath = "nsih.txt"

    genFile = file("nsih.bin",'wb')
    temp = []
    
    with open(nsihFilePath, 'rt') as data :
        for line in data :
            if len(line) <= 7 or (line[0]=='/' and line[1]=='/') :
                continue
            
            temp.append((line.split(' '))[0].strip().lower())

    for i in temp :
        temp2 = 0
        if (len(i) != 8) :
            print "ERROR"
#            binascii.
            break

        #for j in i :
        print binascii.unhexlify(i)
        genFile.write(binascii.unhexlify(i))
          #  print binascii.unhexlify(hex(int(j,16)))

    genFile.close()


def sdboot_gpt_headercut():
    genFile = file("sdboot.bin",'wb')
    
    with open(ORG_GPT_FILE_NAME, 'rb') as data :        
        genFile.write(data.read(0x43f))#1088)) #0~0x43f

    genFile.close()
    
    padAppend(0x43ff - 0x43f + 1)  #512byte * 34sector = 17408

    # with open('sdboot.bin', 'rb+') as filehandle:
    #     filehandle.seek(-1, os.SEEK_END)
    #     filehandle.truncate()
    
def sdboot_dos_headercut():
    genFile = file("sdboot.bin",'wb')
    
    with open(ORG_DOS_FILE_NAME, 'rb') as data :        
        genFile.write(data.read(512)) #0~0x1ff
                                    
    genFile.close()

def padAppend(padSize):
    genFile = file("sdboot.bin",'ab')

    with open(ZERO_PAD_FILE_NAME, 'rb') as data :
        genFile.write(data.read(padSize)) #0~0x1f0 + 16byte

    genFile.close()
        
def main(binType):
    if binType == "gpt":
        sdboot_gpt_headercut()
    elif binType == "dos":
        sdboot_dos_headercut()
    else :
        print "Usage: python bootbingen.py \"gpt\""
        print "Usage: python bootbingen.py \"dos\""

    nsihgen()
    
if __name__ == "__main__":
    try : 
        #profile.run('main()')
        main(sys.argv[1])
    finally : 
        pass
