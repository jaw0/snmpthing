/* URMM1.2a */

#pragma optimize(1)
#pragma registers(0)

#include <mmlib.h>

void beepOn(u_short freq){
	u_char i0=imask, i1=imask1;
	
	if ( freq == 0 )
		beepOff();
	else {
		freq = (u_short)(((long)MHZ*1000000L/2L)/(long)(freq));
		imask = 0; imask1=0;
		*IO_BEEP_CTL |= (u_char)(1<<IOB_BEEP);	
		*IO_BEEP_FREQ = (u_char) (freq & 0xff);
		*IO_BEEP_FREQ = (u_char) (freq >> 8);
		imask = i0; imask1 = i1;
	}
}

void beepOff(void){

	*IO_BEEP_CTL &= ~(u_char)(1<<IOB_BEEP);
}

void beep(u_short freq, u_short msec){
	
	beepOn(freq);
	msleep(msec);
	beepOff();
}
