/* URMM1.2a */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif
#ifndef __HARDWARE_H__
#include <hardware.h>
#endif

/* useful ascii chars */
#define NUL		0
#define EOT		4
#define ACK		6
#define ESC		27
#define XON		17
#define XOFF		19

#define SLIP_END	192
#define SLIP_ESC	219
#define SLIP_ESC_END	220
#define SLIP_ESC_ESC	221

extern vu_char packet_in[];
extern vu_short sp_index;
extern vu_short sp_flags;
#define SPF_SLIP_MODE	0	/* 1 = SLIP, 0 = CLI */
#define SPF_SLIP_GOT_ESC 1
#define SPF_SLIP_DONE	2
#define SPF_GOT_CHAR	3


#define QUEUE_SIZE	16



extern void noop(void);
extern void initSerial(void);
extern void setBaudRate(long);
extern void rcv_disallow(void);
extern void rcv_allow(void);


#endif
