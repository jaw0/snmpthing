#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE (!FALSE)

#define BLANKBYTE 0xFF

long start = 0x0000;
long theend = 0xFFFF;
long length;

int setlength = FALSE;
int setstart = FALSE;
int settheend = FALSE;

char **gargv;
int gargc;
int curline,curchar;
int infname = -1, outfname = -1;
FILE *infile;
unsigned checksum;

void usage(void);

void help(int *arg)
{
	usage();
}

void setStart(int *arg)
{
	if ( setstart ) {
		fprintf(stderr,"%s: multiple start address arguments, only one allowed.\n",gargv[0]);
		usage();
	}
	(*arg)++;
	if ( *arg >= gargc ) {
		fprintf(stderr,"%s: %s: need to specify starting address\n",gargv[0],gargv[*arg-1]);
		usage();
	}
	if ( sscanf(gargv[*arg],"%x",&start) != 1 ) {
		fprintf(stderr,"%s: Bad data for \"%s\" argument - \"%s\"\n",gargv[0],gargv[*arg-1],gargv[*arg]);
		usage();
	}
	setstart = TRUE;
}

void setLength(int *arg)
{
	if ( setlength ) {
		fprintf(stderr,"%s: multiple length arguments, only one allowed.\n",gargv[0]);
		usage();
	}
	(*arg)++;
	if ( *arg >= gargc ) {
		fprintf(stderr,"%s: %s: need to specify length of segment\n",gargv[0],gargv[*arg-1]);
		usage();
	}
	if ( sscanf(gargv[*arg],"%x",&length) != 1 ) {
		fprintf(stderr,"%s: Bad data for \"%s\" argument - \"%s\"\n",gargv[0],gargv[*arg-1],gargv[*arg]);
		usage();
	}
	if ( settheend ) {
		if ( (theend - start + 1) != length ) {
			fprintf(stderr,"%s: start, end, and length arguments do not agree\n",gargv[0]);
			usage();
		}
	} else theend = start + length - 1;
	setlength = TRUE;
}

void setTheend(int *arg)
{
	if ( settheend ) {
		fprintf(stderr,"%s: multiple end address arguments, only one allowed.\n",gargv[0]);
		usage();
	}
	(*arg)++;
	if ( *arg >= gargc ) {
		fprintf(stderr,"%s: %s: need to specify ending address\n",gargv[0],gargv[*arg-1]);
		usage();
	}
	if ( sscanf(gargv[*arg],"%x",&theend) != 1 ) {
		fprintf(stderr,"%s: Bad data for \"%s\" argument - \"%s\"\n",gargv[0],gargv[*arg-1],gargv[*arg]);
		usage();
	}
	if ( setlength ) {
		if ( (theend - start + 1) != length ) {
			fprintf(stderr,"%s: start, end, and length arguments do not agree\n",gargv[0]);
			usage();
		}
	}
	settheend = TRUE;
}

     
#define OPTIONS 4
char *opts[OPTIONS][3] = {
    {"help",(char*)help,"display this help message"},
    {"start",(char*)setStart,"set start address for binary file"},
    {"end",(char*)setTheend,"set maximum end address for binary file"},
    {"length",(char*)setLength,"set maximum length for binary file"},
};
    
	
void usage(void)
{
    int opt;
	
    fprintf(stderr,"usage: %s [options] infile [outfile]\npossible options are:\n",gargv[0]);
    for (opt=0; opt < OPTIONS; opt++) {
	fprintf(stderr,"     -%s: %s\n",opts[opt][0],opts[opt][2]);
    }
    exit(1);
}

unsigned getHexVal(int digits)
{
	unsigned retVal = 0;
	int tmp;

	for (; digits; digits--) {
		tmp = getc(infile);
		curchar++;
		if ( tmp >= '0' && tmp <= '9' ) tmp -= '0';
		else if ( tmp >= 'A' && tmp <= 'F' ) tmp -= ('A' - 10);
		else if ( tmp >= 'a' && tmp <= 'f' ) tmp -= ('a' - 10);
		else {
			fprintf(stderr,"%s: %s (%d,%d): ",gargv[0],gargv[infname],curline,curchar);
			switch ( tmp ) {
				case EOF:
					fprintf(stderr,"Unexpected EOF");
					break;
				case '\n':
					fprintf(stderr,"Unexpected EOL");
					break;
				default:
					fprintf(stderr,"Invalid character '%c' (0x%x)",tmp,tmp);
					break;
			}
			fprintf(stderr,"; expecting at least %d more hex digits\n",(digits+1));
			exit(1);
		}
		retVal <<= 4;
		retVal |= tmp;
		if ( digits % 2 ) {
			checksum += (retVal & 0xff);
			checksum &= 0xff;
		}
	}

	return retVal;
}

