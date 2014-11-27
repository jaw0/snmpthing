
/*
  code to handle the EEPROM
*/

#pragma optimize(3)
#pragma registers(0)
#pragma rc

#include <mmlib.h>
#include <eeprom.h>

struct EEPROM eeprom;	/* a copy placed into ram */

	/* internal only routines */
static void	outEEBit(u_char bit);
static void	outEEBits(u_short bits, int nbits);
static u_char	inEEBit(void);
static u_short	inEEBits(int nbits);
static void	clearEEall(void);

	/* ought not ever need these anywhere else, but jic ... */
u_char	EEchksum(void);
void	writeEEWord(int addrs, u_short data);
u_short	readEEWord(int addrs);
void	EEenableWR(void), EEdisableWR(void);


void set_eeprom_cs(int v){
	
	if( v )
		BitSet(p2_image, 5);
	else
		BitClear(p2_image, 5);
	ioport2 = p2_image;
}

void set_eeprom_sk(int v){
	
	if( v )
		BitSet(p2_image, 7);
	else
		BitClear(p2_image, 7);
	ioport2 = p2_image;
}

static void outEEBit(u_char bit){
	/* send out one bit to the eeprom */

	if( bit )
		BitSet(p2_image, 6);
	else
		BitClear(p2_image, 6);
	ioport2 = p2_image;
	
	set_eeprom_sk(1);       	/* raise sk */
	/* need to pause 1us */
	usleep(1);
	
	set_eeprom_sk(0);       	/* drop sk */
	/* need to pause 1us */
	usleep(1);
}


static u_char inEEBit(void){
	/* read in one bit from the EEPROM */
	/* we sample DO prior the falling edge of SK */
	u_char foo;

	set_eeprom_sk(1);	        /* raise sk */
	usleep(1);

	BitSet(p2_image, 6);            /* put into input mode */
	ioport2 = p2_image;
	foo = ioport2;			/* snarf data */
	
	set_eeprom_sk(0);               /* drop sk */
	usleep(1);

	return BitTest(foo, 6) ? 1 : 0;
}

void initEEPROM(void){
	/* read the EEPROM into RAM */
	int i;

	puts("iE: init...");
	set_eeprom_cs(1);               /* raise cs */
	outEEBits(00, 16);		/* clear out state */
	outEEBits(00, 16);
	set_eeprom_cs(0);               /* drop cs */

	for(i=0; i<(EEPROM_SIZE+1)/2; i++){
		((u_short*)&eeprom)[i] = readEEWord( i );
		puts(".");
	}

	if( eeprom.version != EEPROM_CURRENT_VERSION ){
		/* convert old format ? */
		puts("!v");
		eeprom.diag = 1;
	}
	
	if( eeprom.chksum != EEchksum() ){
		/* bad checksum -- drop into diag mode */
		puts("!x");
		eeprom.diag = 1;
	}
	puts(".done\n");
}

void commitEEPROM(void){
	/* write it back out to the EEPROM from RAM */
	int i;

	eeprom.writecount++;		/* keep track - it can only be
					   reliably written ~40000 times */
	eeprom.version = EEPROM_CURRENT_VERSION;
	eeprom.chksum = EEchksum();	/* generate proper checksum */
	
	EEenableWR();
	for(i=0; i<(EEPROM_SIZE+1)/2; i++)
		writeEEWord(i, ((u_short*)&eeprom)[i] );
	EEdisableWR();

}
	    
u_char EEchksum(void){
	/* calculate checksum */
	int i;
	u_char sum = 0;
	
	for(i=0; i<EEPROM_SIZE; i++)
		sum += ((char*)&eeprom)[i];
	sum -= eeprom.chksum;
	return -sum;
}
	
static void outEEBits(u_short bits, int nbits){
	/* send out a stream of bits to the eeprom -- *MSB* first */
	int i;

	for(i=nbits-1; i>=0; i--){
		outEEBit( ((bits & Bit(i))?1:0) );
	}
}

static u_short inEEBits(int nbits){
	/* read in a stream of bits from eeprom *MSB* first */
	int i;
	u_short foo = 0;

	for(i=0; i<nbits; i++){
		foo <<= 1;
		foo |= inEEBit();
	}
	return foo;
}

/* the following commands assume 93c66 format */

void writeEEWord(int addrs, u_short data){
	/* we assume that writing is enabled */
	u_short foo;

	set_eeprom_cs(1);               /* raise cs */
	outEEBits(0, 16);
	
	foo = addrs;
	foo &= 0xFF;			/* 8 addrs bits */
	foo |= 0x5 << 8;		/* write = 1 01 */

	outEEBits(foo, 11);
	outEEBits(data,16);

	set_eeprom_cs(0);	        /* drop cs */
	usleep(1);
	set_eeprom_cs(1);       	/* raise cs */

	BitSet(p2_image, 6);		/* put into input mode */
	ioport2 = p2_image;
	/* wait till !busy */
	while(!foo){
		foo = ioport2;
		foo &= Bit(6);
	}

	set_eeprom_cs(0);		/* drop cs */
	usleep(1);
	outEEBit(0);
}

