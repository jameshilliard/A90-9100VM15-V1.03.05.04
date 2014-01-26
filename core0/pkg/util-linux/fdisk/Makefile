RGSRC=../../../
include $(RGSRC)/envir.mak

TARGET=sfdisk 
RAMDISK_BIN_FILES+=$(TARGET)
O_OBJS_sfdisk=sfdisk.o i386_sys_types.o partname.o

# add TARGET+=fdisk if you want to compile fdisk aswell
O_OBJS_fdisk=fdisk.o llseek.o fdiskbsdlabel.o fdisksgilabel.o fdisksunlabel.o \
	fdiskaixlabel.o i386_sys_types.o partname.o

CFLAGS+=-I. -I../lib -O2

include $(RGMK)
