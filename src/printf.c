
/*
  $Id: printf.c,v 1.4 1997/10/08 00:00:11 jaw Exp jaw $

  Copyright (c) 1997 OpNet Inc.
*/

#include <stdarg.h>
#include "aliases.h"
#include <eeprom.h>

#ifndef TESTING
#include <mmlib.h>

#pragma registers(0)
#pragma optimize(3)

#else
# define FILE void
  struct EEPROM eeprom;
#endif

/*
        a minimal implementation of printf
	%<flags><width><.prec><type>

	type::
		c	- char
		s	- string
		%	- %
		&	- magic zero-width character
		x	- lower case hex
		X	- UPPER case hex
		d,D	- decimal
		o	- octal
		p	- spin (prec times, delay of width)
		I	- IP addr as dotted quad
		=base	- in base
		=*	- base from arglist
	flags::
		0	- pad w/ leading 0 (! spaces)
		#	- use lower case
		l	- arg is 16bit (! 8bit)
		L	- arg is 32bit

*/


enum {
	PF_ZERO,	/* pad w/ leading zeros */
	PF_LONG,	/* arg is 16 bit, instead of 8 bit */
	PF_DLONG,	/* arg is 32 bit */
	PF_ALT,		/* use lowercase instead of UPPER */
	PF_SIGNED,	/* arg is signed */
	PF_SHOW_PLS,	/* arg is signed, show + if positive */
	PF_LEFT         /* left justify, not right */
};

#define B(x)		(1<<((u_short)(x)))
#define isdig(x)	(((x)>='0') && ((x)<='9'))
static const char Set_A[] =	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char Set_a[] =	"0123456789abcdefghijklmnopqrstuvwxyz";
static const char spinchar[] = "|\\-/";

int putnum(int (*)(void*, char), void *, long, int, long, long, u_short);

int vprintf(int (*ofnc)(void*, char), void *arg, const char *fmt, va_list ap){
	/* Warning:
		there is NO error checking on 
		width, precision, or base
		so do not pass in unreasonable values
		or bad things will happen
	*/

	const char *p = fmt;
	char *s;
	long val, width = 0, prec = 0;
	u_char* addr;
	u_long  addrl;
	u_short flags;
	int base;
	int pos = 0;
	
	while(*p){
		prec = width = flags = 0;
		base = 10;
		if(*p != '%'){
			(*ofnc)(arg, *p++);
			pos++;
		}else{
rflag:
			p++;
reswitch:
			switch(*p){
			  case 's':
				/* %s - string */
				s = va_arg(ap, char*);
				val = 0;
				while(*s){
					(*ofnc)(arg, *s++);
					pos ++;
					/* no more than prec chars out */
					/* width is meaningless */
					if(prec && (++val>prec)) break;
				}
				break;
			  case 'I':
				/* %I - IP addy */
				addrl = va_arg(ap, u_long);
				addr  = (u_char*)&addrl;

				for(val=0; val<4; val++){
					if( val ){
						(*ofnc)(arg, '.');
						pos++;
					}
					pos += putnum(ofnc, arg, addr[val], 10, width, prec, flags);
				}
				break;
				
			  case '&':		/* magic zero-width character */
				break;
			  case '%':
				(*ofnc)(arg, '%');
				pos ++;
				break;
			  case 'c':
				val = va_arg(ap, int);
					/* char gets passed as int */
				(*ofnc)(arg, val);
				pos ++;
			  	break;
			  case 'd':
			  case 'D':
				flags |= B(PF_SIGNED);
				/* fall thru - weeee!!! */
			  case 'u':
			  case 'U':
				base = 10;
				goto donum;
			  case 'o':
				base = 8;
				goto donum;
			  case '=':
				/* arbitrary base */
				if(!prec) prec = 16;
				if(*++p == '*'){
					val = va_arg(ap, int);
					base = (val<0)?-val:val;
					goto donum;
				}
				val = 0;
				while( isdig(*p) ){
					val =  10*val + *p -'0';
					p++;
				}
				base = val;
				p--; /* push back */
				goto donum;
			  case 'x':
				flags |= B(PF_ALT);
				/* fallthru' */
			  case 'X':
				base = 0x10;
		    donum:			
				if( flags & B(PF_DLONG) )
					val = va_arg(ap, u_long);
				else
					val = va_arg(ap, unsigned int);
#if 1				
			 	/* sign extend */
				if(flags & B(PF_SIGNED)){
					if(!(flags & B(PF_DLONG))){
						if(!(flags & B(PF_LONG))){
							/* 8bit */
							val = (long)(char)val;
						}else{
							/* 16 bit */
							val = (long)(short)val;
						}
					}
				}
#else
				if(!(flags & B(PF_LONG)))  val &= 0xff;
#endif
				pos += putnum(ofnc, arg, val, base, width, prec, flags);
				break;
  			  case '*':
				width = va_arg(ap,int);
				goto rflag;		
			  case '.':
				/* set prec */
				if(*++p == '*'){
					prec = va_arg(ap, int);
					goto rflag;
				}
				val = 0;
				while( isdig(*p) ){
					val =  10*val + *p -'0';
					p++;
				}
				prec = val;
				goto reswitch;
			  case '1': case '2': case '3':
			  case '4': case '5': case '6': 
			  case '7': case '8': case '9':
				/* set width */
				val = 0;
				while( isdig(*p) ){
					val = 10*val + *p - '0';
					p++;
				}
				width = val;
				goto reswitch;
#ifndef TESTING
			  case 'p':
				/* spin */
				while(prec--){
					(*ofnc)(arg, spinchar[prec%4]);
					msleep(width);
					(*ofnc)(arg, '\b');
				}
#endif				
				break;
			  case '\0':
				return;
			  case 'L': 
				flags |= B(PF_DLONG);
				/* fall thru' */
			  case 'l': /* long arg */
				flags |= B(PF_LONG);
				goto rflag;
			  case '#':
				flags |= B(PF_ALT);
				goto rflag;
			  case '0': 
				flags |= B(PF_ZERO);
				goto rflag;
  			  case '+':
				flags |= B(PF_SHOW_PLS);
				goto rflag;
			  case '-':
				flags |= B(PF_LEFT);
				goto rflag;
			  default:
				break;

			}
			p++;
		}
	}
	return pos;
}


