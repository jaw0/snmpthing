
/*
  $Id: nstdio.c,v 1.4 1997/10/08 00:00:12 jaw Exp jaw $
*/

#include <mmlib.h>
#include <nstdio.h>

#pragma registers(0)


int finit(FILE *f){

	f->ungotc = 0;
	f->flags  = 0;
	f->prevout = 0;

	BitSet(f->flags, F_ONLCR);
	
	f->d = 0;
}

int putchar(char c){
	return fputc(c, STDOUT);
}
int getchar(void){
	return fgetc(STDIN);
}
void puts(const char *mesg ){
	/* output a <NULL> terminated string (does not output the <NULL>) */
	/* unlike stdio puts does not append newline */
	while( *mesg )
		putchar( *mesg++ );
}

int ungetc(char c, FILE *f){

	f->ungotc = c;
	BitSet(f->flags, F_UNGOT);

	return 1;
}

int fgetc(FILE *f){
	int c;
	
	if( BitTest(f->flags, F_UNGOT) ){
		BitClear(f->flags, F_UNGOT);
		return f->ungotc;
	}
	c = (f->fs->getc)(f);
}

int fputc(char c, FILE *f){

	if( c=='\n' && f->prevout != '\r' && BitTest(f->flags, F_ONLCR) )
		(f->fs->putc)(f, '\r');
	f->prevout = c;
	return (f->fs->putc)(f, c);
}

int fflush(FILE *f){

	return (f->fs->flush)(f);
}

int fstatus(FILE *f){

	return (f->fs->status)(f);
}


	
	

