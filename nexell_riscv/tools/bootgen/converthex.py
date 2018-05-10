import binascii

def converthex():
    srcFilePath = "sdboot.bin"
    hexFileName = "sdboot.hex"

    genFile = file(hexFileName,'wb')
    # temp = []
    
    with open(srcFilePath, 'rb') as data :
        while 1 :
            byte_s = data.read(1)
            if not byte_s :
                break

            # print binascii.hexlify(byte_s) + ' ',
            genFile.write(binascii.hexlify(byte_s)+"\n")
    
    genFile.close()


    
def main():
    converthex()
    #test()
    
if __name__ == "__main__":
    try : 
        #profile.run('main()')
        main()
    finally : 
        pass
