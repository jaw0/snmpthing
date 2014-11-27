
#include <mmlib.h>
#include <proc.h>
#include <nstdio.h>

#pragma optimize(3)
#pragma interrupt(yield=TOV_INT)
#pragma rc

#define SP	(*(volatile u_short*)0x18)
typedef void (*PtrVoidFunc)();

/* we keep lots-o-stuff in registers for speed */
register int currproc;
register static int nextproc, sched_index, ready_index;
register long timeremain;
static u_char set_imask, set_imask1;

register struct Proc proc[NPROC];

/* we create lots of small arrays[NPROC] instead of one nice
   clean simple array of struct as it is faster and we need to 
   to keep the context switch as quick as possible
*/
static const PtrVoidFunc entry[NPROC] = {
	/* if the order of these are changed also change it in proc.h */
	0,		  	/* - maint proc, no entry needed */
	sensor_proc,		/* - reading sensors */
	pid_proc,		/* - moving */
	bargraph_proc,		/* - blinking */
	music_proc,		/* - singing */
	user_proc,		/* - user program -- maze solving ... */
	gaurd_proc
};

static const char timeslice[NPROC] = {
	/* the time slice for each proc */
	/* in timer ticks (~200us) */
	0,		/* maint proc cannot be pre-empted */ 
	20, 	/* sensor proc */
	20,	 	/* pid proc */
	4, 		/* bargraph proc */
	4, 		/* music proc */
	10,		/* user proc */
	4,		/* gaurd proc */
};
static const int priority[NPROC] = {
	0,
	0,
	1,
	4,
	4,
	3,
	4
};	

/* the ready list */
static register int ready_list[NPROC];
static int curr_prr_cnt[NPROC];

#define STACK_SIZE_0	0	/* already has a stack */
#define STACK_SIZE_1	128
#define STACK_SIZE_2	128
#define STACK_SIZE_3	128
#define STACK_SIZE_4	128
#define STACK_SIZE_5	512
#define STACK_SIZE_6	128
#define STACK_SIZE_7	0
#define STACK_SIZE_8	0

static u_char stack_space[
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2 + 
	STACK_SIZE_3 + STACK_SIZE_4 + STACK_SIZE_5 + 
	STACK_SIZE_6 + STACK_SIZE_7 + STACK_SIZE_8
];

static const int stack_offset[NPROC] = {
	STACK_SIZE_0,
	STACK_SIZE_0 + STACK_SIZE_1,
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2,
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2 + STACK_SIZE_3,
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2 + STACK_SIZE_3 + STACK_SIZE_4,
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2 + STACK_SIZE_3 + STACK_SIZE_4 + STACK_SIZE_5,
	STACK_SIZE_0 + STACK_SIZE_1 + STACK_SIZE_2 + STACK_SIZE_3 + STACK_SIZE_4 + STACK_SIZE_5 + STACK_SIZE_6

};

/* these are the initial sp for the process */
static u_short stack_initial[NPROC];

