
all:
	./plot -a < i96.pcb > i96.ps
	./plot -1 < i96.pcb > i96.p1
	./plot -6 < i96.pcb > i96.p6
	./plot -7 -s < i96.pcb | sed '/dup 7 eq/s/1 1 0/0 0 0/' > i96.p7

