

#ifndef _ERROR_H_
#define _ERROR_H_

#define ERROR(section, level, message)		loggit(__LINE__, __FILE__, level, message)
#define ERROR1(section, level, message, a)	loggit(__LINE__, __FILE__, level, message, a)
#define ERROR2(section, level, message, a, b)	loggit(__LINE__, __FILE__, level, message, a, b)
#define ERROR3(section, level, message, a,b,c)	loggit(__LINE__, __FILE__, level, message, a, b, c)
#define DEBUG(message)				loggit(__LINE__, __FILE__, LOG_DEBUG, message)
#define DEBUG1(message, a)			loggit(__LINE__, __FILE__, LOG_DEBUG, message, a)
#define DEBUG2(message, a, b)			loggit(__LINE__, __FILE__, LOG_DEBUG, message, a, b)
#define DEBUG3(message, a, b, c)		loggit(__LINE__, __FILE__, LOG_DEBUG, message, a, b, c)

/*
 *  Priorities (these are ordered)
 */
#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but signification condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */

#pragma varparams(loggit)

extern void loggit(int, const char*, int, const char*, ...);

#endif /* _ERROR_H_ */