u_short readEEWord(int addrs){
	u_short foo;

	set_eeprom_cs(1);
	outEEBits(0, 16);
	
	foo = addrs;
	foo &= 0xFF;			/* 8 addrs bits */
	foo |= 0x6 << 8;		/* read = 1 10 */

	outEEBits(foo, 11);
	foo = inEEBits(16);

	set_eeprom_cs(0);
	usleep(1);
	outEEBit(0);
	return foo;
}

void EEenableWR(void){
	u_short foo;

	set_eeprom_cs(1);
	outEEBits(0, 16);
	
	foo = 0x4C << 4;		/* ewen = 1 00 11 */
	outEEBits(foo, 11);
	set_eeprom_cs(0);
	usleep(1);
	outEEBit(0);
}


void EEdisableWR(void){
	u_short foo;

	set_eeprom_cs(1);
	outEEBits(0, 16);

	foo = 0x40 << 4;		/* ewds = 1 00 00 */
	outEEBits(foo, 11);
	set_eeprom_cs(0);
	usleep(1);
	outEEBit(0);
}

static void clearEEall(void){
	/* clear the whole EEPROM -- dangerous */
	u_short foo;

	EEenableWR();
	set_eeprom_cs(1);
	outEEBits(0, 16);

	foo = 0x44 << 4;		/* wral = 1 00 01 */
	outEEBits(foo, 11);
	outEEBits(0x00, 16);		/* clear to all 00 */

	set_eeprom_cs(0);
	usleep(1);
	set_eeprom_cs(1);
	
	BitSet(p2_image, 6);		/* put into input mode */
	ioport2 = p2_image;
	/* wait till !busy */
	while(!foo){
		foo = ioport2;
		foo &= Bit(6);
	}

	set_eeprom_cs(0);
	usleep(1);
	EEdisableWR();
}
	
void openEEPROM(void){
	/* called by diagnostic monitor for fiddling around */
	/* think old Sun ROM monitor */

	char buffer[64], *p;
	u_short adrs, count;
	u_long quux;
	int i;

	puts("Open EEPROM Mode\n");
	while(1){

		puts(": ");
		getline(buffer, 64, 0);

		switch( buffer[0] ){
		  case '?':
#if 0			
			puts("\te addrs - edit contents\n"
			     "\td - dump contents\n"
			     "\tw - write out changes\n"
			     "\tz - reset contents to 00\n"
			     "\th number - decimal to hex conversion\n"
			     "\ti - re-read eeprom\n"
			     "\tx - exit\n"
			     );
#else
			puts("no help\n");
#endif			
			break;
		  case 'x':
			return;
		  case 'i':
			initEEPROM();
			break;
		  case 'w':
			commitEEPROM();
			break;
#if 0			
		  case 'h':
			quux = getnumat(10, buffer+1);
			printf("%04.4lx %04.4lx\n",
			       (u_short)(quux>>16),
			       (u_short)(quux & 0xffff)
			       );
			break;
#endif
		  case 'd':
			for(i=0;i<EEPROM_SIZE;i++){
				if(!(i%16)){
					snprintf(buffer, sizeof(buffer), "\n%04.4lX :", i);
					puts(buffer);
				}
				snprintf(buffer, sizeof(buffer), " %02.2x", ((u_char*)&eeprom)[i] );
				puts(buffer);
			}
			puts("\n");
			break;
			
		  case 'e':
			adrs = getnumat(16, buffer+1);
			/*
			  hit return to skip to next byte
			  a number to change the value
			  . when done
			  (this is exactly the 'q' command on your Sun3)
			*/
			for( ;adrs<EEPROM_SIZE; adrs++){
				snprintf(buffer, sizeof(buffer), "\n%04.4lX : %02.2x > ",
				       adrs, ((u_char*)&eeprom)[adrs]
				       );
				puts(buffer);
				getline(buffer, 64, 0);
				if(! *buffer) continue;
				if(*buffer== '.') break;
				if(*buffer =='?')
					puts("enter a value, return to skip, or . when done\n");

				count = getnumat(16, buffer);
				((u_char*)&eeprom)[adrs] = count & 0xFF;
			}
			break;

		  case 'z':
			puts("This will completely wipe out the EEPROM\n");
			puts("Do you know what you are doing? ");
			getline(buffer, 64, 0);
			if(buffer[0]!='y') break;
			puts("Are you sure? ");
			getline(buffer, 64, 0);
			if(buffer[0] =='y') clearEEall();
			break;
			
		  default:
			puts("\nHuh?\n");
			break;
		}
	}
}

