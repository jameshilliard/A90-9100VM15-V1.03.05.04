RGSRC=$(E2FSSRC)/../..
include $(RGSRC)/envir.mak
E2FSMK=$(RGMK)

CFLAGS+=-I$(E2FSSRC)/lib -I$(E2FSSRC)/lib/et

LDFLAGS+=-L$(E2FSSRC)/lib/et -L$(E2FSSRC)/lib/ss -lss \
  -L$(E2FSSRC)/lib/e2p -L$(E2FSSRC)/lib/ext2fs \
  -L$(E2FSSRC)/lib/uuid 
LDLIBS:=-lcom_err -le2p -lext2fs -luuid $(LDLIBS)

# XXX CFLAGS -DHAVE... should be replaced by usage of include/libc_config.h
CFLAGS+=-DENABLE_HTREE=1 \
        -DENABLE_SWAPFS=1 \
	-DPACKAGE=\"e2fsprogs\" \
        -DVERSION=\"1.32\" \
	-DSTDC_HEADERS=1 \
	-DHAVE_ALLOCA_H=1 \
	-DHAVE_ALLOCA=1 \
	-DHAVE_UNISTD_H=1 \
	-DHAVE_GETPAGESIZE=1 \
	-DHAVE_ARGZ_H=1 \
	-DHAVE_LIMITS_H=1 \
	-DHAVE_LOCALE_H=1 \
	-DHAVE_NL_TYPES_H=1 \
	-DHAVE_MALLOC_H=1 \
	-DHAVE_STRING_H=1 \
	-DHAVE_UNISTD_H=1 \
	-DHAVE_SYS_PARAM_H=1 \
	-DHAVE_GETCWD=1 \
	-DHAVE_MUNMAP=1 \
	-DHAVE_PUTENV=1 \
	-DHAVE_SETENV=1 \
	-DHAVE_SETLOCALE=1 \
	-DHAVE_STRCHR=1 \
	-DHAVE_STRCASECMP=1 \
	-DHAVE_STRDUP=1 \
	-DHAVE___ARGZ_COUNT=1 \
	-DHAVE___ARGZ_STRINGIFY=1 \
	-DHAVE___ARGZ_NEXT=1 \
	-DHAVE_STPCPY=1 \
	-DHAVE_STPCPY=1 \
	-DHAVE_LC_MESSAGES=1 \
	-DHAVE_STDLIB_H=1 \
	-DHAVE_UNISTD_H=1 \
	-DHAVE_STDARG_H=1 \
	-DHAVE_ERRNO_H=1 \
	-DHAVE_MALLOC_H=1 \
	-DHAVE_MNTENT_H=1 \
	-DHAVE_PATHS_H=1 \
	-DHAVE_DIRENT_H=1 \
	-DHAVE_GETOPT_H=1 \
	-DHAVE_SETJMP_H=1 \
	-DHAVE_SIGNAL_H=1 \
	-DHAVE_TERMIOS_H=1 \
	-DHAVE_LINUX_FD_H=1 \
	-DHAVE_SYS_IOCTL_H=1 \
	-DHAVE_SYS_MOUNT_H=1 \
	-DHAVE_SYS_SYSMACROS_H=1 \
	-DHAVE_SYS_TIME_H=1 \
	-DHAVE_SYS_STAT_H=1 \
	-DHAVE_SYS_TYPES_H=1 \
	-DHAVE_NET_IF_H=1 \
	-DHAVE_NETINET_IN_H=1 \
	-DHAVE_VPRINTF=1 \
	-DSIZEOF_SHORT=2 \
	-DSIZEOF_INT=4 \
	-DSIZEOF_LONG=4 \
	-DSIZEOF_LONG_LONG=8 \
	-DHAVE_GETRUSAGE=1 \
	-DHAVE_LLSEEK=1 \
	-DHAVE_STRCASECMP=1 \
	-DHAVE_SRANDOM=1 \
	-DHAVE_FCHOWN=1 \
	-DHAVE_MALLINFO=1 \
	-DHAVE_FDATASYNC=1 \
	-DHAVE_STRNLEN=1 \
	-DHAVE_SYSCONF=1 \
	-DHAVE_PATHCONF=1 \
	-DHAVE_EXT2_IOCTLS=1

ifdef CONFIG_RG_OS_LINUX_24
  CFLAGS+=-DHAVE_LINUX_MAJOR_H=1
endif

ifdef CONFIG_CPU_BIG_ENDIAN
  CFLAGS+=-DWORDS_BIGENDIAN=1
endif
