
/*
  EEPROM format
*/

#ifndef __EEPROM_H__
#define __EEPROM_H__

struct EEPROM {
	/* 93c66 will hold up to 512 bytes */
	
	u_short	writecount;	/* number of times device has been written to */
	u_short version;        /* eeprom layout version */
#define EEPROM_CURRENT_VERSION	2

	u_char	chksum;		/* eeprom checksum */
	u_char	diag;		/* should we boot up in diag mode? */
	u_short	baud;		/* default baud rate */

	u_long  myipaddr;       /* local ip address */
	u_long  traphost;       /* ip address to send traps to */
	u_long  sysloghost;     /* ip address to send syslog stuff to */

	char    community[16];  /* snmp community string */
	char    passwd[16];     /* telnet password */
	
	u_char  loglevel;       /* anything at this level or lower is logged, 0 disables syslog */
	u_char  traplevel;      /* anything at this level or lower is trapped, 0 disables traps */

	/* trap generation params */
	u_char  inbitmode[24/4];/* mode for each input - 2 bits each */
	u_char  inbitlevel[24]; /* trap level for each input */
	u_char  anahigh[8];     /* analog high threshhold */
	u_char  analow[8];      /* analog high threshhold */
	u_char  analevel[8];    /* trap level for analog inputs */

	/* snmp things */
	char sysname[32];       /* sysName */
	char location[16];      /* sysDescr */
	char contact[16];       /* sysContact */

};

extern struct EEPROM eeprom;

#define EEPROM_SIZE	(sizeof(struct EEPROM))


extern void	initEEPROM(void);	/* called by initHardware */
extern void	commitEEPROM(void);	/* to commit changes to the device */
extern void	openEEPROM(void);	/* interactive monitor */

#endif 

