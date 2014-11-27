

#include <mmlib.h>

#pragma optimize(3)
#pragma interrupt(debug= 15 )
#pragma rc

#define SP	(*(u_short*)0x18)
#define FRP	(*(u_short*)0x30)


void debug(void){
	int i;
	char buf[32];
	u_short *sp;

	snprintf(buf, sizeof(buf), "SP=%lx FR=%lx\n", SP, FRP);
	puts(buf);
	
}


