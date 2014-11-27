
#ifndef TESTING
#include <mmlib.h>
#pragma registers(2)
#pragma optimize(0)
#else
#include <stdio.h>
#endif
#include <eeprom.h>
#include <ip.h>

extern char *getline(char*, int, int);
extern void openEEPROM(void);
extern const char *version(void);

static const struct {
	char *name;
	char *help;
	void *addr;
	int  type;
#define TYPE_STR	1
#define TYPE_UC		2
#define TYPE_US		3
#define TYPE_IP		4
#define TYPE_STR32	5	
} vars[] = {

	{ "baud",      "serial port baud rate",    &eeprom.baud,       TYPE_US  },
	{ "diag",      "diag mode",                &eeprom.diag,       TYPE_UC  },
	{ "traplevel", "trapping level",           &eeprom.traplevel,  TYPE_UC  },
	{ "loglevel",  "syslogging level",         &eeprom.loglevel,   TYPE_UC  },
	{ "community", "SNMP community string",    eeprom.community,   TYPE_STR },
	{ "comm",      0,                          eeprom.community,   TYPE_STR },
	{ "password",  "secret password",          eeprom.passwd,      TYPE_STR },
	{ "passwd",    0,                          eeprom.passwd,      TYPE_STR },
	{ "ipaddr",    "local ip address",         &eeprom.myipaddr,   TYPE_IP  },
	{ "trapaddr",  "trap host ip address",     &eeprom.traphost,   TYPE_IP  },
	{ "traphost",  0,                          &eeprom.traphost,   TYPE_IP  },
	{ "logaddr",   "syslog host ip address",   &eeprom.sysloghost, TYPE_IP  },
	{ "loghost",   0,                          &eeprom.sysloghost, TYPE_IP  },
	{ "sysname",   "system.sysName",           eeprom.sysname,     TYPE_STR32 },
	{ "location",  "system.sysLocation",       eeprom.location,    TYPE_STR },
	{ "contact",   "system.sysContact",        eeprom.contact,     TYPE_STR },

	{0,0,0,0}
};
	

static const char* const tc_modes[] = { "none", "rise", "fall", "change" };
static const char* const x_on_off[] = { "off", "on" };
static const char x_input[] = "input";
static const char x_analog[]= "analog";

static const char x_error[] = "error\n";

static int vers(int argc, char **argv){

	printf("%s\n", version());
	return 0;
}

static int on_or_off(const char *s){
	int n;

	for(n=0; n<2; n++){
		if( !strcmp(s, x_on_off[n])) return n;
	}
	return -1;
}
	
static int trapconf(int argc, char **argv){
	int n, m, l, h;

	if( argc>=4 && !strcmp(x_input, argv[1])){
		n = atoi( argv[2] );
		m = -1;

		if( n > 23 || n < 0) goto error;

		for(m=0; m<4; m++)
			if( !strcmp((char*)tc_modes[m], argv[3])) break;
		if( m==4) goto error;

		eeprom.inbitmode[ n / 4 ] &= 3 << 2 * (n % 4);
		eeprom.inbitmode[ n / 4 ] |= m << 2 * (n % 4);
		if( argc >= 5 )
			eeprom.inbitlevel[ n ] = atoi( argv[4] );
		return 0;
	}
	if( argc>=5 && !strcmp(x_analog, argv[1])){
		n = atoi( argv[2] );
		l = atoi( argv[3] );
		h = atoi( argv[4] );

		if( n > 7 || n < 0 ) goto error;

		eeprom.analow[ n ]  = l;
		eeprom.anahigh[ n ] = h;

		if( argc >= 6 )
			eeprom.analevel[ n ] = atoi( argv[5] );
		return 0;
	}
	if( argc==3 && !strcmp("show", argv[1])){
		if( !strcmp(x_input, argv[2])){
			for(n=0; n<24; n++){
				m = eeprom.inbitmode[ n / 4 ];
				m >>= 2 * (n % 4);
				m &= 3;
				l = eeprom.inbitlevel[ n ];
				printf("input %d: mode=%s, level=%u\n", n, tc_modes[m], l);
			}
			return 0;
		}
		if( !strcmp(x_analog, argv[2])){
			for(n=0; n<8; n++){
				l = eeprom.analow[ n ];
				h = eeprom.anahigh[ n ];
				m = eeprom.analevel[ n ];
				printf("analog %d: lo=%u, hi=%u, level=%u\n", n, l, h, m);
			}
			return 0;
		}
	}
	
    error:
	puts(x_error);
	return -1;
}

