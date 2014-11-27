
/*
  $Id: math.c,v 1.1 1997/08/06 21:17:52 jaw Exp $
*/

#include <mmlib.h>
#pragma registers(0)

static const u_char sindata[] = {
	0, 25, 50, 74, 98, 120, 142, 162, 180,
	197, 212, 225, 236, 244, 250, 254, 255,
	/* extra point eliminates need for bounds check in interp. code */
	254
};

short sin(short x){
	/* returns 255*sin( 2pi * x / 256 ) */
	int neg=0, p, v, adj;

	if (x<0){
		x = -x;
		neg = 1;
	}
	x %= 256;
	if (x>127){
		x &= 127;
		neg = !neg;
	}
	if (x>63){
		x = 64 - x;
	}

	p = x>>2;
	/* x is now in range of [0, 16] */
	
	adj = x%4;
	v = sindata[p];
	if (adj){
		/* interpolate */
		v += (sindata[p+1]-v) * adj / 4;
	}
	return neg?-v:v;
}



