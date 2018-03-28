/*------------------------------------------------------------------------------
 *
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module          : nand error correction test nand device model
 * File            : hexgen.c
 * Description     :
 * Author          : Hans
 * History         : 2017.12.26 first implementation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
	int i, fsize;
	char *ifn = NULL;

	if (argc <= 1) {
		printf("no image file name\n");
		printf("usage:\n hexgen -i <filename>\n");
		return -1;
	}

	ifn = argv[1];

	FILE *f;
	f = fopen(ifn, "rb");
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	printf("%s size is %d\n", ifn, fsize);

	unsigned char *pcdata = malloc(fsize);
	unsigned char *wfile = malloc(fsize * 3);

	fread(pcdata, 1, fsize, f);
	fclose(f);
	for (i = 0; i < fsize; i++)
		sprintf(&wfile[i * 3], "%02X\n", pcdata[i]);

	char ofn[0x100];
	memset(ofn, 0, 0x100);
	sprintf(ofn, "%s.hex", ifn);
	f = fopen(ofn, "wb");
	fwrite(wfile, 1, fsize * 3, f);
	fclose(f);

	printf("nand hex image(%s) generation done\n", ofn);

	return 0;
}
