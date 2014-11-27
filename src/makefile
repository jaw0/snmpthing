

EXE = main
OBJS = main.obj serial.obj hardware.obj timer.obj byteio.obj printf.obj getline.obj getnum.obj nstdio.obj eeprom.obj cli.obj misc.obj ip.obj mib.obj snmp.obj snmp2.obj snmp3.obj version.obj
# OBJS = main.obj serial.obj hardware.obj timer.obj byteio.obj printf.obj getline.obj getnum.obj nstdio.obj monitor.obj snmp.obj snmp2.obj snmp3.obj ip.obj misc.obj math.obj mib.obj

#EXE = test4
#OBJS = test4.obj

ROMSTART = 2000h
ROMEND =   9FFFh

REGSTART = 01ah
REGEND   = 0ffh

RAMSTART = 0A000h
RAMEND =   0FFFFh

USRINCDIR =d:\src
INCDIR = c:\include\ic96

LIBDIR = c:\lib\ic96

CC = ic96
LINKER = rl96
LIBRARIAN = lib96

CFLAGS = searchinclude($(INCDIR),$(USRINCDIR)) code
LFLAGS = purge absstack rom($(ROMSTART)-$(ROMEND)) ram($(REGSTART)-$(REGEND),$(RAMSTART)-$(RAMEND)(stack))
STDOBJS = $(LIBDIR)\cstart.obj
LIBS = $(LIBDIR)\c96.lib

$(EXE).bin:


$(EXE).abs: $(OBJS) $(LIBS)
	chgsep ,\_&\n &&!
$(OBJS)
$(STDOBJS)
$(LIBS)
! rl96.tmp
	4dos /c echo ` &` >>rl96.tmp
	chgsep \_&\n &&!
$(LFLAGS)
! rl96_2.tmp
	4dos /c type rl96_2.tmp >>rl96.tmp
	$(LINKER) & < rl96.tmp
	4dos /c move $(EXE) $(EXE).abs

.c.obj:
	$(CC) $< $(CFLAGS)
 
.abs.hex:
	oh $<
 
.hex.bin:
	oh2bin -s $(ROMSTART) -e $(ROMEND) $<


