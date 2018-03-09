#ifndef _XYZMODEM_H_
#define _XYZMODEM_H_

//#include <type.h>
#include <nx_type.h>
typedef int bool;

#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif

#define ZM_DEBUG_ENB		0

#define xyzModem_xmodem 	1
#define xyzModem_ymodem		2
/* Don't define this until the protocol support is in place */
/*#define xyzModem_zmodem 3 */

#define xyzModem_access		-1
#define xyzModem_noZmodem	-2
#define xyzModem_timeout	-3
#define xyzModem_eof		-4
#define xyzModem_cancel		-5
#define xyzModem_frame		-6
#define xyzModem_cksum		-7
#define xyzModem_sequence	-8

#define xyzModem_close		1
#define xyzModem_abort		2

#define CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT
#define CYGACC_CALL_IF_SET_CONSOLE_COMM(x)

#define diag_vprintf		vprintf
#define diag_printf		printf
#define diag_vsprintf		vsprintf


typedef struct {
	char *filename;
	int mode;
	int chan;
} connection_info_t;

typedef struct {
	int chan;
	unsigned char pkt[1024], *bufp;
	unsigned char blk, cblk, crc1, crc2;
	unsigned char next_blk; /* Expected block */
	int len, mode, total_retries;
	int total_SOH, total_STX, total_CAN;
	bool crc_mode, at_eof, tx_ack;
	unsigned long file_length, read_length;
} xyzsts;

 int xyzModem_getc(int port);
 int xyzModem_stream_open(connection_info_t *info, xyzsts *xyz, int *err);
void xyzModem_stream_close(int *err);
void xyzModem_stream_terminate(xyzsts *xyz, bool abort, int (*getc)(int));
 int xyzModem_stream_read(xyzsts *xyz, char *buf, int size, int *err);
 int xyzModem_error(int err);

#endif /* _XYZMODEM_H_ */
