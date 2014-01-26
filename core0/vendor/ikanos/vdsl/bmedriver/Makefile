RGSRC=../../../../
include $(RGSRC)/envir.mak

BME_MAJOR=251
MOD_TARGET+=bmedrv.o

O_OBJS+=bmeapi.o bmedriver.o 

LINUX_CFLAGS+=-DBME_MAJOR=$(BME_MAJOR) -I.
ifdef CONFIG_OS_LINUX_26
  LINUX_CFLAGS+=-DLINUX_2_6 -DCONFIG_KERNELVERSION
endif

INTERNAL_HEADERS+=bmeapi.h bmedriver.h 

MOD_CFLAGS+=$(VINAX_CFLAGS) -I$(RGSRC)/vendor/infineon/twinpass/bsp/include

RAMDISK_MODULES_FILES+=$(MOD_TARGET)

include $(RGMK)

ifdef IS_BUILDDIR
ramdisk::
	$(RG_MKNOD) $(RAMDISK_DEV_DIR)/bme c $(BME_MAJOR) 0
endif