void main(void){
	/* this will become proc 0
	   and will keep the original stack
	*/
	volatile int i;
	
	currproc = -1;	/* flag indicating single user mode */

	/* initialize proc */
	for(i=0; i<NPROC; i++){
		proc[i].sp = 0;
		proc[i].flags = 0;
		proc[i].wchan = 0;
		stack_initial[i]  = (u_short)stack_space + stack_offset[i] - 2;
		curr_prr_cnt[i] = priority[i];
	}
	
	initHardware();
	imask |= Bit(TOV_INT);
		
	set_imask  = imask;
	set_imask1 = imask1;

	puts("\r\nMouseOS Release 1.0.0 (URMM1.3A) #2 10 Feb 1994\r\n");
	beep(2000, 250);
	puts("Entering Multi User Mode now\r\n");
	msleep(100);
			
	/* enter multi user mode */
	/* from this point on, this proc must
	   *never* call anything that could block
	 */
	disable();
		ready_list[0] = MAINT_PROC;
		timeremain = timeslice[0];
		currproc = sched_index = 0;
	enable();
	/* now running multi user */
	
	/* perform maintainance */
	while(1){
		ready_index = 0;
		for(i=1; i<NPROC; i++){
						
			/* check process if sleeping */
			if( proc[i].flags & PRF_BLOCKED){

				switch (proc[i].wchan){

				  case PRW_CLOCK:
				        /* frere jacques, frere jacques
					   dormez-vous? dormez-vous?
					*/
					if( proc[i].wparams.sleeptill <= getTime()){
				                /* awaken it */
						disable();
						proc[i].wchan = 0;
						proc[i].flags &= ~PRF_BLOCKED;
						enable();
					}
					break;

				  case PRW_ADCCHN:
				        /* check if waiting for a adc conversion */	
					if( proc[i].wparams.doneflag == 0){
						int chan;
				                /* awaken and get result */
						disable();
						chan = proc[i].wparams.ad_chan & 7;
				                /* result should be passed back somewhere else */
						/* this could get over-written */
						proc[i].wparams.ad_result = results[ chan ];
						proc[i].wchan = 0;
						proc[i].flags &= ~PRF_BLOCKED;
						enable();
					}
					break;

				  case PRW_ADCDON:
				        /* check if waiting for adc free */
					if( ad_result_lo & Bit(ADR_STAT)){
				                /* awaken it */
						disable();
						proc[i].wchan = 0;
						proc[i].flags &= ~PRF_BLOCKED;
						enable();
					}
					break;

				  case PRW_STDIO_I:
				  case PRW_STDIO_O:
					int s = (proc[i].wchan == PRW_STDIO_I) ? FST_I : FST_O;

					if( fstatus(proc[i].wparams.file) & s ){
						disable();
						proc[i].wchan = 0;
						proc[i].flags &= ~PRF_BLOCKED;
						enable();
					}
					break;

				  default:
				        /* XXX should not happen */
					proc[i].wchan = 0;
					break;
				}
			}
			if(proc[i].flags == PRF_RUNNABLE){
				/* if it is runnable add it to the ready list */
				if(!curr_prr_cnt[i]--){
					ready_list[ ready_index++ ] = i;
					curr_prr_cnt[i] = priority[i];
				}
			}
		} /* eo for */
		
		/* add self to end of ready list */
		ready_list[ ready_index ] = MAINT_PROC;
		sched_index = 0;

		/* yield the remainder of our slice[sic] */
		disable();
			yield();
		enable();
		
	}
}


void yield(){
	int i;
	/* context switch */
	/* declared as an interupt routine (TOV_INT)
	   this allows us to schedule it to occurr
	   asyncronously at the completion of the timer int
	   (as it is a lower priority interrupt)
	   it also causes all of the requisite registers
	   to be saved on the stack
	*/
	/* ought not use any local (on stack) variables
	   as their value will suddenly change halfway thru'
	   -- but we need to declare at least one
	   so as to cause the frame pointer to be pushed
	*/	
	/* interupts ought be off prior to calling this */
	/* you probobly don't want to call this directly */

	
	ioport1 |= 0x2;	/* for measuring timing of context switch */
	/* it times in at ~89 us */
	
	/* get the next runnable proc of the ready list */
	nextproc = ready_list[ sched_index++ ];

reyield:	
	/* set up its timeslice */
	timeremain = timeslice[nextproc];

	/* context switch -- don't blink */
	proc[ currproc ].sp = SP;
	currproc = nextproc;
	
	if( proc[ currproc ].sp ){
		SP = proc[ currproc ].sp;
	}else{
		/* proc has never run before
		   need to do some setup work 
		*/
		
		/* set up stack */
		SP = stack_initial[ currproc ];

		/* launch process */
		/* make sure ints are setup as they were 
		   (magicly) cleared when we entered yield()
		*/
		imask = set_imask; imask1 = set_imask1;
		enable();
		entry[ currproc ]();
		
		/* should never get here - only if process should
		   somehow return, in which case we kill it
		*/
		disable();
		proc[ currproc ].flags = PRF_DEAD;
		nextproc = MAINT_PROC;
		goto reyield;
	}
	ioport1 &= ~0x2;
}
	
