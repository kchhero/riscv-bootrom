// See LICENSE for license details.
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <nx_qemu_sim_printf.h>

#ifdef SOC_SIM
void kputc(char c)
{
    volatile unsigned char *reg = (unsigned char*)PHY_BASEADDR_DUMMY_MODULE;
    *reg = (unsigned char)c;
}
void _kputs(const char *s)
#else
static inline void _kputs(const char *s)
#endif
{
    char c;
    unsigned int count = 0;
    for (; (c = *s) != '\0'; s++) {
	kputc(c);
        count++;
        if (count > MAX_MESSAGE_LEN) {
            return ;
        }
    }
}

#ifndef SOC_SIM
void kputs(const char *s)
{
  //	_kputs(s);
       	char c;
	for (; (c = *s) != '\0'; s++)
		kputc(c);

	kputc('\r');
	kputc('\n');
}
#endif

void _dprintf(const char *fmt, ...)
{
	va_list vl;
	int is_format, is_long, is_char;
	char c;

	va_start(vl, fmt);
	is_format = 0;
	is_long = 0;
	is_char = 0;
	while ((c = *fmt++) != '\0') {
		if (is_format) {
			switch (c) {
			case 'l':
				is_long = 1;
				continue;
			case 'h':
				is_char = 1;
				continue;
			case 'x': {
				unsigned long n;
				long i;
				if (is_long) {
					n = va_arg(vl, unsigned long);
					i = (sizeof(unsigned long) << 3) - 4;
				} else {
					n = va_arg(vl, unsigned int);
					i = is_char ? 4 : (sizeof(unsigned int) << 3) - 4;
				}
				for (; i >= 0; i -= 4) {
					long d;
					d = (n >> i) & 0xF;
					kputc(d < 10 ? '0' + d : 'a' + d - 10);
				}
				break;
			}
			case 's':
				_kputs(va_arg(vl, const char *));
				break;
			case 'c':
				kputc(va_arg(vl, int));
				break;
			}
			is_format = 0;
			is_long = 0;
			is_char = 0;
		} else if (c == '%') {
			is_format = 1;
		} else {
			kputc(c);
		}
	}
	va_end(vl);
}
