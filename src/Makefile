

CFLAGS = -DTESTING -g -I.
CC = gcc
CI = ci -l -m'periodic'


OBJS = main.o hardware.o timer.o serial.o putchar.o


snmptest: snmp.o snmp2.o snmp3.o mib.o
	gcc $(CFLAGS) -o snmptest snmp.o snmp2.o snmp3.o mib.o

mib.c: mib.pl
	./mib.pl > mib.c


rcs:
	for x in `find . -name RCS -prune -o -type f -name \*\.[ch\] -print`; do $(CI) $$x; done



