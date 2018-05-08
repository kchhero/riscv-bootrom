import binascii
import os

def testhex():
    genFile = file("sdboot_test.hex",'wb')

    for i in range(512) :        
        genFile.write("%02x\n"%(i%255))

    genFile.close()
        
def main():
    testhex()
    
if __name__ == "__main__":
    try : 
        #profile.run('main()')
        main()
    finally : 
        pass
