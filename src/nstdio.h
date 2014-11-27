
/*
  $Id: nstdio.h,v 1.4 1997/10/08 00:00:13 jaw Exp jaw $
*/

#ifndef __NSTDIO_H__
#define __NSTDIO_H__
struct FILE;

struct io_fs {
	int  (*putc)(struct FILE*, char);
	int  (*getc)(struct FILE*);
	int  (*close)(struct FILE*);
	int  (*flush)(struct FILE*);
	int  (*status)(struct FILE*);
};


typedef struct FILE {
	int flags;
#define	F_UNGOT		1	 /* for ungetc */
#define	F_ONLCR		2	 /* map \n -> \r\n on output */
#define	F_IGNCR		3	 /* strip \r on input */
	
	char   ungotc;           /* for ungetc */
	char   prevout;          /* previous output char - for stateful translations */
	const  struct io_fs *fs; /* functions */
	void   *d;               /* type specific data */
	
} FILE;


extern int finit(FILE*);
extern int fputc(char, FILE*);
extern int fgetc(FILE*);
extern int ungetc(char, FILE*);
extern int fflush(FILE*);
extern int fstatus(FILE*);
#define FST_I	1		/* input is available */
#define FST_O	2		/* ok to send */

extern int putchar(char);
extern int getchar(void);

extern void puts(const char*);
extern void putByte(u_char);
extern void putWord(u_short);
extern u_char getByte(void);
extern u_short getWord(void);

extern char *getline(char*, int, int);

extern FILE* serial_port;

/* #include <proc.h> */
#ifdef NPROC
#  define STDIN		proc[currproc].stdin
#  define STDOUT	proc[currproc].stdout
#else
#  define STDIN		serial_port
#  define STDOUT	serial_port
#endif

#endif  /* __NSTDIO_H__ */

