RGSRC=../../../..
include $(RGSRC)/envir.mak

ifdef CONFIG_HW_80211N_ATHEROS_AR50WL
    SUBDIRS+=$(if $(CONFIG_HW_80211N_ATHEROS_AR50WL),wireless_11n)
else
    SUBDIRS+=$(if $(CONFIG_HW_80211G_ATHEROS_AR521X),wireless)
endif

include $(RGMK)

