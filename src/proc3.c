
#include <mmlib.h>
#include <proc.h>

#pragma registers(0)
#pragma rc
#pragma optimize(3)

extern int usedby[8];

void usleep(u_long usec){
	/* sleep for usec microsecs */
	/* you should use this liberally thoughout
	   your code, usleep(0) can be used to forfiet
	   the remainder of your timeslice
	*/
	u_char i0=imask, i1=imask1;

	if(currproc < 0){
		/* we are in single user mode
		   must do a special sleep
		*/
		usec += getTime();
		while( getTime() < usec );
		return;
	}

	imask = 0; imask1 = 0;
	proc[currproc].wparams.sleeptill = getTime() + usec;
	proc[currproc].wchan = PRW_SLEEP;
	proc[currproc].flags |= PRF_BLOCKED;

	yield();
	imask = i0; imask1 = i1;
}

void block_for(u_char what){
	/* block waiting for event to arrive */
	/* you probobly don't want to call this directly
	   -- use block_for_serial() and block_for_adc()
	*/
	u_char i0=imask, i1=imask1;

	if( currproc <= 0 ){
		/* can't block in single user mode */
		return;
	}

	imask = 0; imask1 = 0;
	proc[currproc].wchan = what;
	proc[currproc].flags  |= PRF_BLOCKED;

	yield();
	imask = i0; imask1 = i1;
}	

void block_for_adc_done(u_short chann){
	/* block until the correct adc channel is done */
	u_char i0=imask, i1=imask1;

	imask = 0; imask1 = 0;
	proc[currproc].wparams.ad_chan = chann | Bit(ADR_STAT);	
	display(0x13);
	block_for(PRW_ADCCHN);
	display(0x14);
				
	imask=i0; imask1=i1;
}

void start_adc(u_short cmd){
	u_char i0=imask, i1=imask1;

	imask = 0; imask1 = 0;
	display(0x11);
	if(ad_result_lo & Bit(ADR_STAT))
		block_for_adc_free();
	
	display(0x12);
	puts("StartingADC\r\n");
	
	ad_command = cmd;
	proc[currproc].wparams.doneflag = 1;	
	usedby[ cmd & 7 ] = currproc;
	
	imask=i0; imask1=i1;
}



