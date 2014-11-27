
/* This file is automagically generated -- DO NOT EDIT */

#include <aliases.h>
#include <mib.h>
#include <snmp.h>
#include <eeprom.h>
#include <netstat.h>

static const char snmp_ifdesc[] = "sl0";
static const char snmp_sysoid[] = {1,2,3,4,5,6,7,8};
extern const char _version[];
extern u_long time;

static const u_char oid_aaa[] = { 43,6,1,2,1,1,1,0 };
static const u_char oid_aab[] = { 43,6,1,2,1,1,2,0 };
static const u_char oid_aac[] = { 43,6,1,2,1,1,3,0 };
static const u_char oid_aad[] = { 43,6,1,2,1,1,4,0 };
static const u_char oid_aae[] = { 43,6,1,2,1,1,5,0 };
static const u_char oid_aaf[] = { 43,6,1,2,1,1,6,0 };
static const u_char oid_aag[] = { 43,6,1,2,1,1,7,0 };
static const u_char oid_aah[] = { 43,6,1,2,1,2,1,0 };
static const u_char oid_aai[] = { 43,6,1,2,1,2,2,1,1,1 };
static const u_char oid_aaj[] = { 43,6,1,2,1,2,2,1,2,1 };
static const u_char oid_aak[] = { 43,6,1,2,1,2,2,1,3,1 };
static const u_char oid_aal[] = { 43,6,1,2,1,2,2,1,4,1 };
static const u_char oid_aam[] = { 43,6,1,2,1,2,2,1,5,1 };
static const u_char oid_aan[] = { 43,6,1,2,1,2,2,1,6,1 };
static const u_char oid_aao[] = { 43,6,1,2,1,2,2,1,7,1 };
static const u_char oid_aap[] = { 43,6,1,2,1,2,2,1,8,1 };
static const u_char oid_aaq[] = { 43,6,1,2,1,2,2,1,9,1 };
static const u_char oid_aar[] = { 43,6,1,2,1,2,2,1,10,1 };
static const u_char oid_aas[] = { 43,6,1,2,1,2,2,1,11,1 };
static const u_char oid_aat[] = { 43,6,1,2,1,2,2,1,12,1 };
static const u_char oid_aau[] = { 43,6,1,2,1,2,2,1,13,1 };
static const u_char oid_aav[] = { 43,6,1,2,1,2,2,1,14,1 };
static const u_char oid_aaw[] = { 43,6,1,2,1,2,2,1,15,1 };
static const u_char oid_aax[] = { 43,6,1,2,1,2,2,1,16,1 };
static const u_char oid_aay[] = { 43,6,1,2,1,2,2,1,17,1 };
static const u_char oid_aaz[] = { 43,6,1,2,1,2,2,1,18,1 };
static const u_char oid_aba[] = { 43,6,1,2,1,2,2,1,19,1 };
static const u_char oid_abb[] = { 43,6,1,2,1,2,2,1,20,1 };
static const u_char oid_abc[] = { 43,6,1,2,1,2,2,1,21,1 };

const struct _Mib mib[ 30 ] = {
	{ 8, oid_aaa, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)_version },
	{ 8, oid_aab, TAG_OID, IT_OID|READABLE, (void*)snmp_sysoid },
	{ 8, oid_aac, TAG_TIMETICKS, IT_INT4|READABLE, (void*)&time },
	{ 8, oid_aad, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.contact },
	{ 8, oid_aae, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.sysname },
	{ 8, oid_aaf, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.location },
	{ 8, oid_aag, TAG_INTEGER, IT_CONST|READABLE, (void*)64 },
	{ 8, oid_aah, TAG_INTEGER, IT_CONST|READABLE, (void*)1 },
	{ 10, oid_aai, TAG_INTEGER, IT_CONST|READABLE, (void*)1 },
	{ 10, oid_aaj, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)snmp_ifdesc },
	{ 10, oid_aak, TAG_INTEGER, IT_CONST|READABLE, (void*)28 },
	{ 10, oid_aal, TAG_INTEGER, IT_CONST|READABLE, (void*)2000 },
	{ 10, oid_aam, TAG_GAUGE, IT_INT4|READABLE, (void*)&eeprom.baud },
	{ 10, oid_aan, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_aao, TAG_INTEGER, IT_CONST|READABLE, (void*)1 },
	{ 10, oid_aap, TAG_INTEGER, IT_CONST|READABLE, (void*)1 },
	{ 10, oid_aaq, TAG_TIMETICKS, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_aar, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.inbytes },
	{ 10, oid_aas, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.inpackets },
	{ 10, oid_aat, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_aau, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_aav, TAG_INTEGER, IT_CONST|READABLE, (void*)&netstat.bad_input },
	{ 10, oid_aaw, TAG_INTEGER, IT_CONST|READABLE, (void*)&netstat.bad_proto },
	{ 10, oid_aax, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.outbytes },
	{ 10, oid_aay, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.outpackets },
	{ 10, oid_aaz, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_aba, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_abb, TAG_INTEGER, IT_CONST|READABLE, (void*)0 },
	{ 10, oid_abc, TAG_GAUGE, IT_CONST|READABLE, (void*)0 },
	{0,0,0,0,0}
};


