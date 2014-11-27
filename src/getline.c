
/* 
  $Id: getline.c,v 1.5 1997/10/08 00:00:08 jaw Exp jaw $
*/

#ifndef TESTING
#include <mmlib.h>
#include <nstdio.h>
#pragma registers(4)
#pragma optimize(0)
#define NEWLINE	"\n"
#else
#include <stdio.h>
#define puts(x)	fputs(x, stdout)
#define NEWLINE	"\r\n"
#endif


#define VERSION "Getline 2.0 -- 8 Sept 1997"


#define META(x) ( (x) | 0x100 )
#define BRKT(x)	( (x) | 0x200 )
#define CTRL(x)	( (x) - 'a' + 1 )


int isnonword(char c){
	switch (c){
	  case ' ':  return 1;
	  case '\t': return 1;
	  case '\n': return 1;
	  case '\r': return 1;
	  case '.':  return 1;
	  case '-':  return 1;
	  case ';':  return 1;
	  case ',':  return 1;
	  default:   return 0;
	}
}
		
static void move(char *s, int max, int *i, int from, int to, int beepp){

	if( from < to ){
		for( ; from<max && s[from] && from != to; from++){
			putchar(s[from]);
		}
		*i = from;
	}else{
		for( ; from && from != to; from--){
			putchar('\b');
		}
		*i = from;
	}
	if( beepp && from!=to)
		putchar('\a');
}

static void insert(char *s, int max, int *i, char c){
	int j, x;
	
	j = *i;
	if( j >=max-1 ){
		putchar('\a');
		return;
	}
	do{
		x = s[j];
		s[j] = c;
		putchar(c);
		c = x;
		j++;
	}while(c && j<max-1);
	s[j]=0;
	(*i)++;
	move(s, max, i, j, *i, 0);
}

static int tty_debug(char *s, int max, int *i, char c){
	int j;
	char buf[32];
	
	j = 0;
	printf("%si=%d [", NEWLINE, *i, j);

	while( s[j] ){
		if( s[j] >= ' ' && s[j] <= 0x7E )
			putchar(s[j]);
		else
			putchar('.');
		j++;
	}

	printf("] len=%d%s", j, NEWLINE);
	return 0;
}

static int tty_quote(char*s, int max, int *i, char c){

	putchar('^');
	putchar('\b');
	s[*i++] = c = getchar();
	putchar(c);

	return 0;
}

static int tty_stop(char*s, int max, int *i, char c){

	puts(NEWLINE);
	puts("Job control not supported");
	puts(NEWLINE);
	return 0;
}

static int tty_rprnt(char *s, int max, int *i, char c){
	
	puts(NEWLINE);
	puts(s);
	for(*i=0; s[*i]; (*i)++)
		;
	return 0;
}

static int tty_quit(char *s, int max, int *i, char c){

	*i = 0;
	s[*i] = 0;
	puts("\a^C");
	return 1;
}

static int tty_eol(char *s, int max, int *i, char c){

	puts(NEWLINE);
	return 1;
}

static int tty_version(char *s, int max, int *i, char c){

	puts(NEWLINE);
	puts(VERSION);
	puts(NEWLINE);
	return 0;
}

static int tty_toleftend(char *s, int max, int *i, char c){

	move(s, max, i, *i, 0, 0);
	return 0;
}

static int tty_torightend(char *s, int max, int *i, char c){

	move(s, max, i, *i, max, 0);
	return 0;
}

static int tty_toleft(char *s, int max, int *i, char c){

	move(s, max, i, *i, *i-1, 1);
	return 0;
}

static int tty_toright(char *s, int max, int *i, char c){

	move(s, max, i, *i, *i+1, 1);
	return 0;
}

