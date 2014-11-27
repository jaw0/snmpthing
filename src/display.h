/* URMM1.2a */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif

#define display(val)	*IO_DISP = ((u_char)(val))

#endif
