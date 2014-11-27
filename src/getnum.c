

/*
  $Id: getnum.c,v 1.4 1997/10/08 00:00:08 jaw Exp jaw $
*/

#include <mmlib.h>
#include <nstdio.h>

#pragma registers(0)

static unsigned char vallof(char c){

        if(c>='0' && c<='9') return c - '0';
        if(c>='a' && c<='z') return c - 'a' + 0xa;
        if(c>='A' && c<='Z') return c - 'A' + 0xA;

	return 255;
}

static int islegit(int base, char c){

	if( vallof(c) < base ) return 1;
	return 0;
}

long atoi(char *a){
	return getnumat(10, a);
}

long getnum(int base){
	char buf[64];
	long foo;

	getline(buf, 64, 0);
	foo = getnumat(base, buf);
	return foo;
}

long getnumat(int base, char *buf){
	long val=0L;
	int i=0, neg=0;


	while( !islegit(base, buf[i])){
		if( buf[i]=='-') neg = 1;
		i++;
	}
	while( islegit(base, buf[i]))
		val = (val * (long)base) + (long)vallof(buf[i++]);

	buf[i-1] = 0;
	return neg?-val:val;
}
	
	
