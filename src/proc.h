
#ifndef _proc_h_
#define _proc_h_

#define NPROC 		7

struct Proc {
	/* per process context block */

	FILE *stdin;
	FILE *stdout;
	
	u_short	sp;		/* saved stack pointer */
	u_char	flags;		/* process state */
#define PRF_RUNNABLE	0	
#define PRF_ACTIVE	1	/* never set (anachr) */
#define PRF_BLOCKED	2
#define PRF_DEAD	4
#define PRF_SUSPENDED	8	

	u_char	wchan;		/* what are we waiting on */
#define PRW_TIME	1
#define PRW_ADCDON	2
#define PRW_SERIAL	3
#define	PRW_ADCCHN	4
#define PRW_STDIO_I	5	
#define PRW_STDIO_O	6

	union {			/* params for waiting */
		u_long	sleeptill;
		u_short ad_result;
		u_short ad_chan;
		u_char	doneflag;
		FILE    *file;
	} wparams;
};

extern register struct Proc proc[NPROC];
extern register long timeremain;
extern register int currproc;

/* these correspond to the order used in proc.c */
#define MAINT_PROC	0
#define SENSOR_PROC	1
#define PID_PROC	2
#define BARGRAPH_PROC	3
#define MUSIC_PROC	4
#define USER_PROC	5
#define GAURD_PROC	6

extern void yield(void);
extern void usleep(u_long);
extern void main(void);
extern void block_for(u_char);
extern void block_for_adc_done(u_short);
extern void user_proc(void);
extern void gaurdian_proc(void);
extern void start_adc(u_short);
extern void gaurd_proc(void);

extern void sigsuspend(int);
extern void sigcont(int);
extern void sigkill(int);

#define block_for_serial() block_for(PRW_SERIAL)
#define block_for_adc_free() block_for(PRW_ADCDON)


#endif /* !_proc_h_ */

