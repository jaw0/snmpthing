
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

const struct _Mib mib[ 29 ] = {
	{ 8, oid_aaa, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)_version }	/* system.sysDescr.0 */,
	{ 8, oid_aab, TAG_OID, IT_OID|READABLE, (void*)snmp_sysoid }	/* system.sysObjectID.0 */,
	{ 8, oid_aac, TAG_TIMETICKS, IT_INT4|READABLE, (void*)&time }	/* system.sysUpTime.0 */,
	{ 8, oid_aad, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.contact }	/* system.sysContact.0 */,
	{ 8, oid_aae, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.sysname }	/* system.sysName.0 */,
	{ 8, oid_aaf, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)eeprom.location }	/* system.sysLocation.0 */,
	{ 8, oid_aag, TAG_INTEGER, IT_CONST|READABLE, (void*)64 }	/* system.sysServices.0 */,
	{ 8, oid_aah, TAG_INTEGER, IT_CONST|READABLE, (void*)1 }	/* interfaces.ifNumber.0 */,
	{ 10, oid_aai, TAG_INTEGER, IT_CONST|READABLE, (void*)1 }	/* interfaces.ifTable.ifEntry.ifIndex.1 */,
	{ 10, oid_aaj, TAG_OCTETSTRING, IT_STRING|READABLE, (void*)snmp_ifdesc }	/* interfaces.ifTable.ifEntry.ifDescr.1 */,
	{ 10, oid_aak, TAG_INTEGER, IT_CONST|READABLE, (void*)28 }	/* interfaces.ifTable.ifEntry.ifType.1 */,
	{ 10, oid_aal, TAG_INTEGER, IT_CONST|READABLE, (void*)2000 }	/* interfaces.ifTable.ifEntry.ifMtu.1 */,
	{ 10, oid_aam, TAG_GAUGE, IT_INT4|READABLE, (void*)&eeprom.baud }	/* interfaces.ifTable.ifEntry.ifSpeed.1 */,
	{ 10, oid_aan, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifPhysAddress.1 */,
	{ 10, oid_aao, TAG_INTEGER, IT_CONST|READABLE, (void*)1 }	/* interfaces.ifTable.ifEntry.ifAdminStatus.1 */,
	{ 10, oid_aap, TAG_INTEGER, IT_CONST|READABLE, (void*)1 }	/* interfaces.ifTable.ifEntry.ifOperStatus.1 */,
	{ 10, oid_aaq, TAG_TIMETICKS, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifLastChange.1 */,
	{ 10, oid_aar, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.inbytes }	/* interfaces.ifTable.ifEntry.ifInOctets.1 */,
	{ 10, oid_aas, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.inpackets }	/* interfaces.ifTable.ifEntry.ifInUcastPkts.1 */,
	{ 10, oid_aat, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifInNUcastPkts.1 */,
	{ 10, oid_aau, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifInDiscards.1 */,
	{ 10, oid_aav, TAG_INTEGER, IT_CONST|READABLE, (void*)&netstat.bad_input }	/* interfaces.ifTable.ifEntry.ifInErrors.1 */,
	{ 10, oid_aaw, TAG_INTEGER, IT_CONST|READABLE, (void*)&netstat.bad_proto }	/* interfaces.ifTable.ifEntry.ifInUnknownProtos.1 */,
	{ 10, oid_aax, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.outbytes }	/* interfaces.ifTable.ifEntry.ifOutOctets.1 */,
	{ 10, oid_aay, TAG_INTEGER, IT_INT4|READABLE, (void*)&netstat.outpackets }	/* interfaces.ifTable.ifEntry.ifOutUcastPkts.1 */,
	{ 10, oid_aaz, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifOutNUcastPkts.1 */,
	{ 10, oid_aba, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifOutDiscards.1 */,
	{ 10, oid_abb, TAG_INTEGER, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifOutErrors.1 */,
	{ 10, oid_abc, TAG_GAUGE, IT_CONST|READABLE, (void*)0 }	/* interfaces.ifTable.ifEntry.ifOutQLen.1 */,
	{0,0,0,0,0}
};


