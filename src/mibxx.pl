#!/usr/local/bin/perl
# -*- perl -*-

print <<EOH;

/* This file is automagically generated -- DO NOT EDIT */

#include <aliases.h>
#include <mib.h>
#include <snmp.h>


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

    next if /^$/;

    if( /^[a-z]/ ){
	chop( $oid = `snmptranslate $_[0]` );
    }else{
	$oid = $base . "." . $_[0];
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

    print "const u_char oid_${i}[] = { ", join(",", @so), " };\n";

    $foo = "{ $l, oid_${i}, $_[1], $_[2], (int)$_[3] }";
    push @later, $foo;
    $i++;
}

print "\nconst struct Mib mib[] = {\n";

foreach $f (@later){
    print "\t$f,\n";
}

print "\t{0,0,0,0,0}\n};\n\n\n";




__END__

system.sysDescr.0	TAG_OCTETSTRING	IT_STRING|READABLE	"description"
system.sysContact.0	TAG_OCTETSTRING	IT_STRING|READABLE	"contact"
system.sysName.0	TAG_OCTETSTRING	IT_STRING|READABLE	"name"
system.sysLocation.0	TAG_OCTETSTRING	IT_STRING|READABLE	"location"
system.sysServices.0	TAG_INTEGER	IT_CONST|READABLE	64

interfaces.ifNumber.0				TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifIndex.1		TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifType.1		TAG_INTEGER	IT_CONST|READABLE	28
interfaces.ifTable.ifEntry.ifMtu.1		TAG_INTEGER	IT_CONST|READABLE	1500
interfaces.ifTable.ifEntry.ifSpeed.1		TAG_GAUGE	IT_CONST|READABLE	2400
interfaces.ifTable.ifEntry.ifPhysAddress.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifAdminStatus.1	TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifOperStatus.1	TAG_INTEGER	IT_CONST|READABLE	1
interfaces.ifTable.ifEntry.ifLastChange.1	TAG_TIMETICKS	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInNUcastPkts.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInDiscards.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInErrors.1		TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifInUnknownProtos.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutNUcastPkts.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutDiscards.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutErrors.1	TAG_INTEGER	IT_CONST|READABLE	0
interfaces.ifTable.ifEntry.ifOutQLen.1		TAG_GAUGE	IT_CONST|READABLE	0

