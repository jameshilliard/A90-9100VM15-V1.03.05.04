RGSRC=../../../../../../../../..
include $(RGSRC)/envir.mak

MOD_TARGET=base.o
MOD_2_STAT=base.o

ifdef CONFIG_WESTELL_WIRELESS_ONBOARD
MOD_CFLAGS+=-DWESTELL_WIRELESS_ONBOARD
endif

OX_OBJS=setup.o prom.o interrupt.o danubeIRQ.o reset.o kgdb_serial.o \
  ikos.o dma-core.o
O_OBJS+=$(if $(CONFIG_PCI),pci.o)

INTERNAL_HEADERS=dma-core.h

include $(RGMK)
