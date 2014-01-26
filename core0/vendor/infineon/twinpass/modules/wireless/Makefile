RGSRC=../../../../..
include $(RGSRC)/envir.mak

SUBDIRS+=net80211 sys
INTERNAL_HEADERS+=common_cli.h kthread.h 

MOD_CFLAGS += -DWIRELESS_EVENT_SUPPORT

MOD_TARGET=wifi_proxy.o

ifndef CONFIG_INFINEON_PPE_D4

O_OBJS_$(MOD_TARGET)+=mps0_wifi.o

else

O_OBJS_$(MOD_TARGET)+=mps0_wifi_ppe.o

endif

ifeq ($(CONFIG_TWINPASS_DUALCORE_WLAN),y)
  MOD_2_STAT+=$(MOD_TARGET)
endif

RAMDISK_MODULES_FILES+=$(MOD_TARGET)

include $(RGMK)
