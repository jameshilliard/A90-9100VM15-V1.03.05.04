RGSRC=../../../../../../../../..
include $(RGSRC)/envir.mak

# added "c" to start of name so init function gets called earlier which starts watchdog timer earlier during kernel startup
MOD_TARGET=cport_mod.o
MOD_2_STAT=cport_mod.o
OX_OBJS=port.o

INTERNAL_HEADERS=port_defs.h

include $(RGMK)

