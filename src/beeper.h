/* URMM1.2a */

#ifndef __BEEPER_H__
#define __BEEPER_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif

extern void beepOn(u_short freq);
extern void beepOff(void);
extern void beep(u_short freq, u_short msec);

#endif
