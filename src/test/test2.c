
#include <mmlib.h>

#pragma ccb(0xEC)    /* only here */


void main(void){
	int x, y;

	x = 0;

	*IO_PPI_C = 0x88;
	
	while(1){

		ioport1 = x;
		ioport2 = x;
		*IO_PPI_0 = x;
		*IO_PPI_1 = x;
		*IO_PPI_2 = x;
		
		x++;
	}
}