static int save(int argc, char **argv){

	puts("saving...");
	commitEEPROM();
	printf("done.\nn=%lu, cksum=0x%lx\n",
	       eeprom.writecount,
	       eeprom.chksum);
	return 0;
}

static int openee(int argc, char **argv){

	openEEPROM();
	return 0;
}

static int fiddlemem(int argc, char **argv){
	int a, v;

	if( argc < 2 ){
		puts(x_error);
		return -1;
	}

	a = getnumat(16, argv[1] );
	
	if( argc > 2 ){
		/* set mem */
		v = getnumat(16, argv[2] );
		*(u_char*)a = v;
	}else{
		/* get mem */
		v = *(u_char*)a;
	}

	printf("[%04.4lx]: %02.2x\n", a, v);
	return 0;
}

static int igncr(int argc, char **argv){
	int v;
	
	if( argc < 2 || (v=on_or_off( argv[1] )) == -1){
		puts(x_error);
		return -1;
	}

	if( v )
		BitSet(STDIN->flags, F_IGNCR);
	else
		BitClear(STDIN->flags, F_IGNCR);
	return 0;
}

static int onlcr(int argc, char **argv){
	int v;
	
	if( argc < 2 || (v=on_or_off( argv[1] )) == -1){
		puts(x_error);
		return -1;
	}

	if( v )
		BitSet(STDIN->flags, F_ONLCR);
	else
		BitClear(STDIN->flags, F_ONLCR);
	return 0;
}

	
	

static int show(int, char**);
static int set (int, char**);
static int help(int, char**);


static const struct {
	char *name;
	char *help;
	int (*fnc)(int, char**);
} cmds[] = {

	{ "save",     "save current config",          save },
	{ "_oe",      0,                              openee },
	{ "_mem",     0,                              fiddlemem },
	{ "tc",       "\n\ttc input n rise|fall|change|none [l]\n\ttc analog n lo hi [l]\n\ttc show input|analog",
	                                              trapconf },
	{ "show",     "show var",                     show },
	{ "sho",      0,                              show },
	{ "sh",       0,                              show },
	{ "set",      "set var value",                set },
	{ "help",     "help command",                 help },
	{ "?",        0,                              help },
	{ "version",  "what version",                 vers },
	{ "vers",     0,                              vers },
	{ "igncr",    "igncr on|off",                 igncr },
	{ "onlcr",    "onlcr on|off",                 onlcr },
	{ "exit",     "exit",                         0 },
	{ 0,0,0 }
};

static int help(int argc, char **argv){
	int i;

	/* if no args, show all commands/vars */
	if( argc < 2 ){
		printf("help command|var\ncommands:");
		for(i=0; cmds[i].name; i++)
			if( cmds[i].help )
				printf(" %s", cmds[i].name);
		printf("\nvars:");
		for(i=0; vars[i].name; i++)
			if( vars[i].help )
				printf(" %s", vars[i].name);
		printf("\n");
		return 0;
	}
	
	/* a command ? */
	for(i=0; cmds[i].name; i++)
		if( !strcmp(cmds[i].name, argv[1])) break;
	if( cmds[i].name ){
		printf("%s: %s\n", cmds[i].name, cmds[i].help ? cmds[i].help : "undocumented");
		return 0;
	}

	/* a var */
	for(i=0; vars[i].name; i++)
		if( !strcmp(vars[i].name, argv[1])) break;
	if( vars[i].name ){
		printf("%s: %s\n", vars[i].name, vars[i].help ? vars[i].help : "undocumented");
		return 0;
	}

	printf("%s: no such command or variable\n", argv[1]);
	return -1;
}

