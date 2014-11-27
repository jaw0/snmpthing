
#ifndef __TIMER_H__
#define __TIMER_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif
#ifndef __HARDWARE_H__
#include <hardware.h>
#endif

/* how often we want an interrupt */
#define USEC_TIMER_INTERVAL 200

/* adjustment to time interval due to
   timer interrupt routine overhead
   -- empirically derived from 'oscope
*/
#define USEC_TIMER_OVERHEAD	 50

#define getTime() (time)

extern u_long time;

extern void initTimer(void);

#endif

