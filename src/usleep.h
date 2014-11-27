/* URMM1.2a */

#ifndef __USLEEP_H__
#define __USLEEP_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif

#define msleep(msec) usleep(1000L * (u_long)(msec))

extern void usleep(u_long usec);

#endif
