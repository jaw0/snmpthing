

#ifndef _BUFFERS_H_
#define _BUFFERS_H_

typedef struct {
	u_short size;
	char *data;
} Buf;

#define btod(m,t)       ((t)((m)->data))

#define BUF_SMALL	0
#define BUF_MED		1
#define BUF_BIG		2


#endif /* _BUFFERS_H_ */

