#!/usr/local/bin/perl
# -*- perl -*-

print <<EOH;

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

EOH
    ;


$i = "aaa";

while( <DATA> ){
    chop;
    split;

    if( /:$/ ){
	$base = $_;
	$base =~ s/://;
	next;
    }

    next if /^\s*$/;

    if( /^[a-z]/ ){
	chop( $oid = `snmptranslate $_[0]` );
    }else{
	chop( $oid = `snmptranslate $base.$_[0]` );
	print STDERR "T: $oid\n";
    }
    $oid =~ s/^\.//;

    @suboid = split /\./, $oid;

    $f = $suboid[0] * 40 + $suboid[1];
    $suboid[1] = $f;
    shift @suboid;

    @so = ();
    foreach $s (@suboid){
	if($s > 128){
	    push @so, (0x80 | ($s>>8));
	    push @so, ($s & 0xFF);
	}else{
	    push @so, $s;
	}
    }

    $l = @so;

    print "static const u_char oid_${i}[] = { ", join(",", @so), " };\n";

    $foo = "{ $l, oid_${i}, $_[1], $_[2], (void*)$_[3] }";
    push @later, $foo;
    $i++;
}

print "\nconst struct _Mib mib[ ", $#later + 2, " ] = {\n";
open N,  ">nmib.h";
print N "#define N_MIB ", $#later + 2, "\n\n";
close N;

foreach $f (@later){
    print "\t$f,\n";
}

print "\t{0,0,0,0,0}\n};\n\n\n";




__END__

system.sysDescr.0	TAG_OCTETSTRING	IT_STRING|READABLE	_version
system.sysObjectID.0	TAG_OID		IT_OID|READABLE		snmp_sysoid
system.sysUpTime.0	TAG_TIMETICKS	IT_INT4|READABLE	&time
system.sysContact.0	TAG_OCTETSTRING	IT_STRING|READABLE	eeprom.contact
system.sysName.0	TAG_OCTETSTRING	IT_STRING|READABLE	eeprom.sysname
system.sysLocation.0	TAG_OCTETSTRING	IT_STRING|READABLE	eeprom.location
system.sysServices.0	TAG_INTEGER	IT_CONST|READABLE	64

interfaces.ifNumber.0				TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifIndex.1		TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifDescr.1		TAG_OCTETSTRING	IT_STRING|READABLE	snmp_ifdesc
interfaces.ifTable.ifEntry.ifType.1		TAG_INTEGER	IT_CONST|READABLE	28
interfaces.ifTable.ifEntry.ifMtu.1		TAG_INTEGER	IT_CONST|READABLE	2000
interfaces.ifTable.ifEntry.ifSpeed.1		TAG_GAUGE	IT_INT4|READABLE	&eeprom.baud
interfaces.ifTable.ifEntry.ifPhysAddress.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifAdminStatus.1	TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifOperStatus.1	TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifLastChange.1	TAG_TIMETICKS	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInOctets.1		TAG_INTEGER	IT_INT4|READABLE	&netstat.inbytes
interfaces.ifTable.ifEntry.ifInUcastPkts.1	TAG_INTEGER	IT_INT4|READABLE	&netstat.inpackets
interfaces.ifTable.ifEntry.ifInNUcastPkts.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInDiscards.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInErrors.1		TAG_INTEGER	IT_CONST|READABLE	&netstat.bad_input
interfaces.ifTable.ifEntry.ifInUnknownProtos.1	TAG_INTEGER	IT_CONST|READABLE	&netstat.bad_proto
interfaces.ifTable.ifEntry.ifOutOctets.1	TAG_INTEGER	IT_INT4|READABLE	&netstat.outbytes
interfaces.ifTable.ifEntry.ifOutUcastPkts.1	TAG_INTEGER	IT_INT4|READABLE	&netstat.outpackets
interfaces.ifTable.ifEntry.ifOutNUcastPkts.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutDiscards.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutErrors.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutQLen.1		TAG_GAUGE	IT_CONST|READABLE	0

.1.3.6.1.4.1.opnet.Products.SNMPBox.SNMPBoxV1:



