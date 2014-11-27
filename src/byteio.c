
/*
	$Id: byteio.c,v 1.2 1997/10/08 00:00:14 jaw Exp jaw $

*/

#include <mmlib.h>
#include <nstdio.h>
#pragma registers(0)
#pragma rc


void putByte( u_char byte ){
        /* outputs a one bye value in text format
           like a printf( "%02.2x") */
	
	printf("%02.2x", byte);
}


void putWord( u_short word ){
        /* outputs a 2byte word in text format,
           like a printf("%04.4lx") */
        char buf[16];
	
	printf("%04.4lx", word);
}

u_char getByte(void){
	/* read	2 chars, and return a byte value
	   the inverse of putByte()   */
 
	u_char ch, hc;
 
	ch = (u_char)getchar();		/* msn */
	ch = (ch & 0x7F) - 0x30;
	if(ch >= 0xA) ch -= 7;
	ch = (ch & 0xF)<<4;
 
	hc = (u_char)getchar();		/* lsn */
	hc = (hc & 0x7F) - 0x30;
	if(hc >= 0xA) hc -= 7;
	hc &= 0xF;
 
	return (ch | hc);
}
 
 
u_short	getWord(void){
	/* read	in 4 chars, returns a 2	byte word
	   inverse of putWord()	  */
 
	u_short	w;
 
	w = (u_short)getByte()<<8;	/* msB */
	w |= (u_short)getByte() & 0xFF;	/* lsB */
 
	return w;
}

