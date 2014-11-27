
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


void main(void){
	int i, x = 0;
	
	disable();
        ipend = 0;
        ioc0 = 0;
        ioc1 = Bit(IOC1_TXD) | Bit(IOC1_PWM);
	imask = Bit(INT_SIO) | Bit(INT_TIMER);
	
	spstat_image = 0;

	do {
                /* clear HSI FIFO */
                i = hsi_time;
                i++; /* nop */
                i--;
        } while( ios1&(u_char)(1<<IOS1_HSI_REG));


	/*
	baud_rate = (u_char)(BAUD_RATE_VAL & 0xFF);
        baud_rate = (u_char)(0x80 | (BAUD_RATE_VAL >> 8));
	*/
#if 1
	baud_rate = 0x0C;  /* 8MHz - 9600bps */
	baud_rate = 0x80;
#else
	baud_rate = 0xFF;
	baud_rate = 0xFF;
#endif	
	sp_con = 9;
	r0 = sp_stat;
	sbuf = 0;
	r0 = sbuf;
	spstat_image = Bit(SP_TI);
	sp_qi_len = 0;
	sp_qi_stat = XON;
	sp_qi_head = sp_qi_tail = sp_qi_len = 0;

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
