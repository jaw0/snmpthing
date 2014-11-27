
#include <mmlib.h>
#include <timer.h>

#pragma optimize(3)
#pragma interrupt(timer_int=TIMER_INT)
#pragma rc

#define TIMER_INTERVAL ((MHZ*(USEC_TIMER_INTERVAL-USEC_TIMER_OVERHEAD))/16)

register u_long time;
register static u_short timeto;

void initTimer(void){
	/* initializiation for the timer */

	volatile int i;

	time = 0UL;
	while ( ios0 & (1<<IOS0_CAM) ){
		/* wait till CAM is not full */
		i++ ; /* nop */
		i--;
	}

	timeto = timer1;		/* this ought to provide enough delay */
	timeto += TIMER_INTERVAL;

	hso_command = Bit(HSOC_INT) | 0x8;
	hso_time = timeto;
}


void timer_int(void){
	/* timer interrupt entry point */
	/* this takes ~26 us */
	
	/* try timing a context switch on the scope */
	ioport1 |= 0x1;

#if 0
	/* since we are not using the CAM for anything else
	   it will always be empty, no need to check
	*/
	while ( ios0 & (1<<IOS0_CAM) ){
    	/* wait till CAM is not full */
		i++; /* nop */
		i--;
	}
#endif

	/* reload timer for the next interrupt */
	timeto = timer1 + TIMER_INTERVAL;       /* this ought be enough delay */
	hso_command = Bit(HSOC_INT) | 0x8;
	hso_time = timeto;
	time += (u_long) USEC_TIMER_INTERVAL;

	ioport1 &= ~0x1;

#ifdef MULITPROC	
	/* skip test if in single user mode or maint_proc */
	if( (currproc > 0) && !(--timeremain)){
		/* context switch to next proc */
		/* the following line is magic
		   it will cause a yeild interrupt to occurr when 
		   this routine (the timer int) returns
		   interrupts will automagically be disabled as 
		   a side effect (which they must be)
		*/
		ipend |= 1;	
	}
#endif
	
}

void usleep(u_long usec){
	
	usec += getTime();
	while( getTime() < usec )
		noop();
	return;
}

