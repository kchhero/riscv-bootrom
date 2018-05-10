import binascii

BL1_FILE_NAME = "bl1.bin"

def bl1Create():
    genFile = file("bl1.bin",'wb')

    for j in range(4) :
        for i in range(128) :
            genFile.write(str(i)+' ')

    genFile.close()

def main():
    bl1Create()
    
if __name__ == "__main__":
    try : 
        main()
    finally : 
        pass
