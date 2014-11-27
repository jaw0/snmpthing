
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
#include <error.h>

#define H2NS(x)	hosttonetshort(x)

extern u_short hosttonetshort( u_short );
extern int putlen( u_char *, int, int);
extern int putint( u_char *, long, int, int);
extern int putstr( u_char *, char *, int, int, int);

/* our compiler is pathetic
   this is too large as a single function
*/

static int get_next_object_partB(u_char *curr, int len, int *i, SNMPObject *so){
	int foo, k;

	DEBUG1( "snmp gB len = %lu\n", len);
	/* get length */
	so->len = 0;
	foo = curr[(*i)++];
	if( foo & 0x80 ){
		k = foo & 0x7F;
		while( k-- ){
			foo = curr[(*i)++];
			so->len <<= 7;
			so->len |= foo;
			if( *i > len ){
				ERROR2(LOG_INET, LOG_ERR, "snmp gB2 %lx > %lx\n", *i, len);
				so->type = TAG_ERROR;
				return 0;
			}
		}
	}else{
		so->len = foo;
	}
	
	if( so->len + *i > len ){
		ERROR2(LOG_INET, LOG_ERR, "snmp gB3 %lx > %lx\n", so->len + *i, len);
		so->type = TAG_ERROR;
		return 0;
	}
	
	so->next = curr + so->len + *i;
	so->p  = curr + *i;
	so->tlen = so->len + *i;

	return 1;
}


/* get the next object -- ints are unencoded */
SNMPObject get_next_object( u_char *curr, int len ){
	SNMPObject so;
	int i = 0, j, k;
	int foo;

	DEBUG2( "snmp g p = %lx, len = %lu\n", curr, len);
	so.tlen = 0;
	so.type = curr[i++];
	/* we do not support multibyte type tags */
	if( (so.type & 0x1F) == 0x1F ){
		/* long type */
		ERROR1(LOG_INET, LOG_ERR, "snmp type %lx\n", so.type);
		so.type = TAG_ERROR;
		return so;
	}

	if( !get_next_object_partB(curr, len, &i, &so))
		return so;

	if( so.type == TAG_INTEGER ){
		/* needs to be byte swapped (we are a little indian) */

		for(j=so.len-1; j>=0; j--){
			so.i <<= 8;
			so.i |= (int)curr[i+j];
		}
	}
	return so;
}

