/*
  $Id$

  lcd.c
      all of the LCD handling code


*/


#include <mmlib.h>
#include <nstdio.h>

#pragma optimize(1)
#pragma registers(2)
#pragma rc




FILE lcd_port_file;
FILE *lcd_port;

const struct io_fs lcd_port_fs = {
	lcd_putchar,
	lcd_getchar,
	lcd_noop,	/* close */
	lcd_noop,	/* flush */
	lcd_status
};

void initLCD(void){

	lcd_port = &lcd_port_file;
	finit( lcd_port );
	lcd_port->fs = &lcd_port_fs;
	BitClear(f->flags, F_ONLCR);
	
	/* init seq */

	

}

int lcd_noop(FILE*f){
	return 1;
}

int lcd_status(FILE*f){
	return FST_O;
}

int lcd_getchar(FILE*f){
	return -1;
}

/*
  place data
  pause
  raise E
  pause
  drop E
*/

void lcd_cmd_nib(int n){
	int c;

	c = n;	/* XXX - mutilate */
	ioport1 = c;
	usleep( XXX );

	c |= LCD_ENABLE;
	ioport1 = c;
	usleep( XXX );

	c &= ~ LCD_ENABLE;
	ioport1 = c;
}

void lcd_data_nib(int n){
	int c;

	c = n;	/* XXX - mutilate */
	c |= LCD_ADDR;
	ioport1 = c;
	usleep( XXX );

	c |= LCD_ENABLE;
	ioport1 = c;
	usleep( XXX );

	c &= ~ LCD_ENABLE;
	ioport1 = c;
}

void lcd_cmd(char c){
	lcd_cmd_nib(c>>4);	/* high */
	lcd_cmd_nib(c&0xF);	/* low */
}

void lcd_char(int c){
	lcd_data_nib( c>>4 );	/* high */
	lcd_data_nib( c&0xF );	/* low */
}


	
/*
  esc seq.s:

    +h	home
    +c  clear screen
    
    +r  right
    +l  left
    +t  top row
    +b  bottom row
    +R  scroll right
    +L  scroll left

    +0	display off
    +1	display on

    +_	cursor on
    +o	cusror off

*/

int lcd_putchar(FILE*f, char c){

	if( c == 0x1B )
		return;
	
	if( f->prevout == 0x1B ){
		switch( c ){

		  case 'h':	lcd_cmd( LCDC_HOME );		break;
		  case 'c':	lcd_cmd( LCDC_CLS );		break;
		  case '0':	lcd_cmd( LCDC_DISP_OFF );	break;
		  case '1':	lcd_cmd( LCDC_DISP_ON );	break;
		  case '_':	lcd_cmd( LCDC_CUR_ON );		break;
		  case 'o':	lcd_cmd( LCDC_CUR_OFF );	break;
		  case 'r':	lcd_cmd( LCDC_MOV_CUR_R );	break;
		  case 'l':	lcd_cmd( LCDC_MOV_CUR_L );	break;
		  case 'R':	lcd_cmd( LCDC_SCROLL_R );	break;
		  case 'L':	lcd_cmd( LCDC_SCROLL_L );	break;
		  case 'b':
		  case 't':
			
		}
	}else{
		lcd_char(c);
	}

	return 1;
}




	