static int tty_delete(char *s, int max, int *i, char c){
	int j;
	
	j = *i;
	if( s[*i] ){
		while(s[j]){
			s[j] = s[j + 1];
			if( s[j] ) putchar(s[j]);
			j++;
		}
		puts(" \b");
                /* put cursor back */
		while( j != (*i) + 1 ){
			j--;
			putchar('\b');
		}
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_bksp(char *s, int max, int *i, char c){

	if(!*i){
		putchar('\a');
		return 0;
	}
	(*i)--;
	puts("\b \b");

	return tty_delete(s, max, i, c);
}

static int tty_killr(char *s, int max, int *i, char c){
	int j;
	
	if( s[*i] ){
		for(j=*i; s[j]; j++){
			putchar(' ');
		}
		for(; j!=*i; j--){
			s[j]=0;
			putchar('\b');
		}
		s[j]=0;
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_killl(char *s, int max, int *i, char c){

	if( *i ){
		while(*i)
			tty_bksp(s, max, i, c);
	}else{
		putchar('\a');
	}
	
	return 0;
}

static int tty_wordl(char *s, int max, int *i, char c){
	
	if( *i ){
		while( *i>0 && !isnonword(s[*i - 1]) )
			tty_bksp(s, max, i, c);
		while( *i>0 && isnonword(s[*i - 1]) )
			tty_bksp(s, max, i, c);
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_wordr(char *s, int max, int *i, char c){
	
	if( s[*i] ){
		while( s[*i] && isnonword(s[*i]) )
			tty_delete(s, max, i, c);
		while( s[*i] && !isnonword(s[*i]) )
			tty_delete(s, max, i, c);
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_towordr(char *s, int max, int *i, char c){

	if( s[*i] ){
		while( s[*i] && isnonword(s[*i]) )
			move(s, max, i, *i, *i+1, 0);
		while( s[*i] && !isnonword(s[*i]) )
			move(s, max, i, *i, *i+1, 0);
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_towordl(char *s, int max, int *i, char c){

	if( *i ){
		while( *i>0 && isnonword(s[*i-1]) )
			move(s, max, i, *i, *i-1, 0);
		while( *i>0 && !isnonword(s[*i-1]) )
			move(s, max, i, *i, *i-1, 0);
	}else{
		putchar('\a');
	}
	return 0;
}

static int tty_beep(char*s, int max, int *i, char c){

	putchar('\a');
	return 0;
}

static const struct {
	int c;
	int (*f)(char*, int, int*, char);
} keymap[] = {
	/* bindings for key to function */

	/* first a std. UN*X tty binding */
	{ CTRL('c'), tty_quit       },
	{ CTRL('r'), tty_rprnt      },
	{ CTRL('z'), tty_stop       },
	{ CTRL('v'), tty_quote      },
	{ CTRL('h'), tty_bksp       },
	{ CTRL('j'), tty_eol        },
	{ CTRL('m'), tty_eol        },
	{ CTRL('u'), tty_killl      },
	{ CTRL('w'), tty_wordl      },
	{ 28,        tty_quit       }, /* ^\ */
	{ 127,       tty_bksp       },

	/* and some emacs-isms */
	{ CTRL('a'), tty_toleftend  },
	{ CTRL('b'), tty_toleft     },
	{ CTRL('d'), tty_delete     },
	{ CTRL('e'), tty_torightend },
	{ CTRL('f'), tty_toright    },
	{ CTRL('k'), tty_killr      },
	{ CTRL('q'), tty_quote      },
	{ META('b'), tty_towordl    },
	{ META('d'), tty_wordr      },
	{ META('f'), tty_towordr    },

	/* vt100 arrows (x3.64) */
	{ BRKT('A'), tty_beep       },
	{ BRKT('B'), tty_beep       },
	{ BRKT('C'), tty_toright    },
	{ BRKT('D'), tty_toleft     },

	/* vt52 arrows */
	{ META('A'), tty_beep       },
	{ META('B'), tty_beep       },
	{ META('C'), tty_toright    },
	{ META('D'), tty_toleft     },
	
	/* leave these alone */
	{ META('x'), tty_debug      },
	{ META('v'), tty_version    },

	/* this *must* be last */
	{ 0,         0              }
};

	
char *getline(char *s, int max, int usep){
	int i=0;
	int c, n, r;

#ifdef TESTING	
	system("stty raw -echo");
#endif
	
	if( usep ){
		/* usep = 0 => clear buffer
		          1 => reuse buffer, cursor at right
		         -1 => reuse buffer, cursor at left
		*/
		puts(s);
		for(i=0; s[i]; i++)
			;
		if( usep == -1 ){
			move(s, max, &i, i, 0, 0);
		}
	}else{
		*s = 0;
	}
	while( 1 ){
		c = getchar();

		if( c == 0x1b ){
			/* support only single letter esc seq.s */
			/* support full ansi x3.64 escape seq.s RSN */
			c = 0x100 | getchar() ;
			/* ok - \ex and \e[x so we can do arrow keys */
			if( c == META('[') )
				c = 0x200 | getchar() ;
				
		}
		
		/* search keymap for key */
		for(n=0; keymap[n].f; n++)
			if (c == keymap[n].c) break;  

		if( keymap[n].f ){
			r = (keymap[n].f)(s, max, &i, c);
		}else{
			insert(s, max, &i, c);
			r = 0;
		}

		switch(r){
		  case 1:
#ifdef TESTING
			system("stty -raw echo");
#endif			
			return s;
			break;
		}
		
		
	} /* eow */
	return s;
}

#ifdef TESTING
int main(){
	char buffer[1024] = "123.45.67.89";

	
	while(1){
		fprintf(stderr, "[huh]? ");
		getline(buffer, 1024, 1);
		if( !*buffer )
			break;
		printf("<<%s>>\r\n", buffer);
	}
	
}
#endif

