RGSRC=../../..
include $(RGSRC)/envir.mak

TARGET=dosfsck
RAMDISK_BIN_FILES+=$(TARGET)

O_OBJS=boot.o check.o common.o dosfsck.o fat.o file.o io.o lfn.o

include $(RGMK)
