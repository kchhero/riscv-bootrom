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
        if (len(i) != 8) :
            print "ERROR"
#            binascii.
            break

        temp2 = []
        #i : 01 23 45 67
        temp2.append(i[6])
        temp2.append(i[7])
        temp2.append(i[4])
        temp2.append(i[5])
        temp2.append(i[2])
        temp2.append(i[3])
        temp2.append(i[0])
        temp2.append(i[1])

        print "".join(temp2)

        genFile.write(binascii.unhexlify("".join(temp2)))
          #  print binascii.unhexlify(hex(int(j,16)))

    genFile.close()

        
def main():
    nsihgen()
    
if __name__ == "__main__":
    try : 
        #profile.run('main()')
        main()
    finally : 
        pass
