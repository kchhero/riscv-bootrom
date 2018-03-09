#ifndef __CRYPTO_H__
#define __CRYPTO_H__

void getkeyhash(unsigned int *phash);
void getimagehash(unsigned int *phash);
int headercheck(unsigned int iv[], int dec);
int keycheck(unsigned int iv[], int dec, int verify);
void imagedecrypt(U32 iv[]);

#endif /* __CRYPTO_H__ */
