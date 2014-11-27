
/*
  $Id: snmp.h,v 1.2 1997/09/10 17:14:46 jaw Exp $

  Copyright (c) 1997 OpNet Inc.

*/

/* top 2 bits of tag are class */
#define CLASS_UNIVERSAL		0
#define CLASS_APPLICATION	0x40
#define CLASS_CONTEXT		0x80
#define CLASS_PRIVATE		0xC0

/* next bit specifies simple or compound */
#define TYPE_SIMPLE		0
#define TYPE_COMPOUND		0x20

/* then 5 bits of tag number */
#define TAG_INTEGER		2
#define TAG_OCTETSTRING		4
#define TAG_NULL		5
#define TAG_OID			6
#define TAG_SEQ			16
#define TAG_IPADDR		( CLASS_APPLICATION | 0 )
#define TAG_COUNTER		( CLASS_APPLICATION | 1 )
#define TAG_GAUGE		( CLASS_APPLICATION | 2 )
#define TAG_TIMETICKS		( CLASS_APPLICATION | 3 )
#define TAG_OPAQUE		( CLASS_APPLICATION | 4 )

/* SNMP PDUs */
#define PDU_GETREQUEST		( CLASS_CONTEXT | TYPE_COMPOUND | 0 )
#define PDU_GETNEXT		( CLASS_CONTEXT | TYPE_COMPOUND | 1 )
#define PDU_GETRESPONSE		( CLASS_CONTEXT | TYPE_COMPOUND | 2 )
#define PDU_SETREQUEST		( CLASS_CONTEXT | TYPE_COMPOUND | 3 )
#define PDU_TRAP		( CLASS_CONTEXT | TYPE_COMPOUND | 4 )

/* error status */
#define ERRSTAT_NONE		0
#define ERRSTAT_TOOBIG		1
#define ERRSTAT_NOSUCHNAME	2
#define ERRSTAT_BADVALUE	3
#define ERRSTAT_READONLY	4
#define ERRSTAT_GENERR		5

/* trap types */
#define TRAP_COLDSTART		0
#define TRAP_WARMSTART		1
#define TRAP_LINKDOWN		2
#define TRAP_LINKUP		3
#define TRAP_AUTHFAIL		4
#define TRAP_EGPLOSS		5
#define TRAP_ENTSPEC		6

/* to signal an error */
#define TAG_ERROR		0xFFFF



typedef struct {
	int type;      /* type of object */
	int len;       /* length of object */
	int tlen;      /* total length of representation = length of tag, len, data */
	u_char *next;  /* next object in sequence */
	u_char *p;     /* value data of object */
	long i;        /* unencoded int value data of object */
} SNMPObject;

