/* URMM1.2a */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#ifndef __ALIASES_H__
#include <aliases.h>
#endif

#pragma varparams(printf)
#pragma varparams(fprintf)
#pragma varparams(snprintf)

extern void printf(const char *fmt, ...);
extern void fprintf(FILE *f, const char *fmt, ...);
extern int snprintf(char *buffer, int len, const char *fmt, ...);

#endif




