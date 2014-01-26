RGSRC=../..
include $(RGSRC)/envir.mak

SUBDIRS+=hostapd
LINK_DIRS=hostapd
ifndef CONFIG_RG_GPL
SUBDIRS+=main
endif

include $(RGMK)

