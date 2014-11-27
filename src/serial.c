/*
	$Id: serial.c,v 1.4 1997/10/08 00:00:15 jaw Exp jaw $

	serial.c
		contains all code for serial port handling

*/

#include <mmlib.h>
#include <serial.h>
#include <netstat.h>
#include <nstdio.h>
#include <eeprom.h>

#pragma interrupt (serialInterrupt=INT_SIO)
#pragma optimize(1)
#pragma registers(2)
#pragma rc

void	noop(), initSerial(void);

int serial_putchar(FILE*, char);
int serial_getchar(FILE*);
int serial_noop(FILE*);
int serial_status(FILE*);

vu_char spstat_image;

/* our incoming packet buffer */
vu_char packet_in[2048];
vu_short sp_index;
vu_short sp_flags;

vu_char disallow_input;

vu_char sp_qi_len;
vu_char sp_qi_stat;
vu_char sp_qi_head, sp_qi_tail;
vu_char sp_queuei[QUEUE_SIZE];

#define B(x)	Bit(x)

#define BAUD_RATE       9600


FILE serial_port_file;
FILE *serial_port;

const struct io_fs serial_port_fs = {
	serial_putchar,
	serial_getchar,
	serial_noop,
	serial_noop,
	serial_status
};


void rcv_disallow(void){
	
	disallow_input = 1;
}
void rcv_allow(void){

	sp_index = 0;
	BitClear(sp_flags, SPF_SLIP_DONE);
	BitSet(sp_flags, SPF_SLIP_MODE);
	disallow_input = 0;
}


void noop(void){
}

void initSerial(void){
	
	spstat_image = 0;
	setBaudRate( BAUD_RATE );
	sp_con = 9;
	r0 = sp_stat;
	sbuf = 0;
	r0 = sbuf;
	
	sp_index = 0;
	sp_flags = 0;    /* start in CLI mode */

	spstat_image = Bit(SP_TI);
	sp_qi_len = 0;
	sp_qi_stat = XON;
	sp_qi_head = sp_qi_tail = sp_qi_len = 0;

	serial_port = &serial_port_file;
	finit( serial_port );
	serial_port->fs = &serial_port_fs;

	/* diag mode - stay at 9600 */
	if( ! eeprom.diag )
		setBaudRate( eeprom.baud );
}


void serialInterrupt(void){
        /* entry point for serial interupt (not {ri,ti} interupt)   */
        /* so as to maintain bckwrd compatibility w/ 8096 version  */
 
        u_char ch;
 
        spstat_image |= sp_stat;
        if(spstat_image&Bit(SP_TI)){
                /* it is an xmit int */
        }
        /* don't use else -- could be both or neither */
        if(spstat_image&Bit(SP_RI)){
                /* it is a rcv int */
                ch = sbuf;
                spstat_image &= ~Bit(SP_RI);
		netstat.inbytes ++;
		
		if( BitTest(sp_flags, SPF_SLIP_MODE)){
			/* SLIP mode */
			if( disallow_input )
				return;
			if( BitTest(sp_flags, SPF_SLIP_GOT_ESC) ){
				/* previous char was esc */
				BitClear(sp_flags, SPF_SLIP_GOT_ESC);
				switch (ch) {
				  case SLIP_ESC_ESC:
					ch = SLIP_ESC;
					break;
				  case SLIP_ESC_END:
					ch = SLIP_END;
					break;
				}
			}else{
				if( ch == SLIP_ESC ){
					/* got ESC, make note and leave */
					BitSet(sp_flags, SPF_SLIP_GOT_ESC);
					return;
				}
				if( ch == SLIP_END ){
					/* got END, make note and leave, disable further input */
					/* optimize common case of END at start of packet */
					if( sp_index < 10 ){
						sp_index = 0;
						return;
					}

					rcv_disallow();
					BitSet(sp_flags, SPF_SLIP_DONE);
					return;
				}
			}
			packet_in[sp_index++] = ch;
			if( sp_index > sizeof(packet_in)){
				/* let higher layer generate an error... */
				rcv_disallow();
				BitSet(sp_flags, SPF_SLIP_DONE);
				return;
			}
			
		}else{
			/* CLI mode */
		
			if(sp_qi_len < (QUEUE_SIZE-1)){
				/* add it to Q */
				sp_queuei[sp_qi_head++] = ch;
				sp_qi_head %= QUEUE_SIZE;
				if(++sp_qi_len >= (QUEUE_SIZE/2)){
                                        /* almost full -- turn XOFF */
					sp_qi_stat = (u_char)XOFF;
#                               	ifndef NO_DO_XONXOFF
                                        	sbuf = (u_char)XOFF;
#                               	endif
				}
			}
		} /* if cli mode */
        } /* if sp_ri */
}

void setBaudRate(long baud){
	u_short br;
	u_char i0=imask;

#ifdef _80196_
	br = (u_short)((((long)MHZ*1000000L)/(16L*baud)) -1);
#else
	br = (u_short)((((long)MHZ*1000000L)/(64L*baud)) -1);
#endif
	
	imask=0;
	baud_rate = (u_char)(br & 0xFF);
	baud_rate = (u_char)(0x80 | (br >> 8));
	imask=i0;
}


int serial_putchar(FILE *f, char ch){
	/* outputs a char over the serial line */
	int i;

	netstat.outbytes ++;

#ifdef NPROC
	if( !spstat_image&B(SP_TI))
		block_for_stdio(f, FST_O);
#else	
	while( !(spstat_image&B(SP_TI)) ){
		/* usleep(0); */
		noop();
	}
#endif	
	msleep( 10 );
	spstat_image &=	~B(SP_TI);
	sbuf = ch;
	return 1;
}

int serial_getchar(FILE *f){
	/* get a char from serial line
	   -- yes, it could have been defined to return	a char
	      but this keeps it	ANSI compliant!	*/
 
	char ch;
	u_char i0=imask;
	
	while( !sp_qi_len ){
#ifdef NPROC
		block_for_stdio(f, FST_I);
#else		
		noop();
#endif		
	}
	
	imask=0;
	ch = sp_queuei[sp_qi_tail++];
	sp_qi_tail %= QUEUE_SIZE;
	sp_qi_len--;
	if ((sp_qi_stat	== (u_char)XOFF) && (sp_qi_len < (QUEUE_SIZE/4))){
		/* turn	xon */
#		ifndef NO_DO_XONXOFF
			sbuf = (u_char)XON;
#		endif
	}
	imask=i0;
	return ch;
}

int serial_noop(FILE*f){
	return 1;
}

int serial_status(FILE*f){
	int s = 0;

	if( sp_qi_len ) s |= FST_I;
	if( spstat_image&B(SP_TI) ) s |= FST_O;
	return s;
}


