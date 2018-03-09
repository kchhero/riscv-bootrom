#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "nx_antares_bootheader.h"

#define POLY 0xEDB88320L


static void usage(void);
static unsigned int get_fcs(unsigned int fcs, unsigned int data);
static unsigned int HexAtoInt(const char *string);
static int ProcessNSIH(const char *pfilename, unsigned char *pOutData);

int main(int argc, char **argv)
{
	int ret = -1;
	FILE *f		= NULL;
	int fsize;
	unsigned char* input_name 	= NULL;
	unsigned char* nsih_name 	= NULL; 

	unsigned int param_opt 	= 0;

	unsigned int *pout = (unsigned int *)malloc(0x10000);
	unsigned char* pbuf = (unsigned char *)pout;
	struct nx_bootheader *pbh = (struct nx_bootheader *)pout;

	unsigned int loadaddr	= 0;
	unsigned int launchaddr	= 0;

	if (argc <= 1) {
		usage();
		return 1;
	}

	while (-1 !=(param_opt = getopt(argc, argv, "h:n:i:l:s:"))) {
		switch (param_opt) {
		case 'h':
			usage();
			return 1;
		case 'n': 
			nsih_name 	= strdup(optarg);
			break;
		case 'i':
			input_name 	= strdup(optarg);
			break;
		case 'l':
			loadaddr 	= HexAtoInt(optarg);
			break;
		case 's':
			launchaddr	= HexAtoInt(optarg);
			break;
		default:
			printf("unknown option_num parameter\r\n");
			break;
		}
	}
	if (loadaddr == 0)
		loadaddr = 0xFFFF0000;
	if (launchaddr == 0)
		launchaddr = 0xFFFF0000;

	if (nsih_name == NULL) {
		nsih_name = "NSIH.txt";
		printf("Did not enter the NSIH files.\r\n");
		printf("This has been used as the default NSIH file.\r\n");
	} else {
		int nsize = ProcessNSIH(nsih_name, pbuf);
		if (nsize != 512) {
			printf("nsih generation size error:%d\r\n", nsize);
			return -1;
		}
	}

	if (input_name == NULL) {
		input_name = "antares_bl1.bin";
		printf("Did not enter the Binary files.\r\n");
		printf("This has been used as the default antares_bl1.bin.\r\n");
	}
	char fn[0x100];
#if 0
/*--------------------------------------------------------------------------*/
	memset(fn, 0, 100);
	sprintf(fn, "%s.pub", input_name);
	f = fopen(fn, "r");

	if (!f) {
		printf("%s open failed!! check file!!\n", fn);
		goto ERR_END;
	} else {
		fsize = fread(pbh->rsa_public.rsapublicbootkey, 1, 256, f);
		fclose(f);
		if (fsize != 256) {
			printf("%s key size error (%d)\r\n", fn, fsize);
			return -2;
		}
	}
/*--------------------------------------------------------------------------*/
	memset(fn, 0, 100);
	sprintf(fn, "%s.usr", input_name);
	f = fopen(fn, "r");

	if (!f) {
		printf("%s open failed!! check file!!\n", fn);
		goto ERR_END;
	} else {
		fsize = fread(pbh->rsa_public.rsapublicuserkey, 1, 256, f);
		fclose(f);
		if (fsize != 256) {
			printf("%s key size error (%d)\r\n", fn, fsize);
			return -4;
		}
	}
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
	memset(fn, 0, 100);
	sprintf(fn, "%s.sig", input_name);
	f = fopen(fn, "r");

	if (!f) {
		printf("%s open failed!! check file!!\n", fn);
		goto ERR_END;
	} else {
		fsize = fread(pbh->rsa_public.rsaencryptedsha256hash, 1, 256, f);
		fclose(f);
		if (fsize != 256) {
			printf("%s key size error (%d)\r\n", fn, fsize);
			return -3;
		}
	}
/*--------------------------------------------------------------------------*/
	f = fopen(input_name, "r");
	if (!f) {
		printf("Input File open failed!! check filename!!\n");
		goto ERR_END;
	} else {
		fsize = fread(pbh->rsa_public.rsapublicbootkey, 1,
				0x10000 - sizeof(struct nx_bootinfo) - 256, f);
		fclose(f);
		printf("%s size %d\n", input_name, fsize);
		if (fsize % 0x10)
			fsize = (((fsize + 15) >> 4) << 4);
		printf("size is resing for crypto (%d)\n", fsize);
	}

	unsigned int crc = 0, i;
	unsigned int crcsize = fsize - 512;	// boot.pub, user.pub
	for (i = 0; i < crcsize / 4; i++)
		crc = get_fcs(crc, pbh->image[i]);

	pbh->bi.CRC32 = crc;
	pbh->bi.LoadSize = crcsize;
	pbh->bi.LoadAddr = loadaddr;
	pbh->bi.StartAddr = launchaddr;

/*--------------------------------------------------------------------------*/
	memset(fn, 0, 0x100);
	sprintf(fn, "%s.img", input_name);
	f = fopen(fn, "wb+");

	fwrite(pout, 1, fsize + sizeof(struct nx_bootinfo) + 256, f);
/*--------------------------------------------------------------------------*/
	ret = 0;
	printf("generated %s file\n", fn);

ERR_END:
	if (pout)
		free(pout);
	if (f)
		fclose(f);
	return ret;
}