void main(int argc, char **argv)
{
	int arg,opt;
	int i,j,tmp;
	void (*optFunc)(int *);
	unsigned type;
	unsigned reclen;
	long address;
	FILE *outfile;
	char *fname;
	char *core;
	
	
	gargc = argc;
	gargv = argv;

	if ( argc < 2 ) {
		usage();
	}

	for (arg=1; arg < gargc; arg++) {
		if ( gargv[arg][0] == '-' ) {
			i = 1;
			j = 0;
			optFunc = NULL;
			do {
				for (opt=0; opt < OPTIONS; opt++) {
					if ( !strncmp(&gargv[arg][1],opts[opt][0],i) ) {
						optFunc = (void(*)(int *)) opts[opt][1];
						j++;
					}
				}
				i++;
			} while ( j > 1 );
			if ( j == 1 ) (*optFunc)(&arg);
			else if ( j > 1 ) {
				fprintf(stderr,"%s: %s: Ambiguous option\n",gargv[0],gargv[arg]);
				usage();
			} else {
				fprintf(stderr,"%s: %s: Invalid option\n",gargv[0],gargv[arg]);
				usage();
			}
		} else {
			if ( infname == -1 ) infname = arg;
			else if ( outfname == -1 ) outfname = arg;
			else {
				fprintf(stderr,"%s: Bad or extra argument - \"%s\"\n",gargv[0],gargv[arg]);
				usage();
			}
		}
	}

	if ( infname == -1 ) {
		fprintf(stderr,"%s: need to specify an input file or device\n",gargv[0]);
		usage();
	}

	if ((infile = fopen(gargv[infname], "rt")) == NULL) {
		fprintf(stderr,"%s: %s: No such file\n",gargv[0],gargv[infname]);
		usage();
	}

	if ( outfname == -1 ) {
		for (tmp=(strlen(gargv[infname])-1); tmp >= 0; tmp--) {
			if ( gargv[infname][tmp] == '\\' ) break;
			if ( gargv[infname][tmp] == '.' ) gargv[infname][tmp] = '\0';
		}
		if ( !(fname = malloc(strlen(gargv[infname])+5)) ) {
			fprintf(stderr,"%s: not enough memory to allocate file name buffer\n",gargv[0]);
		}
		sprintf(fname,"%s.bin",gargv[infname]);
	} else fname = gargv[outfname];
	if ( (outfile = fopen(fname, "wb")) == NULL ) {
		fprintf(stderr,"%s: \"%s\": cannot open or create file or device\n",gargv[0],fname);
		usage();
	}

	length = 0;
	curchar = 0;
	checksum = 0;

	core = malloc( theend - start + 1 );
	
	for (;;) {
		tmp = TRUE;
		while ( tmp ) {
			tmp = getc(infile);
			curchar++;
			if ( tmp == EOF ) {
				if ( type == 0x01 ) {
					break;
				} else {
					fprintf(stderr,"%s: %s (%d,%d): Unexpected EOF\n",gargv[0],gargv[infname],curline,curchar);
					exit(-1);
				}
			}
			switch ( tmp ) {
				case '\n':
					curline++;
					curchar = 0;
					break;
				case ' ':
				case '\t':
					break;
				case ':':
					tmp = FALSE;
					break;
				default:
					fprintf(stderr,"%s: %s (%d,%d): Bad record header\n",gargv[0],gargv[infname],curline,curchar);
					exit(-1);
			}
		}
		if ( tmp == EOF ) break;

		reclen = getHexVal(2);
		if ( reclen > 0x10 ) {
			fprintf(stderr,"%s: %s (%d): Bad record length - too long: 0x%x\n",gargv[0],gargv[infname],curline,reclen);
			exit(-1);
		}

		address = (long)getHexVal(4);

		switch ( type = getHexVal(2) ) {
			case 0x00:
				break;
			case 0x01:
				if ( reclen != 0 ) {
					fprintf(stderr,"%s: %s (%d): EOF record must be of zero length\n",gargv[0],gargv[infname],curline);
					exit(-1);
				}
				break;
			case 0x02:
				fprintf(stderr,"%s: %s (%d): Don't know how to deal with extended address\n",gargv[0],gargv[infname],curline);
				exit(-1);
			case 0x03:
				fprintf(stderr,"%s: %s (%d): Don't understand record type \"Start\"\n",gargv[0],gargv[infname],curline);
				exit(-1);
			default:
				fprintf(stderr,"%s: %s (%d): Invalid record type - 0x%x\n",gargv[0],gargv[infname],curline,type);
				exit(-1);
		}

		for (; reclen > 0; reclen--) {
			if ( address >= start && address <= theend ) {
#if 0			
				if ( (address - start) > length ) {
					fseek(outfile,length,SEEK_SET);
					while ( length < (address - start) ) {
						putc(BLANKBYTE,outfile);
						length++;
					}
				} else fseek(outfile,(address-start),SEEK_SET);
				if ( (address - start) >= length ) length++;
				putc(getHexVal(2),outfile);
#else

				if( (address - start) > length ){
					while( length < (address - start)){
						core[ length++ ] = BLANKBYTE;
					}
				}
				
				core[ address-start ] = getHexVal(2);
				if( length < address - start + 1)
					length = address - start + 1;
#endif				
			} else
				getHexVal(2);
			
			address++;
		}
		getHexVal(2);
		if ( checksum ) {
			fprintf(stderr,"%s: %s (%d): Checksum doesn't match\n",gargv[0],gargv[infname],curline);
			exit(-1);
		}
	}

	fwrite(core, length, 1, outfile);

	if ( fclose(infile) == EOF ) {
		fprintf(stderr,"%s: %s: Unable to close file\n",gargv[0],gargv[infname]);
		exit(-1);
	}
	if ( fclose(outfile) == EOF ) {
		fprintf(stderr,"%s: %s: Unable to close file\n",gargv[0],fname);
		exit(-1);
	}

	free(fname);
	exit(0);
}
