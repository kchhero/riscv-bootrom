#ifndef __NX_LIB_H__
#define __NX_LIB_H__

void *nx_memcpy(void *dest, const void *src, int n);
void *nx_memset(void *str, int c, int n);
int nx_memcmp(const void* s1, const void* s2, int n);

int is_gpt_valid(unsigned char *psector);
unsigned int get_first_lba(unsigned char *psector);
unsigned int get_first_empty_lba(unsigned char *psector);
unsigned int is_gpt_part(unsigned char *mbr);

#endif /* __NX_LIB_H__ */
