
#include <mmlib.h>

#pragma ccb(0xEC)    /* only here */
#pragma interrupt (serialInterrupt=INT_SIO)

#define MHZ 	2
#define BAUD_RATE       9600
#define QUEUE_SIZE	16

vu_char spstat_image;
vu_char sp_qi_len;
vu_char sp_qi_stat;
vu_char sp_qi_head, sp_qi_tail;
vu_char sp_queuei[QUEUE_SIZE];


void nop(void){
}

void putchar( char ch ){
        /* outputs a char over the serial line */
	int i;
	
        while( !(spstat_image&Bit(SP_TI)) ){
                nop();
        }
	for(i=0; i<1000; i++){
		nop();
	}
        spstat_image &= ~Bit(SP_TI);
        sbuf = ch;
}
void puts( char *mesg ){
 
        for( ; *mesg; mesg++) putchar( *mesg );
}

void initSerial(void){
	/* initialize the serial port */
 
	imask &= ~ B(INT_SIO);
	ipend &= ~ B(INT_SIO);

	setBaudRate( BAUD_RATE );

	sp_con = Bit(SP_M0) | Bit(SP_REN);
	r0 = sp_stat;
	sbuf = 0;
	r0 = sbuf;
	
	msleep(800);		/* wait	for UART to init */
	
	spstat_image = B(SP_TI);
	sp_index = 0;
	sp_flags = SPF_MODE;    /* start in CLI mode */
	imask |= B(INT_SIO);
	sp_qi_len = 0;
	sp_qi_stat = XON;
	sp_qi_head = sp_qi_tail = sp_qi_len = 0;

}
 

void initHardware(void){
	/* initialize everything
	   this ought be called as the
	   first thing in main()
	*/
	volatile int i;

	imask = 0;
	ipend = 0;
	imask = Bit(INT_SIO) | Bit(INT_TIMER);

#ifdef _80196_	
	imask1 = 0;
	ipend1 = 0;
#endif	
	ioc0 = 0;
	ioc1 = Bit(IOC1_TXD) | Bit(IOC1_PWM);

#ifdef _80196_	
	ioc2 = Bit(IOC2_PWM_SLOW);
#endif
	
	do {
		/* clear HSI FIFO */
		i = hsi_time;
		i++; /* nop */
		i--;
	} while( ios1&(u_char)(1<<IOS1_HSI_REG));

	initSerial();
}


void main(void){
	int i, x = 0;

	disable();

	initHardware();

	enable();

	while(1){
		ioport1 = x;
		/* ioport2 = x; */
		putchar( x );
		nop();
		x++;
	}

}

void serialInterrupt(void){
        /* entry point for serial interupt (not {ri,ti} interupt)   */
        /* so as to maintain bckwrd compatibility w/ 8096 version  */
 
        char ch;
 
        spstat_image |= sp_stat;
        if(spstat_image&Bit(SP_TI)){
                /* it is an xmit int */
  
        }
        /* don't use else -- could be both or neither */
        if(spstat_image&Bit(SP_RI)){
                /* it is a rcv int */
                ch = sbuf;
                spstat_image &= ~Bit(SP_RI);
                if(sp_qi_len < (QUEUE_SIZE-1)){
                        /* add it to Q */
                        sp_queuei[sp_qi_head++] = ch;
                        sp_qi_head %= QUEUE_SIZE;
                        if(++sp_qi_len >= (QUEUE_SIZE/2)){
                                /* almost full -- turn XOFF */
                                sp_qi_stat = (u_char)XOFF;
#                               ifndef NO_DO_XONXOFF
                                        sbuf = (u_char)XOFF;
#                               endif
                        }
                }
        }
}