int putnum(int (*ofnc)(void*, char), void *arg, long val, int base, long width, long prec, u_short flags){
	/* base must be: 2<=base<=36 */

	const char *chrs;
	char tbuf[34];
	int i=0, l, n, neg=0;


	chrs = flags&B(PF_ALT)?Set_a:Set_A;

	if( prec > 32 )
		prec = 32;
	if( width > 32 )
		width = 32;

	if( flags&B(PF_SIGNED) && val < 0 ){
		neg = 1;
		val = - val;
	}

#define TOOMUCH	200
	
	do{
		tbuf[i++] = chrs[ val % base ];
	}while( val /= base /* && i < TOOMUCH */ );

	while( i < prec /* && i < TOOMUCH */ ){
		tbuf[i++] = flags&B(PF_ZERO) ? '0' : ' ';
	}
	
	if( neg )
		tbuf[i++] = '-';
	if( !neg && flags&B(PF_SHOW_PLS) )
		tbuf[i++] = '+';

	l = i;
	width -= i;  /* amount of padding needed */

#if 0	
	/* pad on left */
	if( width > 0 && ! (flags&B(PF_LEFT)) ){
		l += width;
		n = 0;
		while( width-- /* && n++ < TOOMUCH */ ){
			(*ofnc)(arg, ',');
		}
	}
#endif
	/* copy output */
	n = 0;
	while( i /* && n++ < TOOMUCH */ ){
		(*ofnc)(arg, tbuf[--i]);
	}

#if 0
	/* pad on right */
	if( width > 0 && (flags&B(PF_LEFT)) ){
		l += width;
		n = 0;
		while( width-- /* && n++ < TOOMUCH */ ){
			(*ofnc)(arg, '|');
		}
	}
#endif

	return l;
}

/* **************************************************************** */

int printffnc(void *a, char c){
	putchar(c);
}

void printf(const char *fmt, ...){
	va_list ap;

	va_start(ap,fmt);
	vprintf(printffnc, 0, fmt, ap);
}

/* **************************************************************** */

int fprintffnc(void *f, char c){
	fputc(c, f);
}

void fprintf(FILE *f, const char *fmt, ...){
	va_list ap;

	va_start(ap,fmt);
	vprintf(fprintffnc, f, fmt, ap);
}

/* **************************************************************** */

struct SNP {
	char *s;
	int  max;
	int  pos;
};

int snprintffnc(void *a, char c){
	struct SNP *s;
	int p;
	
	s = (struct SNP*)a;
	p = s->pos;
	if( p < s->max-1){
		s->s[p] = c;
		s->s[p+1] = 0;
		s->pos = p + 1;
	}
}

int snprintf(char *buf, int len, const char *fmt, ...){
	va_list ap;
	struct SNP s;
	
	va_start(ap, fmt);
	s.s = buf;
	s.pos = 0;
	s.max = len;

	return vprintf(snprintffnc, (void*)&s, fmt, ap);
}

/* **************************************************************** */

extern void syslog(int, const char *);
void loggit(int line, const char *file, int level, const char *mesg, ...){
	va_list ap;
	char buf[1024];
	struct SNP s;
	int l;

	if( level+1 > eeprom.loglevel )
		return;
	
	va_start(ap, mesg);

	l = snprintf(buf, sizeof(buf), "%s/%I: %s/%ld: ", eeprom.sysname, eeprom.myipaddr, file, line);
	s.s = buf;
	s.pos = l;
	s.max = sizeof(buf);
	vprintf(snprintffnc, (void*)&s, mesg, ap);
	syslog(0, buf);
}



#ifdef TESTING
void main(void){
	char buffer[128];
	int i;

	for(i=0; i<128; i++) buffer[i]='a';

	
	snprintf(buffer, 128, "%d %c %s %02.2x\n", (int)324, (int)0x45, "foobar", (int)32);
	puts(buffer);

	printf("%ld %c %s %02.2x\n", (int)324, (int)0x45, "foobar", (int)32);
	printf("%.6ld %c %s %02.2x\n", (int)324, (int)0x45, "foobar", (int)32);

}

#endif

