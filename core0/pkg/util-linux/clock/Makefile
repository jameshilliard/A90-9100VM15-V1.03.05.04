RGSRC=../../../
include $(RGSRC)/envir.mak

TARGET=hwclock
O_OBJS=hwclock.o cmos.o rtc.o kd.o shhopt.o
CFLAGS+=-I. -I../lib -O2

include $(RGMK)
