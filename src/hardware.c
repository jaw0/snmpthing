
/*
  $Id: hardware.c,v 1.5 1997/10/08 00:00:09 jaw Exp jaw $

*/  

#include <mmlib.h>

#pragma optimize(1)
#pragma registers(0)

void initPPI(void), romMonitor(void), initTimer(void), initEEPROM(void);

u_char p1_image, p2_image;

void initHardware(void){
	/* initialize everything
	   this ought be called as the
	   first thing in main()
	*/
	volatile int i;

	p1_image = p2_image = 0xFF;    /* XXX */
	
	imask = Bit(INT_SIO) | Bit(INT_TIMER);
	ipend = 0;
#ifdef _80196_	
	imask1 = 0;
	ipend1 = 0;
#endif	
	ioc0 = 0;
	ioc1 = Bit(IOC1_TXD) ;

#ifdef _80196_	
	ioc2 = Bit(IOC2_PWM_SLOW);
#endif
	
	do {
		/* clear HSI FIFO */
		i = hsi_time;
		i++; /* nop */
		i--;
	} while( ios1&(u_char)(1<<IOS1_HSI_REG));

	initPPI();		/* order of init *is* important */
	initTimer();
	/* initEEPROM(); */
	initSerial();
	
}

void initPPI(void){

	*IO_PPI_C = (u_char)0x88;
}


void hsout(int b, int v){
	u_short x;

	if( v ) v = 1;
	
	x = 1 | (v?32:0) ;
	hso_command = x;
	hso_time = timer1 + 2;

	while( v && !BitTest(ios0, b) || !v && BitTest(ios0, b))
		;

	return;
}

u_long splx(u_long n){
	u_long m;

#ifdef _80196_	
	m = imask | (imask1 << 16);
#else
	m = imask;
#endif
	
	imask = n & 0xFFFF;
#ifdef _80196_
	imask1 = n >> 16;
#endif

	return m;
}

