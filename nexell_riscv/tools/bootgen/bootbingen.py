import binascii

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
            break

        #for j in i :
        print binascii.unhexlify(i)
        genFile.write(binascii.unhexlify(i))
          #  print binascii.unhexlify(hex(int(j,16)))
                                    
    genFile.close()


def test():
    genFile = file("nsih.bin",'wb')
    print binascii.unhexlify('1a')
    genFile.write(binascii.unhexlify('1a'))
    
def main():
    nsihgen()
    #test()
    
if __name__ == "__main__":
    try : 
        #profile.run('main()')
        main()
    finally : 
        pass
