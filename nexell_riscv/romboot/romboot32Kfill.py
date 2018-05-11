import os

SWALLOW_HEX_FILE_NAME = "build/swallow.hex"

def swallowpadAppend():
    tempSize = os.stat(SWALLOW_HEX_FILE_NAME).st_size
    padSize = (32*1024 - tempSize)

    print "padSize = %d"%padSize
    genFile = file("build/swallow.hex",'at')
    for i in range(padSize/3) :
        genFile.write("00\n")

    genFile.write("00")
    genFile.close()
    
def main():
    swallowpadAppend()
    
if __name__ == "__main__":
    try : 
        main()
    finally : 
        pass

