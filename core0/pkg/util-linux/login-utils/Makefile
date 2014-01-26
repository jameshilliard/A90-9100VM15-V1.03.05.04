# Makefile -- Makefile for util-linux Linux utilities


RGSRC := ../../../
include ../MCONFIG
LDFLAGS = -lcrypt

all: login

login: login.o checktty.o
	$(CC) $(LDFLAGS) -o $@ $< ../lib/setproctitle.o checktty.o 

clean: dummy
	rm -f login *.o

dummy:
