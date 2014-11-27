
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define YYDEBUG 1

	
        extern int yydebug, prompt;
	int yylineno=0, yyrowno=0;
        int yystrlen, yyecho=0;

	int mode = 0;
	FILE *fp;

	char switchon[256];
	int switchtyp;
	int switchcnt = 0;
	int casecnt = 0;
%}

%union {
	int i;
	char s[1024];
}

%token <s> STRING, OTHER
%token <i> FUNCTION, SWITCH, SWITCHB, CASE, BREAK, DEFAULT, OPEN, CLOSE
%token <i> NEWLINE, TEOF, WHITE


%%

File    :  Lines TEOF
        { exit(0); }
        ;

Lines   :  Lines Line
        |  Line
        ;

Line    :  Spec
        |  PassAlong
        ;


PassAlong : Words NEWLINE
          {
		  printf("\n");
	  }
          | NEWLINE
	  {
		  printf("\n");
	  }
          ;


Word  : OTHER
      {
	      printf("%s ", $1);
      }
      | STRING
      {
	      printf("\"%s\" ", $1);
      }
      ;

Words : Words Word
      | Word
      ;


Spec : Function
     | Switch
     | Case
     | Break
     | Default
     ;

Function : FUNCTION OTHER OPEN NEWLINE
         {
		 printf(".proc\n%s:\nF_PROLOGUE()\n", $2);
	 }
         Lines CLOSE
         {
		 printf("\nend$:\nF_EPILOGUE()\nret\n; ****************\n\n" );
	 }
         ;

Switcht : SWITCH  {switchtyp=1;}
        | SWITCHB {switchtyp=2;}
        ;

Switch : Switcht OTHER OPEN NEWLINE
        {
		strcpy(switchon, $2);
		printf(";; switch on %s\n", switchon);
		casecnt = 0;
	}
        Lines CLOSE
        {
		printf("endcase%d$:\n", switchcnt++);
	}
        ;

Case : CASE OTHER NEWLINE
     {
	     printf("CASENEXT%d.%d$:\n", switchcnt, casecnt++);
	     printf("cmp%s %s, %s\n", switchtyp==1?"":"b", switchon, $2);
	     printf("jne CASENEXT%d.%d$\n", switchcnt, casecnt);
     }
     ;

Break : BREAK NEWLINE
      {
	      printf("jmp endcase%d$\n", switchcnt);
      }
      ;

Default: DEFAULT NEWLINE
       {
	       printf("CASENEXT%d.%d$:\n", switchcnt, casecnt++);
       }
       ;

%%
int yywrap(){
        exit(0);
}

void yyerror(s)
        char *s; {
 
        fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}

main(int argc, char**argv){
	yydebug = 0;

	if(argc==2){
		fp = fopen(argv[1], "r");
		if(!fp){
			perror(argv[1]);
			exit(1);
		}
	}else{
		fp = stdin;
	}
	yyparse();
}

int iswhite(int c){
        return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

int yylex(){
	int c, i=0, j=0;
        char buf[1024];

	c = getc(fp);
	while( c==' ' || c=='\t' ){
		c = getc(fp);
	}
	switch (c) {
	  case '\n': yylineno++; yyrowno=0; return NEWLINE;
	  case '{':  yyrowno++; return OPEN;
	  case '}':  yyrowno++; return CLOSE;
	  case EOF:  return TEOF;
		
	}
	while(!iswhite(c)){
		buf[i++] = c;
		c = getc(fp);
		buf[i] = 0;
	}
	ungetc(c, fp);
	
	if(! strcmp(buf, "function")) return FUNCTION;
	if(! strcmp(buf, "break")) return BREAK;
	if(! strcmp(buf, "case")) return CASE;
	if(! strcmp(buf, "switch")) return SWITCH;
	if(! strcmp(buf, "switchb")) return SWITCHB;
	if(! strcmp(buf, "default")) return DEFAULT;
	
	if(! strcmp(buf, "__YY_DEBUG__")){
		yydebug = 1 - yydebug;
		return yylex();
	}

	strcpy(yylval.s, buf);
	return OTHER;
}

	