static unsigned int get_fcs(unsigned int fcs, unsigned int data)
{
	int i;

	fcs ^= data;
	for (i = 0; i < 32; i++) {
		if (fcs & 0x01) 
			fcs = (fcs >> 1) ^ POLY; 
		else
			fcs >>= 1;
	}
	return fcs;
}

// Nexell System Information Header
//------------------------------------------------------------------------------
static int ProcessNSIH(const char *pfilename, unsigned char *pOutData)
{
	FILE *fp;
	int writesize, skipline, line, bytesize, i;
	unsigned char ch;
	unsigned int val = 0, *pBuf = (unsigned int *)pOutData;

	fp = fopen(pfilename, "rb");
	if (!fp) {
		printf("ProcessNSIH : ERROR - Failed to open %s file.\n",
				pfilename);
		return 0;
	}

	bytesize = 0;
	writesize = 0;
	skipline = 0;
	line = 0;

	while (0 == feof(fp)) {
		ch = fgetc(fp);

		if (skipline == 0) {
			if (ch >= '0' && ch <= '9') {
				val <<= 4;
				val |= ch - '0';
				writesize++;
			} else if (ch >= 'a' && ch <= 'f') {
				val <<= 4;
				val |= ch - 'a' + 10;
				writesize++;
			} else if (ch >= 'A' && ch <= 'F') {
				val <<= 4;
				val |= ch - 'A' + 10;
				writesize++;
			} else {
				if (writesize == 8) {
					*pBuf++ = val;
					bytesize += 4;
				} else {
					if (writesize) {
						printf("ProcessNSIH : Error at %d line.\n", line + 1);
						break;
					}
				}
				writesize = 0;
				skipline = 1;
			}
		}

		if (ch == '\n') {
			line++;
			skipline = 0;
			val = 0;
		}
	}

	printf("ProcessNSIH : %d line processed.\n", line + 1);
	printf("ProcessNSIH : %d bytes generated.\n", bytesize);

	fclose(fp);

	return bytesize;
}

static unsigned int HexAtoInt(const char *string)
{
	char ch;
	unsigned int ret = 0;

	while (ch = *string++) {
		ret <<= 4;
		if (ch >= '0' && ch <= '9') {
			ret |= ch - '0' +  0;
		} else if (ch >= 'a' && ch <= 'f') {
			ret |= ch - 'a' + 10;
		} else if (ch >= 'A' && ch <= 'F') {
			ret |= ch - 'A' + 10;
		}
	}

	return ret;
}

static void usage(void)
{
	printf("------------------------------------------------\n");
	printf(" Usage :					\n");
	printf("------------------------------------------------\n");
	printf("   -h [HELP]           : this message		\n");
	printf("   -n [file name]      : [NSIH] file name	\n");
	printf("   -i [file name]      : [INPUT]file name 	\n");
	printf("   -l [load address]   : image load address	\n");
	printf("   	-> Default Load	  Address : 0xFFFF0000	\n");			
	printf("   -s [launch address] : image jump address	\n");
	printf("   	-> Default Launch Address : 0xFFFF0000	\n");
	printf("------------------------------------------------\n");
	printf("\n");
}

