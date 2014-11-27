
#include <mmlib.h>
#include <nstdio.h>
#include <eeprom.h>
#pragma ccb(0xEC)    /* only here */


void nop(void){
}

static const char twiddl[] = "\\|/-";

extern void cli(void), initEEPROM(void), rcv_allow(void), sliploop(void), initNet(void);
extern vu_short sp_flags, sp_index;
extern vu_char packet_in[2048];


void main(void){
	int i=0;
	char buf[64];
	
	disable();
	initHardware();
	enable();

	/* ... */

	puts("Booting...\n");

	initEEPROM();
	initNet();
	
	while(0){
		i++;
		*IO_PPI_0 = i;
		ioport1 = i;
		putchar(i);
	}

	while(0){
		puts("punting to monitor\n");
		cli();
	}

	puts("Hit ESC to enter setup ");
	for(i = 0; i < 10; i++){
		putchar( '9' - i );
		putchar( '\b' );
		msleep( 500 );
		if( fstatus(serial_port) & FST_I ){
			if( fgetc(serial_port) == 0x1B )
				break;
		}
	}
	
	putchar(' ');
	putchar('\n');

	if( i != 10 || eeprom.diag )
		/* esc was hit */
		cli();

	sliploop();
	
	
}

