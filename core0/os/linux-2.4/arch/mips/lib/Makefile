#
# Makefile for MIPS-specific library files..
#

USE_STANDARD_AS_RULE := true

L_TARGET = lib.a

obj-y				+= csum_partial_copy.o \
				   promlib.o rtc-std.o rtc-no.o \
				   watch.o strlen_user.o strncpy_user.o \
				   strnlen_user.o

ifeq ($(CONFIG_CPU_R3000)$(CONFIG_CPU_TX39XX)$(CONFIG_CPU_LX4189),y)
  obj-y	+= r3k_dump_tlb.o
else
  obj-y	+= dump_tlb.o
endif

ifndef CONFIG_CPU_LX4189
  obj-y	+= memcpy.o csum_partial.o memset.o
endif

obj-$(CONFIG_BLK_DEV_FD)	+= floppy-no.o floppy-std.o
obj-$(subst m,y,$(CONFIG_IDE))	+= ide-std.o ide-no.o	# needed for ide module
obj-$(CONFIG_PC_KEYB)		+= kbd-std.o

include $(TOPDIR)/Rules.make