static int show(int argc, char **argv){
	char buf[16];
	int i;

	if( argc != 2 ){
		puts("show: error\n");
		return -1;
	}
	for(i=0; vars[i].name; i++)
		if( !strcmp(vars[i].name, argv[1])) break;
	if( vars[i].name ){
		switch( vars[i].type ){
		  case TYPE_STR:
		  case TYPE_STR32:
			printf("%s = %s\n", argv[1], vars[i].addr);
			break;
		  case TYPE_UC:
			printf("%s = %u\n", argv[1], *(u_char*)vars[i].addr);
			break;
		  case TYPE_US:
			printf("%s = %lu\n", argv[1], *(u_short*)vars[i].addr);
			break;
		  case TYPE_IP:
			printf("%s = %I\n", argv[1], *(u_long*)vars[i].addr);
			break;
		}
	}else{
		printf("show %s: no such var\n", argv[1]);
		return -1;
	}
	return 0;
}

/* set a b  or  a = b */
static int set(int argc, char **argv){
	int i, n;
	char *var;
	u_long v;
	
	if( argc != 3 ){
		puts(x_error);
		return -1;
	}
	if( strcmp("set", argv[0])){
		/* we got a = b */
		var = argv[0];
	}else{
		var = argv[1];
	}
	
	for(i=0; vars[i].name; i++)
		if( !strcmp(vars[i].name, var)) break;
	if( vars[i].name ){
		switch( vars[i].type ){
		  case TYPE_STR:
		  case TYPE_STR32:
			n = (vars[i].type==TYPE_STR)? 16 : 32;
			strncpy( (char*)vars[i].addr, argv[2], n);
			((char*)vars[i].addr)[n-1] = 0;
			break;
		  case TYPE_UC:
			v = atoi( argv[2] );
			*(u_char*)vars[i].addr = v;
			break;
		  case TYPE_US:
			v = atoi( argv[2] );
			*(u_short*)vars[i].addr = v;
			break;
		  case TYPE_IP:
			v = inet_aton( argv[2] );
			*(u_long*)vars[i].addr = v;
			break;
		}
	}else{
		printf("set %s: no such var\n", var);
		return -1;
	}
	return 0;
}

void prompt(void){
	printf("%s> ", eeprom.sysname);
}

int eval(int argc, char **argv){
	int i;
	
	for(i=0; cmds[i].name; i++)
		if( !strcmp(cmds[i].name, argv[0])) break;
	if( cmds[i].name )
		return (*cmds[i].fnc)(argc, argv);

	if( argc > 2 && !strcmp("=", argv[1]))
		return set(argc, argv);

	puts("Huh?\n");
	return -1;
}

void cli(void){
	char buf[128];
	char *argv[16];
	int  argc;
	char *p;
	char qc;
	
	while( 1 ){

		prompt();
		getline(buf, sizeof(buf), 0);
		
		argc = 0;
		p = buf;

		while( argc < sizeof(argv)/sizeof(argv[0]) ){

			/* skip white */
			while( *p ==' ' || *p == '\t' ){
			  next_token:
				*p++ = 0;
			}
			
			if( ! *p ){
				break;
			}
			
                        /* we allow "" inside '' and '' inside "" */
                        if(*p=='"'||*p=='\''){
                                qc = *p;
                                p++;
                                argv[argc++] = p;
                                while( *p && *p!=qc ){
                                        p++;
                                }
                                if( *p )
                                        goto next_token;
                                break;
                        }

			argv[argc++] = p;
			while( *p && *p!=' ' && *p!='\t' )
				p++;
		}

		if( !strcmp("exit", argv[0] ))
			return;
		eval(argc, argv);

	}
}

#ifdef TESTING
void main(){

	cli();
}
#endif


		
			
		
