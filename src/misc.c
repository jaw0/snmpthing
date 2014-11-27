
/*
  $Id: misc.c,v 1.3 1997/10/08 00:00:07 jaw Exp jaw $

  Copyright (c) 1997 OpNet Inc.
*/


#ifndef TESTING
#include <mmlib.h>
#include <nstdio.h>

#pragma registers(0)
#pragma optimize(3)
#else
#include "aliases.h"
#include <stdio.h>
#endif

int strncmp(const char *s1, const char *s2, int n){

	while( *s1 && n-- ){
		if(!*s2) return 1;
		if( *s1++ != *s2++) return 1;
	}
	return *s2==0 ? 0 : 1;
}

int strcmp(const char *s1, const char *s2){

	while( *s1 ){
		if(!*s2) return 1;
		if( *s1++ != *s2++) return 1;
	}
	return *s2==0 ? 0 : 1;
}

int strlen(const char *s){
	int l = 0;

	while( *s++ ) l++;
	return l;
}

void bcopy(const char *s, char *d, int l){

	while(l--){
		*d++ = *s++;
	}
}

void bzero(char *d, int l){

	while(l--){
		*d++ = 0;
	}
}

#if 0
int strcpy(char *d, const char *s){
	int l=0;

	while( *s ){
		*d++ = *s++;
		l++;
	}
	return l;
}
#endif

/* convert ip address from ascii dotted quad to binary in network order */
u_long inet_aton(const char *s){
	u_long a;
	u_char *p;
	int i;

	a = 0;
	p = (u_char *)&a;

	for(i=0; i<4; i++){
		while( *s && *s<='9' && *s>='0' ){
			p[i] *= 10;
			p[i] += *s - '0';
			s++;
		}
		if( i!=3 && *s != '.' )
			/* error */
			return 0xFFFFFFFF;
		s++;
	}

	return a;
}

/* just use printf("%I") */
#if 0
/* convert ip address from binary in network order to ascii dotted quad */
char *inet_ntoa(u_long a, char *s, int len){
	u_char *p;

	p = (u_char*)&a;
#ifdef TESTING
	printf("%d.%d.%d.%d",
		 p[0], p[1], p[2], p[3]);
#else	
	snprintf(s, len, "%u.%u.%u.%u",
		 p[0], p[1], p[2], p[3]);
#endif	
	return s;
}
#endif

#ifdef TESTING
void main(void){
	char buf[64];
	u_long a;

	a = inet_aton("206.84.208.200");
	
	inet_ntoa(a, buf, sizeof buf);

}

#endif

	

