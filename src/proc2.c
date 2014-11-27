
#include <mmlib.h>
#include <proc.h>

#pragma registers(0)
#pragma optimize(3)
#pragma rc

/* we have signals -- just like a real OS! */
void sigsuspend(int pid){
	/* temporarilly suspend a process
	   (until it is restarted with a sigcont)
	*/
	u_char i0=imask, i1=imask1;
	
	imask = 0; imask1 = 0;
	if( pid > 0)
		proc[ pid ].flags |= PRF_SUSPENDED;
	imask = i0; imask1 = i1;
}

void sigcont(int pid){
	/* allow suspended proc to continue */
	u_char i0=imask, i1=imask1;
	
	imask = 0; imask1 = 0;
	proc[ pid ].flags &= ~PRF_SUSPENDED;
	imask = i0; imask1 = i1;
}

void sigkill(int pid){
	/* kill the process */
	u_char i0=imask, i1=imask1;
	
	imask = 0; imask1 = 0;
	if( pid > 0)
		proc[ pid ].flags |= PRF_DEAD;
	imask = i0; imask1 = i1;
}




