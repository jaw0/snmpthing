
/*
  $Id: snmp.c,v 1.5 1997/10/08 00:00:08 jaw Exp jaw $

  Copyright (c) 1997 OpNet Inc.
*/

#ifndef TESTING
#pragma optimize(3)
#pragma registers(4)
#pragma rc
#include <mmlib.h>
#else
#include <aliases.h>
#endif

#include <ip.h>
#include <snmp.h>
#include <mib.h>

#define H2NS(x)	hosttonetshort(x)


#ifdef TESTING
#	define dprintf(x)	printf x
#else
#	define dprintf(x)
#endif


/* insert length */
int putlen( u_char *ebuf, int rlen, int max ){
	int j = 0;

	if( max < 2 )
		return 0;
	if( rlen < 128 ){
		ebuf[-j++] = rlen;
	}else{
		/* we will never send anything longer than will fit in 14bits */
		ebuf[-j++] = rlen & 0x7F;
		ebuf[-j++] = rlen >> 7;
	}
	return j;
}

/* insert int + len + tag */
int putint( u_char *ebuf, long val, int tag, int max ){
	int j = 0;
	
	do {
		ebuf[-j++] = val & 0xFF;
		val >>= 8;
		if( j >= max ){
			return j;
		}
	}while( val );

	j += putlen(ebuf-j, j, max-j);
	ebuf[-j++] = tag;
	return j;
}

/* insert string + len */
int putstr( u_char *ebuf, char *str, int len, int tag, int max ){
	int j;

	for(j=0; j<len; j++)
		ebuf[-j] = str[len-j-1];

	j += putlen(ebuf-j, j, max-j);
	ebuf[-j++] = tag;
	return j;
}
	


#ifdef TESTING

u_char pkt[] = {
	0x45, 0x00, 0x00, 0x4d, 0xb3, 0x1b, 0x00, 0x00, 0x3c, 0x11, 0x8e, 0xd0, 0xce, 0x54, 0xd0, 0x04,
	0xce, 0x54, 0xd0, 0x06, 0x09, 0x1f, 0x00, 0xa1, 0x00, 0x39, 0x4a, 0xbf,
	/* start of snmp data */                                                0x30, 0x82, 0x00, 0x2d,
	0x02, 0x01, 0x00, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0xa0, 0x20, 0x02, 0x04, 0x2e,
	0x23, 0xe6, 0x73, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x82, 0x00, 0x10, 0x30, 0x82, 0x00,
	0x0c, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x05, 0x00, 0x05, 0x00
};


main(){

	snmp_input( pkt, sizeof pkt );

}

#endif

