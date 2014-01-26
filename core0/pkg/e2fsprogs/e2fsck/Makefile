E2FSSRC=..
include $(E2FSSRC)/envir.mak

TARGET=e2fsck
RAMDISK_BIN_FILES+=$(TARGET)

O_OBJS_e2fsck=dict.o unix.o e2fsck.o super.o pass1.o pass1b.o pass2.o pass3.o \
  pass4.o pass5.o journal.o swapfs.o badblocks.o util.o dirinfo.o \
  dx_dirinfo.o ehandler.o problem.o message.o recovery.o region.o revoke.o \
  ea_refcount.o rehash.o

include $(E2FSMK)
