
/*
  diagnostic monitor code

  automatically called at boot time if both red and 
  black button are depressed or if eeprom.diag is set

*/

#include <mmlib.h>
#include <nstdio.h>

#pragma optimize(3)
#pragma registers(0) 

void dnld(void), upld(u_char*, u_char*);
void openEEPROM(void);

void romMonitor(void){

	puts("\nUR-MM 1.3a Micro Mouse Diagnostic Monitor Rev. c1.02\n");
	puts("Awaiting your command\n\n");
	
	while(1){
		char c;
		void (*ex)(void);
		u_char *adrs;
		char buffer[64];
		int i;
		
		puts("> ");

		getline(buffer, 64, 0);
		switch(buffer[0]){
		  case '?':
			puts("\td - Download code in HEX format\n");
			puts("\tu - Upload code in HEX format\n");
			puts("\te - Execute\n");
			puts("\to - Open EEPROM\n");
			puts("\tx - Exit monitor\n");
			/* ... */
			break;
			
		  case 'd':
			dnld();
			break;
			
		  case 'e':
			/* get address */
			puts("\nAddress: ");
			ex = (void(*)(void))getnum(16);
			ex();	/* and away we go ... */
			break;

		  case 'u':
			puts("Start address: ");
			adrs = (u_char*)getnum(16);
			puts("End address: ");
			upld(adrs, (u_char*)getnum(16) );
			break;
			
		  case 'o':
			openEEPROM();
			break;
			
		  case 'x':
		  case 'X':
			/* continue with boot */
			return;
			
		  default:
			puts("\nHuh?\n");
			break;
		}
	}
}

void dnld(void){
	/* download code in intel HEX format 
		may contain misc text as comments
		between records, as long as they 
		contain no ':'s
	*/

	int type=0, len, chk, c;
	char *addrs;

	puts("\nDownloading...\n");
	while(type != 1){
		while(getchar() != ':');	/* ignore upto ':' */
		len = getByte();
		addrs = (char*)getWord();
		type = getByte();
		chk = len + type + (u_short)addrs + ((u_short)addrs>>8);
		while(len--){
			c = getByte();
			*addrs++ = c;
			chk += c;
		}
		chk += getByte();
		if(chk){
			printf("\n\a\aDownload Error: Checksum 0x%0.2x != 0x00\n"
			       "Aborting Download\n", 
				chk);
			return;
		}
	}
	/* done */
}

void upld(u_char* addrs, u_char* upto ){
	int i, len;
	u_char chk;
	
	while(addrs<=upto){
		len = upto-addrs+1;
		if(len>16) len = 16;
		chk = 0;

		putchar(':');
		putByte(len);
		putByte(0);
		putWord((u_short)addrs);
		for(i=0;i<len;i++){
			chk += *addrs;
			putByte( *addrs );
			addrs++;
		}
		putByte( -chk );
	}
	/* puts(":00000001FF"); */
}


	
